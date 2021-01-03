/***************************************************************************
         ofx_container_position.cpp
                         -------------------
    copyright            : (C) 2016 by Jeff Lundblad
    email                : jeffjl.kde@outlook.com
***************************************************************************/
/**@file
 * \brief Implementation of OfxPositionContainer for stocks, bonds, mutual
 funds.
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
 *                     OfxPositionContainer                                *
 ***************************************************************************/

OfxPositionContainer::OfxPositionContainer(LibofxContext *p_libofx_context, OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(p_libofx_context, para_parentcontainer, para_tag_identifier)
{
  memset(&data, 0, sizeof(data));
  type = "POSITION";
}
OfxPositionContainer::~OfxPositionContainer()
{
}
void OfxPositionContainer::add_attribute(const string identifier, const string value)
{
  if (identifier == "UNIQUEID")
  {
    ASSIGN_STRNCPY(data.unique_id, value);
  }
  else if (identifier == "UNIQUEIDTYPE")
  {
    ASSIGN_STRNCPY(data.unique_id_type, value);
  }
  else if (identifier == "HELDINACCT")
  {
    if (value == "CASH")
    {
      ASSIGN(data.heldinaccount_type, data.OFX_HELDINACCT_CASH);
    }
    else if (value == "MARGIN")
    {
      ASSIGN(data.heldinaccount_type, data.OFX_HELDINACCT_MARGIN);
    }
    else if (value == "SHORT")
    {
      ASSIGN(data.heldinaccount_type, data.OFX_HELDINACCT_SHORT);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.heldinaccount_type, data.OFX_HELDINACCT_OTHER);
    }
  }
  else if (identifier == "POSTYPE")
  {
    if (value == "SHORT")
    {
      ASSIGN(data.position_type, data.OFX_POSITION_SHORT);
    }
    else if (value == "LONG")
    {
      ASSIGN(data.position_type, data.OFX_POSITION_LONG);
    }
  }
  else if (identifier == "UNITS")
  {
    ASSIGN(data.units, ofxamount_to_double(value));
  }
  else if (identifier == "UNITPRICE")
  {
    ASSIGN(data.unit_price, ofxamount_to_double(value));
  }
  else if (identifier == "MKTVAL")
  {
    ASSIGN(data.market_value, ofxamount_to_double(value));
  }
  else if (identifier == "DTPRICEASOF")
  {
    ASSIGN(data.date_unit_price, ofxdate_to_time_t(value));
  }
  else if (identifier == "CURRATE")
  {
    ASSIGN(data.currency_ratio, ofxamount_to_double(value));
  }
  else if (identifier == "CURSYM")
  {
    ASSIGN_STRNCPY(data.currency, value);
  }
  else if (identifier == "CURRENCY")
  {
    ASSIGN(data.amounts_are_foreign_currency, false);
  }
  else if (identifier == "ORIGCURRENCY")
  {
    ASSIGN(data.amounts_are_foreign_currency, true);
  }
  else if (identifier == "MEMO")
  {
    ASSIGN_STRNCPY(data.memo, value);
  }
  else if (identifier == "INV401KSOURCE")
  {
    if (value == "PRETAX")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_POSN_SOURCE_PRETAX);
    }
    else if (value == "AFTERTAX")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_POSN_SOURCE_AFTERTAX);
    }
    else if (value == "MATCH")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_POSN_SOURCE_MATCH);
    }
    else if (value == "PROFITSHARING")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_POSN_SOURCE_PROFITSHARING);
    }
    else if (value == "ROLLOVER")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_POSN_SOURCE_ROLLOVER);
    }
    else if (value == "OTHERVEST")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_POSN_SOURCE_OTHERVEST);
    }
    else if (value == "OTHERNONVEST")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_POSN_SOURCE_OTHERNONVEST);
    }
  }
  else
  {
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}
int  OfxPositionContainer::gen_event()
{
  if (data.unique_id_valid == true && MainContainer != NULL)
  {
    data.security_data_ptr = MainContainer->find_security(data.unique_id);
    if (data.security_data_ptr != NULL)
    {
      data.security_data_valid = true;
    }
  }
  libofx_context->positionCallback(data);
  return true;
}

void OfxPositionContainer::add_account(OfxAccountData * account_data)
{
  if (account_data->account_id_valid == true)
  {
    data.account_ptr = account_data;
    ASSIGN_STRNCPY(data.account_id, std::string(account_data->account_id));
  }
}

int  OfxPositionContainer::add_to_main_tree()
{
  if (MainContainer != NULL)
  {
    return MainContainer->add_container(this);
  }
  else
  {
    return false;
  }
}

