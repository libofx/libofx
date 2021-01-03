/***************************************************************************
         ofx_container_account.cpp
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : benoitg@coeus.ca
***************************************************************************/
/**@file
 * \brief Implementation of OfxAccountContainer for bank, credit card and
investment accounts.
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
#include "messages.hh"
#include "libofx.h"
#include "ofx_containers.hh"
#include "ofx_utilities.hh"

extern OfxMainContainer * MainContainer;

/***************************************************************************
 *                      OfxAccountContainer                                *
 ***************************************************************************/

OfxAccountContainer::OfxAccountContainer(LibofxContext *p_libofx_context, OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(p_libofx_context, para_parentcontainer, para_tag_identifier)
{
  memset(&data, 0, sizeof(data));
  type = "ACCOUNT";
  if (para_tag_identifier == "CCACCTFROM")
  {
    /*Set the type for a creditcard account.  Bank account specific
    	OFX elements will set this attribute elsewhere */
    ASSIGN(data.account_type, data.OFX_CREDITCARD);
  }
  if (para_tag_identifier == "INVACCTFROM")
  {
    /*Set the type for an investment account.  Bank account specific
    	OFX elements will set this attribute elsewhere */
    ASSIGN(data.account_type, data.OFX_INVESTMENT);
  }
  if (parentcontainer != NULL && ((OfxStatementContainer*)parentcontainer)->data.currency_valid == true)
  {
    ASSIGN_STRNCPY(data.currency, std::string(((OfxStatementContainer*)parentcontainer)->data.currency));
  }
}
OfxAccountContainer::~OfxAccountContainer()
{
  /*  if (parentcontainer->type == "STATEMENT")
      {
      ((OfxStatementContainer*)parentcontainer)->add_account(data);
      }
      ofx_proc_account_cb (data);*/
}

void OfxAccountContainer::add_attribute(const string identifier, const string value)
{
  if ( identifier == "BANKID")
  {
    m_bankid = value;
    ASSIGN_STRNCPY(data.bank_id, value);
  }
  else if ( identifier == "BRANCHID")
  {
    m_branchid = value;
    ASSIGN_STRNCPY(data.branch_id, value);
  }
  else if ( identifier == "ACCTID")
  {
    m_acctid = value;
    ASSIGN_STRNCPY(data.account_number, value);
  }
  else if ( identifier == "ACCTKEY")
  {
    m_acctkey = value;
  }
  else if ( identifier == "BROKERID")     /* For investment accounts */
  {
    m_brokerid = value;
    ASSIGN_STRNCPY(data.broker_id, value);
  }
  else if ((identifier == "ACCTTYPE") || (identifier == "ACCTTYPE2"))
  {
    data.account_type_valid = true;
    if (value == "CHECKING")
    {
      data.account_type = data.OFX_CHECKING;
    }
    else if (value == "SAVINGS")
    {
      data.account_type = data.OFX_SAVINGS;
    }
    else if (value == "MONEYMRKT")
    {
      data.account_type = data.OFX_MONEYMRKT;
    }
    else if (value == "CREDITLINE")
    {
      data.account_type = data.OFX_CREDITLINE;
    }
    else if (value == "CMA")
    {
      data.account_type = data.OFX_CMA;
    }
    /* AccountType CREDITCARD is set at object creation, if appropriate */
    else
    {
      data.account_type_valid = false;
    }
  }
  else
  {
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}//end OfxAccountContainer::add_attribute()

int OfxAccountContainer::gen_event()
{
  libofx_context->accountCallback(data);
  return true;
}

int  OfxAccountContainer::add_to_main_tree()
{
  gen_account_id ();

  if (MainContainer != NULL)
  {
    return MainContainer->add_container(this);
  }
  else
  {
    return false;
  }
}

void OfxAccountContainer::gen_account_id(void)
{
  if (data.account_type == OfxAccountData::OFX_CREDITCARD)
  {
    STRNCPY(data.account_id, string(data.account_id) + m_acctid + " " + m_acctkey);
    STRNCPY(data.account_name, string(data.account_name) + "Credit card " + m_acctid);
  }
  else if (data.account_type == OfxAccountData::OFX_INVESTMENT)
  {
    STRNCPY(data.account_id, string(data.account_id) + m_brokerid + " " + m_acctid);
    STRNCPY(data.account_name, string(data.account_name) + "Investment account " +
            m_acctid + " at broker " + m_brokerid);
  }
  else
  {
    STRNCPY(data.account_id, string(data.account_id) + m_bankid + " " + m_branchid + " " + m_acctid);
    STRNCPY(data.account_name, string(data.account_name) + "Bank account " + m_acctid);
  }
  data.account_id_valid = true;
}//end OfxAccountContainer::gen_account_id()
