function g = sigmoid(z)
%SIGMOID Compute sigmoid functoon
%   J = SIGMOID(z) computes the sigmoid of z.

% You need to return the following variables correctly 
g = zeros(size(z));

% ====================== YOUR CODE HERE ======================
% Instructions: Compute the sigmoid of each value of z. (z can be a matrix,
%               vector or scalar).

% add by Chang Liu, for calculating th sigmoid function of each element
g = 1./(1+exp(-z));

% =============================================================

end
