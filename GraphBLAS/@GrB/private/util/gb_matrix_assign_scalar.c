//------------------------------------------------------------------------------
// gb_matrix_assign_scalar: assign scalar into a GraphBLAS matrix
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: GPL-3.0-or-later

//------------------------------------------------------------------------------

#include "gb_matlab.h"

#define OK2(method)                                         \
{                                                           \
    GrB_Info info = method ;                                \
    if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))    \
    {                                                       \
        ERROR ("GrB:error") ;                               \
    }                                                       \
}

// if do_subassign true:  GxB_Matrix_subassign_[TYPE]
// if do_subassign false: GrB_Matrix_assign_[TYPE]

// The input scalar is held as A(0,0) in a GrB_Matrix A.  If A(0,0) is not
// present in A, the value zero is used.

void gb_matrix_assign_scalar
(
    GrB_Matrix C,               // C can be of any type
    const GrB_Matrix M,
    const GrB_BinaryOp op,
    const GrB_Matrix A,         // the scalar is in A(0,0)
    const GrB_Index *I,
    const GrB_Index ni,
    const GrB_Index *J,
    const GrB_Index nj,
    const GrB_Descriptor d,
    bool do_subassign           // true: use GxB_subassign, false: GrB_assign
)
{

    GrB_Type atype ;
    OK (GxB_Matrix_type (&atype, A)) ;
    if (atype == GrB_BOOL)
    {
        bool x = false ;
        OK2 (GrB_Matrix_extractElement_BOOL (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_BOOL (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_BOOL (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_INT8)
    {
        int8_t x = 0 ;
        OK2 (GrB_Matrix_extractElement_INT8 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_INT8 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_INT8 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_INT16)
    {
        int16_t x = 0 ;
        OK2 (GrB_Matrix_extractElement_INT16 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK (GxB_Matrix_subassign_INT16 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_INT16 (C, M, op, x, I, ni, J, nj, d));
        }
    }
    else if (atype == GrB_INT32)
    {
        int32_t x = 0 ;
        OK2 (GrB_Matrix_extractElement_INT32 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_INT32 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_INT32 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_INT64)
    {
        int64_t x = 0 ;
        OK2 (GrB_Matrix_extractElement_INT64 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_INT64 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_INT64 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_UINT8)
    {
        uint8_t x = 0 ;
        OK2 (GrB_Matrix_extractElement_UINT8 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_UINT8 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_UINT8 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_UINT16)
    {
        uint16_t x = 0 ;
        OK2 (GrB_Matrix_extractElement_UINT16 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_UINT16 (C, M, op, x, I, ni, J, nj, d));
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_UINT16 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_UINT32)
    {
        uint32_t x = 0 ;
        OK2 (GrB_Matrix_extractElement_UINT32 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_UINT32 (C, M, op, x, I, ni, J, nj, d));
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_UINT32 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_UINT64)
    {
        uint64_t x = 0 ;
        OK2 (GrB_Matrix_extractElement_UINT64 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_UINT64 (C, M, op, x, I, ni, J, nj, d));
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_UINT64 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_FP32)
    {
        float x = 0 ;
        OK2 (GrB_Matrix_extractElement_FP32 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_FP32 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_FP32 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GrB_FP64)
    {
        double x = 0 ;
        OK2 (GrB_Matrix_extractElement_FP64 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_FP64 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GrB_Matrix_assign_FP64 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GxB_FC32)
    {
        GxB_FC32_t x = GxB_CMPLXF (0,0) ;
        OK2 (GxB_Matrix_extractElement_FC32 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_FC32 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GxB_Matrix_assign_FC32 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else if (atype == GxB_FC64)
    {
        GxB_FC64_t x = GxB_CMPLX (0,0) ;
        OK2 (GxB_Matrix_extractElement_FC64 (&x, A, 0, 0)) ;
        if (do_subassign)
        { 
            OK1 (C, GxB_Matrix_subassign_FC64 (C, M, op, x, I, ni, J, nj, d)) ;
        }
        else
        { 
            OK1 (C, GxB_Matrix_assign_FC64 (C, M, op, x, I, ni, J, nj, d)) ;
        }
    }
    else
    {
        ERROR ("unsupported type") ;
    }
}

