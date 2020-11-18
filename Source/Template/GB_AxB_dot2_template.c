//------------------------------------------------------------------------------
// GB_AxB_dot2_template:  C=A'B, C<!M>=A'*B, or C<M>=A'*B via dot products
//------------------------------------------------------------------------------

// TODO: rename GB_bitmap_AxB_dot_template.c

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

{

    //--------------------------------------------------------------------------
    // C=A'*B, C<M>=A'*B, or C<!M>=A'*B where C is bitmap
    //--------------------------------------------------------------------------

    int tid ;
    #pragma omp parallel for num_threads(nthreads) schedule(dynamic,1) \
        reduction(+:cnvals)
    for (tid = 0 ; tid < ntasks ; tid++)
    {

        //----------------------------------------------------------------------
        // get the task descriptor
        //----------------------------------------------------------------------

        int a_tid = tid / nbslice ;
        int b_tid = tid % nbslice ;
        int64_t kA_start = A_slice [a_tid] ;
        int64_t kA_end   = A_slice [a_tid+1] ;
        int64_t kB_start = B_slice [b_tid] ;
        int64_t kB_end   = B_slice [b_tid+1] ;

        //----------------------------------------------------------------------
        // C=A'*B, C<M>=A'*B, or C<!M>=A'*B via dot products
        //----------------------------------------------------------------------

        for (int64_t kB = kB_start ; kB < kB_end ; kB++)
        {

            //------------------------------------------------------------------
            // get B(:,j)
            //------------------------------------------------------------------

            #if GB_B_IS_SPARSE_OR_HYPER
            // B is sparse or hypersparse
            int64_t j = GBH (Bh, kB) ;
            int64_t pB_start = Bp [kB] ;
            int64_t pB_end   = Bp [kB+1] ;
            int64_t bjnz = pB_end - pB_start ;
                #if ( GB_A_IS_SPARSE_OR_HYPER )
                // get the first and last index in B(:,j)
                int64_t ib_first = Bi [pB_start] ;
                int64_t ib_last  = Bi [pB_end-1] ;
                #endif
            #else
            // B is bitmap or full
            int64_t j = kB ;
            int64_t pB_start = kB * vlen ;
            int64_t bjnz = vlen ;
            #endif
            // no work to do if B(:,j) is empty
            if (bjnz == 0) continue ;

            //------------------------------------------------------------------
            // get C(:,j)
            //------------------------------------------------------------------

            int64_t pC_start = j * cvlen ;

            //------------------------------------------------------------------
            // get M(:,j), if present
            //------------------------------------------------------------------

            #if defined ( GB_ANY_FIRSTJ_SPECIALIZED )
            // M is bitmap, and pM is the same as pC_start
            #elif defined ( GB_MASK_IS_PRESENT )
            // TODO: delete this and scatter M into the C bitmap if sparse,
            // or use in-place is M is dense, bitmap, or full
            // find vector j in M
            int64_t pM, pM_end ;
            bool mdense = false ;           // TODO remove this
            if (M_is_bitmap_or_full)
            {
                // M is bitmap or full
                pM = pC_start ;
            }
            else
            {
                // M is hypersparse or sparse
                int64_t mpleft = 0 ;
                GB_lookup (M_is_hyper, Mh, Mp, mvlen, &mpleft, mnvec-1, j,
                    &pM, &pM_end) ;
                int64_t mjnz = pM_end - pM ;
                mdense = (mjnz == mvlen) ;  // TODO remove this
            }
            #endif

            //------------------------------------------------------------------
            // C(:,j)<#M(:,j)> = A'*B(:,j), or C(:,j) = A'*B(:,j) if no mask
            //------------------------------------------------------------------

            for (int64_t kA = kA_start ; kA < kA_end ; kA++)
            {

                //--------------------------------------------------------------
                // get A(:,i)
                //--------------------------------------------------------------

                #if defined ( GB_ANY_FIRSTJ_SPECIALIZED )
                // A is sparse
                int64_t i = kA ;
                #elif GB_A_IS_SPARSE_OR_HYPER
                int64_t i = GBH (Ah, kA) ;
                #else
                // A is bitmap or full
                int64_t i = kA ;
                #endif

                //--------------------------------------------------------------
                // get M(i,j)
                //--------------------------------------------------------------

                #if defined ( GB_ANY_FIRSTJ_SPECIALIZED )
                // M is bitmap and structural; Mask_comp either true or false
                if (Mb [pC_start + i] ^ Mask_comp)
                #elif defined ( GB_MASK_IS_PRESENT )
                bool mij ;
                if (M_is_bitmap_or_full || mdense)
                { 
                    // M(:,j) is dense: M is full, bitmap, or M sparse/hyper,
                    // with a fully-populated vector M(:,j)
                    mij = GBB (Mb, pM + i) && GB_mcast (Mx, pM + i, msize) ;
                }
                else
                {
                    // M(:,j) is sparse:
                    // TODO: delete this and scatter M into the C bitmap
                    // instead.
                    bool found ;
                    int64_t pright = pM_end - 1 ;
                    GB_BINARY_SEARCH (i, Mi, pM, pright, found) ;
                    mij = found && GB_mcast (Mx, pM, msize) ;
                }
                if (mij ^ Mask_comp)
                #endif
                { 

                    //----------------------------------------------------------
                    // C(i,j) = A(:,i)'*B(:,j)
                    //----------------------------------------------------------

                    #if GB_A_IS_SPARSE_OR_HYPER
                    int64_t pA     = Ap [kA] ;
                    int64_t pA_end = Ap [kA+1] ;
                    int64_t ainz = pA_end - pA ;
                    if (ainz == 0) continue ;
                    #else
                    int64_t pA     = (kA  ) * vlen ;
                    int64_t pA_end = (kA+1) * vlen ;
                    #endif
                    #include "GB_AxB_dot2_cij.c"
                }
            }
        }
    }
}

#undef GB_A_IS_SPARSE_OR_HYPER
#undef GB_A_IS_BITMAP
#undef GB_A_IS_FULL
#undef GB_B_IS_SPARSE_OR_HYPER
#undef GB_B_IS_BITMAP
#undef GB_B_IS_FULL

