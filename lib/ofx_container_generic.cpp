/***************************************************************************
         ofx_container_generic.cpp 
                             -------------------
    copyright            : (C) 2002 by Benoit Grégoire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
 * \brief Implementation of OfxGenericContainer
*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include "ParserEventGeneratorKit.h"
#include "messages.hh"
#include "libofx.h"
#include "ofx_containers.hh"

extern OfxMainContainer * MainContainer;

OfxGenericContainer::OfxGenericContainer()
{
  parentcontainer=NULL;
  type="";
  tag_identifier="";
}
OfxGenericContainer::OfxGenericContainer(OfxGenericContainer *para_parentcontainer)
{
  parentcontainer = para_parentcontainer;
  if(parentcontainer!=NULL&&parentcontainer->type=="DUMMY"){
    message_out(DEBUG,"OfxGenericContainer(): The parent is a DummyContainer!");
  }
}
OfxGenericContainer::OfxGenericContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier)
{
  parentcontainer = para_parentcontainer;
  tag_identifier = para_tag_identifier;
  if(parentcontainer!=NULL&&parentcontainer->type=="DUMMY"){
    message_out(DEBUG,"OfxGenericContainer(): The parent for this "+tag_identifier+" is a DummyContainer!");
  }
}
void OfxGenericContainer::add_attribute(const string identifier, const string value)
{
  /*If an attribute has made it all the way up to the Generic Container's add_attribute, 
    we don't know what to do with it! */
    message_out(ERROR, "WRITEME: "+identifier+" ("+value+") is not supported by the "+type+" container");
}
OfxGenericContainer* OfxGenericContainer::getparent()
{
  return parentcontainer;
}

int  OfxGenericContainer::gen_event()
{
  /* No callback is ever generated for pure virtual containers */
  return false;
}

int  OfxGenericContainer::add_to_main_tree()
{
  if(MainContainer != NULL)
    {
      return MainContainer->add_container(this);
    }
  else
    {
      return false;
    }
}
  
