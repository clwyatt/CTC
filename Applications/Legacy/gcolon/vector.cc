#include "vector.h"

float *vector(int nl,int nh)
{
        float *v;
        v=(float *)malloc((unsigned) (nh-nl+1)*sizeof(float));
        if (!v) nrerror("allocation failure in vector()");
        return v-nl;
}
