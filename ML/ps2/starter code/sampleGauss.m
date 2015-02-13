%% Multivariate Gaussian Distribution

function S = sampleGauss(arg1, arg2, arg3)
% Returns n samples (in the rows) from a multivariate Gaussian distribution
% Examples:
% S = sampleGauss(mu, Sigma, 10)

switch nargin
    case 3
		mu = arg1; Sigma = arg2; n = arg3;
    otherwise
        error('bad num args');
end

% Initialize some useful values
S = zeros(n, length(mu));
A = chol(Sigma, 'lower');
Z = randn(length(mu), n);

% ====================== YOUR CODE HERE ======================
% Hint: use either 'repmat' or 'bsxfun'
% S = <<FILL THIS IN>>
% =============================================================

end
