#include <fstream>
#include <cstdio>
#include <stdlib.h>

#define tol 0.0001

main(int argc, char *argv[])
{
if (argc <3) /* Read parameters*/
{
	cout << "Usage : searchlog centerx centery centerz radius"<<endl;
	
	exit(1);
	}
ifstream  infile;
char entry[256];
int id ;
float x,y,z,r;
infile.open("capdlog.dat");
float cx =  atof(argv[1]);
float cy =  atof(argv[2]);
float cz =  atof(argv[3]);
char dummy[80];
r =  atof(argv[4]);

	if(!infile){
		cerr << " Cannot open file" << endl;
		exit(-1);
	}
	
	while(!infile.eof())
	{
	infile.getline(dummy, 256);
	    infile >> id >> x >> y >> z;	
	    infile.getline(entry, 100);
	    if ((cx-x)*(cx-x) + (cy-y)*(cy-y) + (cz-z)*(cz-z) < r*r)
		cout <<id<<" "<< x << " "<<y <<" "<< z<<" "<< entry << endl;
	}
	infile.close();
	
}
