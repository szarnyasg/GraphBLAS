function codegen_sel_method (opname, func, atype, kind)
%CODEGEN_SEL_METHOD create a selection function, C = select (A,thunk)
%
% codegen_sel_method (opname, func, atype, kind)

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
% SPDX-License-Identifier: Apache-2.0

f = fopen ('control.m4', 'w') ;

[aname, unsigned, ~] = codegen_type (atype) ;

if (nargin < 4)
    kind = 'GB_ENTRY_SELECTOR';
end
name = sprintf ('%s_%s', opname, aname) ;

is_nonzombie = (isequal (opname, 'nonzombie') && ~isequal (atype, 'GB_void')) ;

% function names
if (is_nonzombie)
    fprintf (f, 'define(`_sel_phase1'', `_sel_phase1__(none)'')\n') ;
else
    fprintf (f, 'define(`_sel_phase1'', `_sel_phase1__%s'')\n', name) ;
end
fprintf (f, 'define(`_sel_phase2'', `_sel_phase2__%s'')\n', name) ;

if isequal (opname, 'nonzombie') || isequal (opname, 'resize') 
    fprintf (f, 'define(`_sel_bitmap'', `_sel_bitmap__(none)'')\n') ;
    fprintf (f, 'define(`if_bitmap'', `#if 0'')\n') ;
    fprintf (f, 'define(`endif_bitmap'', `#endif'')\n') ;
else
    fprintf (f, 'define(`_sel_bitmap'', `_sel_bitmap__%s'')\n', name) ;
    fprintf (f, 'define(`if_bitmap'', `'')\n') ;
    fprintf (f, 'define(`endif_bitmap'', `'')\n') ;
end

% the type of A (no typecasting)
fprintf (f, 'define(`GB_atype'', `%s'')\n', atype) ;

% create the operator to test the numerical values of the entries
if (isempty (func))
    fprintf (f, 'define(`GB_test_value_of_entry'', `(no test; %s ignores values)'')\n', opname) ;
else
    fprintf (f, 'define(`GB_test_value_of_entry'', `%s'')\n', func) ;
end

fprintf (f, 'define(`GB_kind'', `#define %s'')\n', kind) ;

% get vector index for user-defined select operator
if (isequal (opname, 'user'))
    fprintf (f, 'define(`GB_get_j'', `int64_t j = GBH (Ah, k)'')\n') ;
else
    fprintf (f, 'define(`GB_get_j'', `;'')\n') ;
end

% get scalar thunk
if (~isequal (atype, 'GB_void') && ~isempty (strfind (opname, 'thunk')))
    fprintf (f, 'define(`GB_get_thunk'', `%s thunk = (*xthunk) ;'')\n', atype) ;
else
    fprintf (f, 'define(`GB_get_thunk'', `;'')\n') ;
end

% enable phase1
if (is_nonzombie)
    % nonzombie: phase1 uses a single worker: GB_sel_phase1__nonzombie_any
    fprintf (f, 'define(`if_phase1'', `#if 0'')\n') ;
    fprintf (f, 'define(`endif_phase1'', `#endif'')\n') ;
else
    fprintf (f, 'define(`if_phase1'', `'')\n') ;
    fprintf (f, 'define(`endif_phase1'', `'')\n') ;
end

% for phase2: copy the numerical value of the entry
if (isequal (opname, 'eq_zero'))
    fprintf (f, 'define(`GB_select_entry'', `/* assignment skipped, Cx already all zero */'')\n') ;
elseif (isequal (opname, 'eq_thunk'))
    if (isequal (atype, 'GB_void'))
        fprintf (f, 'define(`GB_select_entry'', `memcpy (Cx +((pC)*asize), xthunk, asize)'')\n') ;
    else
        fprintf (f, 'define(`GB_select_entry'', `Cx [pC] = thunk'')\n') ;
    end
else
    if (isequal (opname, 'nonzero') && isequal (atype, 'bool'))
        fprintf (f, 'define(`GB_select_entry'', `Cx [pC] = true'')\n') ;
    elseif (isequal (atype, 'GB_void'))
        fprintf (f, 'define(`GB_select_entry'', `memcpy (Cx +((pC)*asize), Ax +((pA)*asize), asize)'')\n') ;
    else
        fprintf (f, 'define(`GB_select_entry'', `Cx [pC] = Ax [pA]'')\n') ;
    end
end

fclose (f) ;

% construct the *.c file
cmd = sprintf (...
'cat control.m4 Generator/GB_sel.c | m4 | tail -n +14 > Generated/GB_sel__%s.c', ...
name) ;
fprintf ('.') ;
system (cmd) ;

% append to the *.h file
cmd = sprintf (...
'cat control.m4 Generator/GB_sel.h | m4 | tail -n +14 >> Generated/GB_sel__include.h') ;
system (cmd) ;

delete ('control.m4') ;


