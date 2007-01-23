/********************************************************
File: lsf2oogl.cc
Abstract: Convert levelset file to oogl surface file
Created: 06/02/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.7 $) by $Author: clwyatt $ on $Date: 2002/07/31 20:25:52 $
*********************************************************/
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <list>
    
#include "Subscript.hh"
#include "Volume.hh"
#include "Vector.hh"
#include "Mesh.hh"

using namespace IMTK;

Mesh * tetradecomp(Volume<float> &V, int method);

void waltzcube(Volume<float> & V, Vector<unsigned int> seed, float threshold, Mesh & mesh);

void load_seed_file(const string &filename, list< Subscript<3> > &seeds)
{
	ifstream ifs;

	ifs.open( filename.c_str() );

	if(!ifs){
		cerr << "Error opening seed file " << filename << endl;
		exit(EXIT_FAILURE);
	}

	int Pn, Px, Py, Pz;
	IMTK::Subscript<3> tempseed;
	while( ifs.good() ){
		ifs >> Pn >> Px >> Py >> Pz;

		clog << "Seed " << Pn << " at " << Px << ", " << Py << ", " << Pz << endl;
		tempseed[0] = Px;
		tempseed[1] = Py;
		tempseed[2] = Pz;

		if( ifs.good() ) seeds.push_front(tempseed);
	}

}

void read_rawf_file(char *name, IMTK::Volume<float> &vol)
{
        ifstream  infile;
	IMTK::Subscript<3> bounds;
	float temp;
        int i, j, k;

	bounds = vol.getBounds();

	infile.open( name );

	if(!infile){
		cerr << "Error opening rawf file " << name << endl;
		exit(-1);
	}

	for(k=0; k < bounds[2]; k++)
		for(j=0; j < bounds[1]; j++)
			for(i=0; i < bounds[0]; i++){
				infile.read((char *)(&temp), 4);
				vol.setVoxel(i, j, k, temp);
			}

	infile.close();
};



// void ooglwrite(Mesh *mesh, char * name)
// {
//         ofstream  outfile;
// 	char entry[100];
// 	int i;
// 	Vertex *v1, *v2, *v3;

// 	outfile.open( name );

// 	outfile << "appearance { shading flat }\n";
// 	outfile << "OFF\n";

// 	sprintf(entry, "%i %i 0\n\n", mesh->getNverticies(), mesh->getNtriangles());
// 	outfile << entry;

// 	Mesh::vertexIterator mvi = mesh->vertexBegin();
// 	i = 0;
// 	while( mvi !=  mesh->vertexEnd() ){
// 		sprintf(entry, "%f %f %f\n", (*mvi)->getX(), (*mvi)->getY(), (*mvi)->getZ());
// 		outfile << entry;
// 		// label vertex 
// 		(*mvi)->label(i);
// 		i++;
// 		mvi++;
// 	}

// 	outfile << "\n";

// 	Mesh::triangleIterator mti = mesh->triangleBegin();
// 	while( mti !=  mesh->triangleEnd() ){
// 		v1 = (*mti).getV1();
// 		v2 = (*mti).getV2();
// 		v3 = (*mti).getV3();
// 		sprintf(entry, "3\t%i %i %i\n", v1->label(), v2->label(), v3->label());
// 		outfile << entry;
// 		mti++;
// 	}

// 	outfile.close();

// };

typedef struct{
	long xdim;
	long ydim;
	long zdim;
	long bitdepth;
	float xorig;
	float yorig;
	float zorig;
	float dx;
	float dy;
	float dz;

} RVF_GEOM;

void readRvfGeom(char *fileName, RVF_GEOM &geom, int &error){

        ifstream  infile;
	long sx, sy, sz, bitdepth;
	float dx, dy, dz, xo, yo, zo;

	error = 0;

	infile.open( fileName );

	if(!infile){
		cerr << "Error opening rvf(?) file " << fileName << endl;
		error = 1;
		return;
	}

        infile.seekg(12);

        infile.read((char *)(&sx), 4);
        infile.read((char *)(&sy), 4);
        infile.read((char *)(&sz), 4);
	infile.read((char *)(&bitdepth), 4);

	infile.seekg(52);
        infile.read((char *)(&xo), sizeof(float));
        infile.read((char *)(&dx), sizeof(float));
        infile.read((char *)(&yo), sizeof(float));
        infile.read((char *)(&dy), sizeof(float));
        infile.read((char *)(&zo), sizeof(float));
        infile.read((char *)(&dz), sizeof(float));
	
	infile.close();

	if(dx == 0) dx = 1.0;
	if(dy == 0) dy = 1.0;
	if(dz == 0) dz = 1.0;
	 
	clog << "Volume size: " <<  sx << " x " << sy << " x " << sz;
	clog << "       (mm): " << sx*dx << "mm x " << sy*dy << "mm x " << sz*dz << "mm";
	clog << "     origon: " << xo << ", " << yo << ", " << zo << endl;

	geom.xdim = sx;
	geom.ydim = sy;
	geom.zdim = sz;
	geom.bitdepth = bitdepth;
	geom.xorig = xo;
	geom.yorig = yo;
	geom.zorig = zo;
	geom.dx = dx;
	geom.dy = dy;
	geom.dz = dz;

};

/*
* main routine
*/

int main(int argv, char **argc)
{
   char infilename[100];
   char rvffilename[100];
   char outfilename[100];
   char seedfilename[100];
   int method, format;
   Subscript<3> bounds;
   ifstream  infile;

   if(argv < 5)
    {
      cerr << "Usage: lsf2oogl infile.rvf lsf.raw outfile.oogl format(1=oogl,2=mesh,3=vtk) [method(1 = cubes (default) or 2 = tetrahedrons or 3 = waltzer)] [seedfile]" << endl;
      exit(EXIT_FAILURE);
    }

   //parse command line, no checking!
   strncpy(rvffilename, *(argc+1), 100);
   strncpy(infilename, *(argc+2), 100);
   strncpy(outfilename, *(argc+3), 100);
   format = atoi(*(argc+4));

   if( (format != 1) && (format != 2) && (format != 3)){
	   cerr << "ERROR: format must be one or 2" << endl;
	   cerr << "Usage: lsf2oogl infile.rvf lsf.raw outfile.oogl format(1=oogl,2=mesh) [method(1 = cubes (default) or 2 = tetrahedrons or 3 = waltzer )] [seedfile]" << endl;
	   exit(EXIT_FAILURE);
   }

   if(argv >= 6){
	   method = atoi(*(argc+5));
   }
   else{
	   method = 1;
   }

   if( (argv < 7) && (method == 3)){
	   cerr << "ERROR: seedfile needed with method 3" << endl;
	   cerr << "Usage: lsf2oogl infile.rvf lsf.raw outfile.oogl format(1=oogl,2=mesh) [method(1 = cubes (default) or 2 = tetrahedrons or 3 = waltzer )] [seedfile]" << endl;
	   exit(EXIT_FAILURE);
   }
   if(method == 3){
	   strncpy(seedfilename, *(argc+6), 100);
   }

   clog << "Using method " << method << " Format " << format << endl;
   
   clog << "Reading Geometry information from rvf file: " << rvffilename << endl;
   RVF_GEOM geom;
   int error;
   readRvfGeom(rvffilename, geom, error); 
   if(error) exit(EXIT_FAILURE);
   
   bounds[0] = geom.xdim;
   bounds[1] = geom.ydim;
   bounds[2] = geom.zdim;
   clog << "Reading data from raw file: " << bounds[0] << " x " << bounds[1] << " x " << bounds[2] << endl;
   Volume<float> data(bounds);
   data.setVoxelSpacing(geom.dx, geom.dy, geom.dz);
   data.setOrigin(geom.xorig, geom.yorig, geom.zorig);
   read_rawf_file(infilename, data);
   
   //Compute surface
   clog << "Computing the isosurface" << endl;
	   Mesh * m;
   if(method < 3){
	   m = tetradecomp(data, method);
   }
   else if (method == 3){
	   m = new Mesh(false);

	   m->setBounds(bounds[0], bounds[1], bounds[2]);
	   m->setVoxelSize(geom.dx, geom.dy, geom.dz);
	   m->setOrigin(geom.xorig, geom.yorig, geom.zorig);

	   clog << "Parsing seed file." << endl;
	   list< IMTK::Subscript<3> > seeds;
	   load_seed_file(string(seedfilename), seeds);

	   list< Subscript<3> >::iterator it = seeds.begin();
	   while(it != seeds.end() ){
		   
		   clog << (*it)[0] << " " <<  (*it)[1] << " " << (*it)[2] << endl;
		   Vector<unsigned int> seed(3);
		   seed[0] = (*it)[0];
		   seed[1] = (*it)[1];
		   seed[2] = (*it)[2];
		   waltzcube(data, seed, 0.0, *m);
		   
		   it++;
	   }
	   
   }
   else{
	   cerr << "ERROR: invalid method number: " << method << endl;
	   exit(EXIT_FAILURE);
   }

   
   //write surface
   clog << "Writing surface" << endl;
   
   ofstream outfile; // this declaration can't be inside a case statement even though it is only needed in case 2
	   
   switch(format){
	   
   case 1: m->ooglwrite(outfilename);
	   break;
	   
   case 2: outfile.open(outfilename);
	   if(!outfile){ //open failed
		   exit(EXIT_FAILURE);
	   }
	   m->write(outfile);
	   outfile.close();
	   
	   break;
	   
   case 3: m->vtkwrite(outfilename);
	   break;
	   
   default:
	   
	   cerr << "Internal Error\n" << endl;
	   exit(EXIT_FAILURE);
   }

   delete m;
}

