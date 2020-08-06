//------------------------------------------------------------------------------
// GB_subassign_14: C(I,J)<!M> = A ; using S
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// Method 14: C(I,J)<!M> = A ; using S

// M:           present
// Mask_comp:   true
// C_replace:   false
// accum:       NULL
// A:           matrix
// S:           constructed

// FULL TODO: kernel: C(I,J)<!M>=A when A and C are both dense

#include "GB_subassign_methods.h"

GrB_Info GB_subassign_14
(
    GrB_Matrix C,
    // input:
    const GrB_Index *I,
    const int64_t nI,
    const int Ikind,
    const int64_t Icolon [3],
    const GrB_Index *J,
    const int64_t nJ,
    const int Jkind,
    const int64_t Jcolon [3],
    const GrB_Matrix M,
    const bool Mask_struct,         // if true, use the only structure of M
    const GrB_Matrix A,
    const GrB_Matrix S,
    GB_Context Context
)
{

    //--------------------------------------------------------------------------
    // get inputs
    //--------------------------------------------------------------------------

    ASSERT (!GB_IS_BITMAP (C)) ;        // TODO
    ASSERT (!GB_IS_BITMAP (M)) ;        // TODO
    ASSERT (!GB_IS_BITMAP (A)) ;        // TODO
    ASSERT (!GB_IS_BITMAP (S)) ;        // TODO

    GB_EMPTY_TASKLIST ;
    ASSERT (!GB_JUMBLED (C)) ;
    GB_MATRIX_WAIT_IF_JUMBLED (S) ;
    GB_MATRIX_WAIT_IF_JUMBLED (M) ;
    GB_MATRIX_WAIT_IF_JUMBLED (A) ;

    GB_ENSURE_SPARSE (C) ;
    GB_GET_C ;
    GB_GET_MASK ;
    const bool M_is_hyper = (Mh != NULL) ;
    const int64_t Mnvec = M->nvec ;
    GB_GET_A ;
    GB_GET_S ;
    GrB_BinaryOp accum = NULL ;

    //--------------------------------------------------------------------------
    // Method 14: C(I,J)<!M> = A ; using S
    //--------------------------------------------------------------------------

    // Time: Close to optimal.  Omega(nnz(S)+nnz(A)) is required, and the
    // sparsity of !M cannot be exploited.  The time taken is
    // O((nnz(A)+nnz(S))*log(m)) where m is the # of entries in a vector of M.

    // Method 06s and 14 are very similar.

    //--------------------------------------------------------------------------
    // Parallel: Z=A+S (Methods 02, 04, 09, 10, 11, 12, 14, 16, 18, 20)
    //--------------------------------------------------------------------------

    GB_SUBASSIGN_TWO_SLICE (A, S) ;

    //--------------------------------------------------------------------------
    // phase 1: create zombies, update entries, and count pending tuples
    //--------------------------------------------------------------------------

    int taskid ;
    #pragma omp parallel for num_threads(nthreads) schedule(dynamic,1) \
        reduction(+:nzombies)
    for (taskid = 0 ; taskid < ntasks ; taskid++)
    {

        //----------------------------------------------------------------------
        // get the task descriptor
        //----------------------------------------------------------------------

        GB_GET_TASK_DESCRIPTOR_PHASE1 ;

        //----------------------------------------------------------------------
        // compute all vectors in this task
        //----------------------------------------------------------------------

        for (int64_t k = kfirst ; k <= klast ; k++)
        {

            //------------------------------------------------------------------
            // get A(:,j) and S(:,j)
            //------------------------------------------------------------------

            int64_t j = GBH (Zh, k) ;
            GB_GET_MAPPED (pA, pA_end, pA, pA_end, Ap, j, k, Z_to_X, Avlen) ;
            GB_GET_MAPPED (pS, pS_end, pB, pB_end, Sp, j, k, Z_to_S, Svlen) ;

            //------------------------------------------------------------------
            // get M(:,j)
            //------------------------------------------------------------------

            int64_t pM_start, pM_end ;
            GB_VECTOR_LOOKUP (pM_start, pM_end, M, j) ;
            bool mjdense = (pM_end - pM_start) == Mvlen ;

            //------------------------------------------------------------------
            // do a 2-way merge of S(:,j) and A(:,j)
            //------------------------------------------------------------------

            // jC = J [j] ; or J is a colon expression
            // int64_t jC = GB_ijlist (J, j, Jkind, Jcolon) ;

            // while both list S (:,j) and A (:,j) have entries
            while (pS < pS_end && pA < pA_end)
            {
                int64_t iS = GBI (Si, pS, Svlen) ;
                int64_t iA = GBI (Ai, pA, Avlen) ;

                if (iS < iA)
                {
                    // S (i,j) is present but A (i,j) is not
                    GB_MIJ_BINARY_SEARCH_OR_DENSE_LOOKUP (iS) ;
                    mij = !mij ;
                    if (mij)
                    { 
                        // ----[C . 1] or [X . 1]-------------------------------
                        // [C . 1]: action: ( delete ): becomes zombie
                        // [X . 1]: action: ( X ): still zombie
                        GB_C_S_LOOKUP ;
                        GB_DELETE_ENTRY ;
                    }
                    GB_NEXT (S) ;
                }
                else if (iA < iS)
                {
                    // S (i,j) is not present, A (i,j) is present
                    GB_MIJ_BINARY_SEARCH_OR_DENSE_LOOKUP (iA) ;
                    mij = !mij ;
                    if (mij)
                    { 
                        // ----[. A 1]------------------------------------------
                        // [. A 1]: action: ( insert )
                        task_pending++ ;
                    }
                    GB_NEXT (A) ;
                }
                else
                {
                    // both S (i,j) and A (i,j) present
                    GB_MIJ_BINARY_SEARCH_OR_DENSE_LOOKUP (iA) ;
                    mij = !mij ;
                    if (mij)
                    { 
                        // ----[C A 1] or [X A 1]-------------------------------
                        // [C A 1]: action: ( =A ): A to C no accum
                        // [X A 1]: action: ( undelete ): zombie lives
                        GB_C_S_LOOKUP ;
                        GB_noaccum_C_A_1_matrix ;
                    }
                    GB_NEXT (S) ;
                    GB_NEXT (A) ;
                }
            }

            // while list S (:,j) has entries.  List A (:,j) exhausted
            while (pS < pS_end)
            {
                // S (i,j) is present but A (i,j) is not
                int64_t iS = GBI (Si, pS, Svlen) ;
                GB_MIJ_BINARY_SEARCH_OR_DENSE_LOOKUP (iS) ;
                mij = !mij ;
                if (mij)
                { 
                    // ----[C . 1] or [X . 1]-----------------------------------
                    // [C . 1]: action: ( delete ): becomes zombie
                    // [X . 1]: action: ( X ): still zombie
                    GB_C_S_LOOKUP ;
                    GB_DELETE_ENTRY ;
                }
                GB_NEXT (S) ;
            }

            // while list A (:,j) has entries.  List S (:,j) exhausted
            while (pA < pA_end)
            {
                // S (i,j) is not present, A (i,j) is present
                int64_t iA = GBI (Ai, pA, Avlen) ;
                GB_MIJ_BINARY_SEARCH_OR_DENSE_LOOKUP (iA) ;
                mij = !mij ;
                if (mij)
                { 
                    // ----[. A 1]----------------------------------------------
                    // [. A 1]: action: ( insert )
                    task_pending++ ;
                }
                GB_NEXT (A) ;
            }
        }

        GB_PHASE1_TASK_WRAPUP ;
    }

    //--------------------------------------------------------------------------
    // phase 2: insert pending tuples
    //--------------------------------------------------------------------------

    GB_PENDING_CUMSUM ;

    #pragma omp parallel for num_threads(nthreads) schedule(dynamic,1) \
        reduction(&&:pending_sorted)
    for (taskid = 0 ; taskid < ntasks ; taskid++)
    {

        //----------------------------------------------------------------------
        // get the task descriptor
        //----------------------------------------------------------------------

        GB_GET_TASK_DESCRIPTOR_PHASE2 ;

        //----------------------------------------------------------------------
        // compute all vectors in this task
        //----------------------------------------------------------------------

        for (int64_t k = kfirst ; k <= klast ; k++)
        {

            //------------------------------------------------------------------
            // get A(:,j) and S(:,j)
            //------------------------------------------------------------------

            int64_t j = GBH (Zh, k) ;
            GB_GET_MAPPED (pA, pA_end, pA, pA_end, Ap, j, k, Z_to_X, Avlen) ;
            GB_GET_MAPPED (pS, pS_end, pB, pB_end, Sp, j, k, Z_to_S, Svlen) ;

            //------------------------------------------------------------------
            // get M(:,j)
            //------------------------------------------------------------------

            int64_t pM_start, pM_end ;
            GB_VECTOR_LOOKUP (pM_start, pM_end, M, j) ;
            bool mjdense = (pM_end - pM_start) == Mvlen ;

            //------------------------------------------------------------------
            // do a 2-way merge of S(:,j) and A(:,j)
            //------------------------------------------------------------------

            // jC = J [j] ; or J is a colon expression
            int64_t jC = GB_ijlist (J, j, Jkind, Jcolon) ;

            // while both list S (:,j) and A (:,j) have entries
            while (pS < pS_end && pA < pA_end)
            {
                int64_t iS = GBI (Si, pS, Svlen) ;
                int64_t iA = GBI (Ai, pA, Avlen) ;

                if (iS < iA)
                { 
                    // S (i,j) is present but A (i,j) is not
                    GB_NEXT (S) ;
                }
                else if (iA < iS)
                {
                    // S (i,j) is not present, A (i,j) is present
                    GB_MIJ_BINARY_SEARCH_OR_DENSE_LOOKUP (iA) ;
                    mij = !mij ;
                    if (mij)
                    { 
                        // ----[. A 1]------------------------------------------
                        // [. A 1]: action: ( insert )
                        int64_t iC = GB_ijlist (I, iA, Ikind, Icolon) ;
                        GB_PENDING_INSERT (Ax +(pA*asize)) ;
                    }
                    GB_NEXT (A) ;
                }
                else
                { 
                    // both S (i,j) and A (i,j) present
                    GB_NEXT (S) ;
                    GB_NEXT (A) ;
                }
            }

            // while list A (:,j) has entries.  List S (:,j) exhausted
            while (pA < pA_end)
            {
                // S (i,j) is not present, A (i,j) is present
                int64_t iA = GBI (Ai, pA, Avlen) ;
                GB_MIJ_BINARY_SEARCH_OR_DENSE_LOOKUP (iA) ;
                mij = !mij ;
                if (mij)
                { 
                    // ----[. A 1]----------------------------------------------
                    // [. A 1]: action: ( insert )
                    int64_t iC = GB_ijlist (I, iA, Ikind, Icolon) ;
                    GB_PENDING_INSERT (Ax +(pA*asize)) ;
                }
                GB_NEXT (A) ;
            }
        }

        GB_PHASE2_TASK_WRAPUP ;
    }

    //--------------------------------------------------------------------------
    // finalize the matrix and return result
    //--------------------------------------------------------------------------

    GB_SUBASSIGN_WRAPUP ;
}

