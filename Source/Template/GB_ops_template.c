//------------------------------------------------------------------------------
// GB_ops_template.c: built-in unary and binary functions and operators
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// This file is #include'd many times in GB_ops.c to define the built-in unary
// and binary operators.  In that file, GB_TYPE is a built-in C type (bool,
// int8_t, uint64_t, double, etc) for the inputs x and y, and GB(x) is the
// corresponding macro that creates the function name (GB_*_BOOL, GB_*_INT8,
// etc).

//------------------------------------------------------------------------------
// unary functions z=f(x) where z and x have the same type
//------------------------------------------------------------------------------

GB_OP1 (GxB_, ONE, "one") ;

#if defined ( GB_COMPLEX )

    // complex types
    GB_OP1 (GxB_, IDENTITY , "identity" ) ;
    GB_OP1 (GxB_, AINV     , "ainv"     ) ;
    GB_OP1 (GxB_, MINV     , "minv"     ) ;

#else

    // real types
    GB_OP1 (GrB_, IDENTITY , "identity" ) ;
    GB_OP1 (GrB_, AINV     , "ainv"     ) ;
    GB_OP1 (GrB_, MINV     , "minv"     ) ;

    // z=abs(x), z and x have the same type (now in v1.3 spec)
    GB_OP1 (GrB_, ABS      , "abs"      ) ;

    // GxB_ABS_* is now GrB_ABS_* (but keep the old GxB_ABS_* name as well)
    GB_OP1_RENAME (GxB_, GrB_, ABS) ;

    // LNOT is only defined for real types, not complex
    GB_OP1 (GxB_, LNOT     , "not"      ) ;

#endif

#if defined ( GB_FLOATING_POINT )

    GB_OP1 (GxB_, SQRT     , "sqrt"     ) ;
    GB_OP1 (GxB_, LOG      , "log"      ) ;
    GB_OP1 (GxB_, EXP      , "exp"      ) ;

    GB_OP1 (GxB_, SIN      , "sin"      ) ;
    GB_OP1 (GxB_, COS      , "cos"      ) ;
    GB_OP1 (GxB_, TAN      , "tan"      ) ;

    GB_OP1 (GxB_, ASIN     , "asin"     ) ;
    GB_OP1 (GxB_, ACOS     , "acos"     ) ;
    GB_OP1 (GxB_, ATAN     , "atan"     ) ;

    GB_OP1 (GxB_, SINH     , "sinh"     ) ;
    GB_OP1 (GxB_, COSH     , "cosh"     ) ;
    GB_OP1 (GxB_, TANH     , "tanh"     ) ;

    GB_OP1 (GxB_, ASINH    , "asinh"    ) ;
    GB_OP1 (GxB_, ACOSH    , "acosh"    ) ;
    GB_OP1 (GxB_, ATANH    , "atanh"    ) ;

    GB_OP1 (GxB_, CEIL     , "ceil"     ) ;
    GB_OP1 (GxB_, FLOOR    , "floor"    ) ;
    GB_OP1 (GxB_, ROUND    , "round"    ) ;
    GB_OP1 (GxB_, TRUNC    , "trunc"    ) ;

    GB_OP1 (GxB_, EXP2     , "exp2"     ) ;
    GB_OP1 (GxB_, EXPM1    , "expm1"    ) ;
    GB_OP1 (GxB_, LOG10    , "log10"    ) ;
    GB_OP1 (GxB_, LOG1P    , "log1p"    ) ;
    GB_OP1 (GxB_, LOG2     , "log2"     ) ;

    #if defined ( GB_COMPLEX )
    // complex only
    GB_OP1 (GxB_, CONJ     , "conj"     ) ;
    #else
    // real only
    GB_OP1 (GxB_, LGAMMA   , "lgamma"   ) ;
    GB_OP1 (GxB_, TGAMMA   , "tgamma"   ) ;
    GB_OP1 (GxB_, ERF      , "erf"      ) ;
    GB_OP1 (GxB_, ERFC     , "erfc"     ) ;
    GB_OP1 (GxB_, FREXPX   , "frexpx"   ) ;
    GB_OP1 (GxB_, FREXPE   , "frexpe"   ) ;
    #endif

#endif

#if defined ( GB_SIGNED_INT ) || defined ( GB_UNSIGNED_INT )

    // bitwise complement, added for v1.3 spec
    GB_OP1 (GrB_, BNOT     , "bnot"     ) ;

#endif

//------------------------------------------------------------------------------
// unary functions z=f(x) where z and x can have different types
//------------------------------------------------------------------------------

#if defined ( GB_FLOAT )

    // z = f(x) where x is float, and z is bool
    GB_OP1z (GxB_, ISINF     , "isinf"     , bool   , GrB_BOOL ) ;
    GB_OP1z (GxB_, ISNAN     , "isnan"     , bool   , GrB_BOOL ) ;
    GB_OP1z (GxB_, ISFINITE  , "isfinite"  , bool   , GrB_BOOL ) ;

#elif defined ( GB_DOUBLE )

    // z = f(x) where x is double, and z is bool
    GB_OP1z (GxB_, ISINF     , "isinf"     , bool   , GrB_BOOL ) ;
    GB_OP1z (GxB_, ISNAN     , "isnan"     , bool   , GrB_BOOL ) ;
    GB_OP1z (GxB_, ISFINITE  , "isfinite"  , bool   , GrB_BOOL ) ;

#elif defined ( GB_FLOAT_COMPLEX )

    // z = f(x) where x is float complex, and the type of z is listed below:
    GB_OP1z (GxB_, ABS       , "abs"       , float  , GrB_FP32) ;
    GB_OP1z (GxB_, ISINF     , "isinf"     , bool   , GrB_BOOL) ;
    GB_OP1z (GxB_, ISNAN     , "isnan"     , bool   , GrB_BOOL) ;
    GB_OP1z (GxB_, ISFINITE  , "isfinite"  , bool   , GrB_BOOL ) ;

    GB_OP1z (GxB_, CREAL     , "creal"     , float  , GrB_FP32) ;
    GB_OP1z (GxB_, CIMAG     , "cimag"     , float  , GrB_FP32) ;
    GB_OP1z (GxB_, CARG      , "carg"      , float  , GrB_FP32) ;

#elif defined ( GB_DOUBLE_COMPLEX )

    // z = f(x) where x is double _Complex, and the type of z is listed below:
    GB_OP1z (GxB_, ABS       , "abs"       , double , GrB_FP64) ;
    GB_OP1z (GxB_, ISINF     , "isinf"     , bool   , GrB_BOOL) ;
    GB_OP1z (GxB_, ISNAN     , "isnan"     , bool   , GrB_BOOL) ;
    GB_OP1z (GxB_, ISFINITE  , "isfinite"  , bool   , GrB_BOOL ) ;

    GB_OP1z (GxB_, CREAL     , "creal"     , double , GrB_FP64) ;
    GB_OP1z (GxB_, CIMAG     , "cimag"     , double , GrB_FP64) ;
    GB_OP1z (GxB_, CARG      , "carg"      , double , GrB_FP64) ;

#endif

//------------------------------------------------------------------------------
// binary functions z=f(x,y) where z, x, and y all have the same type
//------------------------------------------------------------------------------

GB_OP2 (GxB_, RMINUS , "rminus")
GB_OP2 (GxB_, RDIV   , "rdiv"  )
GB_OP2 (GxB_, PAIR   , "pair"  )
GB_OP2 (GxB_, ANY    , "any"   )
GB_OP2 (GxB_, ISEQ   , "iseq"  )
GB_OP2 (GxB_, ISNE   , "isne"  )
GB_OP2 (GxB_, POW    , "pow"   )

#if defined ( GB_COMPLEX ) 

    // complex types
    GB_OP2 (GxB_, FIRST  , "first" )
    GB_OP2 (GxB_, SECOND , "second")
    GB_OP2 (GxB_, PLUS   , "plus"  )
    GB_OP2 (GxB_, MINUS  , "minus" )
    GB_OP2 (GxB_, TIMES  , "times" )
    GB_OP2 (GxB_, DIV    , "div"   )

#else

    // real types
    GB_OP2 (GrB_, FIRST  , "first" )
    GB_OP2 (GrB_, SECOND , "second")
    GB_OP2 (GrB_, PLUS   , "plus"  )
    GB_OP2 (GrB_, MINUS  , "minus" )
    GB_OP2 (GrB_, TIMES  , "times" )
    GB_OP2 (GrB_, DIV    , "div"   )

    GB_OP2 (GrB_, MIN    , "min" )
    GB_OP2 (GrB_, MAX    , "max" )

    GB_OP2 (GxB_, LOR    , "or"  )
    GB_OP2 (GxB_, LAND   , "and" )
    GB_OP2 (GxB_, LXOR   , "xor" )

    GB_OP2 (GxB_, ISGT   , "isgt")
    GB_OP2 (GxB_, ISLT   , "islt")
    GB_OP2 (GxB_, ISGE   , "isge")
    GB_OP2 (GxB_, ISLE   , "isle")

#endif

#if defined (GB_FLOAT) || defined (GB_DOUBLE)

    // these operators are only defined for float and double
    GB_OP2 (GxB_, ATAN2    , "atan2"    )
    GB_OP2 (GxB_, HYPOT    , "hypot"    )
    GB_OP2 (GxB_, FMOD     , "fmod"     )
    GB_OP2 (GxB_, REMAINDER, "remainder")
    GB_OP2 (GxB_, COPYSIGN , "copysign" )
    GB_OP2 (GxB_, LDEXP    , "ldexp"    )

#endif

#if defined ( GB_SIGNED_INT ) || defined ( GB_UNSIGNED_INT )

    // bitwise binary operators, added for v1.3 spec.  For integer only (int*,
    // and uint*), not for logical or floating-point types.
    GB_OP2 (GrB_, BOR      , "bitwise_or"   ) ;
    GB_OP2 (GrB_, BAND     , "bitwise_and"  ) ;
    GB_OP2 (GrB_, BXOR     , "bitwise_xor"  ) ;
    GB_OP2 (GrB_, BXNOR    , "bitwise_xnor" ) ;

    GB_OP2 (GxB_, BGET     , "bitwise_get"   ) ;
    GB_OP2 (GxB_, BSET     , "bitwise_set"   ) ;
    GB_OP2 (GxB_, BCLR     , "bitwise_clear" ) ;

    GB_OP2shift (GxB_, BSHIFT, "bitwise_shift") ;

#endif

//------------------------------------------------------------------------------
// binary functions z=f(x,y) where z, x, and y can have different types
//------------------------------------------------------------------------------

#if defined ( GB_FLOAT )

    // z = cmplx(x,y) where z is float complex, x and y are float
    GB_OP2z (GxB_, CMPLX, "cmplx", GxB_FC32_t, GxB_FC32)

#endif

#if defined ( GB_DOUBLE )

    // z = cmplx(x,y) where z is double _Complex, x and y are double
    GB_OP2z (GxB_, CMPLX, "cmplx", GxB_FC64_t, GxB_FC64)

#endif

#if defined ( GB_COMPLEX )

    // complex types
    GB_OP2z (GxB_, EQ, "eq", bool, GrB_BOOL)
    GB_OP2z (GxB_, NE, "ne", bool, GrB_BOOL)

#else

    // real types
    GB_OP2z (GrB_, EQ, "eq", bool, GrB_BOOL)
    GB_OP2z (GrB_, NE, "ne", bool, GrB_BOOL)
    GB_OP2z (GrB_, GT, "gt", bool, GrB_BOOL)
    GB_OP2z (GrB_, LT, "lt", bool, GrB_BOOL)
    GB_OP2z (GrB_, LE, "le", bool, GrB_BOOL)
    GB_OP2z (GrB_, GE, "ge", bool, GrB_BOOL)

#endif

//------------------------------------------------------------------------------
// unary typecast operators, used in GB_cast_factory.c
//------------------------------------------------------------------------------

// z = cast (x) where ztype is given by GB_TYPE, and xtype is listed below
extern void GB_CAST_NAME (bool    ) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (int8_t  ) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (uint8_t ) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (int16_t ) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (uint16_t) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (int32_t ) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (uint32_t) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (int64_t ) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (uint64_t) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (float   ) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (double  ) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (GxB_FC32_t) (void *z, const void *x, size_t s) ;
extern void GB_CAST_NAME (GxB_FC64_t) (void *z, const void *x, size_t s) ;

//------------------------------------------------------------------------------
// clear macros for next use of this file
//------------------------------------------------------------------------------

#undef GB
#undef GB_TYPE
#undef GB_XTYPE
#undef GrB_NAME
#undef GB_CAST_NAME
#undef GB_FLOATING_POINT
#undef GB_COMPLEX
#undef GB_FLOAT
#undef GB_DOUBLE
#undef GB_FLOAT_COMPLEX
#undef GB_DOUBLE_COMPLEX
#undef GB_SIGNED_INT
#undef GB_UNSIGNED_INT

