#include <slist.h>
#include <fstream>
#include <cstdio>
#define tol 0.0001
#define VIEWABLE_OUT 1
#define CHECKOUT 2
#define SERIALIZE 3

main(int argc, char *argv[])
{
if (argc <2) /* Read parameters*/
{
	cout << "Usage : getboundoogl oogl_file flag x y z" << endl;
	cout << " oogl_file : file name" << endl;
	cout << " flag : viewable(1) output or not" << endl;
	cout << " x y z : checking point coordinates" << endl;	
	exit(1);
	}
ifstream  infile;
char entry[256];
int i, ii,j ;
infile.open(argv[1]);
int dx1, dx2,dx3;
int flag = atoi(argv[2]);
 
 
float x = -100000;
float y = -100000;
float z = -100000;

if(argc == 6)
{
x = atof(argv[3]); 
y = atof(argv[4]); 
z = atof(argv[5]); 
}
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
	if(flag == VIEWABLE_OUT)
		clog << nv << " vertices, " << nt << " triangles" << endl;
	int vtotal = 0;

	infile.getline(entry, 100);
	
	vx = new float[nv];
	vy = new float[nv];
	vz = new float[nv];
	
	if (vx==NULL ||vy==NULL||vz==NULL)
		{
		cout <<"can not load memory \n" <<endl;
		exit(1);
		}
	float a, b ,c;
	float maxx = -10000,minx = 10000;
	float maxy = -10000,miny = 10000;
	float maxz = -10000,minz = 10000;
	//read in vertices
	
	for(i=0; i < nv; i++){
		infile >> vx[i] >> vy[i] >> vz[i] ;// >> a >> b>> c;
		if (vx[i] > maxx) maxx = vx[i];
		if (vx[i] < minx) minx = vx[i];
		if (vy[i] > maxy) maxy = vy[i];
		if (vy[i] < miny) miny = vy[i];
		if (vz[i] > maxz) maxz = vz[i];
		if (vz[i] < minz) minz = vz[i];
		 
	}	
	
	int GOTIT = 0;
	//if(minx < x && miny <y && minz < z && maxx >x &&maxy > y&& maxz > z)
	if(((maxx+minx)/2 - x)*((maxx+minx)/2 - x)+((maxy+miny)/2 - y)*((maxy+miny)/2- y) + ((maxz+minz)/2 - z)*((maxz+minz)/2 - z) < 10)	
		GOTIT = 1;
	if(flag == VIEWABLE_OUT)
	{
	cout << "x:" <<minx<<" "<<maxx<< "\n";
	cout << "     center "<<(maxx+minx)/2 <<"; range "<<maxx-minx<<endl; 	
	cout << "y:" <<miny<<" "<<maxy<< "\n";
	cout << "     center "<<(maxy+miny)/2 <<"; range "<<maxy-miny<<endl; 	
	cout << "z:" <<minz<<" "<<maxz<< "\n";
	cout << "     center "<<(maxz+minz)/2 <<"; range "<<maxz-minz<<endl; 
	if (GOTIT) cout << "true polyp
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" <<endl;	
	}
	
	if(flag == CHECKOUT)
	{
	/*cout << " "<<(maxx+minx)/2<<" "<<(maxy+miny)/2 <<" "<<(maxz+minz)/2 <<" "
	     <<maxx-minx<< " "<<maxy-miny<< " "<<maxz-minz<<endl;*/
		if (GOTIT) cout << argv[1] <<" has true polyp !!!!" <<endl;	

	}
	
	if(flag == SERIALIZE)
	{
	cout << " "<<(maxx+minx)/2<<" "<<(maxy+miny)/2 <<" "<<(maxz+minz)/2 <<" "
	     <<"40 40 40 1"<<endl;

	}
	
	int found = 0;
	j = 0;
	infile.close();
	
	delete [] vx;
	delete [] vy;
	delete [] vz;
};
