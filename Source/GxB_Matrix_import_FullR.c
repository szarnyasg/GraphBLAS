//------------------------------------------------------------------------------
// GxB_Matrix_import_FullR: import a matrix in full format, held by row
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_export.h"

GrB_Info GxB_Matrix_import_FullR  // import a full matrix, held by row
(
    GrB_Matrix *A,      // handle of matrix to create
    GrB_Type type,      // type of matrix to create
    GrB_Index nrows,    // number of rows of the matrix
    GrB_Index ncols,    // number of columns of the matrix

    void **Ax,          // values
    GrB_Index Ax_size,  // size of Ax in bytes
    bool is_uniform,    // if true, A has uniform values (TODO:::unsupported)

    const GrB_Descriptor desc
)
{ 

    //--------------------------------------------------------------------------
    // check inputs and get the descriptor
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GxB_Matrix_import_FullR (&A, type, nrows, ncols, "
        "&Ax, Ax_size, is_uniform, desc)") ;
    GB_BURBLE_START ("GxB_Matrix_import_FullR") ;
    GB_GET_DESCRIPTOR (info, desc, xx1, xx2, xx3, xx4, xx5, xx6, xx7) ;

    //--------------------------------------------------------------------------
    // import the matrix
    //--------------------------------------------------------------------------

    info = GB_import (A, type, ncols, nrows, false,
        NULL, 0,        // Ap
        NULL, 0,        // Ah
        NULL, 0,        // Ab
        NULL, 0,        // Ai
        Ax,   Ax_size,  // Ax
        0, false, 0,
        GxB_FULL, false,                    // full by row
        is_uniform, Context) ;              // full by row

    GB_BURBLE_END ;
    return (info) ;
}

