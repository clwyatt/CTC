// Implementation of class AnalyzeHdr, Analyze header file structure.

#include "AnalyzeHdr.hh"
#include <stdio.h>


header_key::header_key( void ) {
  sizeof_hdr = 0;
  data_type[0] = '\0';
  db_name[0] = '\0';
  extents = 0;
  session_error = 0;
  regular = '\0';
  hkey_un0 = '\0';
}

void header_key::fill() {
  // These fields do not depend on the Advance object.
  regular = 'r';
  sizeof_hdr = sizeof(struct dsr);
  extents = 16384;

  // Fields that depend on the Advance object.
  strcpy( data_type,"UNKNOW" );
}

image_dimension::image_dimension( void ) {
  for ( int i = 0; i < 8; i++ ) {
    dim[i] = 0;
    pixdim[i] = 0.0;
  }
  vox_units[0] = '\0';
  cal_units[0] = '\0';
  unused1 = 0;
  datatype = 0;
  bitpix = 0;
  dim_un0 = 0;
  vox_offset = 0;
  roi_scale = 0;
  funused1 = 0;
  funused2 = 0;
  cal_max = 0;
  cal_min = 0;
  compressed = 0;
  verified = 0;
  glmax = 0;
  glmin = 0;
}

void image_dimension::print( void ) {
  int		i; 

  printf( "----------  image_dimension::print ----------\n" );
  for ( i = 0; i < 8; i++ )
    printf( "\tDim %d", i );
  printf( "\nCount" );
  for ( i = 0; i < 8; i++ )
    printf( "\t%-3d", dim[i] );
  printf( "\nDim" );
  for ( i = 0; i < 8; i++ )
    printf( "\t%-6.3f", pixdim[i] );
  printf( "\nVoxel units:\t%s\n", vox_units );
  printf( "Calibr. units:\t%s\n", cal_units );
  printf( "Data type:\t%d\n", datatype );
  printf( "Bits per pixel:\t%d\n", bitpix );
  printf( "Global max:\t%d\n", glmax );
  printf( "Global min:\t%d\n", glmin );
  printf( "------------------------------------------\n" );
}

void image_dimension::fill(int xdim,int  ydim,int zdim, float dx, float dy,
float dz,int type)
{

  dim[T_DIM] = 4;		// All Analyze files have time dimension.
  dim[X_DIM] = xdim;
  dim[Y_DIM] = ydim;
  dim[Z_DIM] = zdim;
  
  // No of frames in this file depends on whether saving as one vol.
  dim[N_DIM] = 1;

  strcpy( cal_units,  "pixel");
  sprintf( vox_units, "pixel" );


    datatype = types[type];
    bitpix = bits[type]; //assume 16 bit images
    glmax = maxval[type];
    glmin = minval[type];
  //}
  pixdim[X_DIM] = dx;
  pixdim[Y_DIM] = dy;
  pixdim[Z_DIM] = dz;

}

data_history::data_history( void ) {
  descrip[0] = '\0';
  aux_file[0] = '\0';
  orient = '\0';
  originator[0] = '\0';
  generated[0] = '\0';
  scannum[0] = '\0';
  patient_id[0] = '\0';
  exp_date[0] = '\0';
  exp_time[0] = '\0';
  hist_un0[0] = '\0';
  views = 0;
  vols_added = 0;
  start_field = 0;
  field_skip = 0;
  omax = omin = 0;
  smax = smin = 0;

//    strcpy(descrip, "descrip");
//    strcpy(aux_file, "aux_file");
//    orient = 'o';
//    strcpy(originator, "originator");
//    strcpy(generated, "generated");
//    strcpy(scannum, "scannum");
//    strcpy(patient_id, "patient_id");
//    strcpy(exp_date, "exp_date");
//    strcpy(exp_time, "exp_time");
//    strcpy(hist_un0, "h0");
//    views = 1100;
//    vols_added = 1101;
//    start_field = 1102;
//    field_skip = 1103;
//    omax = omin = 1104;
//    smax = smin = 1105;
}


void data_history::fill( int fr ) {
  float scnst = 0;
  float frmdu = 0;
  float frmst = 0;
  float midPoint = 0;

  // Use descrip field to hold frame start & duration and scan start.
  sprintf( descrip, "frmS %6d, frmD %6d, scanS %6d", 
	   (int)frmst, (int)frmdu, (int)scnst );
  // Use scannum field to hold frame no in study and frame midpoint (sec).
  sprintf( scannum, "%02d %d", fr, (int)midPoint );
  strcpy( patient_id, "imtk" );
  patient_id[9] = '\0';
  strcpy( exp_date, "exp_date" );
  strcpy( exp_time, "exp_time" );
}

void data_history::print( void ) {
  printf( "----------  data_history::print ----------\n" );
  printf( "descrip:    %s\n", descrip );
  printf( "scannum:    %s\n", scannum );
  printf( "patient_id: %s\n", patient_id );
  printf( "------------------------------------------\n" );
  
}

