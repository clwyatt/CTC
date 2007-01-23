#include "DataIO.hh"
#include <slist>
#include <stack>
#include <stdio.h>
#include "mvVec3i.hh"
#include "mvVec3f.hh"
#include "MarchingCube.hh"
#include "Triangle_with_Features.hh"
#include "LabeledVolume.hh"

#define MAX_TRIANGLES 2500000
/* for research purpose, I would add another stack to remember how many segments we have for each colon
and how is the parameters for each segments, in order to remove the stomach. Also the statistics on
seeds efficiecy will be shown */
#define RESEARCH
//#undef RESEARCH


typedef struct _colormap {
float r;
float g;
float b;
float o;
} ColorMap;

ColorMap colormap[10]; // assume less than 10 segments for a single colon


void WriteOFF(std::slist<Triangle *> TriList, const char * filename , 
	float xdim, float ydim, float zdim, float xorigin, float yorigin ,float zorigin, int color);
 

int main (int argc, char *argv[])
{
int i,j,k, ii;
int error;
int totaltri = 0;
bool mask = false;
if (argc != 5) /* Read parameters*/
{
	cout << "Usage : segment rvf_file seeds_file threshold" << endl;
	exit(1);
	}
LabeledVolume<unsigned short> volume; 
readRVF(argv[1], volume ,mask,   error);
volume.AllocLabels();
cout << "read in RVF !!"<< endl;
int sum = 0;
int x,y,z;
/* simple 3D segmentation is from gere */
ifstream seedfile;
seedfile.open(argv[2]);
float threshold = atof(argv[3]);
int above = atoi(argv[4]);
cout <<"Threshold is :"<<threshold <<endl; 
mvVec3i s;
std::stack <mvVec3i> openStack;


colormap[1].r = 1.0, colormap[1].g=0.0, colormap[1].b=0.0, colormap[1].o = 1.0;
colormap[2].r = 0.0, colormap[2].g=1.0, colormap[2].b=0.0, colormap[2].o = 1.0;
colormap[3].r = 0.0, colormap[3].g=0.0, colormap[3].b=1.0, colormap[3].o = 1.0;
colormap[4].r = 1.0, colormap[4].g=1.0, colormap[4].b=0.0, colormap[4].o = 1.0;
colormap[5].r = 1.0, colormap[5].g=0.0, colormap[5].b=1.0, colormap[5].o = 1.0;
colormap[6].r = 0.0, colormap[6].g=1.0, colormap[6].b=1.0, colormap[6].o = 1.0;
colormap[7].r = 0.5, colormap[7].g=0.0, colormap[7].b=0.0, colormap[7].o = 1.0;
colormap[8].r = 0.0, colormap[8].g=0.5, colormap[8].b=0.0, colormap[8].o = 1.0;


#ifdef RESEARCH 
std::stack <mvVec3i> seedsStack;
unsigned char segment;
bool firstpoint = true;
mvVec3f firstvoxel;
int maxz = 0;
#endif 

while(!seedfile.eof())
{
	seedfile >> x >> y >> z;
	cout << x << " " << y << " " << z<<endl;
        // get the seed first 
        s = mvVec3i(x,y,z);
#ifdef RESEARCH 
	seedsStack.push(s);
#else	
	openStack.push(s);
#endif	
}


IMTK::Subscript<3> dim = volume.getBounds();
unsigned short v;

float xdim, ydim, zdim;
float xorigin, yorigin, zorigin;

volume.getVoxelSpacing(xdim, ydim, zdim);
volume.getOrigin(xorigin, yorigin, zorigin);

cout << "xdim " << xdim << "ydim " << ydim << "zdim " << zdim <<endl;
cout << "xo " << xorigin << "yo " << yorigin<< "zo " << zorigin <<endl;

MarchingCube mCube;
std::slist<Triangle *> tlist, TriList;
std::slist<Triangle *>::iterator tlistIter;
Point p;
mvVec3i a, b, seed;

bool overlimit = false;
#ifdef RESEARCH 
ofstream seedr; //seeds statistics file
seedr.open("/nrc/capd/work/Newdata/seedResult.txt",  ofstream::out | ofstream::app);


int distribute[20] ;
float maxdistance[20]; // the maximum distance from first bundary voxel to all the other boundaries
for (i = 0; i < 20; i++) 
	{
	distribute[i] = 0;
	maxdistance[i] = 0;
	}
segment = 0;
while(!seedsStack.empty())
{
seed = seedsStack.top();
seedsStack.pop();
sum = 0;
totaltri = 0;
overlimit = false;
int index = volume.Label(seed.x, seed.y,seed.z) ;
if( index == 0)
	{
	segment += 1;
	cout << "the " <<(int) segment<< "th segment" <<endl; 
	cout <<seed.x << " "<<seed.y <<" "<< seed.z <<endl;
	distribute[(int) segment] += 1;
	volume.Label(seed.x, seed.y,seed.z, (unsigned char)segment);
	openStack.push(seed);
	firstpoint = true;
	}
else
	{
	distribute[(int) index] += 1;	
	cout <<seed.x << " "<<seed.y <<" "<< seed.z <<endl;
	continue;
	}	

#endif
while(!openStack.empty())
        {
        a = openStack.top();
        openStack.pop();
//	outfile << a.x <<" "<<a.y<<" "<<a.z<< " "<<endl;
        sum = sum+1;
	cout << "\r" << sum<< " " <<totaltri;
	if( totaltri > MAX_TRIANGLES && (!overlimit)) 
		{
		cout << endl << "Segment over limit; " << openStack.size() << "voxels left in stack. "<< endl;
		overlimit = true;
		//exit(0);
		} 
	unsigned int ux, uy, uz;
        for (i = -1; i<=1; i++)
                {
                for (j=-1;j<=1; j++)
                        {
                        for (k=-1; k <=1; k++)
                                {
                                if (a.x + i < 0 ||a.y + j < 0 || a.z + k < 1 ) //jump over the first black slice 
                                        continue;
                                if (a.x + i>= (int)dim[0]-1 ||a.y + j >= (int)dim[1]-1 || a.z + k>= (int)dim[2]-1)
                                        continue;	
				ux = (unsigned int) (a.x + i);
				uy = (unsigned int) (a.y + j);
				uz = (unsigned int) (a.z + k);
				int label = (int)volume.Label(ux, uy,uz);
				if(label != 0 ) continue;

				b = mvVec3i(ux,uy,uz);		

				mCube = MarchingCube(b);
				mCube.setThreshold(threshold); 	
				mCube.setThrFlag(above);	
				for (ii=0;ii<8;ii++)
				{
					p = mCube.getPosition(ii);
					if(p.x < 0 || p.y < 0 || p.z < 0 || p.x > dim[0]-1||p.y >
						dim[1]-1 || p.z > dim[2]-1)
						break;	
					float va = (float) volume.getVoxel((unsigned int)p.x, (unsigned int)p.y,(unsigned int)p.z);
					mCube.setValue(ii, va); 
				}
				if(ii==8) //over the loop and all inside volume	
				{
					if(mCube.isoPointExist())
					{
					mCube.getIsoPoints();
					tlist = mCube.getTriangles();
					//getchar();
					tlistIter = tlist.begin();
					while(tlistIter != tlist.end())
						{
						TriList.push_front(*tlistIter);
						tlistIter ++;
						}	
					totaltri += tlist.size();
					//openStack.push(mvVec3i(a.x+i, a.y+j, a.z+k)); 					
                                	//volume.Label(ux, uy, uz, (char) 1);
					//continue;			
#ifdef RESEARCH
					float cur_dist;
					mvVec3f currentvoxel;
					if( firstpoint ) {
						firstvoxel = mvVec3f( (float) ux, (float)uy, (float) uz);
						firstpoint = false;
						maxz = uz; 
						}
					else 
						{
						currentvoxel = mvVec3f( (float) ux, (float)uy, (float) uz);
						cur_dist = firstvoxel.get_Distance(currentvoxel);
						if(cur_dist > maxdistance[(int)segment]) maxdistance[(int)segment] = cur_dist;
						if(maxz < uz) maxz = uz; 
						} 					
#endif								
					}
				}
                                if(i==0 && j==0 && k==0) continue;
				if( overlimit) continue;
				
				v = 	volume.getVoxel(ux, uy,uz);
                                if(v <= threshold) 				
					{
					if(above==0)						
                                        	{
						openStack.push(mvVec3i(a.x+i, a.y+j, a.z+k)); 
					//	cout <<" Added one " <<endl;
						}
					}	
				else
					{
					if(above==1)						
                                        	{
						openStack.push(mvVec3i(a.x+i, a.y+j, a.z+k)); 
					//	cout <<" Added one " <<endl;
						}
					}		
				//openStack.push(mvVec3i(a.x+i, a.y+j, a.z+k)); 
#ifdef RESEARCH 				
                                volume.Label(ux, uy, uz, (unsigned char) segment);
#else
                                volume.Label(ux, uy, uz, (unsigned char) 1);
#endif				
                                } //end of for loop k
                        } // end of for loop j
                }  // end of for loop i 
        } // end of while loop         
#ifdef RESEARCH 

 
//cout << "Segment "<< (int) segment << " has "<< sum << " voxel." << endl;
//cout << "        has " << distribute[(int)segment] << "seeds"<<endl;
//cout << "	 max distance "<< maxdistance[(int)segment] << endl;

seedr << "Segment "<< (int) segment << " has "<< sum << " voxel." << endl;
seedr << "	 max distance "<< maxdistance[(int)segment] << endl;
seedr << " 	 maxz " << maxz << " zsize " << dim[2] << endl;

if( maxz < dim[2] /2 && maxdistance[segment] < 85 || TriList.size() < 50)
	{
	cout << "segment " << (int)segment <<" is in stomach " << endl;
	cout << " Because " << maxz << " smaller than " << dim[2] << endl;
	} 
else	
	{
	char filename[80];
	sprintf( filename, "oogl%d.oogl", (int)segment);
	WriteOFF(TriList, filename, xdim, ydim, zdim,xorigin,yorigin ,zorigin, (int) segment); 
	}
TriList.clear(); 
} // end of for loop of a seeds

#endif

cout << endl<<totaltri <<endl;     

#ifndef RESEARCH
WriteOFF(TriList, "ooglnew.oogl", xdim, ydim, zdim,xorigin,yorigin ,zorigin, 0);
for (i =1 ; i < (int)segment; i++)
	seedr << i<< "the segment takes " << distribute[i] << "seeds"<<endl;
seedr.close();
#endif
			

}



void WriteOFF(std::slist<Triangle *> TriList, const char * filename , 
	float xdim, float ydim, float zdim, float xorigin, float yorigin ,float zorigin, int color)
{
ofstream  outfile;
std::slist<Triangle *>::iterator tlistIter;
outfile.open(filename);
char entry[80];
long totaltri = TriList.size();  
outfile << "appearance { shading flat }\n";
outfile << "OFF\n";

sprintf(entry, "%i %i 0\n\n", totaltri*3, totaltri);
outfile << entry;
int vindex = 0;
tlistIter = TriList.begin();
while(tlistIter != TriList.end())
	{
	if(zdim * xdim * ydim < 0)
		{
		outfile <<
			((*tlistIter)->getV2())->getPX()*xdim + xorigin <<" "<<
			((*tlistIter)->getV2())->getPY()*ydim + yorigin <<" "<<
			((*tlistIter)->getV2())->getPZ()*zdim + zorigin <<endl;
		outfile << 
			((*tlistIter)->getV1())->getPX()*xdim + xorigin <<" "<<
			((*tlistIter)->getV1())->getPY()*ydim + yorigin <<" "<<
			((*tlistIter)->getV1())->getPZ()*zdim + zorigin <<endl;
		}
	else
		{
		outfile << 
			((*tlistIter)->getV1())->getPX()*xdim + xorigin <<" "<<
			((*tlistIter)->getV1())->getPY()*ydim + yorigin <<" "<<
			((*tlistIter)->getV1())->getPZ()*zdim + zorigin <<endl;
		outfile <<
			((*tlistIter)->getV2())->getPX()*xdim + xorigin <<" "<<
			((*tlistIter)->getV2())->getPY()*ydim + yorigin <<" "<<
			((*tlistIter)->getV2())->getPZ()*zdim + zorigin <<endl;
		}
				
	outfile <<
		((*tlistIter)->getV3())->getPX()*xdim + xorigin <<" "<<
		((*tlistIter)->getV3())->getPY()*ydim + yorigin <<" "<<
		((*tlistIter)->getV3())->getPZ()*zdim + zorigin <<endl;
		tlistIter ++;			
	}	
outfile <<endl;
for (int i = 0;i<totaltri; i++)
	{
	outfile<<"3 "<<vindex ++<<" ";
	outfile<<vindex ++<<" ";
	outfile<<vindex ++<<" ";
	if (color == 0)
		outfile<<endl;
	else
		{
		outfile << colormap[color].r << " ";
		outfile << colormap[color].g << " ";
		outfile << colormap[color].b << " ";
		outfile << colormap[color].o << endl;
		}			
	}
outfile.close();
	
}
