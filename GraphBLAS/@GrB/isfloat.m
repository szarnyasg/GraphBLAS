function s = isfloat (G)
%ISFLOAT true for floating-point GraphBLAS matrices.
% isfloat (G) is true if the GraphBLAS matrix G has a type of 'double',
% 'single', 'single complex', or 'double _Complex'.
%
% See also GrB/isnumeric, GrB/isreal, GrB/isinteger, GrB/islogical,
% GrB.type, GrB/isa, GrB.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights
% Reserved. http://suitesparse.com.  See GraphBLAS/Doc/License.txt.

t = gbtype (G.opaque) ;
s = contains (t, 'double') || contains (t, 'single') ;

