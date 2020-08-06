//------------------------------------------------------------------------------
// GB_convert_any_to_hyper: convert any matrix to hypersparse
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// OK: BITMAP

#include "GB.h"
#define GB_FREE_ALL GB_phbix_free (A) ;

GrB_Info GB_convert_any_to_hyper // convert to hypersparse
(
    GrB_Matrix A,           // matrix to convert to hypersparse
    GB_Context Context
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GrB_Info info ;
    ASSERT_MATRIX_OK (A, "A being converted to hyper", GB0) ;
    ASSERT (GB_ZOMBIES_OK (A)) ;
    ASSERT (GB_JUMBLED_OK (A)) ;
    ASSERT (GB_PENDING_OK (A)) ;

    //--------------------------------------------------------------------------
    // convert A to hypersparse
    //--------------------------------------------------------------------------

    if (A->h != NULL)
    { 
        // already hypersparse, nothing to do
        ;
    }
    else if (GB_IS_FULL (A))
    { 
        // convert from full to hypersparse
        GB_OK (GB_convert_full_to_sparse (A, Context)) ;
        GB_OK (GB_convert_sparse_to_hyper (A, Context)) ;
    }
    else if (GB_IS_BITMAP (A))
    { 
        // convert from bitmap to hypersparse
        GB_OK (GB_convert_bitmap_to_sparse (A, Context)) ;
        GB_OK (GB_convert_sparse_to_hyper (A, Context)) ;
    }
    else
    { 
        // convert from sparse to hypersparse
        GB_OK (GB_convert_sparse_to_hyper (A, Context)) ;
    }

    //--------------------------------------------------------------------------
    // return result
    //--------------------------------------------------------------------------

    ASSERT_MATRIX_OK (A, "A to hypersparse", GB0) ;
    ASSERT (A->h != NULL) ;
    return (GrB_SUCCESS) ;
}

