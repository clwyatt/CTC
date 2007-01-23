// dbh.h
//
// (c) Copyright, 1986-1995
// Biomedical Imaging Resource
// Mayo Foundation
//
// Annotated by Andy Crabb, Johns Hopkins University Radiology Dept, 01/00.
// database sub-definitions
// Required fields shown by '*' after offset & size.

#ifndef DBH_H
#define DBH_H



#define MYINT8 0
#define MYINT16 1
#define MYINT32 2
#define MYFLOAT 3
#define MYDOUBLE 4
#define MYUINT8 5
#define MYUINT16 6
#define MYUINT32 7


// Acceptable values for hdr.dime.datatype 

const int DT_NONE            = 0;
const int DT_UNKNOWN         = 0;   // Unknown data type.
const int DT_BINARY          = 1;   // 1 bit per voxel.
const int DT_UNSIGNED_CHAR   = 2;   // 8 bits per voxel.
const int DT_SIGNED_SHORT    = 4;   // 16 bits per voxel.
const int DT_SIGNED_INT      = 8;   // 32 bits per voxel.
const int DT_FLOAT           = 16;  // 32 bits per voxel.
const int DT_COMPLEX         = 32;  // 64 bits per voxel (2 floats)
const int DT_DOUBLE          = 64;  // 64 bits per voxel.
const int DT_RGB             = 128;
const int DT_ALL             = 255;
const int DT_SIGNED_CHAR = 130;
const int DT_UNSIGNED_SHORT  = 132;
const int DT_UNSIGNED_INT  = 136;


const short types[8]   = { DT_SIGNED_CHAR  ,   
		     DT_SIGNED_SHORT ,
		     DT_SIGNED_INT,
		     DT_FLOAT,
		     DT_DOUBLE,
		     DT_UNSIGNED_CHAR  ,   
		     DT_UNSIGNED_SHORT ,
		     DT_UNSIGNED_INT};
		
const float maxval[8]  = {127, 32767, 214748/*3647*/, 1e64, 1e64, 255 ,65535 ,4292967295};
const float minval[8]  = {-128, -32768 ,-214748/*3648*/ ,-1e64, -1e64, 0, 0, 0};
const short bits[8]    = 	{ 8,     16 ,    32,   32  , 64,     8    , 16   ,  32};

// Indexes into dim array.
const int T_DIM = 0;	// Time dimension; No of dimension in database.
const int X_DIM = 1;	// X dim; Pixels per row OR voxel width in mm.
const int Y_DIM = 2;	// Y dim; Rows per slice OR voxel height in mm.
const int Z_DIM = 3;	// Z dim; Slices per vol OR slice thickness in mm.
const int N_DIM = 4;	// Number of volumes per file.


struct header_key {         
  int	sizeof_hdr;	// 0  + 4   * Byte size of header file.
  char	data_type[10];	// 4  + 10   
  char	db_name[18];	// 14 + 18  
  int	extents;	// 32 + 4   * = 16384; Min contig img file size.
  short	session_error;	// 36 + 2   
  char	regular;	// 38 + 1   * = 'r'; All images & vols same size
  char	hkey_un0;	// 39 + 1   

  header_key( void );
  void fill( void );
};           		// total=40 

struct image_dimension {    // Organization and size of images.
  short dim[8];         // 0 + 16   Image dimensions (see const defs).
  char	vox_units[4];	// 16 + 4   Voxel spatial units of measure.
  char	cal_units[8];	// 20 + 4   Name of the calibration unit.
  short unused1;	// 24 + 2   
  short datatype;	// 30 + 2   See datatype defs DT_xxx.
  short bitpix;         // 32 + 2   Bits per pixel.
  short dim_un0;        // 34 + 2   Unused.
  float	pixdim[8];      // 36 + 32  Parallel to dim[], real world meas.
  float vox_offset;	// 68 + 4   Voxel byte offset in .img file.
  float roi_scale;      // 72 + 4   
  float funused1;       // 76 + 4   
  float funused2;       // 80 + 4   
  float cal_max;        // 84 + 4   Calibrated max val.
  float cal_min;        // 88 + 4   Calibrated min val.
  int	compressed;     // 92 + 4   
  int	verified;       // 96 + 4   
  int	glmax, glmin;   // 100 + 8  Global max min pixel vals for database.

  image_dimension( void );
  void fill( int xdim, int ydim, int zdim, float dx, float dy, float dz, int index);
  void print( void );
};          		// total=108
         
struct data_history {   // Not required.
  char	descrip[80];	// 0 + 80   
  char	aux_file[24];   // 80 + 24  
  char	orient;         // 104 + 1  
  char	originator[10]; // 105 + 10 
  char	generated[10];  // 115 + 10 
  char	scannum[10];    // 125 + 10 
  char	patient_id[10]; // 135 + 10 
  char	exp_date[10];   // 145 + 10 
  char	exp_time[10];   // 155 + 10 
  char	hist_un0[3];    // 165 + 3  
  int	views;          // 168 + 4  
  int	vols_added;     // 172 + 4  
  int	start_field;    // 176 + 4  
  int	field_skip;     // 180 + 4  
  int	omax,omin;      // 184 + 8  
  int	smax,smin;      // 192 + 8  

  data_history( void );
  void print( void );
  void fill(int fr );
};                     	// total=200

struct dsr {
  header_key	  hk;   // 0 + 40   
  image_dimension dime; // 40 + 108 
  data_history	  hist; // 148 + 200
};                     	// total=348

typedef struct 
{
  float real;
  float imag;
} COMPLEX;



#endif
