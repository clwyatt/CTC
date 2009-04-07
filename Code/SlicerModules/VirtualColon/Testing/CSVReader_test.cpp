/*
 * CSVReader_test.cpp
 *
 *  Created on: Oct 9, 2008
 *      Author: haxu
 */

/*
 int main(int argc, char** argv) {

  CSVReader obj;
  obj.parseFile("/home/haxu/program/Slicer3/Modules/VirtualColon/Testing/test.csv");

  return 0;
}
*/

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "../CSVReader.hpp"
#include "../VCPredictSet.hpp"
#include "../VCPolypSet.hpp"
#include "../VCVoxelSet.hpp"


BOOST_AUTO_TEST_CASE(CSVReaderTest)
{
  CSVReader csv("/home/haxu/program/Slicer3/Modules/VirtualColon/Testing/predict.csv", "s,d,d,f,f");
  int retval = csv.parseFile();
  BOOST_CHECK_EQUAL(retval, 0);
  BOOST_CHECK_EQUAL(csv.getNumRow(), 75);
  BOOST_CHECK_EQUAL(csv.getNumColumn(), 5);

  const vector<string> & col0 = csv.getStringColumn(0);
  BOOST_CHECK_EQUAL(col0.size(), csv.getNumRow());

  const vector<int> & col1 = csv.getIntColumn(1);
  BOOST_CHECK_EQUAL(col1.size(), csv.getNumRow());

  const vector<int> & col2 = csv.getIntColumn(2);
  BOOST_CHECK_EQUAL(col2.size(), csv.getNumRow());

  const vector<double> & col3 = csv.getDoubleColumn(3);
  BOOST_CHECK_EQUAL(col3.size(), csv.getNumRow());

  const vector<double> & col4 = csv.getDoubleColumn(4);
  BOOST_CHECK_EQUAL(col4.size(), csv.getNumRow());

  /*
   * WRAMC_VC-080/4/156,159,0,0.9543683771757236,0.04563162282427644
   * WRAMC_VC-119/4/228,201,0,0.9971939916585995,0.0028060083414005333
   * ...
   * WRAMC_VC-019/2/36,168,1,0.05905134500592293,0.9409486549940771
   */
  double tol = 0.0000001;

  // first row
  BOOST_CHECK_EQUAL(col0.front(), "WRAMC_VC-080/4/156");
  BOOST_CHECK_EQUAL(col1.front(), 159);
  BOOST_CHECK_EQUAL(col2.front(), 0);
  BOOST_CHECK_CLOSE(col3.front(), 0.9543683771757236, tol);
  BOOST_CHECK_CLOSE(col4.front(), 0.04563162282427644, tol);

  // second row
  BOOST_CHECK_EQUAL(col0.at(1), "WRAMC_VC-119/4/228");
  BOOST_CHECK_EQUAL(col1.at(1), 201);
  BOOST_CHECK_EQUAL(col2.at(1), 0);
  BOOST_CHECK_CLOSE(col3.at(1), 0.9971939916585995, tol);
  BOOST_CHECK_CLOSE(col4.at(1), 0.0028060083414005333, tol);

  // last row
  BOOST_CHECK_EQUAL(col0.back(), "WRAMC_VC-019/2/36");
  BOOST_CHECK_EQUAL(col1.back(), 168);
  BOOST_CHECK_EQUAL(col2.back(), 1);
  BOOST_CHECK_CLOSE(col3.back(), 0.05905134500592293, tol);
  BOOST_CHECK_CLOSE(col4.back(), 0.9409486549940771, tol);

  // throw exception
  BOOST_CHECK_THROW(csv.getStringColumn(1), boost::bad_any_cast);
}


BOOST_AUTO_TEST_CASE(VCPredictSetTest) {
  VCPredictSet preds = VCPredictSet("/home/haxu/program/Slicer3/Modules/VirtualColon/Testing/predict.csv");
  shared_ptr< vector<VCPredict> > patPreds = preds.findPredict("WRAMC_VC-007", "4");
  BOOST_CHECK_EQUAL(patPreds->size(), 2);

  /*
  VCPredict polypPred = preds.findPredict("WRAMC_VC-080", "4", "159");
  BOOST_CHECK_EQUAL(polypPred.getPolypId(), "159");
  BOOST_CHECK_EQUAL(polypPred.getTarget(), 0);
  BOOST_CHECK_EQUAL(polypPred.getPatientId(), "WRAMC_VC-080");
  BOOST_CHECK_EQUAL(polypPred.getStudyId(), "4");
  */
  VCPredict_op_less op;
  sort(patPreds->begin(), patPreds->end(), op);
  for (int i = 0; i < patPreds->size(); i++)
    cout << patPreds->at(i).getPatientId() << "," << patPreds->at(i).getStudyId() << "," << patPreds->at(i).getPolypId() << endl;

}

BOOST_AUTO_TEST_CASE(VCPolypSetTest) {
  VCPolypSet polyps = VCPolypSet("/home/haxu/program/Slicer3/Modules/VirtualColon/Testing/WRAMC_VC-001_2_Polyps.csv");
  shared_ptr< vector<VCPolyp> > vecPolyp = polyps.getPolyps();
  BOOST_CHECK_EQUAL(vecPolyp->size(), 137);

  double tol = 0.0000001;
  VCPolyp polyp = vecPolyp->at(0);
  BOOST_CHECK_EQUAL(polyp.getPolypId(), "1");
  BOOST_CHECK_CLOSE(polyp.getX(), -8.5317, tol);
  BOOST_CHECK_CLOSE(polyp.getY(), 25.134, tol);
  BOOST_CHECK_CLOSE(polyp.getZ(), -462.8, tol);

  VCPolyp_op_less op;
  sort(vecPolyp->begin(), vecPolyp->end(), op);
  for (int i = 0; i < vecPolyp->size(); i++)
    cout << vecPolyp->at(i).getPolypId() << endl;
}

BOOST_AUTO_TEST_CASE(VCVoxelSetTest) {
  VCVoxelSet voxels = VCVoxelSet("/home/haxu/program/Slicer3/Modules/VirtualColon/Testing/WRAMC_VC-001_2_Voxels.csv");
  shared_ptr< vector<VCVoxel> > vecVoxel = voxels.findVoxels("1");
  BOOST_CHECK_EQUAL(vecVoxel->size(), 182);
  vecVoxel = voxels.getVoxels();
  BOOST_CHECK_EQUAL(vecVoxel->size(), 35591);

  // first line in CVS file
  // 1,-6.0689,21.948,-463.06,244,327,22,0.066503,0.084962,0.074126,0.44379,342.86,0.0065974,0.083114,0.065492,0.10074,0

  double tol = 0.0000001;
  VCVoxel voxel = vecVoxel->at(0);
  BOOST_CHECK_EQUAL(voxel.getPolypId(), "1");
  BOOST_CHECK_CLOSE(voxel.getX(), -6.0689, tol);
  BOOST_CHECK_CLOSE(voxel.getY(), 21.948, tol);
  BOOST_CHECK_CLOSE(voxel.getZ(), -463.06, tol);
}
