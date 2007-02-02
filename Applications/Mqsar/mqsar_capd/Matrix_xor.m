
function Difference = Matrix_xor(Mask)
% This m-file shows the total number of bits different in a pairwise
% comparison of the deciding top 10 models 


[M, N] =size(Mask);

K = min(M,N);

for i=1:K
    for j=1:K
        Difference(i,j) = sum(xor(Mask(:,i),Mask(:,j)));
    end;
end;

