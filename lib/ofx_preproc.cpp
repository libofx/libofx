/***************************************************************************
          ofx_preproc.cpp 
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�oir
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
#include "../config.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "ParserEventGeneratorKit.h"
#include "libofx.h"
#include "messages.hh"
#include "ofx_sgml.hh"
#include "ofc_sgml.hh"
#include "ofx_preproc.hh"
#include "ofx_utilities.hh"
#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#ifdef OS_WIN32
# include "win32.hh"
#endif

#define LIBOFX_DEFAULT_INPUT_ENCODING "CP1252"
#define LIBOFX_DEFAULT_OUTPUT_ENCODING "UTF-8"

using namespace std;
/**
   \brief The number of different paths to search for DTDs.
*/
#ifdef MAKEFILE_DTD_PATH
const int DTD_SEARCH_PATH_NUM = 4;
#else
const int DTD_SEARCH_PATH_NUM = 3;
#endif
 
/**
   \brief The list of paths to search for the DTDs.
*/ 
const char *DTD_SEARCH_PATH[DTD_SEARCH_PATH_NUM] = { 
#ifdef MAKEFILE_DTD_PATH
  MAKEFILE_DTD_PATH , 
#endif
  "/usr/local/share/libofx/dtd/", 
  "/usr/share/libofx/dtd/", 
  "~/"};
const unsigned int READ_BUFFER_SIZE = 1024;

/** @brief File pre-processing of OFX AND for OFC files 
*
* Takes care of comment striping, dtd locating, etc.
*/
CFCT int ofx_proc_file(LibofxContextPtr ctx, const char * p_filename)
  {
  LibofxContext *libofx_context;
  bool ofx_start=false;
  bool ofx_end=false;

  ifstream input_file;
  ofstream tmp_file;
  char buffer[READ_BUFFER_SIZE];
  char iconv_buffer[READ_BUFFER_SIZE * 2];
  string s_buffer;
  char *filenames[3];
  char tmp_filename[256];
#ifdef HAVE_ICONV
	iconv_t conversion_descriptor;
#endif
  libofx_context=(LibofxContext*)ctx;

  if(p_filename!=NULL&&strcmp(p_filename,"")!=0)
    {
    message_out(DEBUG, string("ofx_proc_file():Opening file: ")+ p_filename);
    
    input_file.open(p_filename);
    mkTempFileName("libofxtmpXXXXXX", tmp_filename, sizeof(tmp_filename));
    mkstemp(tmp_filename);
    tmp_file.open(tmp_filename);

    message_out(DEBUG,"ofx_proc_file(): Creating temp file: "+string(tmp_filename));
    if(!input_file){
      message_out(ERROR,"ofx_proc_file():Unable to open the input file "+string(p_filename));
    }
    else if(!tmp_file){
      message_out(ERROR,"ofx_proc_file():Unable to open the output file "+string(tmp_filename));
    }
    else
      {
	int header_separator_idx;
	string header_name;
	string header_value;
	string ofx_encoding;
	string ofx_charset;
	do {
          input_file.getline(buffer, sizeof(buffer),'\n');
	  //cout<<buffer<<"\n";
	  s_buffer.assign(buffer);
	  //cout<<"input_file.gcount(): "<<input_file.gcount()<<" sizeof(buffer): "<<sizeof(buffer)<<endl;
	  if(input_file.gcount()<(sizeof(buffer)-1))
	    {
	      s_buffer.append("\n");
	    }
	  else if( !input_file.eof()&&input_file.fail())
	    {
	      input_file.clear();
	    }
	  int ofx_start_idx;
          if (ofx_start==false &&
              (
               (libofx_context->currentFileType()==OFX&&
                ((ofx_start_idx=s_buffer.find("<OFX>"))!=
                 string::npos||(ofx_start_idx=s_buffer.find("<ofx>"))!=string::npos))
               || (libofx_context->currentFileType()==OFC&&
                   ((ofx_start_idx=s_buffer.find("<OFC>"))!=string::npos||
                    (ofx_start_idx=s_buffer.find("<ofc>"))!=string::npos))
              )
             )
            {
              ofx_start=true;
              s_buffer.erase(0,ofx_start_idx);//Fix for really broken files that don't have a newline after the header.
              message_out(DEBUG,"ofx_proc_file():<OFX> or <OFC> has been found");
#ifdef HAVE_ICONV
	      string fromcode;
	      string tocode; 
	      if(ofx_encoding.compare("USASCII")==0){
		if(ofx_charset.compare("ISO-8859-1")==0){
		  fromcode="ISO-8859-1";
		}
		else if(ofx_charset.compare("1252")==0){
		  fromcode="CP1252";
		}
		else if(ofx_charset.compare("NONE")==0){
		  fromcode=LIBOFX_DEFAULT_INPUT_ENCODING;
		}
	      }
	      else if(ofx_encoding.compare("USASCII")==0) {
		fromcode="UTF-8";
	      }
	      else
		{
		  fromcode=LIBOFX_DEFAULT_INPUT_ENCODING;
		}
	      tocode = LIBOFX_DEFAULT_OUTPUT_ENCODING;
	      message_out(DEBUG,"ofx_proc_file(): Setting up iconv for fromcode: "+fromcode+", tocode: "+tocode);
	      conversion_descriptor = iconv_open (tocode.c_str(), fromcode.c_str());
#endif
            }
	  else {
	    //We are still in the headers
	    if ((header_separator_idx=s_buffer.find(':')) != string::npos) {
	      //Header processing
	      header_name.assign(s_buffer.substr(0,header_separator_idx));
	      header_value.assign(s_buffer.substr(header_separator_idx+1));
	      message_out(DEBUG,"ofx_proc_file():Header: "+header_name+" with value: "+header_value+" has been found");
	      if(header_name.compare("ENCODING")==0) {
		ofx_encoding.assign(header_value);
	      }
	      if(header_name.compare("CHARSET")==0) {
		ofx_charset.assign(header_value);
	      }
	    }
	  }

          if(ofx_start==true && ofx_end==false){
            s_buffer=sanitize_proprietary_tags(s_buffer);
            //cout<< s_buffer<<"\n";
#ifdef HAVE_ICONV
	    memset(iconv_buffer,0,READ_BUFFER_SIZE * 2);
	    size_t inbytesleft = strlen(s_buffer.c_str());
	    size_t outbytesleft = READ_BUFFER_SIZE * 2 - 1;
#ifdef OS_WIN32
	    const char * inchar = (const char *)s_buffer.c_str();
#else
	    char * inchar = (char *)s_buffer.c_str();
#endif
	    char * outchar = iconv_buffer;
	    int iconv_retval = iconv (conversion_descriptor,
		    &inchar, &inbytesleft,
		   &outchar, &outbytesleft);
	    if(iconv_retval==-1){
	      message_out(ERROR,"ofx_proc_file(): Conversion error");
	    }
	    s_buffer = iconv_buffer;
#endif
	      tmp_file.write(s_buffer.c_str(), s_buffer.length());
	  }
	  
          if (ofx_start==true &&
              (
               (libofx_context->currentFileType()==OFX &&
                ((ofx_start_idx=s_buffer.find("</OFX>"))!=string::npos ||
                 (ofx_start_idx=s_buffer.find("</ofx>"))!=string::npos))
               || (libofx_context->currentFileType()==OFC &&
                   ((ofx_start_idx=s_buffer.find("</OFC>"))!=string::npos ||
                    (ofx_start_idx=s_buffer.find("</ofc>"))!=string::npos))
              )
             )
            {
              ofx_end=true;
              message_out(DEBUG,"ofx_proc_file():</OFX> or </OFC>  has been found");
            }

        } while(!input_file.eof()&&!input_file.bad());
      }
    input_file.close();
    tmp_file.close();
#ifdef HAVE_ICONV
	      iconv_close(conversion_descriptor);
#endif
    char filename_openspdtd[255];
    char filename_dtd[255];
    char filename_ofx[255];
    strncpy(filename_openspdtd,find_dtd(ctx, OPENSPDCL_FILENAME).c_str(),255);//The opensp sgml dtd file
    if(libofx_context->currentFileType()==OFX)
      {
        strncpy(filename_dtd,find_dtd(ctx, OFX160DTD_FILENAME).c_str(),255);//The ofx dtd file
      }
    else if(libofx_context->currentFileType()==OFC)
      {
        strncpy(filename_dtd,find_dtd(ctx, OFCDTD_FILENAME).c_str(),255);//The ofc dtd file
      }
    else
      {
        message_out(ERROR,string("ofx_proc_file(): Error unknown file format for the OFX parser"));
      }

    if((string)filename_dtd!="" && (string)filename_openspdtd!="")
      {
        strncpy(filename_ofx,tmp_filename,255);//The processed ofx file
        filenames[0]=filename_openspdtd;
        filenames[1]=filename_dtd;
        filenames[2]=filename_ofx;
        if(libofx_context->currentFileType()==OFX)
          {
            ofx_proc_sgml(libofx_context, 3,filenames);
          }
        else if(libofx_context->currentFileType()==OFC)
          {
            ofc_proc_sgml(libofx_context, 3,filenames);
          }
        else
          {
            message_out(ERROR,string("ofx_proc_file(): Error unknown file format for the OFX parser"));
          }
        if(remove(tmp_filename)!=0)
          {
            message_out(ERROR,"ofx_proc_file(): Error deleting temporary file "+string(tmp_filename));
          }
      }
    else
      {
        message_out(ERROR,"ofx_proc_file(): FATAL: Missing DTD, aborting");
      }
  }
  else{
    message_out(ERROR,"ofx_proc_file():No input file specified");
  }
  return 0;
}



CFCT int libofx_proc_buffer(LibofxContextPtr ctx,
                            const char *s, unsigned int size){
  ofstream tmp_file;
  string s_buffer;
  char *filenames[3];
  char tmp_filename[256];
  ssize_t pos;
  LibofxContext *libofx_context;

  libofx_context=(LibofxContext*)ctx;

  if (size==0) {
    message_out(ERROR,
                "ofx_proc_file(): bad size");
    return -1;
  }
  s_buffer=string(s, size);

  mkTempFileName("libofxtmpXXXXXX", tmp_filename, sizeof(tmp_filename));
  mkstemp(tmp_filename);
  tmp_file.open(tmp_filename);

  message_out(DEBUG,"ofx_proc_file(): Creating temp file: "+string(tmp_filename));
  if(!tmp_file){
    message_out(ERROR,"ofx_proc_file():Unable to open the output file "+string(tmp_filename));
    return -1;
  }

  if (libofx_context->currentFileType()==OFX) {
    pos=s_buffer.find("<OFX>");
    if (pos==string::npos)
      pos=s_buffer.find("<ofx>");
  }
  else if (libofx_context->currentFileType()==OFC) {
    pos=s_buffer.find("<OFC>");
    if (pos==string::npos)
      pos=s_buffer.find("<ofc>");
  }
  else {
    message_out(ERROR,"ofx_proc(): unknown file type");
    return -1;
  }
  if (pos==string::npos || pos > s_buffer.size()) {
    message_out(ERROR,"ofx_proc():<OFX> has not been found");
    return -1;
  }
  else {
    // erase everything before the OFX tag
    s_buffer.erase(0, pos);
    message_out(DEBUG,"ofx_proc_file():<OF?> has been found");
  }

  if (libofx_context->currentFileType()==OFX) {
    pos=s_buffer.find("</OFX>");
    if (pos==string::npos)
      pos=s_buffer.find("</ofx>");
  }
  else if (libofx_context->currentFileType()==OFC) {
    pos=s_buffer.find("</OFC>");
    if (pos==string::npos)
      pos=s_buffer.find("</ofc>");
  }
  else {
    message_out(ERROR,"ofx_proc(): unknown file type");
    return -1;
  }

  if (pos==string::npos || pos > s_buffer.size()) {
    message_out(ERROR,"ofx_proc():</OF?> has not been found");
    return -1;
  }
  else {
    // erase everything after the /OFX tag
    if (s_buffer.size() > pos+6)
      s_buffer.erase(pos+6);
    message_out(DEBUG,"ofx_proc_file():<OFX> has been found");
  }

  s_buffer=sanitize_proprietary_tags(s_buffer);
  tmp_file.write(s_buffer.c_str(), s_buffer.length());

  tmp_file.close();

  char filename_openspdtd[255];
  char filename_dtd[255];
  char filename_ofx[255];
  strncpy(filename_openspdtd,find_dtd(ctx, OPENSPDCL_FILENAME).c_str(),255);//The opensp sgml dtd file
  if(libofx_context->currentFileType()==OFX){
    strncpy(filename_dtd,find_dtd(ctx, OFX160DTD_FILENAME).c_str(),255);//The ofx dtd file
  }
  else if(libofx_context->currentFileType()==OFC){
    strncpy(filename_dtd,find_dtd(ctx, OFCDTD_FILENAME).c_str(),255);//The ofc dtd file
  }
  else {
    message_out(ERROR,string("ofx_proc_file(): Error unknown file format for the OFX parser"));
  }

  if((string)filename_dtd!="" && (string)filename_openspdtd!=""){
    strncpy(filename_ofx,tmp_filename,255);//The processed ofx file
    filenames[0]=filename_openspdtd;
    filenames[1]=filename_dtd;
    filenames[2]=filename_ofx;
    if(libofx_context->currentFileType()==OFX){
      ofx_proc_sgml(libofx_context, 3,filenames);
    }
    else if(libofx_context->currentFileType()==OFC){
      ofc_proc_sgml(libofx_context, 3,filenames);
    }
    else {
      message_out(ERROR,string("ofx_proc_file(): Error unknown file format for the OFX parser"));
    }
    if(remove(tmp_filename)!=0){
      message_out(ERROR,"ofx_proc_file(): Error deleting temporary file "+string(tmp_filename));
    }
  }
  else {
    message_out(ERROR,"ofx_proc_file(): FATAL: Missing DTD, aborting");
  }

  return 0;
}






/**
   This function will strip all the OFX proprietary tags and SGML comments from the SGML string passed to it
*/

string sanitize_proprietary_tags(string input_string)
{
  unsigned int i;
  size_t input_string_size;
  bool strip=false;
  bool tag_open=false;
  int tag_open_idx=0;//Are we within < > ?
  bool closing_tag_open=false;//Are we within </ > ?
  int orig_tag_open_idx=0;
  bool proprietary_tag=false; //Are we within a proprietary element?
  bool proprietary_closing_tag=false;
  int crop_end_idx=0;
  char buffer[READ_BUFFER_SIZE]="";
  char tagname[READ_BUFFER_SIZE]="";
  int tagname_idx=0;
  char close_tagname[READ_BUFFER_SIZE]="";
 
  for(i=0;i<READ_BUFFER_SIZE;i++){
    buffer[i]=0;
    tagname[i]=0;
    close_tagname[i]=0;
  }
  
  input_string_size=input_string.size();
  
  for(i=0;i<=input_string_size;i++){
    if(input_string.c_str()[i]=='<'){
      tag_open=true;
      tag_open_idx=i;
      if(proprietary_tag==true&&input_string.c_str()[i+1]=='/'){
	//We are now in a closing tag
        closing_tag_open=true;
	//cout<<"Comparaison: "<<tagname<<"|"<<&(input_string.c_str()[i+2])<<"|"<<strlen(tagname)<<endl;
        if(strncmp(tagname,&(input_string.c_str()[i+2]),strlen(tagname))!=0){
          //If it is the begining of an other tag
	  //cout<<"DIFFERENT!"<<endl;
          crop_end_idx=i-1;
	  strip=true;
        }
        else{
          //Otherwise, it is the start of the closing tag of the proprietary tag
          proprietary_closing_tag=true;
        }
      }
      else if(proprietary_tag==true){
	//It is the start of a new tag, following a proprietary tag
	crop_end_idx=i-1;
	strip=true;
      }
    }
    else if(input_string.c_str()[i]=='>'){
      tag_open=false;
      closing_tag_open=false;
      tagname[tagname_idx]=0;
      tagname_idx=0;
      if(proprietary_closing_tag==true){
        crop_end_idx=i;
	strip=true;
      }
    }
    else if(tag_open==true&&closing_tag_open==false){
      if(input_string.c_str()[i]=='.'){
        if(proprietary_tag!=true){
          orig_tag_open_idx = tag_open_idx;
          proprietary_tag=true;
        }
      }
      tagname[tagname_idx]=input_string.c_str()[i];
      tagname_idx++;
    }
    //cerr <<i<<endl;
    if(strip==true && orig_tag_open_idx < input_string.size())
      {
	input_string.copy(buffer,(crop_end_idx-orig_tag_open_idx)+1,orig_tag_open_idx);
	message_out(INFO,"sanitize_proprietary_tags() (end tag or new tag) removed: "+string(buffer));
	input_string.erase(orig_tag_open_idx,(crop_end_idx-orig_tag_open_idx)+1);
	i=orig_tag_open_idx-1;
	proprietary_tag=false;
	proprietary_closing_tag=false;
        closing_tag_open=false;
	tag_open=false;
	strip=false;
      }

  }//end for
  if(proprietary_tag==true && orig_tag_open_idx < input_string.size()){
    if(crop_end_idx==0){//no closing tag
      crop_end_idx=input_string.size()-1;
    }
    input_string.copy(buffer,(crop_end_idx-orig_tag_open_idx)+1,orig_tag_open_idx);
    message_out(INFO,"sanitize_proprietary_tags() (end of line) removed: "+string(buffer));
    input_string.erase(orig_tag_open_idx,(crop_end_idx-orig_tag_open_idx)+1);
  }
  return input_string;
}



/**
   This function will try to find a DTD matching the requested_version and return the full path of the DTD found (or an empty string if unsuccessfull)
   *
   Please note that currently the function will ALWAYS look for version 160, since OpenSP can't parse the 201 DTD correctly
*/
string find_dtd(LibofxContextPtr ctx, string dtd_filename)
{
  int i;
  ifstream dtd_file;
  string dtd_path_filename;
  bool dtd_found=false;

  dtd_path_filename=((LibofxContext*)ctx)->dtdDir();
  if (!dtd_path_filename.empty()) {
    dtd_path_filename.append(dtd_filename);
    dtd_file.clear();
    dtd_file.open(dtd_path_filename.c_str());
    if(dtd_file){
      message_out(STATUS,"find_dtd():DTD found: "+dtd_path_filename);
      dtd_file.close();
      dtd_found=true;
    }
  }

  if (!dtd_found) {
    for(i=0;i<DTD_SEARCH_PATH_NUM&&dtd_found==false;i++){
      dtd_path_filename=DTD_SEARCH_PATH[i];
      dtd_path_filename.append(dtd_filename);
      dtd_file.clear();
      dtd_file.open(dtd_path_filename.c_str());
      if(!dtd_file){
	message_out(DEBUG,"find_dtd():Unable to open the file "+dtd_path_filename);
      }
      else{
	message_out(STATUS,"find_dtd():DTD found: "+dtd_path_filename);
	dtd_file.close();
	dtd_found=true;
      }
    }
  }

  if(dtd_found==false){
    message_out(ERROR,"find_dtd():Unable to find the DTD named " + dtd_filename);
    dtd_path_filename="";
  }
  return dtd_path_filename;
}


