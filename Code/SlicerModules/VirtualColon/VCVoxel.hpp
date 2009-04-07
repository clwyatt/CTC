/*
 * VCVoxel.hpp
 *
 *  Created on: Oct 13, 2008
 *      Author: haxu
 */

#ifndef VCVOXEL_HPP_
#define VCVOXEL_HPP_

#include <string>

using namespace std;

class VCVoxel {
public:
  VCVoxel(string polypId_, double x_, double y_, double z_)
    : _polypId(polypId_), _x(x_), _y(y_), _z(z_) {
  }

  virtual ~VCVoxel() { }

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
  VCVoxel() { }

protected:
  string _polypId;
  double _x, _y, _z;
};

#endif /* VCVOXEL_HPP_ */
