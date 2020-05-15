function C = atanh (G)
%ATANH inverse hyperbolic tangent.
% C = atanh (G) computes the inverse hyberbolic tangent of each entry of a
% GraphBLAS matrix G.  C is complex if G is complex, or if any (abs(G)>1).
%
% See also GrB/tan, GrB/atan, GrB/tanh, GrB/atan2.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights
% Reserved. http://suitesparse.com.  See GraphBLAS/Doc/License.txt.

if (isreal (G))
    if (any (abs (G) > 1, 'all'))
        if (isequal (GrB.type (G), 'single'))
            G = GrB (G, 'single complex') ;
        else
            G = GrB (G, 'double _Complex') ;
        end
    elseif (~isfloat (G))
        G = GrB (G, 'double') ;
    end
end

C = GrB.apply ('atanh', G) ;

