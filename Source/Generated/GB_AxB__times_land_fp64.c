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
#include "GB_atomics.h"
#include "GB_AxB_saxpy3.h"
#include "GB_AxB__include.h"

// The C=A*B semiring is defined by the following types and operators:

// A'*B function (dot2):     GB_Adot2B__times_land_fp64
// A'*B function (dot3):     GB_Adot3B__times_land_fp64
// C+=A'*B function (dot4):  GB_Adot4B__times_land_fp64
// A*B function (saxpy3):    GB_Asaxpy3B__times_land_fp64

// C type:   double
// A type:   double
// B type:   double

// Multiply: z = ((aik != 0) && (bkj != 0))
// Add:      cij *= z
//           'any' monoid?  0
//           atomic?        1
//           OpenMP atomic? 1
// MultAdd:  cij *= ((aik != 0) && (bkj != 0))
// Identity: 1
// Terminal: ;

#define GB_ATYPE \
    double

#define GB_BTYPE \
    double

#define GB_CTYPE \
    double

// aik = Ax [pA]
#define GB_GETA(aik,Ax,pA) \
    double aik = Ax [pA]

// bkj = Bx [pB]
#define GB_GETB(bkj,Bx,pB) \
    double bkj = Bx [pB]

#define GB_CX(p) Cx [p]

// multiply operator
#define GB_MULT(z, x, y) \
    z = ((x != 0) && (y != 0))

// multiply-add
#define GB_MULTADD(z, x, y) \
    z *= ((x != 0) && (y != 0))

// monoid identity value
#define GB_IDENTITY \
    1

// break if cij reaches the terminal value (dot product only)
#define GB_DOT_TERMINAL(cij) \
    ;

// simd pragma for dot-product loop vectorization
#define GB_PRAGMA_VECTORIZE_DOT \
    GB_PRAGMA_SIMD

// simd pragma for other loop vectorization
#define GB_PRAGMA_VECTORIZE GB_PRAGMA_SIMD

// declare the cij scalar
#define GB_CIJ_DECLARE(cij) \
    double cij

// save the value of C(i,j)
#define GB_CIJ_SAVE(cij,p) Cx [p] = cij

// cij = Cx [pC]
#define GB_GETC(cij,pC) \
    cij = Cx [pC]

// Cx [pC] = cij
#define GB_PUTC(cij,pC) \
    Cx [pC] = cij

// Cx [p] = t
#define GB_CIJ_WRITE(p,t) Cx [p] = t

// C(i,j) += t
#define GB_CIJ_UPDATE(p,t) \
    Cx [p] *= t

// x + y
#define GB_ADD_FUNCTION(x,y) \
    x * y

// type with size of GB_CTYPE, and can be used in compare-and-swap
#define GB_CTYPE_PUN \
    uint64_t

// bit pattern for bool, 8-bit, 16-bit, and 32-bit integers
#define GB_CTYPE_BITS \
    0

// 1 if monoid update can skipped entirely (the ANY monoid)
#define GB_IS_ANY_MONOID \
    0

// 1 if monoid update is EQ
#define GB_IS_EQ_MONOID \
    0

// 1 if monoid update can be done atomically, 0 otherwise
#define GB_HAS_ATOMIC \
    1

// 1 if monoid update can be done with an OpenMP atomic update, 0 otherwise
#define GB_HAS_OMP_ATOMIC \
    1

// 1 for the ANY_PAIR semirings
#define GB_IS_ANY_PAIR_SEMIRING \
    0

// 1 if PAIR is the multiply operator 
#define GB_IS_PAIR_MULTIPLIER \
    0

// atomic compare-exchange
#define GB_ATOMIC_COMPARE_EXCHANGE(target, expected, desired) \
    GB_ATOMIC_COMPARE_EXCHANGE_64 (target, expected, desired)

#if GB_IS_ANY_PAIR_SEMIRING

    // result is purely symbolic; no numeric work to do.  Hx is not used.
    #define GB_HX_WRITE(i,t)
    #define GB_CIJ_GATHER(p,i)
    #define GB_HX_UPDATE(i,t)
    #define GB_CIJ_MEMCPY(p,i,len)

#else

    // Hx [i] = t
    #define GB_HX_WRITE(i,t) Hx [i] = t

    // Cx [p] = Hx [i]
    #define GB_CIJ_GATHER(p,i) Cx [p] = Hx [i]

    // Hx [i] += t
    #define GB_HX_UPDATE(i,t) \
        Hx [i] *= t

    // memcpy (&(Cx [p]), &(Hx [i]), len)
    #define GB_CIJ_MEMCPY(p,i,len) \
        memcpy (Cx +(p), Hx +(i), (len) * sizeof(double))

#endif

// disable this semiring and use the generic case if these conditions hold
#define GB_DISABLE \
    (GxB_NO_TIMES || GxB_NO_LAND || GxB_NO_FP64 || GxB_NO_TIMES_FP64 || GxB_NO_LAND_FP64 || GxB_NO_TIMES_LAND_FP64)

//------------------------------------------------------------------------------
// C=A'*B or C<!M>=A'*B: dot product (phase 2)
//------------------------------------------------------------------------------

GrB_Info GB_Adot2B__times_land_fp64
(
    GrB_Matrix C,
    const GrB_Matrix M, const bool Mask_struct,
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

GrB_Info GB_Adot3B__times_land_fp64
(
    GrB_Matrix C,
    const GrB_Matrix M, const bool Mask_struct,
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

GrB_Info GB_Adot4B__times_land_fp64
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

#include "GB_AxB_saxpy3_template.h"

GrB_Info GB_Asaxpy3B__times_land_fp64
(
    GrB_Matrix C,
    const GrB_Matrix M, bool Mask_comp, const bool Mask_struct,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    GB_saxpy3task_struct *GB_RESTRICT TaskList,
    const int ntasks,
    const int nfine,
    const int nthreads,
    GB_Context Context
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_AxB_saxpy3_template.c"
    return (GrB_SUCCESS) ;
    #endif
}

#endif

