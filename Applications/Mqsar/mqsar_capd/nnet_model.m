function [TrPrdAct,TrPrdBin, TstPrdAct,TstPrdBin, stats] = nnet_model(TrX,TrAct,TrBins,varargin);
%  NNET based modeling
%
% [Member_matrix, class] = nnet_model(TrnX,TrnAct,TrnBins,TstX,TstAct,TstBins)
%
%	Input and output arguments ([]'s are optional): 
%	TrX	(matrix)	training data matrix
%   TrAct  (vector)    training sample's bioactivities
%	TrBins	(vector)	training sample's categories
%	
%   varargin:
%   'Test'   indicates that there is an external test data. 
%   TstX	(matrix)	test data matrix
%   TstAct  (vector)    test sample's bioactivities
%	TstBins	(vector)	test sample's categories
%
%   Outputs:
%	prd_class	(vector)	the calculated bins for test set
%	stats	(struct)	the statistical result
%	calc_class	(vector)	the calculated bins for training set
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
%
% Ersin Bayram, June 2004

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Cutoff = [];
% Receptor = 'None';
NNtype = 'Feed Forward';
NNmethod = 'Predict';
NNfunction = 'tansig';
NNtraining = 'trainlm';
HidStart = 3;
HidStop = 7;
outlayer = 1;
TstX = [];
TstAct = [];
TstBins =[];
sortgoodtobad = 0;

%% Check arguments
i=1;
while i<=size(varargin,2), 
	argok = 1;
	if ischar(varargin{i}), 
		switch varargin{i}, 
			% unambiguous values
%             case 'Receptor'
%                 i = i + 1;
% 				Receptor = varargin{i};
            case 'Cutoff',
				i = i + 1;
				Cutoff = varargin{i};
            case 'Type'
                i = i + 1;
                NNtype = varargin{i};
			case 'Method',
                i = i + 1;
				NNmethod = varargin{i};
			case 'Hidden Layer',
				i = i + 1;
                HidStart = varargin{i};
                HidStart2 = HidStart(2);
                HidStart = HidStart(1);
                i = i + 1;
                HidStop = varargin{i};
                HidStop2 = HidStop(2);
                HidStop = HidStop(1);
            case 'Method',
				i = i + 1;
                NNmethod = varargin{i};
            case 'Training',
				i = i + 1;
                name = varargin{i};
                switch name
                case 'Leven. Marq'
                    NNtraining = 'trainlm';
                case 'Grad Descent'
                    NNtraining = 'traingd';
                case 'GDes Moment'
                    NNtraining = 'traingdm';
                case 'GDes Adapt Lrn'
                    NNtraining = 'traingda';    
                case 'GDes Mom ALrn'
                    NNtraining = 'traingdx';
                otherwise
                    NNtraining = 'Default';
                end;    
            case 'Function',
				i = i + 1;
                NNfunction = varargin{i};  
            case 'Epoch',
				i = i + 1;
                NNepoch = varargin{i};    
			case {'Test'},
				i = i + 1; TstX = varargin{i};
                i = i + 1; TstAct = varargin{i};
                i = i + 1; TstBins = varargin{i};
            case {'Bin Sort'},
				i = i + 1; sortgoodtobad = varargin{i};    
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(NNET) Ignoring invalid argument: #' num2str(i)]); 
	end
	i = i+1; 
end

% Need to know which bins are good or bad for specificity analysis
%~strcmp(Receptor,'dhfr') & ~strcmp(Receptor,'topliss') 
if sortgoodtobad 
    GB = 1; BB = max(TrBins); 
else 
    GB = max(TrBins); BB = 1; 
end; 

% build and normalize traing/test som data structures (sD) and (tD)
%=====================================
%NNET based ensemble method
% Feed forward backpropagation network is used here
InputMin = min(TrX)';
InputMax = max(TrX)';

tmpact = zeros(size(TrAct)); 
tmpbins = zeros(size(TrAct));
tmpnet = [];
tmpacc = 0;

for Hid1=HidStart:HidStop
   for Hid2=HidStart2:HidStop2 
       switch NNmethod
       case 'Classify'
            switch NNtype
            case 'Feed Forward'      
                switch NNtraining
                case 'Default'     
                    if (Hid1==0) | (Hid2==0)
                        net = newff([InputMin InputMax],[size(TrX,2) max(Hid1,Hid2) max(TrBins)]);
                    else    
                        net = newff([InputMin InputMax],[size(TrX,2) Hid1 Hid2 max(TrBins)]);          
                    end;    
                    otherwise    
                    if (Hid1==0) | (Hid2==0)
                        net = newff([InputMin InputMax],[size(TrX,2) max(Hid1,Hid2) max(TrBins)],{'purelin' NNfunction 'purelin'}, NNtraining);           
                    else
                        net = newff([InputMin InputMax],[size(TrX,2) Hid1 Hid2 max(TrBins)],{'purelin' NNfunction NNfunction 'purelin'}, NNtraining);           
                    end;
                end; 
            case 'RBF'
                
            case 'Regression'
            
            case 'Cascade FF'
            
            case 'LVQ'
                   
            case 'Competitive'
                    
            end;   
            net.trainParam.epochs = NNepoch;
            net.trainParam.show = nan;
            num_class = max(TrBins);
            train_bins = repmat(TrBins, 1, num_class);
            for idx = 1:num_class
                train_bins(train_bins(:, idx) ~= idx, idx) = -1;
                train_bins(train_bins(:, idx) == idx, idx) = 1;
            end
            net = train(net,TrX',train_bins');
            net_bin_tr = sim(net,TrX')';
            [nouse, idx]= max(net_bin_tr, [], 2); % maximum for each data
            net_bin_tr = idx;
            
            net_acc_tr = sum(net_bin_tr == TrBins)/length(TrBins);
            if net_acc_tr > tmpacc
               tmpnet = net;
               tmpact = net_bin_tr;
               tmpbins = net_bin_tr;
               tmpacc = net_acc_tr;
               tmplayer1 = Hid1;
               tmplayer2 = Hid2;
            end;   
            
       case 'Predict'
            switch NNtype
            case 'Feed Forward'
                switch NNtraining
                case 'Default' 
                    if (Hid1==0) | (Hid2==0)
                        net = newff([InputMin InputMax],[size(TrX,2) max(Hid1,Hid2) 1]);
                    else
                        net = newff([InputMin InputMax],[size(TrX,2) Hid1 Hid2 1]);
                    end;    
                otherwise    
                    if (Hid1==0) | (Hid2==0)
                        net = newff([InputMin InputMax],[size(TrX,2) max(Hid1,Hid2) 1],{NNfunction NNfunction 'purelin'}, NNtraining);           
                    else
                        net = newff([InputMin InputMax],[size(TrX,2) Hid1 Hid2 1],{NNfunction NNfunction NNfunction 'purelin'}, NNtraining);           
                    end;
                end;
            case 'RBF'
                
            case 'Regression'
            
            case 'Cascade FF'
            
            case 'LVQ'
                   
            case 'Competitive'
                    
            end;   
            net.trainParam.epochs = NNepoch;
            net.trainParam.show = nan;
            net = train(net,TrX',TrAct');
            net_act_tr = sim(net,TrX')';        
            net_bin_tr = ones(size(net_act_tr));
            for i=1:length(Cutoff)
                  net_bin_tr(net_act_tr>=Cutoff(i)) = i+1;
            end;
            
            net_acc_tr = sum(net_bin_tr == TrBins)/length(TrBins);
            if net_acc_tr > tmpacc
               tmpnet = net;
               tmpact = net_act_tr;
               tmpbins = net_bin_tr;
               tmpacc = net_acc_tr;
               tmplayer1 = Hid1;
               tmplayer2 = Hid2;
            end;   
       end; % switch NNmethod  
   end;     % for Hid1
end;        % for Hid2    
      
net = tmpnet;
net_act_tr = tmpact;
net_bin_tr = tmpbins;
net_acc_tr = tmpacc;        
net_layer = [tmplayer1 tmplayer2];

% Error on test
if ~isempty(TstX)
    switch NNmethod
        case 'Classify'
            net_bin_tst = sim(net,TstX')';
            [nouse, idx]= max(net_bin_tst, [], 2); % maximum for each row
            net_bin_tst = idx;
            net_act_tst = net_bin_tst;
        case 'Predict'
            net_act_tst = sim(net,TstX')';
            net_bin_tst = ones(size(net_act_tst));
            for i=1:length(Cutoff)
                  net_bin_tst(net_act_tst>=Cutoff(i)) = i+1;
            end;
    end
    TstPrdAct = net_act_tst;
    TstPrdBin = net_bin_tst;

    stats.test_rmserror = sqrt(sum((TstPrdAct-TstAct).^2)/length(TstAct));
    stats.test_correct_rate = sum(TstPrdBin == TstBins)/length(TstBins);
%     stats.test_miss = sum((TstPrdBin == BB).*(TstBins == GB))/length(TstBins);
%     stats.test_falsehit = sum((TstPrdBin == GB).*(TstBins == BB))/length(TstBins);
%     stats.test_false_pos = sum((TstPrdBin == GB).*(TstBins ~= GB))/length(TstBins); 
%     stats.test_false_neg = sum((TstPrdBin ~= GB).*(TstBins == GB))/length(TstBins);
    
    % NOTE: if there are three or more classes, the stats may be different 
    % from the original calculation. 1 - positive; 2 - negative
    stats.test_false_pos = sum((TstPrdBin == 1).*(TstBins == 2))/sum(TstBins == 2);
    stats.test_false_neg = sum((TstPrdBin == 2).*(TstBins == 1))/sum(TstBins == 1);
    stats.test_miss = stats.test_false_neg;
    stats.test_falsehit = stats.test_false_pos;
else
    TstPrdAct = [];
    TstPrdBin = [];
end;

TrPrdAct = net_act_tr;
TrPrdBin = net_bin_tr;
stats.rmserror = sqrt(sum((TrPrdAct-TrAct).^2)/length(TrAct));
stats.correct_rate = sum(TrPrdBin == TrBins)/length(TrBins);
% stats.false_pos = sum((TrPrdBin == GB).*(TrBins ~= GB))/length(TrBins);
% stats.false_neg = sum((TrPrdBin ~= GB).*(TrBins == GB))/length(TrBins);
% stats.miss = sum((TrPrdBin == BB).*(TrBins == GB))/length(TrBins);
% stats.falsehit = sum((TrPrdBin == GB).*(TrBins == BB))/length(TrBins);

% 1 - positive; 2 - negative
stats.false_pos = sum((TrPrdBin == 1).*(TrBins == 2))/sum(TrBins == 2);
stats.false_neg = sum((TrPrdBin == 2).*(TrBins == 1))/sum(TrBins == 1);
stats.miss = stats.false_neg;
stats.falsehit = stats.false_pos;
stats.hiddenlayer = net_layer;
stats.varnum = size(TrX,2);
