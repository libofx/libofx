/**@file libofx_context.hh
 @brief Main state object passed everywhere in the library
 @author (C) 2004 by Benoit Grégoire
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
#include "context.hh"

using namespace std;



/** @note:  Actual object returned is LibofxContext *
*/
CFCT LibofxContextPtr libofx_get_new_context()
{
  return new LibofxContext();
}

CFCT int libofx_free_context( LibofxContextPtr libofx_context_param)
{
  delete (LibofxContext *)libofx_context_param;
}
