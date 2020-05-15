function ctype = optype (a, b)
%GRB.OPTYPE determine the default type of a binary operator.
% ctype = GrB.optype (a, b) returns a string that defines the
% default type of operator to use for two inputs A and B, of type
% atype and btype, respectively.  The input a can be either the
% matrix A or the string atype = GrB.type (A), and likewise for b.
%
% The rules are listed below; the first
% one that applies is used:
%
% (1) same:
%
%   if A and B have the same type:  optype is the type of A and B.
%
% (2) any logical:
%
%   if A or B are logical: optype is from the other operand (B or A)
%
% (3) both integer:
%
%   if A and B are both integers (with ka and kb bits, respectively):
%       optype is signed if either A or B are signed, and the optype has
%       max(ka,kb) bits.  For example, uint32*int8 uses an int32 optype.
%
% (4) mixing integer and floating-point:
%
%   if one operand is any integer, and the other is any floating-point
%       (single, double, single complex, or double _Complex): optype has
%       the floating-point type of the other operand.
%
% (5) both floating-point:
%
%   if A or B are single: optype is from the other operand.
%   if A or B are double: if the other is single complex or double
%       complex, optype is double _Complex; otherwise optype is double.
%   if A or B are single complex:  if the other operand is double or
%       double _Complex, optype is double _Complex; otherwise it is single
%       complex.
%   if A or B are double _Complex: optype is double _Complex.
%
% Example:
%
%   GrB.optype ('uint32', 'int8')
%   GrB.optype (uint32 (magic (4)), rand (4))
%
% See also GrB.binopinfo, GrB.semiringinfo.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights
% Reserved. http://suitesparse.com.  See GraphBLAS/Doc/License.txt.

if (ischar (a)) 
    atype = a ;
else
    atype = GrB.type (a) ;
end

if (ischar (b)) 
    btype = b ;
else
    btype = GrB.type (b) ;
end

ctype = gboptype (atype, btype) ;

