/***************************************************************************
         ofx_container_main.cpp 
                             -------------------
    copyright            : (C) 2002 by Benoit Grégoire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
 * \brief Implementation of OfxMainContainer
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
#include <iostream>
#include "ParserEventGeneratorKit.h"
#include "messages.hh"
#include "libofx.h"
#include "ofx_containers.hh"

OfxMainContainer::OfxMainContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
 
//statement_tree_top=statement_tree.insert(statement_tree_top, NULL);
//security_tree_top=security_tree.insert(security_tree_top, NULL);

}
OfxMainContainer::~OfxMainContainer()
{
  message_out(DEBUG,"Entering the main container's destructor");
  tree<OfxGenericContainer *>::iterator tmp = security_tree.begin();

  while(tmp!=security_tree.end())
    {
      message_out(DEBUG,"Deleting "+(*tmp)->type);
      delete (*tmp);
      ++tmp;
    }
  tmp = account_tree.begin();
  while(tmp!=account_tree.end())
    {
      message_out(DEBUG,"Deleting "+(*tmp)->type);
      delete (*tmp);
      ++tmp;
    }
}
int OfxMainContainer::add_container(OfxGenericContainer * container)
{
  message_out(DEBUG,"OfxMainContainer::add_container for element " + container->tag_identifier + "; destroying the generic container");
  /* Call gen_event anyway, it could be a status container or similar */
  container->gen_event();
  delete container;
}

int OfxMainContainer::add_container(OfxSecurityContainer * container)
{
  message_out(DEBUG,"OfxMainContainer::add_container, adding a security");
  security_tree.insert_after(security_tree.end(), container);
}

int OfxMainContainer::add_container(OfxAccountContainer * container)
{
  message_out(DEBUG,"OfxMainContainer::add_container, adding an account");
  security_tree.insert(account_tree.end(), container);
}

int OfxMainContainer::add_container(OfxStatementContainer * container)
{
  message_out(DEBUG,"OfxMainContainer::add_container, adding a statement");
  tree<OfxGenericContainer *>::sibling_iterator tmp =  account_tree.begin();
  tmp += (account_tree.number_of_siblings(account_tree.begin()))-1;
  
  if(tmp!=account_tree.end())
    {
      message_out(DEBUG,"1: tmp is valid, Accounts are present");
      tree<OfxGenericContainer *>::iterator child = account_tree.begin(tmp);
      if(child!=account_tree.end(tmp))
	{
	  message_out(DEBUG,"There are already children for this account");
	  security_tree.insert(child, container);
	  
	}
      else
	{
	  message_out(DEBUG,"There are no children for this account");
	  security_tree.append_child(tmp,container);
	}
      container->add_account(&( ((OfxAccountContainer *)(*tmp))->data));
      return true;
    }
  else
    {
      return false;
    }
}

int OfxMainContainer::add_container(OfxTransactionContainer * container)
{
  message_out(DEBUG,"OfxMainContainer::add_container, adding a transaction");
  tree<OfxGenericContainer *>::sibling_iterator tmp =  account_tree.begin();
  tmp += (account_tree.number_of_siblings(account_tree.begin()))-1;

  if(tmp!=account_tree.end())
    {
      message_out(DEBUG,"1: tmp is valid, Accounts are present");
      security_tree.append_child(tmp,container);
      container->add_account(&(((OfxAccountContainer *)(*tmp))->data));
      return true;
    }
  else
    {
      return false;
    }
}

int  OfxMainContainer::gen_event()
{
  message_out(DEBUG,"Begin walking the trees of the main container to generate events");
  tree<OfxGenericContainer *>::iterator tmp = security_tree.begin();
  //cerr<<security_tree.size()<<endl;
  int i = 0;
  while(tmp!=security_tree.end())
    {
      message_out(DEBUG,"Looping...");
      //cerr <<i<<endl;
      i++;
      (*tmp)->gen_event();
      ++tmp;
    }
  tmp = account_tree.begin();
  //cerr<<account_tree.size()<<endl;
  i = 0;
  while(tmp!=account_tree.end())
    {
      //cerr<< "i="<<i<<"; depth="<<account_tree.depth(tmp)<<endl;
      i++;
      (*tmp)->gen_event();
      ++tmp;
    }
  return true;
}

OfxSecurityData *  OfxMainContainer::find_security(string unique_id)
{
 message_out(DEBUG,"OfxMainContainer::find_security() Begin.");

  tree<OfxGenericContainer *>::sibling_iterator tmp = security_tree.begin();
  OfxSecurityData * retval = NULL;
  while(tmp!=security_tree.end()&&retval==NULL)
    {
      if(((OfxSecurityContainer*)(*tmp))->data.unique_id==unique_id)
	{ message_out(DEBUG,(string)"Security "+((OfxSecurityContainer*)(*tmp))->data.unique_id+" found.");
	  retval=&((OfxSecurityContainer*)(*tmp))->data;
	}
      ++tmp;
    }
  return retval;
}
