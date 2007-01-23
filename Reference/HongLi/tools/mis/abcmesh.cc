#include <fstream>
#include <cstdio>
#include <stdlib.h>

int main (int argc, char *argv[])
{
int i,j,k, ii;
float a,b,c;
ofstream file;
 
if (argc <= 3) /* Read parameters*/
	{
	cout << "usage : abcmesh a b c interval" <<endl;
	exit(0);
	}

a = atof(argv[1]);
b = atof(argv[2]);
c = atof(argv[3]);

float interval = atof(argv[4]);	

file.open(argv[5]);

file << "INST"<< endl;
file << "transform  {"<< endl;
file << interval << " 0 0 0"<< endl;
file <<"0 "<<interval << " 0 0"<< endl;
file << "0 0 "<<interval << " 0"<< endl;
file << "-7 -7 0 1"<< endl;
file <<"}"<<endl;

file <<" geom { " << endl;
file << "ZMESH" <<endl;
file <<"14 14"<<endl;

for (i = -7;i< 7;i++) 
	{
	for (j = -7; j< 7 ;j++)
		file << a*i*i + 2*b*i*j+ c * j *j <<" ";
	file <<endl;
	}		
file << " }"<<endl;
file.close();
}



	
