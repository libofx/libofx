/***************************************************************************
 $RCSfile: win32.hh,v $
 -------------------
 cvs         : $Id: win32.hh,v 1.2 2007-10-27 12:08:16 aquamaniac Exp $
 begin       : Sat Oct 27 2007
 copyright   : (C) 2007 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 * This file is part of the project "LibOfx".                              *
 * Please see toplevel file COPYING of that project for license details.   *
 ***************************************************************************/

#ifndef LIBOFX_WIN32_HH
#define LIBOFX_WIN32_HH



#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#ifdef OS_WIN32

int mkstemp(char *tmpl);


#endif


int mkUniqueFile(const char *tmpl, char *buffer, unsigned int size);


#endif

