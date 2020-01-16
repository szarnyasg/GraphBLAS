//------------------------------------------------------------------------------
// GB_AxB:  hard-coded functions for semiring: C<M>=A*B or A'*B
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// If this file is in the Generated/ folder, do not edit it (auto-generated).

#include "GB.h"
#ifndef GBCOMPACT
#include "GB_control.h"
#include "GB_ek_slice.h"
#include "GB_bracket.h"
#include "GB_iterator.h"
#include "GB_sort.h"
#include "GB_saxpy3.h"
#include "GB_AxB__include.h"

// The C=A*B semiring is defined by the following types and operators:

// A'*B function (dot2):     GB_Adot2B__any_land_uint16
// A'*B function (dot3):     GB_Adot3B__any_land_uint16
// C+=A'*B function (dot4):  GB_Adot4B__any_land_uint16
// A*B function (saxpy3):    GB_Asaxpy3B__any_land_uint16

// C type:   uint16_t
// A type:   uint16_t
// B type:   uint16_t

// Multiply: z = ((aik != 0) && (bkj != 0))
// Add:      cij = z
//           atomic?        1
//           OpenMP atomic? 0
// MultAdd:  uint16_t x_op_y = ((aik != 0) && (bkj != 0)) ; cij = x_op_y
// Identity: 0
// Terminal: break ;

#define GB_ATYPE \
    uint16_t

#define GB_BTYPE \
    uint16_t

#define GB_CTYPE \
    uint16_t

// aik = Ax [pA]
#define GB_GETA(aik,Ax,pA) \
    uint16_t aik = Ax [pA]

// bkj = Bx [pB]
#define GB_GETB(bkj,Bx,pB) \
    uint16_t bkj = Bx [pB]

#define GB_CX(p) Cx [p]

// multiply operator
#define GB_MULT(z, x, y) \
    z = ((x != 0) && (y != 0))

// multiply-add
#define GB_MULTADD(z, x, y) \
    uint16_t x_op_y = ((x != 0) && (y != 0)) ; z = x_op_y

// monoid identity value
#define GB_IDENTITY \
    0

// break if cij reaches the terminal value (dot product only)
#define GB_DOT_TERMINAL(cij) \
    break ;

// simd pragma for dot product
#define GB_DOT_SIMD \
    ;

// declare the cij scalar
#define GB_CIJ_DECLARE(cij) \
    uint16_t cij

// save the value of C(i,j)
#define GB_CIJ_SAVE(cij,p) Cx [p] = cij

// cij = Cx [pC]
#define GB_GETC(cij,pC) \
    cij = Cx [pC]

// Cx [pC] = cij
#define GB_PUTC(cij,pC) \
    Cx [pC] = cij

// For saxpy3:

// Cx [p] = t
#define GB_CIJ_WRITE(p,t) Cx [p] = t

// C(i,j) += t
#define GB_CIJ_UPDATE(p,t) \
    Cx [p] = t

// Cx [p] = Hx [i]
#define GB_CIJ_GATHER(p,i) Cx [p] = Hx [i]

// Hx [i] += t
#define GB_HX_UPDATE(i,t) \
    Hx [i] = t

// x + y
#define GB_ADD_FUNCTION(x,y) \
    y

// type with size of GB_CTYPE, and can be used in compare-and-swap
#define GB_CTYPE_PUN \
    uint16_t

// Hx [i] = t
#define GB_HX_WRITE(i,t) Hx [i] = t

// 1 if monoid update can be done atomically, 0 otherwise
#define GB_HAS_ATOMIC \
    1

// 1 if monoid update can be done with a #pragma omp atomic update, 0 otherwise
#define GB_HAS_OMP_ATOMIC \
    0

// memcpy (&(Cx [p]), &(Hx [i]), len)
#define GB_CIJ_MEMCPY(p,i,len) \
    memcpy (Cx +(p), Hx +(i), (len) * sizeof(uint16_t))

// disable this semiring and use the generic case if these conditions hold
#define GB_DISABLE \
    (GxB_NO_ANY || GxB_NO_LAND || GxB_NO_UINT16 || GxB_NO_ANY_UINT16 || GxB_NO_LAND_UINT16 || GxB_NO_ANY_LAND_UINT16)

//------------------------------------------------------------------------------
// C=A'*B or C<!M>=A'*B: dot product (phase 2)
//------------------------------------------------------------------------------

GrB_Info GB_Adot2B__any_land_uint16
(
    GrB_Matrix C,
    const GrB_Matrix M,
    const GrB_Matrix *Aslice, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    int64_t *GB_RESTRICT B_slice,
    int64_t *GB_RESTRICT *C_counts,
    int nthreads, int naslice, int nbslice
)
{ 
    // C<M>=A'*B now uses dot3
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #define GB_PHASE_2_OF_2
    #include "GB_AxB_dot2_meta.c"
    #undef GB_PHASE_2_OF_2
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// C<M>=A'*B: masked dot product method (phase 2)
//------------------------------------------------------------------------------

GrB_Info GB_Adot3B__any_land_uint16
(
    GrB_Matrix C,
    const GrB_Matrix M,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    const GB_task_struct *GB_RESTRICT TaskList,
    const int ntasks,
    const int nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_AxB_dot3_template.c"
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// C+=A'*B: dense dot product
//------------------------------------------------------------------------------

GrB_Info GB_Adot4B__any_land_uint16
(
    GrB_Matrix C,
    const GrB_Matrix A, bool A_is_pattern,
    int64_t *GB_RESTRICT A_slice, int naslice,
    const GrB_Matrix B, bool B_is_pattern,
    int64_t *GB_RESTRICT B_slice, int nbslice,
    const int nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_AxB_dot4_template.c"
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// C=A*B, C<M>=A*B, C<!M>=A*B: saxpy3 method (Gustavson + Hash)
//------------------------------------------------------------------------------

GrB_Info GB_Asaxpy3B__any_land_uint16
(
    GrB_Matrix *Chandle,
    const GrB_Matrix M, bool Mask_comp,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    GB_saxpy3task_struct *GB_RESTRICT *TaskList_handle,
    void *Work [3], size_t Worksize [3],
    const int ntasks,
    const int nfine,
    const int nthreads,
    GB_Context Context
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    // get copies of these pointers for GB_FREE_ALL
    GB_saxpy3task_struct *GB_RESTRICT TaskList = (*TaskList_handle) ;
    GrB_Matrix C = (*Chandle) ;
    int64_t *Hi_all = Work [0] ;
    int64_t *Hf_all = Work [1] ;
    GB_void *Hx_all = Work [2] ;
    size_t Hi_size_total = Worksize [0] ;
    size_t Hf_size_total = Worksize [1] ;
    size_t Hx_size_total = Worksize [2] ;
    GB_GET_NTHREADS_MAX (nthreads_max, chunk, Context) ;
    #include "GB_AxB_saxpy3_template.c"
    return (GrB_SUCCESS) ;
    #endif
}

#endif
