#include "DataIO.hh"
#include <slist>
#include <stack>
#include <stdio.h>
#include "mvVec2i.hh"
#include "mvVec2f.hh"
#include "mvVec3i.hh"
#include "mvVec3f.hh"
#include "nrvector.hh"
#include "Image.hh"
#include "Subscript.hh"
#include "hotelling.hh"
#include "Triangle_with_Features.hh"
#include "LabeledVolume.hh"


#define ANALYSE


int main (int argc, char *argv[])
{
unsigned int i,j,k, ii;
int error;
int totaltri = 0;
bool mask = false;

#ifdef ANALYSE
ofstream  analysefile;
analysefile.open("/nrc/capd/work/Newdata/seedsinfo.dat",ofstream::out | ofstream::app);
#endif

if (argc != 4) /* Read parameters*/
{
	cout << "Usage : getseeds rvf_file threshold above" << endl;
	exit(1);
	}
LabeledVolume<unsigned short> volume; 
readRVF(argv[1], volume ,mask,   error);
volume.AllocLabels();
cout << "read in RVF !!"<< endl;
int sum = 0;
int x,y,z;
float threshold = atof(argv[2]);
int above = atoi(argv[3]);
cout <<"Threshold is :"<<threshold <<endl; 

IMTK::Subscript<3> dim = volume.getBounds();
unsigned short v;

float xdim, ydim, zdim;
float xorigin, yorigin, zorigin;

volume.getVoxelSpacing(xdim, ydim, zdim);
volume.getOrigin(xorigin, yorigin, zorigin);

//cout << "xdim " << xdim << "ydim " << ydim << "zdim " << zdim <<endl;
//cout << "xo " << xorigin << "yo " << yorigin<< "zo " << zorigin <<endl;
ofstream  outfile, logfile, cath;
outfile.open( "seeds.dat" );
logfile.open( "log.dat" );
cath.open("cath.dat");

k = 0;
unsigned int  index = 0;
IMTK::Subscript<3>  theSub;
theSub[0] = 0;
theSub[1] = 0;
IMTK::Subscript<2> bounds2d;
bounds2d[0] = dim[0];
bounds2d[1] = dim[1];
unsigned short * p;	

std::stack <mvVec2i> openStack;
std::stack <mvVec2i> closeStack;
mvVec2i s;
float xsum; 
float ysum;
mvVec2i a, b;
IMTK::Image <unsigned short> * image =new IMTK:: Image<unsigned short> (bounds2d);

mvVec2f * vList = new mvVec2f[10000];

float **eigenvectors, *eigenvalues;

eigenvalues = nrvector<float>(1,2);
eigenvectors = matrix<float>(1,2,1,2);

for(k =0; k< dim[2]; k = k + 5)
        {
	theSub[2] = k;
	index = theSub.getIndex(dim);
	p = volume.begin()+index;
	image->fill(0);
	for ( i = 0 ; i < bounds2d[1]; i ++)
		 for ( j = 0; j < bounds2d[0]; j ++)
		     {
		     sum = 0; 
		     xsum = ysum = 0;
		    
		     if(  *(p + j + i * bounds2d[0]) >  threshold || image->getPixel(j,i) == 1)
		       {
		       //nothing;
		     	}
		     else
		     {
		     
		     //1. put seed into open stack; 
        		s = mvVec2i(j,i);
			openStack.push(s);
		     
		     //2. region grow;
			while(!openStack.empty())
			{
			   a = openStack.top();
                           openStack.pop();
				//	outfile << a.x <<" "<<a.y<<" "<<a.z<< " "<<endl;
	                   //cout << "\r" << sum;
                          // closeStack.push(a);
			   
			   xsum += (float) a.x;
			   ysum += (float) a.y;
			   if(sum < 10000)
			   	{
				vList[sum].x = (float) a.x;
			   	vList[sum].y = (float) a.y;
       				}                    	
			   sum = sum+1;
			   
                           //find neighbors of a
	                   unsigned int ux, uy;
                           for (int ii = -1; ii<=1; ii++)
                           {
                              for (int jj=-1;jj<=1; jj++)
                                 {
				
                                 if (a.x + ii < 0 ||a.y + jj < 0) 
                                        continue;
                                 if (a.x + ii>= (int)dim[0]-1 ||a.y + jj >= (int)dim[1]-1)
                                        continue;
				 ux = (unsigned int) (a.x + ii);
				 uy = (unsigned int) (a.y + jj);

				
				 if(image->getPixel(ux, uy ) == 1) continue;
                                 
				 if(*(p + ux + uy * bounds2d[0]) <= threshold) 				
					{
					if(above==0)						
                                        	{
						openStack.push(mvVec2i(a.x+ii, a.y+jj)); 
					//	*(p + ux + uy * bounds2d[0]) = 3000;
					//	cout <<" Added one " <<endl;
						}
					}	
				else
					{
					if(above==1)						
                                        	{
						openStack.push(mvVec2i(a.x+ii, a.y+jj)); 
					//	*(p + ux + uy * bounds2d[0]) = 3000;
					//	cout <<" Added one " <<endl;
						}
					}		
				//openStack.push(mvVec3i(a.x+i, a.y+j, a.z+k)); 
                                image->setPixel(ux, uy, (char) 1);
 				 
					
				 } //end of jj loop
			   } // end of ii loop	 	
		     } // end of while loop
		     
		     //3. whether this region is a Colon region
		     if(sum > 1000 && ((sum < 6500 && k < dim[2] - 100) || (sum < 7500 && k >= dim[2] - 100)))
		     	{
		       /* call hotelliing algorithm to get the eigenvectors and eigenvalues
			   */
			  
			  hotelling(vList, sum, eigenvalues, eigenvectors);
			  float mn = min(fabs(eigenvalues[1]), fabs(  eigenvalues[2]));
			  float mx = max(fabs(eigenvalues[1]), fabs(  eigenvalues[2]));
			  int xx = (int) ( xsum / (float) sum);
			  int yy = (int) (ysum/(float) sum);
			     logfile << "ev : " << mx << " " << mn <<" sum :" <<sum <<"Pos: " <<xx<<", "<<yy<<", "<<k <<endl;
			  if( 	3.1415926 * sqrt(mn * mx) * 3.5 <  sum 
			  	&& sqrt(mn) > 10 
				&& fabs(mx/mn) < 3
				&& mn < 200 && k < dim[2] - 100)
			    {
			    outfile <<xx <<" "<< yy<< " " << k << endl ; 
#ifdef ANALYSE
			    analysefile<<xx << " "<<yy<<" "<<k <<" " <<mx <<" "<<mn<< " "
			    <<sum<<" "<<endl;
#endif			
				}
				
			  if( 	3.1415926 * sqrt(mn * mx) * 3.5 <  sum + 300  // 300 is for the catheter size
			  	&& sqrt(mn) > 10 
				&& fabs(mx/mn) < 3
				&& mn < 400 && k >=  dim[2] - 100)
			    {
			    outfile <<xx <<" "<< yy<< " " << k << endl ; 
#ifdef ANALYSE
			    analysefile<<xx << " "<<yy<<" "<<k <<" " <<mx <<" "<<mn<< " "
			    <<sum<<" "<<endl;
#endif			
				}
				
		         }
		     //4. clear out and set seeds. 

		     }
		}//end of search 
//	cout <<"slice " <<k <<endl;
}          

#ifdef ANALYSE
analysefile.close();
#endif

//find the central line of the catheter
threshold = 550;
theSub[2] = dim[2]-1;
index = theSub.getIndex(dim);
p = volume.begin()+index;
image->fill(0);
int found = 0;
int xx, yy;

for(k = dim[2] - 1; k > dim[2] - 5 ; k = k - 1)
{
   for ( i = 0 ; i < bounds2d[1]; i ++)
	{
	for ( j = 0; j < bounds2d[0]; j ++)
	     {
	     sum = 0; 
	     xsum = ysum = 0;
		if(  *(p + j + i * bounds2d[0]) >  threshold || image->getPixel(j,i) == 1)
	       {
	       //nothing;
	     	}
	     else
	     {	     
	     //1. put seed into open stack; 
        	s = mvVec2i(j,i);
		openStack.push(s);
		     
		     //2. region grow;
			while(!openStack.empty())
			{
			   a = openStack.top();
                           openStack.pop();
				//	outfile << a.x <<" "<<a.y<<" "<<a.z<< " "<<endl;
                          // closeStack.push(a);
			   
			   xsum += (float) a.x;
			   ysum += (float) a.y;
			   if(sum < 150)
			   	{
				vList[sum].x = (float) a.x;
			   	vList[sum].y = (float) a.y;
       				}                    	
			   sum = sum+1;
			   
                           //find neighbors of a
	                   unsigned int ux, uy;
                           for (int ii = -1; ii<=1; ii++)
                           {
                              for (int jj=-1;jj<=1; jj++)
                                 {
				
                                 if (a.x + ii < 0 ||a.y + jj < 0) 
                                        continue;
                                 if (a.x + ii>= (int)dim[0]-1 ||a.y + jj >= (int)dim[1]-1)
                                        continue;
				 ux = (unsigned int) (a.x + ii);
				 uy = (unsigned int) (a.y + jj);

				
				 if(image->getPixel(ux, uy ) == 1) continue;
                                 
				 if(*(p + ux + uy * bounds2d[0]) <= threshold) 				
					{
					if(above==0)						
                                        	{
						openStack.push(mvVec2i(a.x+ii, a.y+jj)); 
					//	*(p + ux + uy * bounds2d[0]) = 3000;
					//	cout <<" Added one " <<endl;
						}
					}	
				else
					{
					if(above==1)						
                                        	{
						openStack.push(mvVec2i(a.x+ii, a.y+jj)); 
					//	*(p + ux + uy * bounds2d[0]) = 3000;
					//	cout <<" Added one " <<endl;
						}
					}		
				//openStack.push(mvVec3i(a.x+i, a.y+j, a.z+k)); 
                                image->setPixel(ux, uy, (char) 1);
 				 
					
				 } //end of jj loop
			   } // end of ii loop	 	
		     } // end of while loop
		     //cout << sum << endl;
		     //3. whether this region is a Colon region
		     if(sum > 20 && sum < 200)
		     	{
			  xx = (int) ( xsum / (float) sum);
			  yy = (int) (ysum/(float) sum);
			  if ( xx > (float) dim[0] / 4.0 && xx < dim[0] - (float)dim[0] / 4.0 && 
			  	yy > (float) dim[1] / 4.0 && yy < dim[1] - (float)dim[1] / 4.0 )
				{
		       /* call hotelliing algorithm to get the eigenvectors and eigenvalues
			   */
			  
			  	hotelling(vList, sum, eigenvalues, eigenvectors);
			  	float mn = min(fabs(eigenvalues[1]), fabs(  eigenvalues[2]));
			  	float mx = max(fabs(eigenvalues[1]), fabs(  eigenvalues[2]));
				cout<<mn <<" "<<mx << " "<< endl;
				if ( mn > 3 && mx < 20 && mx / mn < 4)
				{
					cath << xx << " " << yy << " " <<dim[2] << endl; 
					found = 1;
					break;
				}
			        }	
		       }
		   } //end of else   
		} //end of i
	if ( found == 1) break;	
	}//end of j	       
    if(found == 1) break;	
}

int over = 0;
for(; found == 1 && k >  dim[2] - 100; k = k - 1)
        {
	theSub[2] = k;
	index = theSub.getIndex(dim);
	p = volume.begin()+index;
	image->fill(0);
	sum = 0; 
	xsum = ysum = 0;
	if(  *(p + xx + yy * bounds2d[0]) >  threshold )
	{
//		cout << " no seeds in new slice " <<endl;
		break;
	}
	else
	{
		     
		//1. put seed into open stack; 
        	s = mvVec2i(xx, yy);
		openStack.push(s);
		     
		//2. region grow;
		while(!openStack.empty())
		{
		   	a = openStack.top();
                        openStack.pop();
			xsum += (float) a.x;
			ysum += (float) a.y;
			if(sum < 200)
			{
				vList[sum].x = (float) a.x;
			   	vList[sum].y = (float) a.y;
       			} 
			else
			{
			over = 1;
			cout << "leaked in new slice " << endl;
			break;
			}                   	
			 sum = sum+1;
			   
                         //find neighbors of a
	                 unsigned int ux, uy;
                         for (int ii = -1; ii<=1; ii++)
                         {
                              for (int jj=-1;jj<=1; jj++)
                                 {
				
                                 if (a.x + ii < 0 ||a.y + jj < 0) 
                                        continue;
                                 if (a.x + ii>= (int)dim[0]-1 ||a.y + jj >= (int)dim[1]-1)
                                        continue;
				 ux = (unsigned int) (a.x + ii);
				 uy = (unsigned int) (a.y + jj);

				
				 if(image->getPixel(ux, uy ) == 1) continue;
                                 
				 if(*(p + ux + uy * bounds2d[0]) <= threshold) 				
					{
					if(above==0)						
                                        	{
						openStack.push(mvVec2i(a.x+ii, a.y+jj)); 
					//	*(p + ux + uy * bounds2d[0]) = 3000;
					//	cout <<" Added one " <<endl;
						}
					}	
				else
					{
					if(above==1)						
                                        	{
						openStack.push(mvVec2i(a.x+ii, a.y+jj)); 
					//	*(p + ux + uy * bounds2d[0]) = 3000;
					//	cout <<" Added one " <<endl;
						}
					}		
				//openStack.push(mvVec3i(a.x+i, a.y+j, a.z+k)); 
                                image->setPixel(ux, uy, (char) 1);
 				 
					
				 } //end of jj loop
			   } // end of ii loop	 	
		     } // end of while loop
		     
		if ( over != 1) 
			{
			xx = (int) xsum / (float)sum;
			yy = (int) ysum / (float)sum;
			cath << xx << " " << yy << " " << k << endl;
			}
		else
			break;
//	cout <<"slice " <<k <<endl;
	} // end of else       
} //end of for loop   

delete image;	                              
delete [] vList;
free_matrix(eigenvectors, 1,2,1,2);
free_vector(eigenvalues, 1,2);

//writeRVF("newrvcf.rvf", volume, error);
//cout << endl<<totaltri <<endl;     
outfile.close();  
logfile.close();  
cath.close();
}


