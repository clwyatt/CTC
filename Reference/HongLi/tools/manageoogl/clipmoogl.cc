#include <slist.h>
#include <fstream>
#include <cstdio>
#define tol 0.0001

main(int argc, char *argv[])
{
if (argc <3) /* Read parameters*/
{
	cout << "Usage : clipoogl pre_oogl_file oogl_file centerx centery
	cemterz sizex sizey sizez flag reverse" << endl;
	cout << "    flag : 1/0--w/o direction. " << endl;
	cout << "    reverse : 1/0-- flip/not surface." << endl;
	
	exit(1);
	}
ofstream  outfile;
ifstream  infile;
char entry[256];
int i, ii,j ;
outfile.open(argv[2]);
infile.open(argv[1]);
int dx1, dx2,dx3;
float fromx =  atof(argv[3]) - atof(argv[6])/2;
float tox =  atof(argv[3]) + atof(argv[6])/2;
float fromy =  atof(argv[4]) - atof(argv[7])/2;
float toy =  atof(argv[4]) + atof(argv[7])/2;
float fromz =  atof(argv[5]) - atof(argv[8])/2;
float toz =  atof(argv[5]) + atof(argv[8])/2;
int flag = atoi(argv[9]);
int reverse = atoi(argv[10]);

cout << fromx <<" "<<tox<<"  "<<fromy <<" "<<toy<<"  "<<fromz <<" "<<toz<<endl;

int tritotal = 0;

	int  nv, nt, temp, idx1, idx2, idx3;
	float *vx, *vy, *vz, *tmp;
	char emptyLine[40] = {"                                   "};
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
	outfile << "appearance { shading flat }" << endl;
	outfile << "OFF" << endl;
	long pos = outfile.tellp();
	outfile << emptyLine << endl <<endl;
	
	for(i=0; i < nv; i++){
		if(flag!=0)
			infile >> vx[i] >> vy[i] >> vz[i] >> a >> b>> c;
		else
			infile >> vx[i] >> vy[i] >> vz[i]; 
	
		subindex[i]=-1;
		if (vx[i] > maxx) maxx = vx[i];
		if (vx[i] < minx) minx = vx[i];
		if (vy[i] > maxy) maxy = vy[i];
		if (vy[i] < miny) miny = vy[i];
		if (vz[i] > maxz) maxz = vz[i];
		if (vz[i] < minz) minz = vz[i];
		 
		if(vx[i] < tox && vx[i] > fromx && \
		   	vy[i] < toy && vy[i] > fromy && \
		   	vz[i] < toz && vz[i] > fromz)
			{
			subindex[i] = vtotal;
			vtotal ++;
			if(flag !=0)
				outfile << vx[i]<<" "<< vy[i] <<" "<< vz[i]<< " "<<a << " "<<b<< " "<<c<<endl;
			else 
				outfile << vx[i]<<" "<< vy[i] <<" "<< vz[i]<<endl;

			cout <<vtotal<<"\r";
				}
	}	
	cout << "x:" <<minx<<" "<<maxx;
	cout << "  y:" <<miny<<" "<<maxy;
	cout << "  z:" <<minz<<" "<<maxz<<"\n";
	int found = 0;
	j = 0;
	 int num = 0;
	 float red,green,blue;
	//read in triangles and add to mesh
	for(i=0; i < nt; i++){
		infile >> temp >> idx1 >> idx2 >> idx3>> red >> green >>blue;
		if(subindex[idx1] == -1 || subindex[idx2] == -1 || subindex[idx3] == -1)
		{
			continue;
		}
		else{
			if(reverse != 1)
				sprintf(entry, "3 %d %d %d ", subindex[idx1], subindex[idx2], subindex[idx3]);
			else
				sprintf(entry, "3 %d %d %d ", subindex[idx2], subindex[idx1], subindex[idx3]);
			outfile <<entry<<endl;
			num++;
			cout <<num<<"\r";
			
		}
		}
	cout << "Total picked V: "<<vtotal << ". T: "<< num <<endl;	
	nt = num;
	outfile.seekp(pos);
	outfile <<vtotal<<" "<<nt<<" 1";		
	outfile.close();
	char cmd[256];	
	if(vtotal == 0 || num == 0)
		{
		sprintf(cmd, "/bin/rm %s",  argv[2]);
		system(cmd);
		}
			
	infile.close();
	
	delete [] vx;
	delete [] vy;
	delete [] vz;
	delete [] subindex;
};
