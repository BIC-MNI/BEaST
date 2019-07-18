/*  beast_lib.c
 *
 *  Copyright 2011  Simon Fristed Eskildsen, Vladimir Fonov,
 *   	      	    Pierrick Coupé, Jose V. Manjon
 *
 *  This file is part of mincbeast.
 *
 *  mincbeast is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mincbeast is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mincbeast.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  For questions and feedback, please contact:
 *  Simon Fristed Eskildsen <eskild@gmail.com> 
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif //HAVE_CONFIG_H


#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "array_alloc.h"
#include "beast.h"

#include "ParseArgv.h"

char* create_minc_timestamp(int argc,char *argv[])
{
  char *timestamp;
  char cur_time[1024];
  time_t t;
  struct tm *tmp;
  size_t str_len;
  int i;

  t = time(NULL);
  tmp = localtime(&t);
  
  strftime(cur_time, sizeof(cur_time), "%a %b %d %T %Y>>>", tmp);
  /* Get the time, overwriting newline */
  str_len=strlen(cur_time);
  for (i=0; i<argc; i++) 
    str_len+=strlen(argv[i])+1;
  
  timestamp=(char *)malloc(str_len+3);
  strcpy(timestamp,cur_time);
  
  /* Copy the program name and arguments */
  for (i=0; i<argc; i++) {
    strcat(timestamp,argv[i]);
    strcat(timestamp," ");
  }
  strcat(timestamp,"\n");
  return timestamp;
}

int init_arguments(beast_options * _options)
{
  _options->sparse_stride=1;
  _options->lambda1      = 0.15;
  _options->lambda2      = 0.0;
  _options->sparse_mode  = 2;
  _options->outputprob   = FALSE;
  _options->flipimages   = FALSE;
  _options->load_moments = FALSE;
  _options->fill_output  = FALSE;
  _options->verbose      = FALSE;
  _options->medianfilter = FALSE;
  _options->patchfilter  = FALSE;
  _options->abspath      = FALSE;
  _options->same_res     = TRUE;
  _options->clobber      = FALSE;
  _options->nomask       = FALSE;
  _options->nopositive   = FALSE;
  _options->use_sparse   = FALSE;
  _options->v2           = FALSE;
  _options->use_double   = FALSE;
  
  _options->voxelsize    = 4;
  _options->sizepatch    = 1;
  _options->searcharea   = 2;
  _options->alpha        = 0.5;
  _options->beta         = 0.25;
  _options->threshold    = 0.95;
  _options->selectionsize = 20;
  _options->kappa_limit  = 0.9;
  _options->positive_file = NULL;
  _options->selection_file= NULL;
  _options->count_file  = NULL;
  _options->conf_file   = NULL;
  _options->mask_file   = NULL;
  _options->compare_file= NULL;
  
  _options->library_prefix = "library";
  return 0;
}


int get_arguments(int argc, char  *argv[] , beast_options * _options)
{
  init_arguments(_options);
  
  /* Argument table */
  ArgvInfo argTable[] = {
    {
      "-probability", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->outputprob,
      "Output the probability map instead of crisp mask."
    },
    {
      "-flip", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->flipimages,
      "Flip images around the mid-sagittal plane to increase patch count."
    },
    {
      "-load_moments", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->load_moments,
      "Do not calculate moments instead use precalculated library moments. (for optimization purposes)"
    },
    {
      "-fill", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->fill_output,
      "Fill holes in the binary output."
    },
    {
      "-median", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->medianfilter,
      "Apply a median filter on the probability map."
    },
    {
      "-nlm_filter", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->patchfilter,
      "Apply an NLM filter on the probability map (experimental)."
    },
    {
      "-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->verbose,
      "Enable verbose output."
    },
    {
      "-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->clobber,
      "Clobber output files"
    },
    {
      "-abspath", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->abspath,
      "File paths in the library are absolute (default is relative to library root)."
    },

    {
      "-voxel_size", ARGV_INT, (char *) 1, (char *) &_options->voxelsize,
      "Specify voxel size for calculations (4, 2, or 1). Assumes no multiscale. Use configuration file for multiscale."
    },
    {
      "-patch_size", ARGV_INT, (char *) 1, (char *) &_options->sizepatch,
      "Specify patch size for single scale approach."
    },
    {
      "-search_area", ARGV_INT, (char *) 1, (char *) &_options->searcharea,
      "Specify size of search area for single scale approach."
    },
    {
      "-alpha", ARGV_FLOAT, (char *) 1, (char *) &_options->alpha,
      "Specify confidence level Alpha."
    },
    {
      "-beta", ARGV_FLOAT, (char *) 1, (char *) &_options->beta,
      "Specify smoothness factor Beta."
    },
    {
      "-threshold", ARGV_FLOAT, (char *) 1, (char *) &_options->threshold,
      "Specify threshold for patch selection."
    },
    {
      "-selection_num", ARGV_INT, (char *) 1, (char *) &_options->selectionsize,
      "Specify number of selected images."
    },

    {
      "-positive", ARGV_STRING, (char *) 1, (char *) &_options->positive_file,
      "Specify mask of positive segmentation (inside mask) instead of the default mask."
    },
    {
      "-output_selection", ARGV_STRING, (char *) 1, (char *) &_options->selection_file,
      "Specify file to output selected files."
    },
    {
      "-count", ARGV_STRING, (char *) 1, (char *) &_options->count_file,
      "Specify file to output the patch count."
    },
    {
      "-configuration", ARGV_STRING, (char *) 1, (char *) &_options->conf_file,
      "Specify configuration file."
    },
    {
      "-mask", ARGV_STRING, (char *) 1, (char *) &_options->mask_file,
      "Specify a segmentation mask instead of the the default mask."
    },
    {
      "-same_resolution", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->same_res,
      "Output final mask with the same resolution as input file."
    },
    {
      "-no_same_resolution", ARGV_CONSTANT, (char *) FALSE, (char *) &_options->same_res,
      "Output final mask downsampled at processing resolution."
    },
    {
      "-no_mask", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->nomask,
      "Do not apply a segmentation mask. Perform the segmentation over the entire image."
    },
    {
      "-no_positive", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->nopositive,
      "Do not apply a positive mask."
    },
    {
      "-ssd", ARGV_CONSTANT, (char *) FALSE, (char *) &_options->use_sparse,
      "Use SSD patch merging (opposite to sparse, default)"
    },
#ifdef USE_SPAMS
    {
      "-sparse", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->use_sparse,
      "Use sparse patch merging."
    },
    {
      "-lambda1", ARGV_FLOAT, (char *) 1, (char *) &_options->lambda1,
      "Sparsity cost lambda1."
    },
    {
      "-lambda2", ARGV_FLOAT, (char *) 1, (char *) &_options->lambda2,
      "Sparsity cost lambda2."
    },
    {
      "-sparse_mode", ARGV_INT, (char *) 1, (char *) &_options->sparse_mode,
      "Sparse mode."
    },
    
    {
      "-stride", ARGV_INT, (char *) 1, (char *) &_options->sparse_stride,
      "Stride for spars segmentation speedup with possible quality degradation. (DON'T USE!)"
    },
#endif
    //library_prefix
    {
      "-library_prefix", ARGV_STRING, (char *) 1, (char *) &_options->library_prefix,
      "library prefix, for cross-validation experiment."
    },
    {
      "-v1", ARGV_CONSTANT, (char *) FALSE, (char *) &_options->v2,
      "Run mincbeast v1 (preselection done at each resoltution independently). (default)"
    },
    {
      "-v2", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->v2,
      "Run mincbeast v2 (preselection done onece at 1mm)."
    },
    {
      "-double", ARGV_CONSTANT, (char *) TRUE, (char *) &_options->use_double,
      "Use double precision for calculations."
    },
    {
      "-single", ARGV_CONSTANT, (char *) FALSE, (char *) &_options->use_double,
      "Use single precision for calculations. (default)"
    },
    {
      "-compare", ARGV_STRING, (char *) 1, (char *) &_options->compare_file,
      "Reference mask for comparision."
    },
    {
      "-kappa_limit", ARGV_FLOAT, (char *) 1, (char *) &_options->kappa_limit,
      "Specify threshold for overlap kappa (for comparision)."
    },
    
    {NULL, ARGV_END, NULL, NULL, NULL}
  };
  
  _options->history_label=create_minc_timestamp(argc,argv);

  /* Get arguments */
  if ( ParseArgv(&argc, argv, argTable, 0) || (argc < 4) ) {
    fprintf(stderr,LICENSE);
    fprintf(stderr,REFERENCE);
    fprintf(stderr,
            "\nUsage: %s [options] <library dir> <input> <output>\n",
            argv[0]);
    fprintf(stderr,"       %s -help\n\n", argv[0]);

    return STATUS_ERR;
  }

  _options->libdir      = argv[argc-3];
  _options->input_file  = argv[argc-2];
  _options->output_file = argv[argc-1];
  
  return STATUS_OK;
}


void cleanup_arguments(beast_options * _options)
{
  free(_options->history_label);
  
  if(_options->mask_file)
    free(_options->mask_file);
  if(_options->positive_file)
    free(_options->positive_file);
}

/* kate: indent-mode cstyle; indent-width 2; replace-tabs on; */

