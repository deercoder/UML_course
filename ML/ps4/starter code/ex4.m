%% Exercise 4: Boosting
%
%  Instructions
%  ------------
% 
%  This file contains code that helps you get started on the
%  boosting exercise. You will need to complete the following functions 
%  in this exericse:
%
%     Adaboost.m
%     errorE.m
%     distributionW.m
%     weakerLearnerC.m
%     plotDecisionBoundary.m
%     distSample.m
%
%  For this exercise, you will not need to change any code in this file,
%  or any other files other than those mentioned above.
%

%% Initialization
clear ; close all; clc

%% Load Data

load('XORTraining.mat');

X = patterns'; y = targets';

%% ==================== Part 1: Visualizing the Data ==================

fprintf(['Plotting data with + indicating (y = 1) examples and o ' ...
         'indicating (y = 0) examples.\n']);

plotData(X, y);

fprintf('\nProgram paused. Press enter to continue.\n');
pause;

%% ==================== AdaBoost ============================
fprintf('\nRunning AdaBoost, set the Kmax to 20, 50, 100.\n');
fprintf('\nProgram paused. Press enter to continue.\n');
pause;

tic
x1_min = min(patterns(1,:));
x1_max = max(patterns(1,:));
x2_min = min(patterns(2,:));
x2_max = max(patterns(2,:));
x1 = linspace(x1_min, x1_max, sqrt(length(patterns)));
x2 = linspace(x2_min, x2_max, sqrt(length(patterns)));
[p,q] = meshgrid(x1, x2);
test_patterns = [p(:) q(:)]';
[test_targets, E] = AdaBoost(patterns, targets, test_patterns, 100, 'weakLearnerC', []);
disp(E);
plotData(test_patterns', test_targets');
toc

fprintf('\nProgram paused. Press enter to continue.\n');
pause;

%% ==================== Smiley Face ============================
clear ; close all; clc

%% Load and Plot Data
fprintf(['Plotting data\n']);
fprintf('\nProgram paused. Press enter to continue.\n');

load('smile_10_percent.mat');
X = patterns';
y = targets';
plotData(X, y);

pause;

fprintf('\nRunning AdaBoost against the smile face.\n');
fprintf('\nProgram paused. Press enter to continue.\n');

tic
x1_min = min(patterns(1,:));
x1_max = max(patterns(1,:));
x2_min = min(patterns(2,:));
x2_max = max(patterns(2,:));
x1 = linspace(x1_min, x1_max, 100);
x2 = linspace(x2_min, x2_max, 100);
[p,q] = meshgrid(x1, x2);
test_patterns = [p(:) q(:)]';
[test_targets, E] = AdaBoost(patterns, targets, test_patterns, 100, 'weakLearnerC', []);  
disp(E);
toc

plotData(X, y);
hold on
plotDecisionBoundary(test_patterns, test_targets);
hold off
plotData(test_patterns', test_targets');


