function WritePLSOut(fout, Names, E, Y, BestPrinCompn, Fitness)
% write the PLS results to the output file
% --------------------------------------------------------------------
[r c] = size(Y);
prdY = Y + E;
fprintf(fout, '%s%i\n', 'The best component number: ', BestPrinCompn);
fprintf(fout, '%s%5.3f', 'Model R^2: ', Fitness(1));
if size(Fitness, 2) > 1
    fprintf(fout, '\t%s%5.3f\n', 'Q^2: ', Fitness(2));
else
    fprintf(fout, '\n');
end
fprintf(fout, '%s\t', 'Name');
for j=1:c
    fprintf(fout, '%s\t%s\t', ['Actual' num2str(j)], ['Calculate', num2str(j)]);
end
fprintf(fout, '\n');
for i=1:r
    fprintf(fout, '%s\t', char(Names(i)));
    for j=1:c
        fprintf(fout, '%8.3f\t%8.3f\t', Y(i, j), prdY(i, j));
    end
    fprintf(fout, '\n');
end

