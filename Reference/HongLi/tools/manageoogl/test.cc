#include <slist.h>
#include <fstream>
#include <cstdio>
#define tol 0.0001

main(int argc, char *argv[])
{
cout <<"here"<<endl;
if (argc <3) /* Read parameters*/
{
	cout << "Usage : manage pre_oogl_file oogl_file" << endl;
	exit(1);
	}
ofstream  outfile;
ifstream  infile;
char entry[256];
int i, ii,j ;
infile.open(argv[1]);
float a,b,c;int tritotal = 0;
	int  nv, nt, temp, idx1, idx2, idx3;
	float *vx, *vy, *vz, *tmp;
	vx=vy=vz=NULL;
	if(!infile){
		cerr << "Error in ooglread(...): cannot open file" << endl;
		exit(-1);
	}
	
	for (i = 0;i<10; i++)
	{
	infile.getline(entry, 100);
	
	if(entry[0] == '#') cout << "#"<<endl;
	if(entry[0] == ' ' )cout << "space"<<endl;
	if( entry[0] == '\t') cout << "tab"<<endl;
	if(entry[0] == '\0') cout << "return"<<endl;
	
	//infile >> a >> b >>c;
	
	cout<<a<<" " <<b<<" " <<c<<endl;
	cout <<endl;
	getchar();
	}

	infile.close();
	
};
