/***************************************************************************
          ofx_preproc.cpp 
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
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "ParserEventGeneratorKit.h"
#include "libofx.h"
#include "messages.hh"
#include "ofx_sgml.hh"
#include "ofx_preproc.hh"

using namespace std;
const unsigned int READ_BUFFER_SIZE = 1024;

int ofx_proc_file(int argc, char *argv[])
{
  bool ofx_start=false;
  bool ofx_end=false;

  ifstream input_file;
  ofstream tmp_file;
  char buffer[READ_BUFFER_SIZE];
  char tmp;
  string s_buffer;
  char *filenames[3];
  char tmp_filename[50];
  char filename_dtd[255];
  char filename_ofx[255];

  if(argc >= 2){
    message_out(DEBUG, string("ofx_proc_file():Opening file: ")+argv[1]);
    
    input_file.open(argv[1]);
    strncpy(tmp_filename,"/tmp/libofxtmpXXXXXX",50);
    mkstemp(tmp_filename);
    tmp_file.open(tmp_filename);

    message_out(DEBUG,"ofx_proc_file(): Creating temp file: "+string(tmp_filename));
    if(!input_file){
      message_out(ERROR,"ofx_proc_file():Unable to open the input file "+string(argv[1]));
    }
    else if(!tmp_file){
      message_out(ERROR,"ofx_proc_file():Unable to open the output file "+string(tmp_filename));
    }
    else
      {
	do{
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
	  if(ofx_start==false&&(s_buffer.find("<OFX>")!=string::npos||s_buffer.find("<ofx>")!=string::npos)){
	    ofx_start=true;
	    message_out(DEBUG,"ofx_proc_file():<OFX> has been found");
	  }
	  
	  if(ofx_start==true&&ofx_end==false){
	    s_buffer=sanitize_proprietary_tags(s_buffer);
	    //cout<< s_buffer<<"\n";
	    tmp_file.write(s_buffer.c_str(), s_buffer.length());
	  }
	  
	  if(ofx_start==true&&(s_buffer.find("</OFX>")!=string::npos||s_buffer.find("</ofx>")!=string::npos)){
	    ofx_end=true;
	    message_out(DEBUG,"ofx_proc_file():</OFX> has been found");
	  }
	  
	}while(!input_file.eof()&&!input_file.bad());
      }
    input_file.close();
    tmp_file.close();
    strncpy(filename_dtd,find_dtd().c_str(),255);//The dtd file
    if(filename_dtd!=NULL)
      {
	strncpy(filename_ofx,tmp_filename,255);//The processed ofx file
	filenames[0]=filename_dtd;
	filenames[1]=filename_ofx;
	ofx_proc_sgml(2,filenames);
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
    if(strip==true)
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
  if(proprietary_tag==true){
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
string find_dtd(const int requested_version)
{
  int i;
  ifstream dtd_file;
  string dtd_filename;
  string dtd_path_filename;
  bool dtd_found=false;

  dtd_filename="ofx160.dtd";

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
  if(dtd_found==false){
    message_out(ERROR,"find_dtd():Unable to find the DTD for the requested version");
    dtd_path_filename="";
  }
  return dtd_path_filename;
}


