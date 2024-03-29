/*    ANALYZE TM Header File Format
*
*  (c) Copyright, 1986-1995
*  Biomedical Imaging Resource
*  Mayo Foundation
*
*  dbh.h
*
*  databse sub-definitions
*/
struct header_key         /* header key */ 
{                         /* off + size */
int sizeof_hdr;           /* 0 + 4 */
char data_type[10];       /* 4 + 10 */
char db_name[18];         /* 14 + 18 */
int extents;              /* 32 + 4 */
short int session_error;  /* 36 + 2 */
char regular;             /* 38 + 1 */
char hkey_un0;            /* 39 + 1 */
};                        /* total=40 bytes */

struct image_dimension 
{                             /* off + size */
	short int dim[8];     /* 0 + 16 */
	short int unused8;    /* 16 + 2 */
	short int unused9;    /* 18 + 2 */
	short int unused10;   /* 20 + 2 */
	short int unused11;   /* 22 + 2 */
	short int unused12;   /* 24 + 2 */
	short int unused13;   /* 26 + 2 */
	short int unused14;   /* 28 + 2 */
	short int datatype;   /* 30 + 2 */
	short int bitpix;     /* 32 + 2 */
	short int dim_un0;    /* 34 + 2 */
	float pixdim[8];      /* 36 + 32 */
	/* 
	   pixdim[] specifies the voxel dimensitons: 
	   pixdim[1] - voxel width
	   pixdim[2] - voxel height
	   pixdim[3] - interslice distance
	   ...etc
	*/
	float vox_offset;     /* 68 + 4 */
	float funused1;       /* 72 + 4 */
	float funused2;       /* 76 + 4 */
	float funused3;       /* 80 + 4 */
	float cal_max;        /* 84 + 4 */
	float cal_min;        /* 88 + 4 */
	float compressed;     /* 92 + 4 */
	float verified;       /* 96 + 4 */
	int glmax,glmin;      /* 100 + 8 */
};                            /* total=108 bytes */

struct data_history 
{                             /* off + size */
	char descrip[80];     /* 0 + 80 */
	char aux_file[24];    /* 80 + 24 */
	char orient;          /* 104 + 1 */
	char originator[10];  /* 105 + 10 */
	char generated[10];   /* 115 + 10 */
	char scannum[10];     /* 125 + 10 */
	char patient_id[10];  /* 135 + 10 */
	char exp_date[10];    /* 145 + 10 */
	char exp_time[10];    /* 155 + 10 */
	char hist_un0[3];     /* 165 + 3 */
	int views;             /* 168 + 4 */
	int vols_added;       /* 172 + 4 */
	int start_field;      /* 176 + 4 */
	int field_skip;       /* 180 + 4 */
	int omax, omin;       /* 184 + 8 */
	int smax, smin;       /* 192 + 8 */
};

struct dsr
{ 
	struct header_key hk; 	     /* 0 + 40 */
	struct image_dimension dime; /* 40 + 108 */
	struct data_history hist;    /* 148 + 200 */
};                                   /* total= 348 bytes */
