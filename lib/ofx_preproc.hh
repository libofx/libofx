/***************************************************************************
                          ofx_preproc.h
                             -------------------
    copyright            : (C) 2002 by Benoit Grégoire
    email                : bock@step.polymtl.ca
 ***************************************************************************/
/**@file
 * \brief Preprocessing of the OFX files before parsing 
 *
 Implements the pre-treatement of the OFX file prior to parsing:  OFX header striping, OFX proprietary tags and SGML comment striping, locating the appropriate DTD.
*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef OFX_PREPROC_H
#define OFX_PREPROC_H

/**
   \brief The number of different paths to search for DTDs.
   This must match the number of entry in DTD_SEARCH_PATH 
*/
#ifdef MAKEFILE_DTD_PATH
const int DTD_SEARCH_PATH_NUM = 4;
#else
const int DTD_SEARCH_PATH_NUM = 3;
#endif
 
/**
   \brief The list of paths to search for the DTDs.
   (Note that this should be modified so that the selected path from the makefile is added here automatically)
*/ 
const char *DTD_SEARCH_PATH[DTD_SEARCH_PATH_NUM] = { 
#ifdef MAKEFILE_DTD_PATH
  MAKEFILE_DTD_PATH , 
#endif
  "/usr/local/share/libofx/dtd/", 
  "/usr/share/libofx/dtd/", 
  "~/"};

#define OPENSPDCL_FILENAME "opensp.dcl"
#define OFX160DTD_FILENAME "ofx160.dtd"

///Removes proprietary tags and comments.
string sanitize_proprietary_tags(string input_string);
///Find the appropriate DTD for the file version.
string find_dtd(string dtd_filename);

#endif
