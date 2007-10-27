/***************************************************************************
 $RCSfile: win32.hh,v $
 -------------------
 cvs         : $Id: win32.hh,v 1.1 2007-10-27 11:29:14 aquamaniac Exp $
 begin       : Sat Oct 27 2007
 copyright   : (C) 2007 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 * This file is part of the project "LibOfx".                              *
 * Please see toplevel file COPYING of that project for license details.   *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#ifdef OS_WIN32

int mkstemp(char *tmpl);


#endif

