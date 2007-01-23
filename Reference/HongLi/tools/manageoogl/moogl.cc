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
outfile.open(argv[2]);
infile.open(argv[1]);
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
	
	vx = new float[nv];
	vy = new float[nv];
	vz = new float[nv];
	
	if (vx==NULL ||vy==NULL||vz==NULL)
		{
		cout <<"can not load memory \n" <<endl;
		exit(1);
		}
	float a, b ,c;
	//read in vertices
	for(i=0; i < nv; i++){
		infile >> vx[i] >> vy[i] >> vz[i] >> a >> b>> c;
		subindex[i]=-1;
	//	 cout << vx[i] <<" " <<vy[i]<<" " <<vz[i]<<" " <<a<<" " <<b<<" " <<c<<endl;
		cout<< (int)i <<" / "<<nv<<"\r";	
	}	
	
	printf(" file loaded \n!");
	int num = 0;
	int found = 0;
	j = 0;
	for (i =0;i<nv; i++)
		{
		if(subindex[i] == -1)
		{
		  subindex[i] = num ++;
		  for( ii = i+1;ii<nv; ii++) 
			{
			if (subindex[ii] == -1)
				{
				if((vx[i] == vx[ii]) && (vy[i]==vy[ii]) && (vz[i]==vz[ii])) 					
					{	subindex[ii] = subindex[i];
						}
				}
			}			
		  }
		cout<< (int)i <<" / "<<nv<<"\r";
		  
		}		
		cout <<endl;
	cout << "non duplicated vertices number is "<< num<<endl;

	outfile << "appearance { shading flat }" << endl;
	outfile << "OFF" << endl;
	sprintf(entry, "%d %d 1", num, nt);
	outfile << entry <<endl<<endl;
	num = -1;
	for (i=0; i<nv; i++)
		{
		if(subindex[i] > num)
			{
			sprintf(entry, "%f %f %f ", vx[i], vy[i],vz[i]);
			outfile <<entry<<endl;
			num = num +1;
			}
		}
	
	//read in triangles and add to mesh
	for(i=0; i < nt; i++){
		infile >> temp >> idx1 >> idx2 >> idx3;
		sprintf(entry, "3 %d %d %d ", subindex[idx1], subindex[idx2], subindex[idx3]);
		outfile <<entry<<endl;
		}
	outfile.close();	
	infile.close();
	
	delete [] vx;
	delete [] vy;
	delete [] vz;
	delete [] subindex;
};
