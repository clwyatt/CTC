/********************************************************
File: Curve.hh
Abstract:  A curve mesh class.
Created: 06/01/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:32 $
*********************************************************/

//prevent recursive includes

#ifndef _Curve_hh
#define _Curve_hh 1

#include <slist>

#include "Data.hh"
#include "RBTree.hh"
#include "Line.hh"

/** Curve Mesh Class.

A class implementing the storage of 2D
curve mesh points. Verticies may be shared or
not. The default is shared. If verticies are shared they are stored
in a Red/Black Tree for efficient insert and searching
and in a single-linked list for iterating. If they are not
shared they are stored in a single-linked list for iterating.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version  $Revision: 1.1.1.1 $
*/

namespace IMTK{

class Curve: public Data
{
private:

	/** Element of CurveVertexSet */
	struct CurveVertexElement
	{
	public:
	
		//key for this node
		double key;

		//list of vertices within 0.5 of grid location
		std::slist<Vertex2D *> verts;
	};

	/** Efficient Store/Search for verticies */
	class CurveVertexSet: public RBTree<CurveVertexElement>
	{
	public:
	
		typedef std::slist<Vertex2D *>::iterator iterator;

	private:

		bool shared;

		std::slist<Vertex2D *> linearList;

	public:
		// bounds of underlying mesh grid
		int bounds[2];

		// number of verticies stored
		int numverts;

		/// Constructor
		CurveVertexSet();

		/// Custom Constructor
		CurveVertexSet(bool shareflg);

		/// Copy Constructor
		CurveVertexSet(const CurveVertexSet & CVS);

		/// Destructor
		~CurveVertexSet();

		/** Insert Vertex, sharing common verticies
		 * i.e. there are no duplicate verticies in the set.
		 * @param v the vertex
		 * @return the pointer to the already allocated or
		 * newly allocated vertex.
		 */
		Vertex2D * insert(Vertex2D v);

		/// iterator begin
		iterator begin();

		/// iterator end
		iterator end();
		
	};

	/// Shared vertex set, stored for efficient search
	CurveVertexSet *verticies;

	/// List of triangles in mesh
	std::slist<Line> lines;
       
protected:

public:

	typedef CurveVertexSet::iterator vertexIterator;

	typedef std::slist<Line>::iterator lineIterator;

	/// Default Constructor
	Curve();

	/// Custom Constructor
	Curve(bool shareflg);

	/// Copy Constructor (inline)
	Curve(const Curve & C){
		// copy not implemented
		assert(0);
	}

	/** Destructor
	 * For efficiency reasons, the vertices are deleted when
	 * the CurveVertexSet is deleted.
	 */
	~Curve();

	/**
	 * Set bounds of underlying mesh grid
	 * 
	 * @param xdim
	 * @param ydim
	 */
	void setBounds(int xdim, int ydim);

	/**
	 * Add line to the mesh
	 * 
	 * @param l the line to add to mesh
	 */
	void addLine(Line l);

	/**
	 * Get number of Verticies
	 * 
	 * @return number of verticies
	 */
	int getNverticies();

	/**
	 * Get number of line segments
	 * 
	 * @return number of line segments
	 */
	int getNlines();

	/**
	 * Get point array for rendering in OpenGL using 
	 * glDrawArraysEXT or glVertex2fv
	 */
	float * getPoints(int * nP);

	/// vertex iterator begin
	vertexIterator vertexBegin();

	/// vertex iterator end
	vertexIterator vertexEnd();

	/// line iterator begin
	lineIterator lineBegin();

	/// line iterator end
	lineIterator lineEnd();

	/// clear all data
	void clear();

	/// clear all data
	void clear(bool shareflag);

};
}

#endif
