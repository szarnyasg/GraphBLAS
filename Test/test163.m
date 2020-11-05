function test163
%TEST163 test C<!M>=A'*B where C and M are sparse

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
% http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

GrB.burble (1)
semiring.add = 'plus' ;
semiring.multiply = 'times' ;
semiring.class = 'double' ;

rng ('default') ;

n = 1000 ;
m = 10 ;
% n = 3 ;
% m = 3 ;
A = sprand (m, n, 0.1) ;
% A = sparse (rand (m,n)) ;
B = sprand (m, n, 0.1) ;
% B = sparse (rand (m,n)) ;
M = logical (sprand (n, n, 0.1)) ;
Cin = sparse (n, n) ;
dtn = struct ('inp0', 'tran', 'mask', 'complement', 'axb', 'dot') ;

C1 = double (~M) .* (A'*B) ;
C2 = GB_mex_mxm (Cin, M, [ ], semiring, A, B, dtn) ;
GB_spec_compare (C1, C2) ;

GrB.burble (0)