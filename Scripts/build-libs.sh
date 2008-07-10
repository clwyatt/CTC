#!/bin/bash
# This script eases the installation and maintinance of the
# several libraries and tools. Type build.sh for usage.
# clw 10/26/06


if [[ "${CXX}_0" = "_0" ]]
then
    CXX=gcc
fi

if [ $# -eq 0 ]
then
  echo "Usage: `basename $0` options (init, rebuild)"
  echo "       init initial build, checking out source from version control"
  echo "       rebuild rebuild, no version control access (for tweaking parameters)"
  exit          
fi  

TOPDIR=$PWD

##############################################################################
if [ $1 == "init" ]
then

mkdir CTC-Dependencies; cd CTC-Dependencies;


##########################
# VTK Library
##########################
echo "Checking out VTK from CVS"
TMPFILE=/tmp/build`date +%s`.expect
(cat <<'HERE' 
spawn cvs -d :pserver:anonymous@public.kitware.com:/cvsroot/VTK login
expect "*password:"
send "vtk\r"
expect eof
HERE
) > $TMPFILE

expect $TMPFILE
/bin/rm $TMPFILE

echo "Password OK"

cvs -d :pserver:anonymous@public.kitware.com:/cvsroot/VTK checkout -r release-4-4-2 VTK >> cvs.log 2>&1
cvs -d :pserver:anonymous@public.kitware.com:/cvsroot/VTK logout

echo "Building VisualizationToolkit"
VTKBUILDDIR=VTK-`uname -s`-`uname -m`-$CXX
mkdir $VTKBUILDDIR
cd $VTKBUILDDIR
export CFLAGS=-DUSE_NON_CONST
export CXXFLAGS=-DUSE_NON_CONST
# python libs?
# -lrt ?
cmake -D BUILD_EXAMPLES=OFF -D BUILD_TESTING=OFF -D BUILD_SHARED_LIBS=ON -D VTK_WRAP_TCL=ON -D VTK_USE_TK=ON ../VTK/ >> make.log 2>&1

make >> make.log 2>&1

export VTK_DIR=${TOPDIR}/CTC-Dependencies/$VTKBUILDDIR

cd $TOPDIR
cd CTC-Dependencies;


##########################
# InsightToolkit Library
##########################
echo "Checking out InsightToolkit from CVS"
TMPFILE=/tmp/build`date +%s`.expect
(cat <<'HERE' 
spawn cvs -d :pserver:anoncvs@www.itk.org:/cvsroot/Insight login
expect "*password:"
send "\r"
expect eof
HERE
) > $TMPFILE

expect $TMPFILE
/bin/rm $TMPFILE

echo "Password OK"

cvs -d :pserver:anoncvs@www.itk.org:/cvsroot/Insight co -r ITK-3-4 Insight >> cvs.log 2>&1
#cvs -d :pserver:anoncvs@www.itk.org:/cvsroot/Insight co Insight Applications
cvs -d :pserver:anoncvs@www.itk.org:/cvsroot/Insight logout

echo "Building InsightToolkit"
ITKBUILDDIR=ITK-`uname -s`-`uname -m`-$CXX
mkdir $ITKBUILDDIR
cd $ITKBUILDDIR
cmake -D BUILD_EXAMPLES=OFF -D BUILD_TESTING=OFF ../Insight/ >> make.log 2>&1
make >> make.log 2>&1

export ITK_DIR=${TOPDIR}/CTC-Dependencies/$ITKBUILDDIR

cd $TOPDIR
cd CTC-Dependencies;



##########################
# FLTK Library
##########################
echo "Checking out FLTK from Subversion"
svn co http://svn.easysw.com/public/fltk/fltk/branches/branch-1.1/ fltk-1.1

echo "Building FLTK"
FLTKBUILDDIR=FLTK-`uname -s`-`uname -m`-$CXX
mkdir $FLTKBUILDDIR
cd $FLTKBUILDDIR
export CXXFLAGS=-fpermissive
cmake -D BUILD_EXAMPLES=OFF -D BUILD_TESTING=OFF ../fltk-1.1/ >> make.log 2>&1
make >> make.log 2>&1

export FLTK_DIR=${TOPDIR}/CTC-Dependencies/$FLTKBUILDDIR

cd $TOPDIR
cd CTC-Dependencies;




##########################
# Write ENV setup scripts
##########################
echo "export VTK_DIR="$VTK_DIR >> bsl-kit-setup.sh
echo "export ITK_DIR="$ITK_DIR >> bsl-kit-setup.sh
echo "export FLTK_DIR="$FLTK_DIR >> bsl-kit-setup.sh

echo "setenv VTK_DIR " $VTK_DIR >> bsl-kit-setup.csh
echo "setenv ITK_DIR " $ITK_DIR >> bsl-kit-setup.csh
echo "setenv FLTK_DIR " $FLTK_DIR >> bsl-kit-setup.csh

fi



##############################################################################
if [ $1 == "rebuild" ]
then

cd CTC-Dependencies;



##########################
# VTK Library
##########################

echo "Rebuilding VisualizationToolkit"
VTKBUILDDIR=VTK-`uname -s`-`uname -m`-$CXX
rm -rf $VTKBUILDDIR
mkdir $VTKBUILDDIR
cd $VTKBUILDDIR
export CFLAGS=-DUSE_NON_CONST
export CXXFLAGS=-DUSE_NON_CONST
# python libs?
# -lrt ?
cmake -D BUILD_EXAMPLES=OFF -D BUILD_TESTING=OFF -D BUILD_SHARED_LIBS=ON -D VTK_WRAP_TCL=ON -D VTK_USE_TK=ON ../VTK/ >> make.log 2>&1

make >> make.log 2>&1

export VTK_DIR=${TOPDIR}/CTC-Dependencies/$VTKBUILDDIR

cd $TOPDIR
cd CTC-Dependencies;


##########################
# InsightToolkit Library
##########################
echo "Rebuilding InsightToolkit"
ITKBUILDDIR=ITK-`uname -s`-`uname -m`-$CXX
rm -rf  $ITKBUILDDIR
mkdir $ITKBUILDDIR
cd $ITKBUILDDIR
cmake -D BUILD_EXAMPLES=OFF -D BUILD_TESTING=OFF ../Insight/ >> make.log 2>&1
make >> make.log 2>&1

export ITK_DIR=${TOPDIR}/CTC-Dependencies/$ITKBUILDDIR

cd $TOPDIR
cd CTC-Dependencies;

##########################
# FLTK Library
##########################
echo "Rebuilding FLTK"

FLTKBUILDDIR=FLTK-`uname -s`-`uname -m`-$CXX
rm -rf $FLTKBUILDDIR
mkdir $FLTKBUILDDIR
cd $FLTKBUILDDIR
export CXXFLAGS=-fpermissive
cmake -D BUILD_EXAMPLES=OFF -D BUILD_TESTING=OFF ../fltk-1.1/ >> make.log 2>&1
make >> make.log 2>&1

export FLTK_DIR=${TOPDIR}/CTC-Dependencies/$FLTKBUILDDIR

cd $TOPDIR
cd CTC-Dependencies;

fi



