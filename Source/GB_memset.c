//------------------------------------------------------------------------------
// GB_memset: parallel memset
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// Note that this function uses its own hard-coded chunk size.

// OK: no change for BITMAP

#include "GB.h"

#define GB_MEM_CHUNK (1024*1024)

void GB_memset                  // parallel memset
(
    void *dest,                 // destination
    const int c,                // value to to set
    size_t n,                   // # of bytes to set
    int nthreads                // # of threads to use
)
{

    if (nthreads <= 1 || n <= GB_MEM_CHUNK)
    { 

        //----------------------------------------------------------------------
        // memset using a single thread
        //----------------------------------------------------------------------

        memset (dest, c, n) ;
    }
    else
    {

        //----------------------------------------------------------------------
        // memset using a multiple threads
        //----------------------------------------------------------------------

        size_t nchunks = 1 + (n / GB_MEM_CHUNK) ;
        if (((size_t) nthreads) > nchunks)
        { 
            nthreads = (int) nchunks ;
        }
        GB_void *pdest = (GB_void *) dest ;

        int64_t k ;
        #pragma omp parallel for num_threads(nthreads) schedule(dynamic,1)
        for (k = 0 ; k < nchunks ; k++)
        {
            size_t start = k * GB_MEM_CHUNK ;
            if (start < n)
            { 
                size_t chunk = GB_IMIN (n - start, GB_MEM_CHUNK) ;
                memset (pdest + start, c, chunk) ;
            }
        }
    }
}

