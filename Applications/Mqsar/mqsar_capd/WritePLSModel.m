function WritePLSModel(fmodel, B, BestPrinCompn, E, Fitness, Expterm, LogTerm, W, Q, P, MeanStd)
% write the PLS model parameter to the model file
% --------------------------------------------------------------------
[r c] = size(B);
fprintf(fmodel, '%s\t%i\t%i\n', 'Coefficient', r, c);
for i=1:r
    for j=1:c
        fprintf(fmodel, '%8.3f\t', B(i, j));
    end
    fprintf(fmodel, '\n');
end
fprintf(fmodel, '%s\t%i\n', 'The best components is ', BestPrinCompn);
fprintf(fmodel, '%s%5.3f', 'Model R^2: ', Fitness(1));
if size(Fitness, 2) > 1
    fprintf(fmodel, '\t%s%5.3f\n', 'Q^2: ', Fitness(2));
else
    fprintf(fmodel, '\n');
end
[r c] = size(Expterm);
if ~isequal(Expterm, zeros(r, 1))
    fprintf(fmodel, '%s\t%i\n', 'Exponent items: ', r);
    for i=1:r
        fprintf(fmodel, '%8.3f\t', Expterm(i));
    end
    fprintf(fmodel, '\n');
end
[r c] = size(LogTerm);
if ~isequal(LogTerm, zeros(r, 1))
    fprintf(fmodel, '%s\t%i\n', 'Logarithm items shift:', r);
    for i=1:r
        fprintf(fmodel, '%8.3f\t', LogTerm(i));
    end
    fprintf(fmodel, '\n');
end
[r c] = size(MeanStd);
if ~isequal(MeanStd, zeros(r, c))
    fprintf(fmodel, '%s\t%i\n', 'Mean-Standard items:', r);
    for i=1:r
        for j=1:c
            fprintf(fmodel, '%8.3f\t', MeanStd(i, j));
        end
        fprintf(fmodel, '\n');
    end
end

% W term
[r c] = size(W);
fprintf(fmodel, '%s\n', 'W parameters');
for i=1:r
    for j=1:c
        fprintf(fmodel, '%8.3f\t', W(i, j));
    end
    fprintf(fmodel, '\n');
end
% Q term
[r c] = size(Q);
fprintf(fmodel, '%s\n', 'Q parameters');
for i=1:r
    for j=1:c
        fprintf(fmodel, '%8.3f\t', Q(i, j));
    end
    fprintf(fmodel, '\n');
end
% P term
[r c] = size(P);
fprintf(fmodel, '%s\n', 'P parameters');
for i=1:r
    for j=1:c
        fprintf(fmodel, '%8.3f\t', P(i, j));
    end
    fprintf(fmodel, '\n');
end