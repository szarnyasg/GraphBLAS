//------------------------------------------------------------------------------
// gb_export: export a GrB_Matrix as a MATLAB matrix or GraphBLAS struct
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// mxArray pargout [0] = gb_export (&C, kind) ; exports C as a MATLAB matrix
// and frees the remaining content of C.

#include "gb_matlab.h"

mxArray *gb_export              // return the exported MATLAB matrix or struct
(
    GrB_Matrix *C_handle,       // GrB_Matrix to export and free
    kind_enum_t kind            // GrB, sparse, or full
)
{

    // TODO:: add an option to export as a MATLAB matrix, but keep the matrix
    // structure: full if C is dense or full, sparse otherwise.

    if (kind == KIND_SPARSE)
    { 

        //----------------------------------------------------------------------
        // export C as a MATLAB sparse matrix
        //----------------------------------------------------------------------

        // Typecast to double, if C is integer (int8, ..., uint64)

        return (gb_export_to_mxsparse (C_handle)) ;

    }
    else if (kind == KIND_FULL)
    { 

        //----------------------------------------------------------------------
        // export C as a MATLAB dense matrix
        //----------------------------------------------------------------------

        // No typecasting is needed since MATLAB supports all the same types.

        // ensure nvals(C) is equal to nrows*ncols
        GrB_Index nrows, ncols, nvals ;
        OK (GrB_Matrix_nvals (&nvals, *C_handle)) ;
        OK (GrB_Matrix_nrows (&nrows, *C_handle)) ;
        OK (GrB_Matrix_ncols (&ncols, *C_handle)) ;
        CHECK_ERROR ((double) nrows * (double) ncols != (double) nvals,
            "matrix must be full to export as full matrix") ;

        void *Cx ;
        GrB_Type ctype ;
        GrB_Index Cx_size ;
        OK (GxB_Matrix_export_FullC (C_handle, &ctype, &nrows, &ncols,
            &Cx, &Cx_size, NULL)) ;

        return (gb_export_to_mxfull (&Cx, nrows, ncols, ctype)) ;

    }
    else
    { 

        //----------------------------------------------------------------------
        // export C as a MATLAB struct containing a verbatim GrB_Matrix
        //----------------------------------------------------------------------

        // No typecasting is needed since the MATLAB struct can hold all of the
        // opaque content of the GrB_Matrix.

        return (gb_export_to_mxstruct (C_handle)) ;
    }
}

