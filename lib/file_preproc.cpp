/***************************************************************************
          file_type_detect.cpp 
                             -------------------
    copyright            : (C) 2004 by Benoit Grégoire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
 * \brief File type detection, etc. 
 *
 * Implements AutoDetection of file type, and handoff to specific parsers.
*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "libofx.h"
#include "messages.hh"
#include "ofx_preproc.hh"
#include "file_preproc.hh"

using namespace std;
const unsigned int READ_BUFFER_SIZE = 1024;

/**
 * Instance of the callback registry
 */
struct OfxCallbackRegistry cb_registry;

void ofx_prep_cb(
		 int (*ofx_statement)(const struct OfxStatementData data, void * user_data),
		 int (*ofx_account)(const struct OfxAccountData data, void * user_data),
		 int (*ofx_transaction)(const struct OfxTransactionData data, void * user_data),
		 int (*ofx_security)(const struct OfxSecurityData data, void * user_data),
		 int (*ofx_status)(const struct OfxStatusData data, void * user_data)
		 )
{
  /* assign callbacks...these can be overridden in client code */
  
  cb_registry.ofx_statement_cb=ofx_statement;
  cb_registry.ofx_transaction_cb=ofx_transaction;
  cb_registry.ofx_security_cb=ofx_security;
  cb_registry.ofx_account_cb=ofx_account;
  cb_registry.ofx_status_cb=ofx_status;
};

/* get_file_type_description returns a string description of a LibofxFileType 
 * suitable for debugging output or user communication.
 */
const char * get_file_type_description(enum LibofxFileType file_type)
{
  const char * retval;
  switch (file_type)
    {
    case UNKNOWN: retval="UNKNOWN (File format couldn't be sucessfully identified)";
      break;
    case AUTODETECT: retval="AUTODETECT (File format will be automatically detected later)";
      break;
    case OFX: retval="OFX (Open Financial eXchange (OFX or QFX))";
      break;
    case OFC: retval="OFC (Microsoft Open Financial Connectivity)";
      break;
    case QIF: retval="QIF (Intuit Quicken Interchange Format)";
      break;
    default: retval="WRITEME: The file format passed as parameter is unknown to get_file_type_description()";
    }
  return retval;
};

int libofx_proc_file(const char * p_filename, LibofxFileType p_file_type)
{
  LibofxFileType file_type;

  if(p_file_type==AUTODETECT)
    {
      message_out(INFO, string("libofx_proc_file(): File format not specified, autodecting..."));
      file_type = libofx_detect_file_type( p_filename);
      message_out(INFO, string("libofx_proc_file(): Detected file format: ")+get_file_type_description(file_type ));
    }
  else
    {
      message_out(INFO, string("libofx_proc_file(): File format forced to: ")+get_file_type_description(file_type ));
      file_type = p_file_type;
    }

  switch (file_type)
    {
    case OFX: ofx_proc_file(p_filename, file_type);
      break;
    case OFC: ofx_proc_file(p_filename, file_type);
      break;
    default: message_out(ERROR, string("libofx_proc_file(): Detected file format not yet supported ou couldn't detect file format; aborting."));
    }
  return 0;
}

enum LibofxFileType libofx_detect_file_type(const char * p_filename)
{
  enum LibofxFileType retval = UNKNOWN;
  ifstream input_file;
  char buffer[READ_BUFFER_SIZE];
  char tmp;
  string s_buffer;
  bool type_found=false;
  
  if(p_filename!=NULL&&strcmp(p_filename,"")!=0)
    {
      message_out(DEBUG, string("libofx_detect_file_type():Opening file: ")+p_filename);
      
      input_file.open(p_filename);
      
      if(!input_file)
	{
	  message_out(ERROR,"libofx_detect_file_type():Unable to open the input file "+string(p_filename));
	}
      else
	{
	  do
	    {
	      input_file.getline(buffer, sizeof(buffer),'\n');
	      //cout<<buffer<<"\n";
	      s_buffer.assign(buffer);
	      //cout<<"input_file.gcount(): "<<input_file.gcount()<<" sizeof(buffer): "<<sizeof(buffer)<<endl;
	      if(input_file.gcount()<(sizeof(buffer)-1))
		{
		  s_buffer.append("\n");//Just in case...
		}
	      else if( !input_file.eof()&&input_file.fail())
		{
		  input_file.clear();
		}
	      
	      if(s_buffer.find("<OFX>")!=string::npos||s_buffer.find("<ofx>")!=string::npos)
		{
		  message_out(DEBUG,"libofx_detect_file_type():<OFX> tag has been found");
		  retval=OFX;
		  type_found=true;
		}
	      else if(s_buffer.find("<OFC>")!=string::npos||s_buffer.find("<ofc>")!=string::npos)
		{
		  message_out(DEBUG,"libofx_detect_file_type():<OFC> tag has been found");
		  retval=OFC;
		  type_found=true;
		}
	      
	    }while(type_found==false&&!input_file.eof()&&!input_file.bad());
	}
      input_file.close();
    }
  else{
    message_out(ERROR,"libofx_detect_file_type(): No input file specified");
  }
  if(retval==UNKNOWN)
    message_out(ERROR,"libofx_detect_file_type(): Failed to identify input file format");
  return retval;
}
  




