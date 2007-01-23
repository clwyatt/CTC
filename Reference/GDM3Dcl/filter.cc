#include "filter.hh"

int calcGaussKernel(float sigma, float * &kernel)
{
	int i, j, k, N, center, index; 
	float x, y, z;
	float a, b; 

	N = 4*((int)ceil(sigma)) + 1;

	center = 2*((int)ceil(sigma));

	kernel = new float[N*N*N];

	a = 15.74960995*sigma*sigma*sigma;
	b = 2.0*sigma*sigma;

	a = 1/a;
	b = 1/b;

	for(i=0; i < N; i++)
		for(j=0; j < N; j++)
			for(k=0; k < N; k++){
				index = k*N*N + i*N + j;
				x = (float)(i-center);
				y = (float)(j-center);
				z = (float)(k-center);

				kernel[index] = a*exp(-b*(x*x + y*y + z*z));

			}

	return N;

};

void gaussian_smooth(IMTK::Volume<unsigned short> &volume, float sigma){

	IMTK::Volume<unsigned short> copy;
	float *kernel;
	float value, sum;
	int size, index, center;
	IMTK::Subscript<3> bounds;
	int i, j, k, ii, jj, kk, x, y, z;

	size = calcGaussKernel(sigma, kernel);

	clog << "Size of Filter Kernel is " << size << " for a sigma of " << sigma << endl;

	bounds = volume.getBounds();
	copy.setBounds(bounds);
	for(i=0; i < bounds[0]*bounds[1]*bounds[2]; i++) copy[i] = volume[i];

	center = 2*((int)ceil(sigma));

	for(i=center; i < bounds[0]-center; i++)
		for(j=center; j < bounds[1]-center; j++)
			for(k=center; k < bounds[2]-center; k++){
				
				sum = 0;
				for(ii=0; ii < size; ii++)
					for(jj=0; jj < size; jj++)
						for(kk=0; kk < size; kk++){
							index = kk*size*size + ii*size + jj;
							x = i+(ii-center);
							y = j+(jj-center);
							z = k+(kk-center);
							
							value = (float)copy.getVoxel(x, y, z);

							sum = sum + kernel[index]*value;

						}
				volume.setVoxel(i, j, k, sum);
			}

	delete [] kernel;
}
