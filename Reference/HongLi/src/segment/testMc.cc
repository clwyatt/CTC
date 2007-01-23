//testMarchingCbue
#include "DataIO.hh"
#include <stack>
#include <stdio.h>
#include "mvVec3i.hh"
#include "mvVec3f.hh"
#include "MarchingCube.hh"
#include "Triangle_with_Features.hh"

int main (int argc, char *argv[])
{
int ii;
std::slist<Triangle *> tlist;
std::slist<Triangle *>::iterator tlistIter;
ofstream  outfile;

if (argc <= 10) /* Read parameters*/
{
	cout << "Usage : testMc threshold inoutflag v0 v1 .... v7" << endl;
	exit(1);
	}

int totaltri = 0;

float threshold = atof(argv[1]);
int above = atoi(argv[2]);

MarchingCube mCube;
std::slist<IsoPoint *> isolist;

mCube = MarchingCube(mvVec3i(0,0,0));
mCube.setThreshold(threshold); 	
mCube.setThrFlag(above == 1);	
				
for (ii=0;ii<8;ii++)
	{
	float va = (float)atoi(argv[ii+3]);
	mCube.setValue(ii, va); 
	}
if(mCube.isoPointExist())
	{
	isolist = mCube.getIsoPoints();
	cout <<"number of iso: "<< isolist.size()<<endl;
	tlist = mCube.getTriangles();
	}
else{
	cout <<"No iso Points "<<endl;
	}
totaltri = tlist.size();
char entry[80];
outfile.open("oogltest.oogl");

outfile << "appearance { shading flat }\n";
outfile << "OFF\n";

sprintf(entry, "%i %i 0\n\n", totaltri*3, totaltri);
outfile << entry;
int vindex = 0;
tlistIter = tlist.begin();
while(tlistIter != tlist.end())
	{
	outfile << 
		((*tlistIter)->getV1())->getPX()  <<" "<<
		((*tlistIter)->getV1())->getPY()  <<" "<<
		((*tlistIter)->getV1())->getPZ() <<endl;
	outfile <<
		((*tlistIter)->getV2())->getPX()<<" "<<
		((*tlistIter)->getV2())->getPY()<<" "<<
		((*tlistIter)->getV2())->getPZ()<<endl;
	outfile <<
		((*tlistIter)->getV3())->getPX() <<" "<<
		((*tlistIter)->getV3())->getPY()<<" "<<
		((*tlistIter)->getV3())->getPZ()<<endl;
		tlistIter ++;
	}	
outfile <<endl;
for (int i = 0;i<totaltri; i++)
	{
	outfile<<"3 "<<vindex ++<<" ";
	outfile<<vindex ++<<" ";
	outfile<<vindex ++<<endl;
	}
outfile.close();
outfile.open("outtest.list");
outfile<<"LIST"<<endl;
outfile << "<oogltest.oogl"<<endl;
outfile << "<unit.vect"<<endl;
outfile.close();
}	
	
