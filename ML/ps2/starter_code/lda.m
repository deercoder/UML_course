function [w, accuracy ] = lda(X, y)
% Linear Discriminant Analysis

% get the pos and neg data
pos_data = X(logical(y==1),:);
neg_data = X(logical(y==0),:);

% Mean of each class
pos_mean = mean(pos_data);
neg_mean = mean(neg_data);

% Center the data
pos_data = pos_data' - pos_mean' * ones(1, size(pos_data,1));
neg_data = neg_data' - neg_mean' * ones(1, size(neg_data,1));

% Covariance of the data
all_data = [pos_data, neg_data];
cov_all = cov(all_data');

% Get w and training accuracy
w = cov_all\(neg_mean - pos_mean);
accuracy = predict(X - pos_mean - neg_mean);

% Plot Gaussian Ellipsoids
h_pos = plot_gaussian_ellipsoid(pos_mean, cov_all);
h_neg = plot_gaussian_ellipsoid(neg_mean, cov_all);
set(h_pos,'color','r');
set(h_neg,'color','g');
end

