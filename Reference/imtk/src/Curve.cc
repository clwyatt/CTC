/********************************************************
File: Curve.cc
Abstract: implementation for Curve

See header (Curve.hh) for documentation

Last Revision ($Revision: 1.4 $) 
by $Author: clwyatt $ 
on $Date: 2001/05/24 17:33:43 $
*********************************************************/

#include "Curve.hh"

using namespace IMTK;
using namespace std;

Curve::CurveVertexSet::CurveVertexSet(){
	bounds[0] = 0;
	bounds[1] = 0;

	numverts = 0;

	shared = true;
}

Curve::CurveVertexSet::CurveVertexSet(bool shareflg){
	bounds[0] = 0;
	bounds[1] = 0;

	numverts = 0;

	shared = shareflg;
}

Curve::CurveVertexSet::CurveVertexSet(const CurveVertexSet & CVS){

	// not implemented
	assert(0);
}

Curve::CurveVertexSet::~CurveVertexSet(){

	CurveVertexElement e;
	std::list<Vertex2D *>::iterator sli;

	while( !RBTree<CurveVertexElement>::isEmpty() ){
		e = RBTree<CurveVertexElement>::getRoot();

		// delete each vertex from pointer list
		sli = e.verts.begin();
		while(sli != e.verts.end()){
			delete *sli;
			sli++;
		}
		RBTree<CurveVertexElement>::deleteNode(e.key);
	}
}

Vertex2D * Curve::CurveVertexSet::insert(Vertex2D v){

	double key;
	RBTree<CurveVertexElement>::Node * n;
	CurveVertexElement *e;
	Vertex2D * newv;
	int x, y, found;
	bool inlist;

	switch(shared){

	case true:

		x = (int)(v.getX() + 0.5);
		y = (int)(v.getY() + 0.5);
		
		key = bounds[1]*x + y;

		n = RBTree<CurveVertexElement>::binTreeSearch(key, &found);
		
		// if found search the node vertex sublist for a match
		if(found){
			inlist = false;
			e = n->getData();
			std::list<Vertex2D *>::iterator sli = e->verts.begin();
			while(sli != e->verts.end()){
				if(**sli == v){
					inlist = true;
					break;
				}
				sli++;
			}
			
			if(!inlist){
				newv = new Vertex2D(v);
				e->verts.push_front(newv);
				linearList.push_front(newv);
				numverts++;
				return newv;
			}
			else{
				return *sli;
			}
		}
		else{ // if not found add the correct node
			e = new CurveVertexElement;
			newv = new Vertex2D(v);
			e->key = key;
			e->verts.push_front(newv);
			linearList.push_front(newv);
			
			RBTree<CurveVertexElement>::insertNode(*e, key);
			delete e;
			
			numverts++;
			
			return newv;
		}

	case false:

		newv = new Vertex2D(v);
		linearList.push_front(newv);
		numverts++;
		return newv;
	
	default:
		// should never happen
		abort();
	}
}

Curve::CurveVertexSet::iterator Curve::CurveVertexSet::begin(){

	return linearList.begin();
}

Curve::CurveVertexSet::iterator Curve::CurveVertexSet::end(){

	return linearList.end();
}

Curve::Curve(){

	verticies = new CurveVertexSet();
}

Curve::Curve(bool shareflg){

	verticies = new CurveVertexSet(shareflg);
}

Curve::~Curve(){

	// for efficiency reasons, the vertices are deleted when
	// the CurveVertexSet is deleted.
}

void Curve::setBounds(int xdim, int ydim){

	verticies->bounds[0] = xdim;
	verticies->bounds[1] = ydim;
}

void Curve::addLine(Line l){

	Vertex2D *v1, *v2;
	Line newl;

	v1 = verticies->insert(*(l.getV1()));
	v2 = verticies->insert(*(l.getV2()));

	newl.set(v1, v2);

	lines.push_front(newl);
}

int Curve::getNverticies(){

	return verticies->numverts;
}

int Curve::getNlines(){

	return lines.size();
}

// THIS NEEDS FIXING!!!! BAD APPROACH->MEMORY LEAK
float * Curve::getPoints(int * nP){

	float * points;
	int i, l;
	std::list<Line>::iterator sli;
	Vertex2D * v;

	l = lines.size();

	points = new float[6*l];

	sli = lines.begin();
	i = 0;
	while(sli != lines.end()){
		v = sli->getV1();
		points[2*i] = v->getX();
		points[2*i + 1] = v->getY();

		v = sli->getV2();
		i++;
		points[2*i] = v->getX();
		points[2*i + 1] = v->getY();

		i++;
	}
	
	return points;
}

Curve::vertexIterator Curve::vertexBegin(){

	return verticies->begin();
}

Curve::vertexIterator Curve::vertexEnd(){

	return verticies->end();
}

Curve::lineIterator Curve::lineBegin(){

	return lines.begin();
}

Curve::lineIterator Curve::lineEnd(){

	return lines.end();
}

void Curve::clear(){

	delete verticies;
	verticies = new CurveVertexSet();

	lines.clear();
}

void Curve::clear(bool shareflag){

	delete verticies;
	verticies = new CurveVertexSet(shareflag);

	lines.clear();
}
