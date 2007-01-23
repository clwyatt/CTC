/********************************************************
File: Mesh.hh
Abstract:  A triangle mesh class.
Created: 06/01/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.6 $) 
by $Author: clwyatt $ 
on $Date: 2002/04/17 20:45:01 $
*********************************************************/

//prevent recursive includes

#ifndef _Mesh_hh
#define _Mesh_hh 1

#include <list>
#include <vector>

#include "Data.hh"
#include "RBTree.hh"
#include "Triangle.hh"


/** Triangle Mesh Class.

A class implementing the storage of 3D
triangle mesh points. Verticies may be shared or
not. The default is unshared. If verticies are shared they are stored
in a Red/Black Tree for efficient insert and searching
and in a single-linked list for iterating. If they are not
shared they are stored in a single-linked list for iterating.

The triangles are stored is a singly linked list initially,
for best performance when inserting vertices. The triangles
are converted to a vector for faster local computations on a call
to finalize. Inserts after finilization are expensive.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version  $Revision: 1.6 $
*/

namespace IMTK{

class Mesh: public Data
{
private:

	/** Element of MeshVertexSet */
	struct MeshVertexElement
	{
	public:
	
		//key for this node
		double key;

		//list of vertices within 0.5 of grid location
		std::list<Vertex *> verts;
	};

	/** Efficient Store/Search for verticies */
	class MeshVertexSet: public RBTree<MeshVertexElement>
	{
	public:
	
		typedef std::list<Vertex *>::iterator iterator;

	private:

		bool shared;

		std::list<Vertex *> linearList;

	public:
		// bounds of underlying mesh grid
		int bounds[3];

		// voxel size of underlying mesh grid
		float voxelSize[3];

		// origin of underlying mesh grid
		float origin[3];

		// number of vertices stored
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
		Vertex * insert(Vertex v);

		/** 
		 * Get vertex pointer for manipulation.
		 *
		 */
		Vertex * get(Vertex &v, bool &found);

		/// iterator begin
		iterator begin();

		/// iterator end
		iterator end();

		/// are vertices shared
		bool isShared();
		
	};

	/// Shared vertex set, stored for efficient search
	MeshVertexSet *verticies;

	/// Lists of triangles in mesh
	std::list<Triangle> triangles;
	std::vector<Triangle> finalTlist;

	/// is the list of triangles an list (best for Mesh construction) or vector (best for mesh computations)
	bool finalized;
       
protected:

public:

	typedef MeshVertexSet::iterator vertexIterator;

	typedef std::list<Triangle>::iterator triangleIterator;

	/// Default Constructor shared = false is deafault
	Mesh();

	/// Custom Constructor
	Mesh(bool shareflg);

	/// Copy Constructor (inline)
	Mesh(const Mesh & M){
		// copy not implemented
		assert(0);
	}

	/** Destructor
	 * For efficiency reasons, the vertices are deleted when
	 * the MeshVertexSet is deleted.
	 */
	~Mesh();

	/**
	 * Set bounds of underlying mesh grid
	 * 
	 * @param xdim
	 * @param ydim
	 * @param zdim
	 */
	void setBounds(int xdim, int ydim, int zdim);

	/**
	 * Set voxel size of underlying mesh grid
	 * 
	 * @param dx
	 * @param dy
	 * @param dz
	 */
	void setVoxelSize(float dx, float dy, float dz);

	/**
	 * Set origin of underlying mesh grid
	 * 
	 * @param xo
	 * @param yo
	 * @param zo
	 */
	void setOrigin(float xo, float yo, float zo);

	/**
	 * Add triangle to the mesh
	 * 
	 * @param tri the triangle to add to mesh
	 */
	void addTriangle(Triangle tri);

	/**
	 * Finalize the mesh.
	 * 
	 * This converts the triangle list to fixed length array
	 */
	void finalize();

	/**
	 * See if the mesh is finalized.
	 * 
	 * @return true if mesh has been finalized
	 */
	bool isFinalized(){
		return finalized;
	}

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
	 * Fill an array with copies of vertices
	 * 
	 */
	void getVertices(std::vector<Vertex> &theVertArray);

	/**
	 * Get a submesh centered at vertex of given depth
	 * 
	 */
	bool getSubMesh(Vertex & center, unsigned int depth, Mesh & subMesh);

	/**
	 * Get point array for rendering in OpenGL using 
	 * glDrawArraysEXT or glVertex3fv
	 */
	float * getPoints(int * nP);

	/**
	 * Get normal array for rendering in OpenGL using 
	 * glDrawArraysEXT or glVertex3fv
	 */
	float * getNormals(int * nP);

	/// vertex iterator begin
	vertexIterator vertexBegin();

	/// vertex iterator end
	vertexIterator vertexEnd();

	/// triangle iterator begin
	triangleIterator triangleBegin();

	/// triangle iterator end
	triangleIterator triangleEnd();

	/// delete all vertices and triangles
	void clear(bool shareflg);

	/**
	 * Find intersection of triangles in the mesh and the given vertex/normal
	 * line, ordered from nearest to furtherest using Euclidean distance
	 * 
	 * @param theVertex the vertex defining the line (point, normal)
	 * @param theIntersection the vertex of the intersection
	 * @param closestMeshVertex the closest mesh vertex to the intersection
	 *
	 * @return bool indicating wether an intersection exists or another failure occured
	 */
	bool lineIntersection(Vertex &theVertex, Vertex &theIntersection, Vertex &closestMeshVertex); 

	/**
	 * write the mesh data to the given file output stream
	 * 
	 * See notes for read
	 *
	 * @param theOutStream
	 */
	bool write(std::ofstream & theOutStream);

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
	bool read(std::ifstream & theInStream);

	/**
	 * Write mesh as oogl file
	 * 
	 * @param name pointer to file name
	 */
	void ooglwrite(const char * name);

	/**
	 * Write mesh as vtk polydata file
	 * 
	 * @param name pointer to file name
	 */
	void vtkwrite(const char * name);
	
	/**
	 * Read mesh from oogl file
	 * 
	 * @param name pointer to file name
	 */
	void ooglread(const char * name);

	/**
	 * Print info about the mesh
	 * 
	 */
	void printInfo();
	
};

}

#endif
