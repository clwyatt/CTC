function [BestModel,BestDesc] = stepwisereg(xdata, ydata, varargin);
% STEPWISE REGRESSION ANALYSIS
%
% Performs stepwise regression to build QSAR equation
%
% [Predict,TP,BestModel,BestDesc] = stepregress(inputfile,Nobins,Fenter,Fremove,ModelOrder); 
% 
% Inputs:
% -------------
% xdata:		Independent X block data matrix (each row for an entry and
%				each column for a independent variable
% ydata:		depentent Y block data vector (one column), and the size 
%				is the number of the entries
% 
% Fenter:     Fvalue to add a descriptor to the model. Optional.
%             Default value: 4
% 
% Fremove:    Fvalue to remove a descriptor from the model. Optional.
%             Default value: 3.9
%
% ModelOrder: Maximum order of the model you want to fit. Optional.
%             Default value: 8. This is used to eliminate the problem 
%             of having close to singular matrix duriong the itreation. 
%             Stepwise behaves strangely with the near singular matrices. 
% 
% Right now to add the nonlinear flavor to the model. It fits the following
% 
% DescriptorSet = [Descriptors exp(Descriptors) Descriptors.^2 log(1+Descriptors.^2)];
% 
%------------------------------------------
% Yun-De Xiao, Nov 6, 2002
%------------------------------------------
currdir = pwd;
nargin = length(varargin);
path(path,currdir);
if nargin ==0		% default arguments
	Fenter = 4;   
	Fremove = 3.9;
    ModelOrder = 8;
elseif nargin==1
	Fenter = varargin{1};
    Fremove = 3.9;
    ModelOrder = 8;
elseif nargin==2
	Fenter = varargin{1};
	Fremove = varargin{2};
    ModelOrder = 8;
elseif nargin==3
	Fenter = varargin{1};
	Fremove = varargin{2};
    ModelOrder = varargin{3};
else
 error('Wrong number of arguments');    
end;

Bioact = ydata;
Datatmp = xdata;

[M,N]  =size(Datatmp);
Data1 = [Datatmp exp(Datatmp) Datatmp.^2 log(1+Datatmp.^2)];

[MM,NN]  =size(Data1);
ZeroCols = [];
Record = 1:NN;

Datain = Data1;

MeanMtx = mean(Datain,1);
SdMtx = std(Datain,1,1);
MeanMtx = ones(MM,1)*MeanMtx;
SdMtx = ones(MM,1)*SdMtx;
for i=1:NN
	Tmp = MeanMtx(:,i) - Datain(:,i);
	if ~any(Tmp) | find(Datain(:,i) == Inf) | (Data1(end-1,i) == Inf)
        ZeroCols = [ZeroCols i];
	end; 
end;

if ~isempty(ZeroCols)
	Data1(:,ZeroCols) = [];
	Record(ZeroCols) = [];
end;

Datain = Data1;

MeanMtx = mean(Data1,1);
SdMtx = std(Data1,1,1);
MeanMtx = ones(MM,1)*MeanMtx;
SdMtx = ones(MM,1)*SdMtx;

[M,N] = size(Datain);

CorrSet = ones(M,1);
Indices = [];
In = [];
Rest=1:N;
Ybar = zeros(M,1);

Done = 0;
iterno = 0;
while not(Done)
	% Calculate the correlation coefficients between each Desc. that are not in the 
	% model and Bioact
  
	Yres = Bioact - Ybar;
	Corr_Coeffs = [];
  
	for i=1:length(Rest)
		TmpCorr = corrcoef(Yres,Datain(:,Rest(i)));
		Corr_Coeffs(i)=  TmpCorr(2,1);
		Iter = i;
	end;
  
	Fvals = Corr_Coeffs.^2 * (N- length(In) - 2) ./ (1 - Corr_Coeffs.^2);
  
	% Sort the correlation coeffs. Take absolute value to account for negative 
	% correlation coeffs. as well. Pick the most correlated descriptor
	[SortedCorr,Indx] = sort(abs(Corr_Coeffs));
	Most_corr = Datain(:,Rest(Indx(end)));
	MaxCorr = SortedCorr(end); 
   
	%Check the Fval of the most correlated descriptor
  
	if Fvals(Indx(end)) < Fenter
		if (iterno ==0)
			BestDesc = Rest(Indx(end));
			CorrSet = [Most_corr CorrSet];
			BestModel = regress(Bioact, CorrSet);
			iterno = 1;
		else    
   			BestDesc = In;
			BestModel = B;
		end;    
		Done = 1;
	else  % If significant 
		iterno = 1; 
		% Add the new most correlated descriptor to the Correlation Set
		CorrSet = [Most_corr CorrSet];
		B = regress(Bioact, CorrSet);
    
		% Remove the significantly correlated desc. from the rest 
		% and add it to the "In" descriptors
		In = [Rest(Indx(end)) In];
		Rest(Indx(end)) = [];
      
		%Save the best model and descriptors 
		BestDesc = In;
		BestModel = B;
      
		% Now start to check the partial corr. coeff. of the descriptors
		% within the model, and verify that they all contribute significantly
		if length(In) > 1
			Coeffs = [];
			SE = [];
			for j = 1:length(In);
				TmpCorrSet = CorrSet;
				TmpCorrSet(:,j) = [];
				TmpB = regress(Bioact,TmpCorrSet);
				TmpY = TmpCorrSet*TmpB;
				SE(j)= sum((Bioact - TmpY).^2);  
			end;
			% Sort the correlation coeffs. Take absolute value to account for negative 
			% correlation coeffs. as well. Pick the least correlated descriptor
			% to test for significance
			[In_Corr,In_Indx] = sort(SE);
			Least_Corr = Datain(:,In(In_Indx(1)));
          
			TmpCorrSet = CorrSet;
			TmpCorrSet(:,In_Indx(1)) = [];
			TmpB = regress(Bioact,TmpCorrSet);
			TmpY = TmpCorrSet*TmpB;
			TmpRes = Bioact - TmpY;
          
			MCres = corrcoef(TmpRes,Least_Corr);
			Cres = MCres(2,1);
			Fres = Cres^2 * (N- (length(In)-1) - 2) ./ (1 - Cres.^2);
          
			if Fres < Fremove
				% Remove the insignificant descriptor from the equation
				In(In_Indx(1)) = [];   
				CorrSet = [Datain(:,In) ones(M,1)];
              
				% Calculate the new regression model based on the remaining 
				% significant descriptors
     
				B  = regress(Bioact, CorrSet);
				Ybar = CorrSet*B;
				%Save the best model and descriptors 
				BestDesc = In;
				BestModel = B;    
			end;    
		end;
	end;   
	if (length(Rest) ==0) | (length(In) > ModelOrder) 
		Done=1;
	end;  
end;		% end while

CorrSet = [Datain(:,sort(BestDesc)) ones(M,1)];
Record = Record(sort(BestDesc));
BestDesc = Record;
BestModel = regress(Bioact, CorrSet);

% End the function