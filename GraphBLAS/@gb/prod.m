function C = prod (G, option)
%PROD Product of elements.
% C = prod (G), where G is an m-by-n GraphBLAS matrix, computes a 1-by-n
% row vector C where C(j) is the product of all entries in G(:,j).  If G
% is a row or column vector, then prod (G) is a scalar product of all the
% entries in the vector.
%
% C = prod (G,'all') takes the product of all elements of G to a single
% scalar.
%
% C = prod (G,1) is the default when G is a matrix, which is to take the
% product of each column, giving a 1-by-n row vector.  If G is already a
% row vector, then C = G.
%
% C = prod (G,2) takes the product of each row, resulting in an m-by-1
% column vector C where C(i) is the product of all entries in G(i,:).
%
% The MATLAB prod (A, ... type, nanflag) allows for different types of
% products to be computed, and the NaN behavior can be specified.  The
% GraphBLAS prod (G,...) uses only a type of 'native', and a nanflag of
% 'includenan'.  See 'help prod' for more details.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
% http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

[m n] = size (G) ;
d = struct ('in0', 'transpose') ;
if (isequal (gb.type (G), 'logical'))
    op = '&.logical' ;
else
    op = '*' ;
end
if (nargin == 1)
    % C = prod (G); check if G is a row vector
    if (isvector (G))
        % C = prod (G) for a vector G results in a scalar C
        if (gb.nvals (G) < m*n)
            C = gb (0, gb.type (G)) ;
        else
            C = gb.reduce (op, G) ;
        end
    else
        % C = prod (G) reduces each column to a scalar,
        % giving a 1-by-n row vector.
        C = (gb.vreduce (op, G, d) .* (gb.coldegree (G) == m))' ;
    end
elseif (isequal (option, 'all'))
    % C = prod (G, 'all'), reducing all entries to a scalar
    if (gb.nvals (G) < m*n)
        C = gb (0, gb.type (G)) ;
    else
        C = gb.reduce (op, G) ;
    end
elseif (isequal (option, 1))
    % C = prod (G,1) reduces each column to a scalar,
    % giving a 1-by-n row vector.
    C = (gb.vreduce (op, G, d) .* (gb.coldegree (G) == m))' ;
elseif (isequal (option, 2))
    % C = prod (G,2) reduces each row to a scalar,
    % giving an m-by-1 column vector.
    C = gb.vreduce (op, G) .* (gb.rowdegree (G) == n) ;
else
    error ('unknown option') ;
end
