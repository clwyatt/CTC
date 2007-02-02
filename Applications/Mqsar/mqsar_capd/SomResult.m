function SomResult(varnamesel, stats, class, names, bins, fout, fmodel, varargin)
	% varnamesel	(vector)	selected key variable's names
	% stats			(structure)	SOM statistics
	% class			(array)		the som cluster results
	% names			(vector)	the training set compound's names
	% bins			(array)		the observed compound's binnings
	% fout			(handle)	the handle of output file
	% fmodel		(handle)	the handle of model file
	
	% Valid argument IDs and corresponding values. Values which are
	% unambiguous (marked with '*') can be given without the preceeding argID.
	%
	%	'test'	*(string)  specifies that it will include test set information
	%	that the following are: nModel (interger) - the model ID; test_names (vector) - the 
	%	list of test compound's names; test_bins (array) - the observe compound's binnings for
	%	test set; pred (array) - the som predicted class for test set

	nModel = [];
	szStr = [];
	szTmp1 = [];
    xmean = [];
    
	% varargin
	i = 1;
	while i <= size(varargin, 2)
		argok = 1;
		if ischar(varargin{i}),
			switch varargin{i}
                case 'Mean'
                    i = i + 1;	xmean = varargin{i};
                case 'Std'
                    i = i + 1;	xstd = varargin{i};
				case {'test'},
					i = i + 1;	nModel = varargin{i};
					i = i + 1;	test_names = varargin{i};
					i = i + 1;	test_bins = varargin{i};
					i = i + 1;	pred = varargin{i};
			end
		end
        if ~argok,
            disp (['(somresult) ignoring invalid argument: #', num2str(i+1)]);
        end
        i = i + 1;
	end
	
	if ~isempty(nModel)
		szStr = ['Model ', num2str(nModel), ' Test Correct rate: ', num2str(stats.test_correct_rate)];
		szTmp1 = ['Test false positives: ', num2str(stats.test_false_pos), ...
					'	Test false negatives: ', num2str(stats.test_false_neg)];
	end
	szStr = [szStr, ' Correct rate: ', num2str(stats.correct_rate), ...
			' and variable number: ', num2str(stats.varnum)];

	szTmp = char(varnamesel(1));
    if ~isempty(xmean)
        szTmp2 = num2str(xmean(1));
        szTmp3 = num2str(xstd(1));
    end;
    
	for i=2:length(varnamesel)
		szTmp = [szTmp, '   ', char(varnamesel(i))];
        if ~isempty(xmean)
            szTmp2 = [szTmp2, ' ', num2str(xmean(i))];
            szTmp3 = [szTmp3, ' ', num2str(xstd(i))];
        end;	
	end

	szTmp1 = [szTmp1, '	false positives: ', num2str(stats.false_pos), ...
			'	false negatives: ', num2str(stats.false_neg), ...
			'	quantization error: ', num2str(stats.quantiz_err), ...
			'	topographic error: ', num2str(stats.topog_err)];

	fprintf(fmodel, '%s\n%s\n%s\t%s\n', szStr, szTmp1, 'Descriptors:', szTmp);
	fprintf(fout, '%s\n%s\n%s\n', szStr, szTmp1, szTmp);

	if ~isempty(xmean)
        fprintf(fmodel, '%s\t%s\n%s\t%s\n', 'Scale mean: ', szTmp2, 'Scale std:', szTmp3);
    end;
    
	fprintf(fout, '%s\t\n', 'Training Set');
	fprintf(fout, '%s\t%s\t%s\n', 'Name', 'Observed', 'Calculated');
	for i=1:length(names)
		fprintf(fout, '%s\t%i\t%i\n', char(names(i)), bins(i), class(i));
	end
	if ~isempty(nModel)
		fprintf(fout, '\n%s\n', 'Test Set');
		fprintf(fout, '%s\t%s\t%s\n', 'Name', 'Observed', 'Predicted');
		for i=1:length(test_names)
			fprintf(fout, '%s\t%i\t%i\n', char(test_names(i)), test_bins(i), pred(i));
		end
	end
	fprintf(fout, '\n');
	fprintf(fmodel, '\n');

