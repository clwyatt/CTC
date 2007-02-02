function testplsc()

Xmat = [7, 7, 13 7; 4, 3, 14, 7; 10, 5, 12, 5; 16, 7, 11, 3; 13, 3, 10, 3];
Y = [14, 7, 8; 10, 7, 6; 8, 5, 5; 2, 4, 7;6, 2, 4];
%[B pc E Emin] = plsc(Xmat, Y, 5, 'Monte Carlo')
[B pc E Fitness] = npls(Xmat, Y, 5, 'LOO')
%[B pc E Fitness] = npls(Xmat, Y, 5, 'LOO', 'Nonlinear', 'Scaling')