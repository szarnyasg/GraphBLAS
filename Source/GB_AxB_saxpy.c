//------------------------------------------------------------------------------
// GB_AxB_saxpy: compute C=A*B, C<M>=A*B, or C<!M>=A*B
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_mxm.h"
#include "GB_bitmap_AxB_saxpy.h"

//------------------------------------------------------------------------------
// GB_AxB_saxpy: compute C=A*B, C<M>=A*B, or C<!M>=A*B
//------------------------------------------------------------------------------

// TODO: pass in user's C and accum, and allow bitmap multiply to work in-place

GrB_Info GB_AxB_saxpy               // C = A*B using Gustavson/Hash/Bitmap
(
    GrB_Matrix C,                   // output, static header
    const GrB_Matrix M,             // optional mask matrix
    const bool Mask_comp,           // if true, use !M
    const bool Mask_struct,         // if true, use the only structure of M
    const GrB_Matrix A,             // input matrix A
    const GrB_Matrix B,             // input matrix B
    const GrB_Semiring semiring,    // semiring that defines C=A*B
    const bool flipxy,              // if true, do z=fmult(b,a) vs fmult(a,b)
    bool *mask_applied,             // if true, then mask was applied
    const GrB_Desc_Value AxB_method,
    const int do_sort,              // if nonzero, try to sort in saxpy3
    GB_Context Context
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    (*mask_applied) = false ;
    ASSERT (C != NULL && C->static_header) ;

    ASSERT_MATRIX_OK_OR_NULL (M, "M for saxpy A*B", GB0) ;
    ASSERT (!GB_PENDING (M)) ;
    ASSERT (GB_JUMBLED_OK (M)) ;
    ASSERT (!GB_ZOMBIES (M)) ;

    ASSERT_MATRIX_OK (A, "A for saxpy A*B", GB0) ;
    ASSERT (!GB_PENDING (A)) ;
    ASSERT (GB_JUMBLED_OK (A)) ;
    ASSERT (!GB_ZOMBIES (A)) ;

    ASSERT_MATRIX_OK (B, "B for saxpy A*B", GB0) ;
    ASSERT (!GB_PENDING (B)) ;
    ASSERT (GB_JUMBLED_OK (B)) ;
    ASSERT (!GB_ZOMBIES (B)) ;

    ASSERT_SEMIRING_OK (semiring, "semiring for saxpy A*B", GB0) ;
    ASSERT (A->vdim == B->vlen) ;

    //--------------------------------------------------------------------------
    // determine the sparsity of C
    //--------------------------------------------------------------------------

    GrB_Info info ;
    int C_sparsity, saxpy_method ;
    GB_AxB_saxpy_sparsity (&C_sparsity, &saxpy_method,
        M, Mask_comp, A, B, Context) ;

    if (M == NULL)
    {
        GBURBLE ("(%s=%s*%s) ",
            GB_sparsity_char (C_sparsity),
            GB_sparsity_char_matrix (A),
            GB_sparsity_char_matrix (B)) ;
    }
    else
    {
        GBURBLE ("(%s%s%s%s%s=%s*%s) ",
            GB_sparsity_char (C_sparsity),
            Mask_struct ? "{" : "<",
            Mask_comp ? "!" : "",
            GB_sparsity_char_matrix (M),
            Mask_struct ? "}" : ">",
            GB_sparsity_char_matrix (A),
            GB_sparsity_char_matrix (B)) ;
    }

    //--------------------------------------------------------------------------
    // select the method to use
    //--------------------------------------------------------------------------

    if (saxpy_method == GB_SAXPY_METHOD_3)
    {

        //----------------------------------------------------------------------
        // saxpy3: general-purpose Gustavson/Hash method, C is sparse/hyper
        //----------------------------------------------------------------------

        // C is sparse or hypersparse

        // This method allocates its own workspace, which is very small if the
        // Hash method is used.  The workspace for Gustavson's method is
        // larger, but saxpy3 selects that method only if the total work is
        // high enough so that the time to initialize the space.  C is sparse
        // or hypersparse.

        info = GB_AxB_saxpy3 (C, C_sparsity, M, Mask_comp, Mask_struct, A, B,
            semiring, flipxy, mask_applied, AxB_method, do_sort, Context) ;

        if (info == GrB_NO_VALUE)
        { 
            // The mask is present but has been discarded since it results in
            // too much work.  The analysis must be redone, which is done by
            // calling this function once again, recursively, without the mask.
            // GB_AxB_saxpy_sparsity will be called again, and it might choose
            // the bitmap method instead.  If saxpy3 is still chosen, this
            // results in a different analysis in GB_AxB_saxpy3, with no mask
            // present.  Otherwise, GB_bitmap_AxB_saxpy, below, is called.
            ASSERT (M != NULL) ;
            info = GB_AxB_saxpy (C, NULL, false, false, A, B, semiring,
                flipxy, mask_applied, AxB_method, do_sort, Context) ;
        }

    }
    else
    {

        //----------------------------------------------------------------------
        // bitmap method: C is bitmap or full
        //----------------------------------------------------------------------

        info = GB_bitmap_AxB_saxpy (C, C_sparsity, M, Mask_comp,
            Mask_struct, A, B, semiring, flipxy, mask_applied, Context) ;
    }

    return (info) ;
}

