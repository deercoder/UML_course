function [w, accuracy ] = lda(X, y)
% Linear Discriminant Analysis

% get the pos and neg data
pos_data = %YOUR CODE HERE
neg_data = %YOUR CODE HERE

% Mean of each class
pos_mean = %YOUR CODE HERE
neg_mean = %YOUR CODE HERE

% Center the data
pos_data = %YOUR CODE HERE
neg_data = %YOUR CODE HERE

% Covariance of the data
cov_all = %YOUR CODE HERE

% Get w and training accuracy
w = %YOUR CODE HERE
accuracy = %YOUR CODE HERE


% Plot Gaussian Ellipsoids
h_pos = plot_gaussian_ellipsoid(pos_mean, cov_all);
h_neg = plot_gaussian_ellipsoid(neg_mean, cov_all);
set(h_pos,'color','r');
set(h_neg,'color','g');
end

