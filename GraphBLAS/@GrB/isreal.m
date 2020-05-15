function s = isreal (G)
%ISREAL true for real GraphBLAS matrices.
% isreal (G) is true for a GraphBLAS matrix G, unless it has a type of
% 'single complex' or 'double _Complex'.
%
% See also GrB/isnumeric, GrB/isfloat, GrB/isinteger, GrB/islogical,
% GrB.type, GrB/isa, GrB.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights
% Reserved. http://suitesparse.com.  See GraphBLAS/Doc/License.txt.

s = ~contains (gbtype (G.opaque), 'complex') ;

