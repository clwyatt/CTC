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
	std::slist<int> * indexlist = new (std::slist<int>)[nv]; 
	std::slist<int>::iterator iter; 
	int vtotal = 0;
	int * subindex = new int[nv];
	int * processed = new int[nv];
	int * subtotal = new int[nv];
	int nprocessed = 0;
        int triindex[nt][3];

	infile.getline(entry, 100);
	
	vx = new float[nv];
	vy = new float[nv];
	vz = new float[nv];
	
	if (vx==NULL ||vy==NULL||vz==NULL ||subindex==NULL||processed
	==NULL||subtotal ==NULL)
		{
		cout <<"can not load memory \n" <<endl;
		exit(1);
		}
	float a, b ,c;
	//read in vertices
	for(i=0; i < nv; i++){
		infile >> vx[i] >> vy[i] >> vz[i] >> a >> b>> c;
		subtotal[i]=0;
		processed[i]=-1;
#ifdef OOGL_IN_VOXEL_UNIT
		vx[i] = vx[i]*dx +xorig;
		vy[i] = vy[i]*dy +yorig;
		vz[i] = vz[i]*dz +zorig;			
#endif		
	}	
	printf(" good ,here !");
	int num = 0;
	int found = 0;
	for (i =0;i<nv; i++)
		{
		found = 0;
		for( ii = 0;ii<nprocessed; ii++) 
			{
			if (i == processed[ii])
				{
				found = 1;
				break;
				}
			}
		if(!found)
			{
			subindex[vtotal]= i;
			indexlist[vtotal].push_front(i);		
			for(int j = i+1; j < nv; j++)
				{
				if(((vx[i] - vx[j])*(vx[i] - vx[j])+ \
					(vy[i]-vy[j])*(vy[i]-vy[j]) + \
					(vz[i]-vz[j])*(vz[i]-vz[j]))<=tol) 
					{
					indexlist[vtotal].push_front(j);		
					processed[nprocessed] = j;
					nprocessed++;
					}
				}
			vtotal++;
			}			
		}		
	cout << "non duplicated vertices num"<< num<<endl;


	//read in triangles and add to mesh
	for(i=0; i < nt; i++){
		found = 0;
		infile >> temp >> idx1 >> idx2 >> idx3;
		for (j = 0;j<vtotal; j++)
			{
			iter = indexlist[j].begin();
			while(iter!= indexlist[j].end())
			     {
			     if(idx1 == (*iter)) idx1 = j;
			     if(idx2 == (*iter)) idx2 = j;
			     if(idx3 == (*iter)) idx3 = j;
			     iter++;
				}
			}	
		if(idx1==idx2||idx2==idx3||idx3==idx1) continue;
		for(ii = 0; ii<tritotal; ii++)
			{
			if(idx1 == triindex[ii][0] &&\
			   idx2 == triindex[ii][1] &&\
			   idx3 == triindex[ii][2]) 
				{
				found = 1;
				break;
				}
			}	
		if(!found)
			{
			triindex[tritotal][0] = idx1;
			triindex[tritotal][1] = idx2;
			triindex[tritotal][2] = idx3;
			tritotal++;
			}
		}		

	outfile << "OFF" << endl;
	sprintf(entry, "%d %d 1", vtotal, tritotal);
	outfile << entry <<endl;
	for (i=0; i<vtotal; i++)
		{
		sprintf(entry, "%f %f %f ", vx[subindex[i]], vy[subindex[i]],vz[subindex[i]]);
		outfile <<entry<<endl;
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
	delete [] indexlist;
};
