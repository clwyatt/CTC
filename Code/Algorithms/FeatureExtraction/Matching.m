function [mindistance, polypid] = Matching(x,y,z,FeatureVector)

mindistance = 10000;
polypid = 10000;

for i=1:size(FeatureVector,1)
        cmp_dist = (FeatureVector(i,2) - x)^2 + (FeatureVector(i,3) - y)^2 + (FeatureVector(i,4) - z)^2;
        if(mindistance > cmp_dist)
            mindistance = cmp_dist;
            polypid = FeatureVector(i,1);
        end
end

mindistance = sqrt(mindistance);
