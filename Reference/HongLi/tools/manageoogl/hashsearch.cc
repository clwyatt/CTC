#include <slist.h>
#include <fstream>
#include <cstdio>
#include <math.h>
#define tol 0.001

main(int argc, char *argv[])
{
static double A = (sqrt(5)-1)/2;
int slots[65536];
if (argc <5) /* Read parameters*/
{
	cout << "Usage : m1oogl pre_oogl_file hashfile  normflag" << endl;
	exit(1);
	}
ofstream  outfile;
ifstream  infile;
char entry[256];
int i, ii,j ;
int searchto = -1;
outfile.open(argv[2]);
infile.open(argv[1]);
int normflag = atoi(argv[3]);

for (i = 0; i < 65535; i++) slots[i] =0 ;
 

cout << "Testing hashing function... "<<endl; 

int dx1, dx2,dx3;
int tritotal = 0;
	int  nv, nt, temp, idx1, idx2, idx3;
	float *vx, *vy, *vz, *tmp;
	vx=vy=vz=NULL;
	if(!infile){
		cerr << "Error in ooglread(...): cannot open file" << endl;
		exit(-1);
	}
	
	infile.getline(entry, 100);
	infile.getline(entry, 100);

	infile >> nv >> nt >> temp;	
	clog << nv << " vertices, " << nt << " triangles" << endl;
	int vtotal = 0;
	int * subindex = new int[nv];
	

	infile.getline(entry, 100);

	double k;	
	float a, b ,c, d, e, f;
	int num = 0;
	//read in vertices
	
	
	for(i=0; i < nv; i++){
		if(normflag)
			infile  >> a >> b >> c >> d >> e >> f;
		else
			infile  >> a >> b >> c;
		k = a+b+c;
		k = k*A;
		num = floor(65536*(k - floor(k));
		slots[num]++;
			
	//go to right place in the hash table					
	}	
	
	for (i=0;i<65536;i++)
		outfile << slots[i] <<endl;
	
		
	outfile.close();	
	infile.close();
	
};
