//------------------------------------------------------------------------------
// GB_bitmap_AxB_saxpy_template.c: C<#M>+=A*B when C is bitmap
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// GB_AxB_saxpy_sparsity determines the sparsity structure for C<M or !M>=A*B
// or C=A*B, and this template is used when C is bitmap.  C can be modified
// in-place if the accum operator is the same as the monoid.

#undef  GB_FREE_ALL
#define GB_FREE_ALL                         \
{                                           \
    GB_FREE_WERK (&Wf, Wf_size) ;           \
    GB_FREE_WERK (&Wax, Wax_size) ;         \
    GB_FREE_WERK (&Wbx, Wbx_size) ;         \
    GB_FREE_WERK (&Wcx, Wcx_size) ;         \
    GB_WERK_POP (GH_slice, int64_t) ;       \
    GB_WERK_POP (A_slice, int64_t) ;        \
    GB_WERK_POP (B_slice, int64_t) ;        \
    GB_WERK_POP (M_ek_slicing, int64_t) ;   \
}

{

    //--------------------------------------------------------------------------
    // declare workspace
    //--------------------------------------------------------------------------

    int8_t  *restrict Wf  = NULL ; size_t Wf_size = 0 ;
    GB_void *restrict Wax = NULL ; size_t Wax_size = 0 ;
    GB_void *restrict Wbx = NULL ; size_t Wbx_size = 0 ;
    GB_void *restrict Wcx = NULL ; size_t Wcx_size = 0 ;
    GB_WERK_DECLARE (GH_slice, int64_t) ;
    GB_WERK_DECLARE (A_slice, int64_t) ;
    GB_WERK_DECLARE (B_slice, int64_t) ;
    GB_WERK_DECLARE (M_ek_slicing, int64_t) ;

    //--------------------------------------------------------------------------
    // determine max # of threads to use
    //--------------------------------------------------------------------------

    GB_GET_NTHREADS_MAX (nthreads_max, chunk, Context) ;

    //--------------------------------------------------------------------------
    // get C, M, A, and B
    //--------------------------------------------------------------------------

    ASSERT (GB_IS_BITMAP (C)) ;                 // C is always bitmap
    int8_t *restrict Cb = C->b ;
    GB_CTYPE *restrict Cx = (GB_CTYPE *) C->x ;
    const int64_t cvlen = C->vlen ;
    ASSERT (C->vlen == A->vlen) ;
    ASSERT (C->vdim == B->vdim) ;
    ASSERT (A->vdim == B->vlen) ;
    int64_t cnvals = C->nvals ;

    const int64_t *restrict Bp = B->p ;
    const int64_t *restrict Bh = B->h ;
    const int8_t  *restrict Bb = B->b ;
    const int64_t *restrict Bi = B->i ;
    const GB_BTYPE *restrict Bx = (GB_BTYPE *) (B_is_pattern ? NULL : B->x) ;
    const int64_t bvlen = B->vlen ;
    const int64_t bvdim = B->vdim ;
    const int64_t bnvec = B->nvec ;

    const bool B_jumbled = B->jumbled ;
    const int64_t bnz = GB_NNZ_HELD (B) ;

    const bool B_is_sparse = GB_IS_SPARSE (B) ;
    const bool B_is_hyper = GB_IS_HYPERSPARSE (B) ;
    const bool B_is_bitmap = GB_IS_BITMAP (B) ;
    const bool B_is_sparse_or_hyper = B_is_sparse || B_is_hyper ;

    const int64_t *restrict Ap = A->p ;
    const int64_t *restrict Ah = A->h ;
    const int8_t  *restrict Ab = A->b ;
    const int64_t *restrict Ai = A->i ;
    const GB_ATYPE *restrict Ax = (GB_ATYPE *) (A_is_pattern ? NULL : A->x) ;
    const int64_t anvec = A->nvec ;
    const int64_t avlen = A->vlen ;
    const int64_t avdim = A->vdim ;

    const bool A_jumbled = A->jumbled ;
    const int64_t anz = GB_NNZ_HELD (A) ;

    const bool A_is_sparse = GB_IS_SPARSE (A) ;
    const bool A_is_hyper = GB_IS_HYPERSPARSE (A) ;
    const bool A_is_bitmap = GB_IS_BITMAP (A) ;
    const bool A_is_sparse_or_hyper = A_is_sparse || A_is_hyper ;

    const int64_t *restrict Mp = NULL ;
    const int64_t *restrict Mh = NULL ;
    const int8_t  *restrict Mb = NULL ;
    const int64_t *restrict Mi = NULL ;
    const GB_void *restrict Mx = NULL ;
    size_t msize = 0 ;
    int64_t mnvec = 0 ;
    int64_t mvlen = 0 ;
    const bool M_is_hyper  = GB_IS_HYPERSPARSE (M) ;
    const bool M_is_sparse = GB_IS_SPARSE (M) ;
    const bool M_is_sparse_or_hyper = M_is_hyper || M_is_sparse ;
    const bool M_is_bitmap = GB_IS_BITMAP (M) ;
    const bool M_is_full   = GB_IS_FULL (M) ;
    int M_nthreads = 0 ;
    int M_ntasks = 0 ;

    if (M != NULL)
    { 
        ASSERT (C->vlen == M->vlen) ;
        ASSERT (C->vdim == M->vdim) ;
        Mp = M->p ;
        Mh = M->h ;
        Mb = M->b ;
        Mi = M->i ;
        Mx = (GB_void *) (Mask_struct ? NULL : (M->x)) ;
        msize = M->type->size ;
        mnvec = M->nvec ;
        mvlen = M->vlen ;

        GB_SLICE_MATRIX (M, 8, chunk) ;

        // if M is sparse or hypersparse, scatter it into the C bitmap
        if (M_is_sparse_or_hyper)
        { 
            // Cb [pC] += 2 for each entry M(i,j) in the mask
            GB_bitmap_M_scatter (C,
                NULL, 0, GB_ALL, NULL, NULL, 0, GB_ALL, NULL,
                M, Mask_struct, GB_ASSIGN, GB_BITMAP_M_SCATTER_PLUS_2,
                M_ek_slicing, M_ntasks, M_nthreads, Context) ;
            // the bitmap of C now contains:
            //  Cb (i,j) = 0:   cij not present, mij zero
            //  Cb (i,j) = 1:   cij present, mij zero
            //  Cb (i,j) = 2:   cij not present, mij 1
            //  Cb (i,j) = 3:   cij present, mij 1
        }
    }

    //--------------------------------------------------------------------------
    // select the method
    //--------------------------------------------------------------------------

    if (B_is_sparse_or_hyper)
    {

        //-----------------------------------------------------
        // C                =               A     *     B
        //-----------------------------------------------------

        // bitmap           .               bitmap      hyper
        // bitmap           .               full        hyper
        // bitmap           .               bitmap      sparse
        // bitmap           .               full        sparse

        //-----------------------------------------------------
        // C               <M>=             A     *     B
        //-----------------------------------------------------

        // bitmap           any             bitmap      hyper
        // bitmap           any             full        hyper
        // bitmap           any             bitmap      sparse
        // bitmap           any             full        sparse

        //-----------------------------------------------------
        // C               <!M>=            A     *     B
        //-----------------------------------------------------

        // bitmap           any             bitmap      hyper
        // bitmap           any             full        hyper
        // bitmap           any             bitmap      sparse
        // bitmap           any             full        sparse

        #undef  GB_PANEL_SIZE
        #define GB_PANEL_SIZE 64

        ASSERT (GB_IS_BITMAP (A) || GB_IS_FULL (A)) ;
        double work = ((double) avlen) * ((double) bnz) ;
        int nthreads = GB_nthreads (work, chunk, nthreads_max) ;
        int naslice, nbslice, ntasks ;

        if (nthreads == 1 || bnvec == 0)
        { 
            // do the entire computation with a single thread
            naslice = 1 ;
            nbslice = 1 ;
        }
        else
        {
            // determine number of slices for A and B
            ntasks = 2 * nthreads ;
            int naslice_max = GB_ICEIL (avlen, GB_PANEL_SIZE) ;
            int dtasks = floor (sqrt ((double) ntasks)) ;
            naslice = GB_IMIN (dtasks, naslice_max) ;
            naslice = GB_IMAX (naslice, 1) ;
            nbslice = ntasks / naslice ;
            nbslice = GB_IMIN (nbslice, bnvec) ;
            if (nbslice > bnvec)
            {
                // too few vectors of B; recompute nbslice and naslice
                nbslice = bnvec ;
                naslice = ntasks / nbslice ;
                naslice = GB_IMIN (naslice, naslice_max) ;
                naslice = GB_IMAX (naslice, 1) ;
            }
        }

        ntasks = naslice * nbslice ;

        // slice the matrix B
        GB_WERK_PUSH (B_slice, nbslice + 1, int64_t) ;
        if (B_slice == NULL)
        { 
            // out of memory
            GB_FREE_ALL ;
            return (GrB_OUT_OF_MEMORY) ;
        }
        GB_pslice (B_slice, Bp, bnvec, nbslice, false) ;

        if (M == NULL)
        { 

            //------------------------------------------------------------------
            // C = A*B, no mask, A bitmap/full, B sparse/hyper
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 0
            #define GB_MASK_IS_BITMAP_OR_FULL  0
            #undef  keep
            #define keep 1
            if (A_is_bitmap)
            {
                // A is bitmap, B is sparse/hyper, no mask
                #undef  GB_A_IS_BITMAP
                #define GB_A_IS_BITMAP 1
                #include "GB_bitmap_AxB_saxpy_A_bitmap_B_sparse_template.c"
            }
            else
            {
                // A is full, B is sparse/hyper, no mask
                #undef  GB_A_IS_BITMAP
                #define GB_A_IS_BITMAP 0
                #include "GB_bitmap_AxB_saxpy_A_bitmap_B_sparse_template.c"
            }
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL

        }
        else if (M_is_sparse_or_hyper)
        { 

            //------------------------------------------------------------------
            // C<M> or <!M>=A*B, M sparse/hyper, A bitmap/full, B sparse/hyper
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 1
            #define GB_MASK_IS_BITMAP_OR_FULL  0
            #undef  keep
            const int8_t keep = (Mask_comp) ? 1 : 3 ;
            if (A_is_bitmap)
            {
                // A is bitmap, M and B are sparse/hyper
                #undef  GB_A_IS_BITMAP
                #define GB_A_IS_BITMAP 1
                #include "GB_bitmap_AxB_saxpy_A_bitmap_B_sparse_template.c"
            }
            else
            {
                // A is full, M and B are sparse/hyper
                #undef  GB_A_IS_BITMAP
                #define GB_A_IS_BITMAP 0
                #include "GB_bitmap_AxB_saxpy_A_bitmap_B_sparse_template.c"
            }
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL

        }
        else
        { 

            //------------------------------------------------------------------
            // C<M> or <!M> = A*B, M bitmap/full, A bitmap/full, B sparse/hyper
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 0
            #define GB_MASK_IS_BITMAP_OR_FULL  1
            #undef  keep
            #define keep 1
            if (A_is_bitmap)
            {
                // A is bitmap, M is bitmap/full, B is sparse/hyper
                #undef  GB_A_IS_BITMAP
                #define GB_A_IS_BITMAP 1
                #include "GB_bitmap_AxB_saxpy_A_bitmap_B_sparse_template.c"
            }
            else
            {
                // A is full, M is bitmap/full, B is sparse/hyper
                #undef  GB_A_IS_BITMAP
                #define GB_A_IS_BITMAP 0
                #include "GB_bitmap_AxB_saxpy_A_bitmap_B_sparse_template.c"
            }
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL
        }

        #undef GB_PANEL_SIZE
        #undef GB_A_IS_BITMAP

    }
    else if (A_is_sparse_or_hyper)
    {

        //-----------------------------------------------------
        // C                =               A     *     B
        //-----------------------------------------------------

        // bitmap           .               hyper       bitmap
        // bitmap           .               sparse      bitmap
        // bitmap           .               hyper       full 
        // bitmap           .               sparse      full

        //-----------------------------------------------------
        // C               <M>=             A     *     B
        //-----------------------------------------------------

        // bitmap           any             hyper       bitmap
        // bitmap           any             sparse      bitmap
        // bitmap           bitmap/full     hyper       full
        // bitmap           bitmap/full     sparse      full

        //-----------------------------------------------------
        // C               <!M>=            A     *     B
        //-----------------------------------------------------

        // bitmap           any             hyper       bitmap
        // bitmap           any             sparse      bitmap
        // bitmap           any             hyper       full 
        // bitmap           any             sparse      full

        ASSERT (GB_IS_BITMAP (B) || GB_IS_FULL (B)) ;
        double work = ((double) anz) * (double) bvdim ;
        int nthreads = GB_nthreads (work, chunk, nthreads_max) ;
        int nfine_tasks_per_vector = 0, ntasks ;
        bool use_coarse_tasks, use_atomics = false ;

        if (nthreads == 1 || bvdim == 0)
        { 
            // do the entire computation with a single thread, with coarse task
            ntasks = 1 ;
            use_coarse_tasks = true ;
            GBURBLE ("(coarse, threads: 1) ") ;
        }
        else if (nthreads <= bvdim)
        {
            // All tasks are coarse, and each coarse task does 1 or more
            // whole vectors of B
            ntasks = GB_IMIN (bvdim, 2 * nthreads) ;
            use_coarse_tasks = true ;
            GBURBLE ("(coarse, threads: %d, tasks %d) ", nthreads, ntasks) ;
        }
        else
        {
            // All tasks are fine.  Each task does a slice of a single vector
            // of B, and each vector of B is handled by the same # of fine
            // tasks.
            use_coarse_tasks = false ;

            // Select between a non-atomic method with Wf/Wx workspace,
            // and an atomic method with no workspace.
            double cnz = ((double) cvlen) * ((double) bvdim) ;
            double intensity = ((double) work) / fmax (cnz, 1) ;
            double workspace = ((double) cvlen) * ((double) nthreads) ;
            double relwspace = workspace / fmax (anz + bnz + cnz, 1) ;
            GBURBLE ("(fine, threads: %d, relwspace: %0.3g, intensity: %0.3g",
                nthreads, relwspace, intensity) ;
            if ((intensity > 64 && relwspace < 0.05) ||
                (intensity > 16 && intensity <= 64 && relwspace < 0.50))
            { 
                // non-atomic method with workspace
                use_atomics = false ;
                ntasks = nthreads ;
                GBURBLE (": non-atomic) ") ;
            }
            else
            { 
                // atomic method
                use_atomics = true ;
                ntasks = 2 * nthreads ;
                GBURBLE (": atomic) ") ;
            }

            nfine_tasks_per_vector = ceil ((double) ntasks / (double) bvdim) ;
            ntasks = bvdim * nfine_tasks_per_vector ;
            ASSERT (nfine_tasks_per_vector > 1) ;

            // slice the matrix A for each team of fine tasks
            GB_WERK_PUSH (A_slice, nfine_tasks_per_vector + 1, int64_t) ;
            if (A_slice == NULL)
            { 
                // out of memory
                GB_FREE_ALL ;
                return (GrB_OUT_OF_MEMORY) ;
            }
            GB_pslice (A_slice, Ap, anvec, nfine_tasks_per_vector, true) ;
        }

        if (M == NULL)
        { 

            //------------------------------------------------------------------
            // C = A*B, no mask, A sparse/hyper, B bitmap/full
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 0
            #define GB_MASK_IS_BITMAP_OR_FULL  0
            #undef  keep
            #define keep 1
            if (B_is_bitmap)
            {
                // A is sparse/hyper, B is bitmap, no mask
                #undef  GB_B_IS_BITMAP
                #define GB_B_IS_BITMAP 1
                #include "GB_bitmap_AxB_saxpy_A_sparse_B_bitmap_template.c"
            }
            else
            {
                // A is sparse/hyper, B is full, no mask
                #undef  GB_B_IS_BITMAP
                #define GB_B_IS_BITMAP 0
                #include "GB_bitmap_AxB_saxpy_A_sparse_B_bitmap_template.c"
            }
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL

        }
        else if (M_is_sparse_or_hyper)
        { 

            //------------------------------------------------------------------
            // C<M> or <!M> = A*B, M and A are sparse/hyper, B bitmap/full
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 1
            #define GB_MASK_IS_BITMAP_OR_FULL  0
            #undef  keep
            const int8_t keep = (Mask_comp) ? 1 : 3 ;
            if (B_is_bitmap)
            {
                // A is sparse/hyper, B is bitmap, M is sparse/hyper
                #undef  GB_B_IS_BITMAP
                #define GB_B_IS_BITMAP 1
                #include "GB_bitmap_AxB_saxpy_A_sparse_B_bitmap_template.c"
            }
            else
            {
                // A is sparse/hyper, B is full, no mask
                #undef  GB_B_IS_BITMAP
                #define GB_B_IS_BITMAP 0
                #include "GB_bitmap_AxB_saxpy_A_sparse_B_bitmap_template.c"
            }
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL

        }
        else
        { 

            //------------------------------------------------------------------
            // C<M> or <!M> = A*B, M bitmap, A sparse, B bitmap
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 0
            #define GB_MASK_IS_BITMAP_OR_FULL  1
            #undef  keep
            #define keep 1
            if (B_is_bitmap)
            {
                // A is sparse/hyper, B is bitmap, M is bitmap/full
                #undef  GB_B_IS_BITMAP
                #define GB_B_IS_BITMAP 1
                #include "GB_bitmap_AxB_saxpy_A_sparse_B_bitmap_template.c"
            }
            else
            {
                // A is sparse/hyper, B is full, M is bitmap/full
                #undef  GB_B_IS_BITMAP
                #define GB_B_IS_BITMAP 0
                #include "GB_bitmap_AxB_saxpy_A_sparse_B_bitmap_template.c"
            }
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL
        }

        #undef GB_B_IS_BITMAP

    }
    else
    {

        //-----------------------------------------------------
        // C                =               A     *     B
        //-----------------------------------------------------

        // bitmap           .               bitmap      bitmap
        // bitmap           .               full        bitmap
        // bitmap           .               bitmap      full
        // full             .               full        full

        //-----------------------------------------------------
        // C               <M>=             A     *     B
        //-----------------------------------------------------

        // bitmap           any             bitmap      bitmap
        // bitmap           any             full        bitmap
        // bitmap           bitmap/full     bitmap      full
        // bitmap           bitmap/full     full        full

        //-----------------------------------------------------
        // C               <!M>=            A     *     B
        //-----------------------------------------------------

        // bitmap           any             bitmap      bitmap
        // bitmap           any             full        bitmap
        // bitmap           any             bitmap      full
        // bitmap           any             full        full

        #define GB_TILE_SIZE 64
        #define GB_KTILE_SIZE 8

        double work = ((double) avlen) * ((double) bvlen) * ((double) bvdim) ;
        int nthreads = GB_nthreads (work, chunk, nthreads_max) ;
        int64_t nI_tasks = (bvdim == 0) ? 1 : (1 + (bvdim-1) / GB_TILE_SIZE) ;
        int64_t nJ_tasks = (avlen == 0) ? 1 : (1 + (avlen-1) / GB_TILE_SIZE) ;
        int64_t ntasks = nI_tasks * nJ_tasks ;

        if (M == NULL)
        { 

            //------------------------------------------------------------------
            // C = A*B, no mask, A and B bitmap/full
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 0
            #define GB_MASK_IS_BITMAP_OR_FULL  0
            #undef  keep
            #define keep 1
            #include "GB_bitmap_AxB_saxpy_A_bitmap_B_bitmap_template.c"
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL

        }
        else if (M_is_sparse_or_hyper)
        { 

            //------------------------------------------------------------------
            // C<M> or <!M> = A*B, M sparse/hyper, A and B bitmap/full
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 1
            #define GB_MASK_IS_BITMAP_OR_FULL  0
            #undef  keep
            const int8_t keep = (Mask_comp) ? 1 : 3 ;
            #include "GB_bitmap_AxB_saxpy_A_bitmap_B_bitmap_template.c"
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL

        }
        else
        { 

            //------------------------------------------------------------------
            // C<M> or <!M> = A*B, all matrices bitmap/full
            //------------------------------------------------------------------

            #define GB_MASK_IS_SPARSE_OR_HYPER 0
            #define GB_MASK_IS_BITMAP_OR_FULL  1
            #undef  keep
            #define keep 1
            #include "GB_bitmap_AxB_saxpy_A_bitmap_B_bitmap_template.c"
            #undef GB_MASK_IS_SPARSE_OR_HYPER
            #undef GB_MASK_IS_BITMAP_OR_FULL
        }
    }

    C->nvals = cnvals ;

    //--------------------------------------------------------------------------
    // if M is sparse, clear it from the C bitmap
    //--------------------------------------------------------------------------

    if (M_is_sparse_or_hyper)
    { 
        // Cb [pC] -= 2 for each entry M(i,j) in the mask
        GB_bitmap_M_scatter (C,
            NULL, 0, GB_ALL, NULL, NULL, 0, GB_ALL, NULL,
            M, Mask_struct, GB_ASSIGN, GB_BITMAP_M_SCATTER_MINUS_2,
            M_ek_slicing, M_ntasks, M_nthreads, Context) ;
    }

    //--------------------------------------------------------------------------
    // free workspace
    //--------------------------------------------------------------------------

    GB_FREE_ALL ;
}

#undef GB_FREE_ALL

