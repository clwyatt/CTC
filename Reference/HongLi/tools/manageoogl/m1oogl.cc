#include <slist.h>
#include <fstream>
#include <cstdio>
#include <math.h>
#define tol 0.001

main(int argc, char *argv[])
{
if (argc <5) /* Read parameters*/
{
	cout << "Usage : m1oogl pre_oogl_file oogl_file  normflag judgeflag  ratio [searchto]" << endl;
	cout << "	normflag : 1/0 w/o normal dir " << endl;
	cout << "	judgeflag : 1/0 judge by equality / by distance " << endl;
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
int judgeflag = atoi(argv[4]);
float ratio = atof(argv[5]);
if(argc == 7) searchto = atoi(argv[6]);

cout << "Managing \"" << argv[1] <<"\" to \"" <<argv[2]<<"\" "<<endl; 

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

	int memsize = (int) (nv * ratio) ;
	vx = new float[memsize];
	vy = new float[memsize];
	vz = new float[memsize];
	
	
	if (vx==NULL ||vy==NULL||vz==NULL)
		{
		cout <<"can not load memory \n" <<endl;
		exit(1);
		}
	float a, b ,c, d, e, f;
	int num = -1;
	int end;
	//read in vertices
	if ( judgeflag == 1){
	cout <<"flag " <<judgeflag <<endl;
	for(i=0; i < nv; i++){
		if(normflag)
			infile  >> a >> b >> c >> d >> e >> f;
		else
			infile  >> a >> b >> c;
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
		  if (searchto != -1)
		  {
		  	if (num > searchto + 2000) 
		  	{
				 i++;
			 	break;
			}
		  }			
		}	 
		cout<< (int)i <<" / "<<nv<<" "<<num<<"\r";	
	}  							 


	//read in vertices
	if(searchto != -1)
	{
	    for(end = num-searchto;i < nv; i++){
	    	if(normflag)
			infile  >> a >> b >> c >> d >> e >> f;
	    	else
			infile  >> a >> b >> c;
		for(j = num ;j >= end ; j--)
			{
				if((a == vx[j]) && (b==vy[j]) && (c==vz[j])) 					
					{	subindex[i] = j;
						break;
						}
			}
		if (j < end)
		  {
		  subindex[i] = ++num;
		  vx[num]=a, vy[num]=b, vz[num]=c ;
		  cout<< (int)i <<" / "<<nv<<" "<<num<<"\r";	
		  end ++;
		  }	 
	    }  							 
	}
	}
	else
	{
	for(i=0; i < nv; i++){
		if(normflag)
			infile  >> a >> b >> c >> d >> e >> f;
		else
			infile  >> a >> b >> c;
		for(j = num ;j >= 0 ; j--)
			{
				if((a - vx[j])*(a - vx[j]) + 
					(b-vy[j]) *(b-vy[j]) + 
					(c-vz[j]) * (c-vz[j]) < tol) 					
					{	subindex[i] = j;
						break;
						}
			}
		if (j < 0)
		{
		  subindex[i] = ++num;
		  vx[num]=a, vy[num]=b, vz[num]=c ;
		  if (searchto != -1)
		  {
		  	if (num > searchto + 2000) 
		  	{
				 i++;
			 	break;
			}
		  }			
		}	 
		cout<< (int)i <<" / "<<nv<<" "<<num<<"\r";	
	}  							 


	//read in vertices
	if(searchto != -1)
	{
	    for(end = num-searchto;i < nv; i++){
	    	if(normflag)
			infile  >> a >> b >> c >> d >> e >> f;
	    	else
			infile  >> a >> b >> c;
		for(j = num ;j >= end ; j--)
			{
				if((a - vx[j])*(a - vx[j]) + 
					(b-vy[j]) *(b-vy[j]) + 
					(c-vz[j]) * (c-vz[j]) < tol) 					
					{	subindex[i] = j;
						break;
						}
			}
		if (j < end)
		  {
		  subindex[i] = ++num;
		  vx[num]=a, vy[num]=b, vz[num]=c ;
		  cout<< (int)i <<" / "<<nv<<" "<<num<<"\r";	
		  end ++;
		  }	 
	    }  							 
	}
	}	

	cout <<endl;
	cout << "non duplicated vertices number is "<< num<<endl;

	outfile << "appearance { shading flat }" << endl;
	outfile << "OFF" << endl;
	long pos = outfile.tellp();
	sprintf(entry, "%d %10d 1", num+1, nt);
	outfile << entry <<endl<<endl;
	for (i=0; i<=num; i++)
		{
			sprintf(entry, "%f %f %f ", vx[i], vy[i],vz[i]);
			outfile <<entry<<endl;
		}
	int trinum = 0 ;
	//read in triangles and add to mesh
	infile.getline(entry, 100);
	for(i=0; i < nt; i++){
		infile >> temp >> idx1 >> idx2 >> idx3;
		if (subindex[idx1] == subindex[idx2] || 
			subindex[idx1] == subindex[idx3] || 
			subindex[idx3] == subindex[idx2])
			continue;
		sprintf(entry, "3 %d %d %d ", subindex[idx1], subindex[idx2], subindex[idx3]);
		outfile <<entry<<endl;
		trinum ++;
		}
	outfile.seekp(pos);
	sprintf(entry, "%d %10d 1", num+1, trinum);
	outfile << entry <<endl<<endl;
		
	outfile.close();	
	infile.close();
	
	delete [] vx;
	delete [] vy;
	delete [] vz;
	delete [] subindex;
};
