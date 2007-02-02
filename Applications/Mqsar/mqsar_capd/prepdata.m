function [CmpNames, VarNames, Xmat, Ymat, Ynames, Bins,Maskmat] = prepdata(userdata);
% Read file according the specify options
  
[CmpNames, VarNames, Xmat, Ymat, Ynames, Bins] = qsarimport(userdata.trfile, 'bioact', ...
                                                 userdata.bioact, 'binning', userdata.bin, ...
                                                 'cutoffs', userdata.cutoff, 'ex', userdata.exclude);

[VarNames, Xmat] = qsarfilter(VarNames, Xmat,userdata.corrfilt, userdata.stdfilt);

if userdata.ShuffleFlag
   RandInd = randperm(length(Ymat)); 
   Ymat = Ymat(RandInd);
   Bins = Bins(RandInd);    
end    

if ~isempty(userdata.partitionfile)
    [rownames columnames Maskmat] = qsarimport(userdata.partitionfile);
    if size(Xmat, 1) ~= size(Maskmat, 1)
        errordlg('The partition file ', ...
                ' Check your data/partition files and try again');
        return;
    end
else
    Maskmat = zeros(length(Ymat),1);
end;    

switch userdata.scaling
case 'Mahalanobis' % Normalize to Mahalanobis distance
    xmean = mean(Xmat);
    xstd = std(Xmat);
    Xmat = (Xmat - ones(size(Xmat,1),1)*xmean)./(ones(size(Xmat,1),1)*xstd);
case 'Min-Max' % Normalize with respect to data range
    xmin = min(Xmat);
    xmax = max(Xmat);            
    Xmat = (Xmat - ones(size(Xmat,1),1)*xmin)./(ones(size(Xmat,1),1)*(xmax - xmin));
end
