//------------------------------------------------------------------------------
// GxB_Scalar_fprint: print and check a GxB_Scalar object
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

#include "GB.h"

GrB_Info GxB_Scalar_fprint          // print and check a GxB_Scalar
(
    GxB_Scalar s,                   // object to print and check
    const char *name,               // name of the object
    GxB_Print_Level pr,             // print level
    FILE *f                         // file for output
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE ("GxB_Scalar_fprint (s, name, pr, f)") ;

    //--------------------------------------------------------------------------
    // print and check the object
    //--------------------------------------------------------------------------

    GrB_Info info = GB_Scalar_check (s, name, pr, f, Context) ;

    //--------------------------------------------------------------------------
    // return result
    //--------------------------------------------------------------------------

    if (info == GrB_INDEX_OUT_OF_BOUNDS)
    { 
        return (GB_ERROR (GrB_INVALID_OBJECT, (GB_LOG,
            "scalar invalid [%s]", GB_NAME))) ;
    }
    else
    { 
        return (info) ;
    }
}
