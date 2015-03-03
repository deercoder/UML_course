%% Exercise 2: LDA


%% Initialization
clear ; close all; clc

%% Load Data
%  The first two columns contains the exam scores and the third column
%  contains the label.

data = load('ex2data1.txt');
X = data(:, [1, 2]); y = data(:, 3);

%% ==================== Part 1: Plotting ====================
%  We start the exercise by first plotting the data to understand the 
%  the problem we are working with.

fprintf(['Plotting data with + indicating (y = 1) examples and o ' ...
         'indicating (y = 0) examples.\n']);

plotData(X, y);

% Put some labels 
hold on;
% Labels and Legend
xlabel('Exam 1 score')
ylabel('Exam 2 score')

% Specified in plot order
legend('Admitted', 'Not admitted')
fprintf('\nProgram paused. Press enter to continue.\n');
pause;


%% ============ Linear Discriminant Analysis ============
[w, accuracy]= lda(X,y);
fprintf('Training Accuracy of LDA is: %d \n', accuracy);
fprintf('\nProgram paused. Press enter to continue.\n');
hold off;
pause;
