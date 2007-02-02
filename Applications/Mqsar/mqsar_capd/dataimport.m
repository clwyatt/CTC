function [rownames,colnames, datain] = dataimport(filename);
% Generic data import routine. Read the row headers and 
% the column headers of the file. The rest of the data is 
% treated as numeric data returned for furhter processing
%
% Usage:
% ------
% [rownames, colnames, datain] = dataimport(filename);
% 
% Inputs:
% -------
% filename   : Input file to be read
%
% Outputs:
% --------
% rownames : Row headers, 1st column of the file.
% colnames : Column headers, 1st row of the file.
% datain   : Numeric data part of the file. The whole
%            file except fo the rist row and columns. 
%
% Ersin Bayram, July 2nd 2002
% Targacept, Inc. 
%---------------------------------------------------------------


%Read in the Numeric data
datain = dlmread(filename,'\t',1,1);
if ~any(datain(:,end))
   datain(:,end)=[];
end;

% Size of the numeric Data
[Height,Width] = size(datain);

% Add one to both height and width for the molecule and column names
Height = Height+1;
Width = Width+1;

Bufsize = (Height+1)*(Width+1);
if Bufsize < 30000
    Bufsize = 30000;
end;    

% Read the molecule names and ignore the rest of the file
rownames  = textread(filename,'%s%*[^\n]','whitespace','\t','bufsize',Bufsize);


% Read the column names from the file. Only the first witdh-th elements
colnames = textread(filename,'%s',Width,'whitespace','\t');

