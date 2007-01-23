/********************************************************
File: Mesh_Features.hh
Abstract:  A Mesh class with features.
Created: 09/25/2001
Author: Hong Li

Last Revision  
by $Author: hongli $ 
on $Date: 2003/10/02 19:44:20 $
*********************************************************/

#ifndef _Mesh_Features_hh
#define _Mesh_Features_hh 1

#define NEIGHBORDEBUG

#define NO_SELF 1

#include <slist>
#include <vector>

#include "Data.hh"
#include "RBTree.hh"
#include "Triangle_with_Features.hh"
#include "Volume.hh"
#include "Mesh.hh"
#include "Vertex_with_Features.hh"
#include "Volume_ext.hh"


/** Triangle Mesh Class with features.

A class implementing the storage of 3D
triangle mesh points. Verticies may be shared or
not. The default is shared. If verticies are shared they are stored
in a Red/Black Tree for efficient insert and searching
and in a single-linked list for iterating. If they are not
shared they are stored in a single-linked list for iterating.

Features are added to Mesh class, which is created by Chris. I am
thinking of better ways to do this, a derived class.
*/

class Mesh_Features : public IMTK::Data
{
public:

	/** Element of MeshVertexSet */
	struct MeshVertexElement
	{
	public:
	
		//key for this node
		double key;

		//list of vertices within 0.5 of grid location
		std::slist<Vertex_with_Features *> verts;
	};

	/** Efficient Store/Search for verticies */
	class MeshVertexSet: public IMTK::RBTree<MeshVertexElement>
	{
	public:
	
		typedef std::slist<Vertex_with_Features *>::iterator iterator;

	public:

		bool shared;

		std::slist<Vertex_with_Features *> linearList;

	public:
		// bounds of underlying mesh grid
		int bounds[3];

		// voxel size of underlying mesh grid
		float voxelSize[3];

		// origin of underlying mesh grid
		float origin[3];

		// number of verticies stored
		int numverts;

		/// Constructor
		MeshVertexSet();

		/// Custom Constructor
		MeshVertexSet(bool shareflg);

		/// Copy Constructor
		MeshVertexSet(const MeshVertexSet & MVS);

		/// Destructor
		~MeshVertexSet();

		/** Insert Vertex, sharing common verticies
		 * i.e. there are no duplicate verticies in the set.
		 * @param v the vertex
		 * @return the pointer to the already allocated or
		 * newly allocated vertex.
		 */
		 Vertex_with_Features * insert(Vertex_with_Features v);
		 Vertex_with_Features * insert(Vertex_with_Features * v);

		/// iterator begin
		iterator begin();

		/// iterator end
		iterator end();
		
		/// are vertices shared
		bool isShared();
	
		
	};

	/// Shared vertex set, stored for efficient search
	MeshVertexSet *verticies;

	/// List of triangles in mesh
	std::slist<Triangle *> pttriangles;
       	int convexityDepth;

protected:

public:

	typedef MeshVertexSet::iterator vertexIterator;

	typedef std::slist<Triangle *>::iterator triangleIterator;

	/// Default Constructor
	Mesh_Features();

	/// Custom Constructor
	Mesh_Features(bool shareflg);

	/// Copy Constructor (inline)
	Mesh_Features(const IMTK::Mesh & M){
		// copy not implemented
		assert(0);
	}

	/** Destructor
	 * For efficiency reasons, the vertices are deleted when
	 * the MeshVertexSet is deleted.
	 */
	~Mesh_Features();

	/**
	 * Set bounds of underlying mesh grid
	 * 
	 * @param xdim
	 * @param ydim
	 * @param zdim
	 */
	void setBounds(int xdim, int ydim, int zdim);

	/**
	 * Add triangle to the mesh
	 * 
	 * @param tri the triangle to add to mesh
	 */
	void addTriangle(Triangle & tri);
	void addTriangle(Vertex_with_Features *v1, Vertex_with_Features *v2, Vertex_with_Features *v3, int label);

	/**
	 * Get number of Verticies
	 * 
	 * @return number of verticies
	 */
	int getNverticies();

	/**
	 * Get number of Faces (triangles)
	 * 
	 * @return number of triangles
	 */
	int getNtriangles();

	/**
	 * Get point array for rendering in OpenGL using 
	 * glDrawArraysEXT or glVertex3fv
	 */
	float * getPoints(int * nP);

	/// vertex iterator begin
	vertexIterator vertexBegin();

	/// vertex iterator end
	vertexIterator vertexEnd();

	/// triangle iterator begin
	triangleIterator triangleBegin();

	/// triangle iterator end
	triangleIterator triangleEnd();
	
	//clear vertices' flag 
	void setVerticesflag(bool f);

	//check vertices' flag only for debug 
	int CheckVerticesflag(bool f);

	///Get the nearest Vertices of a given vertex
	std::slist<Vertex_with_Features *>
	getNeighborVertices(Vertex_with_Features * ptVert, int level);

	/**
	*Get the nearest Vertices of a given vertex with more options
	*/
	std::slist<Vertex_with_Features *> 
	getNeighborVertices(Vertex_with_Features * ptVert, int level, int flag);

	/**
	*Get the nearest Vertices of a given vertex with more options
	*/
	std::slist<Vertex_with_Features *> 
	getNeighborVertices(Vertex_with_Features * ptVert, float distance);
	

	bool
	isIsolate(Vertex_with_Features *ptVert, float range);

	///Calculate the Thickness of a given vertices 
	bool Calc_Thickness(Vertex_with_Features *ptVert,Volume_ext<unsigned short> & volume);
	///Calculate the Curvature of a given vertex 

	///Calculate the Convexity and curvature of a given vertex 
	double Calc_Convexity_and_Curvature(Vertex_with_Features	*ptVert, int level,  int DB, Volume_ext<unsigned short> & volume_ext);
	double Calc_Convexity_and_Curvature(Vertex_with_Features	*ptVert, int level);
	
	//get area covered by vertices  
	double getAreaFromVertexList(std::slist<Vertex_with_Features *> vlist, double & isoPlanicity);
	
	///write OFF file covered by vertices  
	int write_OFF_fromVertexList(std::slist<Vertex_with_Features *>vlist, char * filename);

	///write VECT file covered by vertices  
	int write_VECT_fromVertexList(std::slist<Vertex_with_Features *>vlist,  Vertex_with_Features *vCenter, mvVec3f normal, char * filename);

	float * getNormals(int * nP);

	/// delete all vertices and triangles
	void clear(bool shareflg);


	/**
	 * read the mesh data from the given file input stream
	 * 
	 * Since insertions into the MeshVertexSet are deterministic
	 * for a fixed triangle ordering, the mesh read from a file
	 * should be identical to that written except for the pointer
	 * values themselves. i.e. the iterations should be identical.
	 *
	 * @param theInStream
	 */
	bool read(ifstream & theInStream);

	/**
	 * Write mesh as oogl file
	 * 
	 * @param name pointer to file name
	 */
	/**
	 * Read mesh from oogl file
	 * 
	 * @param name pointer to file name
	 */
	bool ooglread(const char * name);


	
#ifdef NEIGHBORDEBUG
	bool neighborhood_oogl(char * filename, Vertex_with_Features *ptVert, int level);
#endif	


};

/**
* Write mesh as oogl file
* 
* @param mesh reference to a mesh
* @param name pointer to file name
*/
void ooglwrite(Mesh_Features &mesh,int type);

/**
* Read mesh from oogl file
* 
* @param mesh reference to a mesh
* @param name pointer to file name
*/
void ooglread(Mesh_Features &mesh, char * name,float & dx, float & dy, float & dz, float & xorig, float & yorig, float & zorig);

#endif

