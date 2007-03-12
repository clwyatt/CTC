
SET(GCOLON_SRCS
bvolume.cc
canny.cc
curvature.cc
dilate.cc
elongation.cc
fast_calc_dt.cc
free_fvector.cc
free_ivector.cc
free_matrix.cc
gcolon.cc
grad3d.cc
invertMatrix.cc
matrix.cc
morph.cc
nrerror.cc
plane.cc
rbtree.cc
read_dcm.cc
read_raw.cc
read_rvf.cc
region_grow.cc
rot_tran.cc
svbksb.cc
svdcmp.cc
vector.cc)

ADD_EXECUTABLE(gcolon ${GCOLON_SRCS} ) 

TARGET_LINK_LIBRARIES(gcolon)

