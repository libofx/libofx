/***************************************************************************
                          ofx_util.cpp
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : benoitg@coeus.ca
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
#include <assert.h>

#include "ParserEventGeneratorKit.h"
#include "SGMLApplication.h"
#include <ctime>
#include <cstdlib>
#include <string>
#include <locale.h>
#include "messages.hh"
#include "ofx_utilities.hh"

#ifdef __WIN32__
# define DIRSEP "\\"
/* MSWin calls it _mkgmtime instead of timegm */
# define timegm(tm) _mkgmtime(tm)
#else
# define DIRSEP "/"
#endif


using namespace std;
/**
   Convert an OpenSP CharString directly to a C++ stream, to enable the use of cout directly for debugging.
*/
/*ostream &operator<<(ostream &os, SGMLApplication::CharString s)
  {
  for (size_t i = 0; i < s.len; i++)
  {
  os << ((char *)(s.ptr))[i*sizeof(SGMLApplication::Char)];
  }
  return os;
  }*/

/*wostream &operator<<(wostream &os, SGMLApplication::CharString s)
  {
  for (size_t i = 0; i < s.len; i++)
  {//cout<<i;
  os << wchar_t(s.ptr[i*MULTIPLY4]);
  }
  return os;
  }            */

/*wchar_t* CharStringtowchar_t(SGMLApplication::CharString source, wchar_t *dest)
  {
  size_t i;
  for (i = 0; i < source.len; i++)
  {
  dest[i]+=wchar_t(source.ptr[i*sizeof(SGMLApplication::Char)*(sizeof(char)/sizeof(wchar_t))]);
  }
  return dest;
  }*/

string CharStringtostring(const SGMLApplication::CharString source, string &dest)
{
  size_t i;
  dest.assign("");//Empty the provided string
  //  cout<<"Length: "<<source.len<<"sizeof(Char)"<<sizeof(SGMLApplication::Char)<<endl;
  for (i = 0; i < source.len; i++)
  {
    dest += (char)(((source.ptr)[i]));
    //    cout<<i<<" "<<(char)(((source.ptr)[i]))<<endl;
  }
  return dest;
}

string AppendCharStringtostring(const SGMLApplication::CharString source, string &dest)
{
  size_t i;
  for (i = 0; i < source.len; i++)
  {
    dest += (char)(((source.ptr)[i]));
  }
  return dest;
}

/**
 * Converts a date from the YYYYMMDDHHMMSS.XXX[gmt offset:tz name] OFX format (see OFX 2.01 spec p.66) to a C time_t.
 * @param ofxdate date from the YYYYMMDDHHMMSS.XXX[gmt offset:tz name] OFX format
 * @return C time_t in the local time zone
 * @note
 * @li The library always returns the time in the system's local time
 * @note: DEVIATION FROM THE OFX SPECIFICATION:
 * @li: Specification permits timestamps with millisecond precision, but the normal C Library time functions support only second precision.
 * @li: Many banks don't even specify a time, either by providing only an 8-character string (YYYYMMDD) or by presenting 0 for all the time values (i.e. midnight). In those cases we take that to mean that the time isn't significant and set it to a time that is nearly always the same day regardless of timezone: 10:59 UTC. This works in all timezones except -12 and +13.
 */
 time_t ofxdate_to_time_t(const string& ofxdate)
{
  if (ofxdate.empty())
  {
    message_out(ERROR, "ofxdate_to_time_t():  Unable to convert time, string is 0 length!");
    return 0;
  }
  string ofxdate_whole =
    ofxdate.substr(0, ofxdate.find_first_not_of("0123456789"));

  if (ofxdate_whole.size() < 8)
  {
    message_out(ERROR, "ofxdate_to_time_t():  Unable to convert time, string " + ofxdate + " is not in proper YYYYMMDDHHMMSS.XXX[gmt offset:tz name] format!");
    return std::time(NULL);
  }

  struct tm time;
  memset(&time, 0, sizeof(tm));
  time.tm_year = atoi(ofxdate_whole.substr(0, 4).c_str()) - 1900;
  time.tm_mon = atoi(ofxdate_whole.substr(4, 2).c_str()) - 1;
  time.tm_mday = atoi(ofxdate_whole.substr(6, 2).c_str());

  if (ofxdate_whole.size() < 14)
  {
    message_out(WARNING, "ofxdate_to_time_t():  Successfully parsed date part, but unable to parse time part of string " + ofxdate_whole + ". It is not in proper YYYYMMDDHHMMSS.XXX[gmt offset:tz name] format!");
  }
  else
  {
    time.tm_hour = atoi(ofxdate_whole.substr(8, 2).c_str());
    time.tm_min = atoi(ofxdate_whole.substr(10, 2).c_str());
    time.tm_sec = atoi(ofxdate_whole.substr(12, 2).c_str());
  }

  if (time.tm_hour + time.tm_min + time.tm_sec == 0)
  {
    time.tm_hour = 10;
    time.tm_min = 59;
    time.tm_sec = 0;
    return timegm(&time);
  }

  string::size_type startidx = ofxdate.find("[");
  if (startidx != string::npos)
  {
    startidx++;
    string::size_type endidx = ofxdate.find(":", startidx) - 1;
    string offset_str = ofxdate.substr(startidx, (endidx - startidx) + 1);
    float ofx_gmt_offset = atof(offset_str.c_str());
    std::time_t temptime = std::time(nullptr);
    static const double secs_per_hour = 3600.0;
    time.tm_sec -= static_cast<int>(ofx_gmt_offset * secs_per_hour);
    return timegm(&time);
  }

  /* No timezone, assume GMT */
  return timegm(&time);
}

/**
 * Convert a C++ string containing an amount of money as specified by the OFX standard and convert it to a double float.
 *\note The ofx number format is the following:  "." or "," as decimal separator, NO thousands separator.
 */
double ofxamount_to_double(const string ofxamount)
{
  //Replace commas and decimal points for atof()
  string::size_type idx;
  string tmp = ofxamount;

  idx = tmp.find(',');
  if (idx == string::npos)
  {
    idx = tmp.find('.');
  }

  if (idx != string::npos)
  {
    tmp.replace(idx, 1, 1, ((localeconv())->decimal_point)[0]);
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
  if (temp_string.empty())
    return temp_string; // so that size()-1 is allowed below

  const char *whitespace = " \b\f\n\r\t\v";
  const char *abnormal_whitespace = "\b\f\n\r\t\v";//backspace,formfeed,newline,cariage return, horizontal and vertical tabs
  message_out(DEBUG4, "strip_whitespace() Before: |" + temp_string + "|");

  for (i = 0;
       i <= temp_string.size()
       && temp_string.find_first_of(whitespace, i) == i
       && temp_string.find_first_of(whitespace, i) != string::npos;
       i++);
  temp_string.erase(0, i); //Strip leading whitespace

  for (i = temp_string.size() - 1;
       (i > 0)
       && (temp_string.find_last_of(whitespace, i) == i)
       && (temp_string.find_last_of(whitespace, i) != string::npos);
       i--);
  temp_string.erase(i + 1, temp_string.size() - (i + 1)); //Strip trailing whitespace

  while ((index = temp_string.find_first_of(abnormal_whitespace)) != string::npos)
  {
    temp_string.erase(index, 1); //Strip leading whitespace
  };

  message_out(DEBUG4, "strip_whitespace() After:  |" + temp_string + "|");

  return temp_string;
}


std::string get_tmp_dir()
{
  // Tries to mimic the behaviour of
  // http://developer.gnome.org/doc/API/2.0/glib/glib-Miscellaneous-Utility-Functions.html#g-get-tmp-dir
  char *var;
  var = getenv("TMPDIR");
  if (var) return var;
  var = getenv("TMP");
  if (var) return var;
  var = getenv("TEMP");
  if (var) return var;
#ifdef __WIN32__
  return "C:\\";
#else
  return "/tmp";
#endif
}

int mkTempFileName(const char *tmpl, char *buffer, unsigned int size)
{

  std::string tmp_dir = get_tmp_dir();

  strncpy(buffer, tmp_dir.c_str(), size);
  assert((strlen(buffer) + strlen(tmpl) + 2) < size);
  strcat(buffer, DIRSEP);
  strcat(buffer, tmpl);
  return 0;
}



