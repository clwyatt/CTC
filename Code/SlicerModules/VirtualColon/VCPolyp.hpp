/*
 * VCPolyp.hpp
 *
 *  Created on: Oct 12, 2008
 *      Author: haxu
 */

#ifndef VCPOLYP_HPP_
#define VCPOLYP_HPP_

#include <string>
#include <functional>
#include <sstream>

using namespace std;

class VCPolyp {
public:
  VCPolyp() { }
  VCPolyp(string polypId_, double x_, double y_, double z_) :
    _polypId(polypId_), _x(x_), _y(y_), _z(z_) {

  }

  virtual ~VCPolyp() { }

  string getPolypId() const {
    return _polypId;
  }

  double getX() const {
    return _x;
  }

  double getY() const {
    return _y;
  }
  double getZ() const {
    return _z;
  }

protected:
  string _polypId;
  double _x;
  double _y;
  double _z;
};

class VCPolyp_op_less: public binary_function<VCPolyp, VCPolyp, bool> {
public:
  bool operator()(const VCPolyp & a, const VCPolyp & b) const {
    istringstream issa(a.getPolypId()), issb(b.getPolypId());
    int ia, ib;

    if (issa >> ia && issb >> ib)
      return ia < ib;
    else
      return a.getPolypId() < b.getPolypId();
  }
};
#endif /* VCPOLYP_HPP_ */
