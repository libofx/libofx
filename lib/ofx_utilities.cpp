/***************************************************************************
                          ofx_util.cpp
                             -------------------
    copyright            : (C) 2002 by Benoit Grégoire
    email                : bock@step.polymtl.ca
 ***************************************************************************/
/**@file
 * \brief Various simple functions for type conversion & al
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <config.h>
#include <iostream>
#include "ParserEventGeneratorKit.h"
#include "SGMLApplication.h"
#include <time.h>
#include <string>
#include "messages.hh"
#include "ofx_utilities.hh"

using namespace std;

/**
   Convert an OpenSP CharString directly to a C++ stream, to enable the use of cout directly for debugging.
*/ 
ostream &operator<<(ostream &os, SGMLApplication::CharString s)
{
  for (size_t i = 0; i < s.len; i++)
    {//cout<<i<<" "<<(unsigned char)(s.ptr[i])<<endl;
      os << ((char *)(s.ptr))[i*sizeof(SGMLApplication::Char)];
    }
  return os;
}

/*wostream &operator<<(wostream &os, SGMLApplication::CharString s)
  {
  for (size_t i = 0; i < s.len; i++)
  {//cout<<i;
  os << wchar_t(s.ptr[i*MULTIPLY4]);  
  }
  return os;
  }            */

wchar_t* CharStringtowchar_t(SGMLApplication::CharString source, wchar_t *dest)
{
  size_t i;
  for (i = 0; i < source.len; i++)
    {
      dest[i]+=wchar_t(source.ptr[i*sizeof(SGMLApplication::Char)*(sizeof(char)/sizeof(wchar_t))]);
    }
  return dest;
}

string CharStringtostring(const SGMLApplication::CharString source, string &dest)
{
  size_t i;
  dest.assign("");//Empty the provided string
  for (i = 0; i < source.len; i++){
    dest+=((char *)(source.ptr))[i*sizeof(SGMLApplication::Char)];  
  }
  return dest;
}

string AppendCharStringtostring(const SGMLApplication::CharString source, string &dest)
{
  size_t i;
  //cout<<"Length: "<<source.len<<" detected char size: "<<char_size<<endl;
  for (i = 0; i < source.len; i++)
    {
      dest+=((char *)(source.ptr))[i*sizeof(SGMLApplication::Char)]; 
    }
  return dest;
}

/** 
 *  Converts a date from the YYYYMMDDHHMMSS.XXX[gmt offset:tz name] OFX format (see OFX 2.01 spec p.66) to a C time_t.  Note that OFX can specify the time up to milliseconds, but ANSI C does not support this precision cleanly.  So you wont know the millisecond you were ruined...
 */
time_t ofxdate_to_time_t(const string ofxdate)
{
  struct tm time;
  int local_offset; /**< in seconds */
  float ofx_gmt_offset;/**< in fractionnal hours */
  char timezone[4];/**< Original timezone: the library does not expose this value*/
  time_t temptime;
  std::time(&temptime);
  local_offset = (localtime(&temptime))->tm_gmtoff;
  
  if(ofxdate.size()!=0){
    time.tm_year=atoi(ofxdate.substr(0,4).c_str())-1900;
    time.tm_mon=atoi(ofxdate.substr(4,2).c_str())-1;
    time.tm_mday=atoi(ofxdate.substr(6,2).c_str());
    /* if exact time is specified */
    if(ofxdate.size()>8) {
      time.tm_hour=atoi(ofxdate.substr(8,2).c_str());
      time.tm_min=atoi(ofxdate.substr(10,2).c_str());
      time.tm_sec=atoi(ofxdate.substr(12,2).c_str());
    }
    else{
      time.tm_hour=12;
      time.tm_min=0;
      time.tm_sec=0;
    }
    
   
    
    /* Check if the timezone has been specified */
    string::size_type startidx = ofxdate.find("[");
    string::size_type endidx;
    if(startidx!=string::npos){
      startidx++;
      endidx = ofxdate.find(":", startidx)-1;
      ofx_gmt_offset=atof(ofxdate.substr(startidx,(endidx-startidx)+1).c_str());
      startidx = endidx+2;
      strncpy(timezone,ofxdate.substr(startidx,3).c_str(),4);
    }
    else{
      ofx_gmt_offset=0;
      strcpy(timezone, "GMT");
    }
    /* Correct the time for the timezone */
    time.tm_sec = time.tm_sec + (local_offset - ((int)ofx_gmt_offset*60*60));//Convert from fractionnal hours to seconds
  }
  else{
    message_out(ERROR, "ofxdate_to_time_t():Unable to convert time, string is 0 length!");
  }
  return mktime(&time);
}

/**
 * Convert a C++ string containing an amount of money as specified by the OFX standard and convert it to a double float.
 */ 
double ofxamount_to_double(const string ofxamount)
{
  //Replace commas with decimal points for atof()
  string::size_type idx;
  string tmp = ofxamount;
  idx = tmp.find(',');
  if(idx!=string::npos){
    tmp.replace(idx,1,1,'.');
  }
  return atof(tmp.c_str());
}

/**
Many weird caracters can be present inside a SGML element, as a result on the transfer protocol, or for any reason.  This function greatly enhances the reliability of the library by zapping those gremlins (backspace,formfeed,newline,carriage return, horizontal and vertical tabs) as well as removing whitespace at the begining and end of the string.  Otherwise, many problems will occur during stringmatching.
*/
string strip_whitespace(const string para_string)
{
  size_t index;
  size_t i;
  string temp_string = para_string;
  char *whitespace = " \b\f\n\r\t\v";
  char *abnormal_whitespace = "\b\f\n\r\t\v";//backspace,formfeed,newline,cariage return, horizontal and vertical tabs
  message_out(DEBUG4,"strip_whitespace() Before: |"+temp_string+"|");
  for(i=0;i<=temp_string.size()&&temp_string.find_first_of(whitespace, i)==i&&temp_string.find_first_of(whitespace, i)!=string::npos;i++);
  temp_string.erase(0,i);//Strip leading whitespace
  for(i=temp_string.size()-1;(i>=0)&&(temp_string.find_last_of(whitespace, i)==i)&&(temp_string.find_last_of(whitespace, i)!=string::npos);i--);
  temp_string.erase(i+1,temp_string.size()-(i+1));//Strip trailing whitespace
  
while ((index = temp_string.find_first_of(abnormal_whitespace))!=string::npos)
  {
    temp_string.erase(index,1);//Strip leading whitespace
  };
 
 message_out(DEBUG4,"strip_whitespace() After:  |"+temp_string+"|");
 
 return temp_string;
}
