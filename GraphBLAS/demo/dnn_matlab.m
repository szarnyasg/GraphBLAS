function Y = dnn_matlab (W, bias, Y0)
%DNN_MATLAB Sparse deep neural network in pure MATLAB
% Performs ReLU inference using input feature vector(s) Y0, DNN weights W,
% and bias vectors.
%
% Slightly revised from graphchallenge.org.
%
% Usage:
%
%   Y = dnn_matlab (W, bias, Y0)
%
% See also GrB.dnn, dnn_mat2gb.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
% SPDX-License-Identifier: GPL-3.0-or-later

Y = Y0 ;
for i=1:length(W)

    % Propagate through layer.
    Z = Y * W {i} ;

    % Apply bias to non-zero entries.
    Y = Z + (double(logical(Z)) .* bias {i}) ;

    % Threshold negative values.
    Y (Y < 0) = 0 ;

    % Threshold maximum values.
    Y (Y > 32) = 32 ;
end

