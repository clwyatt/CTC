#include <slist.h>
#include <fstream>
#include <cstdio>

main(int argc, char *argv[])
{
cout <<"here"<<endl;
if (argc <3) /* Read parameters*/
{
	cout << "Usage : clipoogl pre_oogl_file oogl_file centerx centery
	cemterz sizex sizey sizez" << endl;
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
float toy =  atof(argv[4]) - atof(argv[7])/2;
float fromz =  atof(argv[5]) - atof(argv[8])/2;
float toz =  atof(argv[5]) - atof(argv[8])/2;


int tritotal = 0;
	int  nv, nt, temp, idx1, idx2, idx3;
	float *vx, *vy, *vz;
	if(!infile){
		cerr << "Error in ooglread(...): cannot open file" << endl;
		exit(-1);
	}
	
	infile.getline(entry, 100);
	infile.getline(entry, 100);

	infile >> nv >> nt >> temp;	
	clog << nv << " vertices, " << nt << " triangles" << endl;
	std::slist<int> * indexlist = new (std::slist<int>)[nv]; 
	std::slist<int>::iterator iter; 
	int vtotal = 0;
	int * vflags = new int[nv];
	int * tflags = new int[nt];
	int * subindex= new int[nv];
	int processed[nv];
	int subtotal[nv];
	int nprocessed = 0;
        int triindex[nt][3];

	infile.getline(entry, 100);
	
	vx = new float[nv];
	vy = new float[nv];
	vz = new float[nv];
	
	int numtotal = 0;
	//read in vertices
	for(i=0; i < nv; i++){
		infile >> vx[i] >> vy[i] >> vz[i];
		if(vx[i] < tox && vx[i] > fromx && \
		   vy[i] < toy && vy[i] > fromy && \
		   vz[i] < toz && vz[i] > fromz)
			{
			subindex[i] = numtotal;
			vflags[i] = 1;
			numtotal ++;
			}
		else vflags[i] = 0;	
	}	

	
	//read in triangles and add to mesh
	for(i=0; i < nt; i++){
		infile >> temp >> idx1 >> idx2 >> idx3;
		if(vflags[idx1] == 1 ||vflags[idx2] == 1 ||vflags[idx3] == 1)
			{
			tflags[i] = 1;
			triindex[tritotal][0] = subindex[idx1];
			triindex[tritotal][1] = subindex[idx2];
			triindex[tritotal][2] = subindex[idx3];
			tritotal++;
			}
		else tflags[i] =0;		
	}		
	
	outfile << "OFF" << endl;
	sprintf(entry, "%d %d 1", vtotal, tritotal);
	outfile << entry <<endl;
	for (i=0; i<nv; i++)
		{
		if(vflags[i] == 1)
			{
			sprintf(entry, "%f %f %f ", vx[i], vy[i],vz[i]);
			outfile <<entry<<endl;
		}
		}
	
	for (i=0;i<tritotal;i++)
		{
		sprintf(entry, "3 %d %d %d ", triindex[i][0],  triindex[i][1], triindex[i][2]);
		outfile <<entry<<endl;
		}
	outfile.close();	
	infile.close();
	
	delete [] vx;
	delete [] vy;
	delete [] vz;
	delete [] subindex;
};
