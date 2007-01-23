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
	
	vx = new float[nv/5];
	vy = new float[nv/5];
	vz = new float[nv/5];

	
	if (vx==NULL ||vy==NULL||vz==NULL)
		{
		cout <<"can not load memory \n" <<endl;
		exit(1);
		}
	float a, b ,c, d, e, f;
	int num = -1;
	//read in vertices
	for(i=0; i < nv; i++){
		infile  >> a >> b >> c >> d >> e >> f;
		for(j = num ;j >= 0 ; j--)
			{
				if((a == vx[j]) && (b==vy[j]) && (c==vz[j])) 					
					{	subindex[i] = j;
						break;
						}
			}
		if (j < 0)
		  {
		  subindex[i] = ++num;
		  vx[num]=a, vy[num]=b, vz[num]=c ;
		  }	 
		//cout<< (int)i <<" / "<<nv<<" "<<num<<"\r";	
	}  							 


	
	
	
	
	printf(" file loaded \n!");
	
	cout <<endl;
	cout << "non duplicated vertices number is "<< num<<endl;

	outfile << "appearance { shading flat }" << endl;
	outfile << "OFF" << endl;
	sprintf(entry, "%d %d 1", num+1, nt);
	outfile << entry <<endl<<endl;
	for (i=0; i<=num; i++)
		{
			sprintf(entry, "%f %f %f ", vx[i], vy[i],vz[i]);
			outfile <<entry<<endl;
		}
	
	//read in triangles and add to mesh
	infile.getline(entry, 100);
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
