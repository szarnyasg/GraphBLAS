//------------------------------------------------------------------------------
// GB_extractTuples: extract all the tuples from a matrix
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// Extracts all tuples from a matrix, like [I,J,X] = find (A).  If any
// parameter I, J and/or X is NULL, then that component is not extracted.  The
// size of the I, J, and X arrays (those that are not NULL) is given by nvals,
// which must be at least as large as GrB_nvals (&nvals, A).  The values in the
// matrix are typecasted to the type of X, as needed.

// This function is not user-callable.  It does the work for the user-callable
// GrB_*_extractTuples functions.

#include "GB.h"

#define GB_FREE_ALL ;

GrB_Info GB_extractTuples       // extract all tuples from a matrix
(
    GrB_Index *I_out,           // array for returning row indices of tuples
    GrB_Index *J_out,           // array for returning col indices of tuples
    void *X,                    // array for returning values of tuples
    GrB_Index *p_nvals,         // I,J,X size on input; # tuples on output
    const GB_Type_code xcode,   // type of array X
    const GrB_Matrix A,         // matrix to extract tuples from
    GB_Context Context
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GrB_Info info ;
    ASSERT_MATRIX_OK (A, "A to extract", GB0) ;
    ASSERT (!GB_IS_BITMAP (A)) ;        // TODO
    ASSERT (p_nvals != NULL) ;

    // delete any lingering zombies and assemble any pending tuples
    GB_MATRIX_WAIT (A) ;        // TODO: allow A to be jumbled

    GB_BURBLE_DENSE (A, "(A %s) ") ;
    ASSERT (xcode <= GB_UDT_code) ;

    // xcode and A must be compatible
    if (!GB_code_compatible (xcode, A->type->code))
    { 
        return (GrB_DOMAIN_MISMATCH) ;
    }

    int64_t anz = GB_NNZ (A) ;

    if (anz == 0)
    { 
        // no work to do
        (*p_nvals) = 0 ;
        return (GrB_SUCCESS) ;
    }

    int64_t nvals = *p_nvals ;          // size of I,J,X on input

    if (nvals < anz && (I_out != NULL || J_out != NULL || X != NULL))
    { 
        // output arrays are not big enough
        return (GrB_INSUFFICIENT_SPACE) ;
    }

    //--------------------------------------------------------------------------
    // determine the number of threads to use
    //--------------------------------------------------------------------------

    GB_GET_NTHREADS_MAX (nthreads_max, chunk, Context) ;
    int nthreads = GB_nthreads (anz + A->nvec, chunk, nthreads_max) ;

    //-------------------------------------------------------------------------
    // handle the CSR/CSC format
    //--------------------------------------------------------------------------

    GrB_Index *I, *J ;
    if (A->is_csc)
    { 
        I = I_out ;
        J = J_out ;
    }
    else
    { 
        I = J_out ;
        J = I_out ;
    }

    //--------------------------------------------------------------------------
    // extract the row indices
    //--------------------------------------------------------------------------

    if (I != NULL)
    { 
        if (A->i == NULL)
        { 
            // A is full; construct the row indices
            int64_t avlen = A->vlen ;
            int64_t p ;
            #pragma omp parallel for num_threads(nthreads) schedule(static)
            for (p = 0 ; p < anz ; p++)
            {
                I [p] = (p % avlen) ;
            }
        }
        else
        { 
            GB_memcpy (I, A->i, anz * sizeof (int64_t), nthreads) ;
        }
    }

    //--------------------------------------------------------------------------
    // extract the column indices
    //--------------------------------------------------------------------------

    if (J != NULL)
    {
        if (!GB_extract_vector_list ((int64_t *) J, A, nthreads))
        { 
            // out of memory
            return (GrB_OUT_OF_MEMORY) ;
        }
    }

    //--------------------------------------------------------------------------
    // extract the values
    //--------------------------------------------------------------------------

    if (X != NULL)
    { 
        // typecast or copy the values from A into X
        GB_cast_array ((GB_void *) X, xcode,
            (GB_void *) A->x, A->type->code, A->type->size, anz, nthreads) ;
    }

    //--------------------------------------------------------------------------
    // return the number of tuples extracted
    //--------------------------------------------------------------------------

    *p_nvals = anz ;            // number of tuples extracted

    return (GrB_SUCCESS) ;
}

