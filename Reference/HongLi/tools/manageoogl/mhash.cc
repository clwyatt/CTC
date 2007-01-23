#include <slist.h>
#include <fstream>
#include <cstdio>
#include <math.h>
#define tol 0.001

#define TABLESIZE  224743
#define TOP 50


typedef struct hashelement
{
int index;
float x;
float y;
float z;
} HASHELEMENT;


typedef struct hashslot
{
int size;
HASHELEMENT * helements;
} HASHSLOT;

main(int argc, char *argv[])
{
static double A = (sqrt(5)-1)/2;
HASHSLOT * slots;
if (argc <3) /* Read parameters*/
{
	cout << "Usage : mhesh pre_oogl_file out_oogl_file  normflag colorflag"<<endl;
	cout << "  	normflag 1/0 ---- w or w/o normal direction"<<endl;
	cout << "	colorflag 1/0 ----- w or w/o color information"<<endl;
	exit(1);
	}
ofstream  outfile;
ifstream  infile;
char entry[256];
int i, ii,j , nv, nt, temp ;
int searchto = -1;
outfile.open(argv[2]);
infile.open(argv[1]);
int normflag = atoi(argv[3]);
int colorflag = atoi(argv[4]);
HASHELEMENT * cur;

slots = (HASHSLOT *) malloc( TABLESIZE * sizeof(HASHSLOT));
if(slots == NULL) {
	cout<<"Not enough memory " <<endl;
	exit(1);
	}

for (i = 0; i < TABLESIZE; i++) 
	{
	slots[i].helements =  (HASHELEMENT *) malloc( TOP * sizeof(HASHELEMENT));
	if(slots[i].helements == NULL) {
		cout<<"Not enough memory for elements" <<endl;
		exit(1);
		}
	slots[i].size = TOP;	
	cur = slots[i].helements;	
	for(j = 0; j < slots[i].size; j++)	
		{
		cur->index = -1;
		cur ++;
		}
	}	

if(!infile){
	cerr << "Error in ooglread(...): cannot open file" << endl;
	exit(-1);
	}
	
infile.getline(entry, 100);
infile.getline(entry, 100);

	infile >> nv >> nt >> temp;	
	clog << nv << " vertices, " << nt << " triangles" << endl;
	int vtotal = 0;
	int ** subindex = (int **) malloc(nv * sizeof(int *));
	if(subindex == NULL) 
		{
		cout<< "Not enough memry "<< endl;
		exit(1);	
		}
	infile.getline(entry, 100);

	double k;	
	float a, b ,c, d, e, f;
	int num = 0;
	//read in vertices
	int total = 0;
	int top = TOP;
	
	for(i=0; i < nv; i++){
		if(normflag)
			infile  >> a >> b >> c >> d >> e >> f;
		else
			infile  >> a >> b >> c;
		subindex[i] = new int[2];
		if( subindex[i] == NULL)
			{
			cout<< "Not enough memry "<< endl; 
			exit(1);	
			}
		k = a+b+c;
		k = k*A;
		num = (int)(TABLESIZE*(k - floor(k)));
		cur = slots[num].helements;
		for (j = 0 ;j<slots[num].size; j++)
			{
			if(cur->index == -1)
				{
				cur->index = 0;
				cur->x = a;
				cur->y = b; 
				cur->z = c;
				subindex[i][0] = num;
				subindex[i][1] = j;
				total ++;
				break;
				}
			else {
				if(cur->index == 0 && cur->x ==a && cur->y == b && cur->z == c)
				{
					subindex[i][0] = num;
					subindex[i][1] = j;
					
					break;			
					}
				}	
			cur ++;	
			}
		if( j == slots[num].size) // over flow
		 	{
			cout << "Expand the memory for slot " << num <<endl;
			slots[num].helements =  (HASHELEMENT *) realloc(slots[num].helements, (slots[num].size + TOP) * sizeof(HASHELEMENT));
			if(slots[num].helements == NULL) {
				cout<<"Not enough memory " <<endl;
				exit(1);
			}
			cur = slots[num].helements;	
			int jj;
			for( jj = 0; jj < slots[num].size; jj++)	
				cur ++;
				
			cur->index = 0;
			cur->x = a;
			cur->y = b; 
			cur->z = c;
			subindex[i][0] = num;
			subindex[i][1] = j;
			total ++;
			jj +=1;
				
			slots[num].size += TOP;		
			for(;jj < slots[num].size; jj++)	
				{
				cur->index = -1;
				cur ++;
				}
				

			}
		cout<< (int)(i*100/nv) << "\% \r";
		}			
		
	//arrange index and start output
	cout <<"total is "<< total <<endl;
	cout << "Finished arrange, start writing... "<<endl;
int idx1, idx2,idx3;
	outfile << "appearance { shading flat }" << endl;
	outfile << "OFF" << endl;
	long pos = outfile.tellp();
	sprintf(entry, "%d %10d 1", total, nt);
	outfile << entry <<endl<<endl;
	total = 0;
	for (i = 0; i < TABLESIZE; i++)
		{
		cur = slots[i].helements;
		for (j = 0 ;j<slots[i].size; j++)
			{
			if(cur->index ==0)
				{
				cur->index = total ++;
				sprintf(entry, "%f %f %f ",cur->x, cur->y,cur->z);
				outfile <<entry<<endl;
				}
			else
				break;
			
			cur++;
			}
		}

	//read in triangles and add to mesh
	infile.getline(entry, 100);
	for(i=0; i < nt; i++){
		if (colorflag)
		{
		float r,g,b,o;
			infile >> temp >> idx1 >> idx2 >> idx3 >> r >> g >> b >> o;
			sprintf(entry, "3 %d %d %d %f %f %f",
				slots[subindex[idx1][0]].helements[subindex[idx1][1]].index, 
				slots[subindex[idx2][0]].helements[subindex[idx2][1]].index, 
				slots[subindex[idx3][0]].helements[subindex[idx3][1]].index,
				r,g,b,o); 		
		}
		else
		{
			infile >> temp >> idx1 >> idx2 >> idx3;
			sprintf(entry, "3 %d %d %d ",
				slots[subindex[idx1][0]].helements[subindex[idx1][1]].index, 
				slots[subindex[idx2][0]].helements[subindex[idx2][1]].index, 
				slots[subindex[idx3][0]].helements[subindex[idx3][1]].index); 
		}
		outfile <<entry<<endl;
		cout<< (int)(i*100/nt) <<"\%\r";
			}	
					
	outfile.seekp(pos);
	sprintf(entry, "%d %10d 1", total, nt);
	outfile << entry <<endl<<endl;

	//release momory				
	for (i = 0; i < TABLESIZE; i++)
		free(slots[i].helements);
	free(slots);
	
	for(i=0; i <nv; i++) delete subindex[i];
	free(subindex);	
		
	outfile.close();	
	infile.close();
	
};
