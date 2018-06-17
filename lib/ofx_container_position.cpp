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
    strncpy(data.unique_id, value.c_str(), sizeof(data.unique_id));
    data.unique_id_valid = true;
  }
  else if (identifier == "UNIQUEIDTYPE")
  {
    strncpy(data.unique_id_type, value.c_str(), sizeof(data.unique_id_type));
    data.unique_id_type_valid = true;
  }
  else if (identifier == "HELDINACCT")
  {
    if (value == "CASH")
    {
      data.heldinaccount_type = data.OFX_HELDINACCT_CASH;
      data.heldinaccount_type_valid = true;
    }
    else if (value == "MARGIN")
    {
      data.heldinaccount_type = data.OFX_HELDINACCT_MARGIN;
      data.heldinaccount_type_valid = true;
    }
    else if (value == "SHORT")
    {
      data.heldinaccount_type = data.OFX_HELDINACCT_SHORT;
      data.heldinaccount_type_valid = true;
    }
    else if (value == "OTHER")
    {
      data.heldinaccount_type = data.OFX_HELDINACCT_OTHER;
      data.heldinaccount_type_valid = true;
    }
  }
  else if (identifier == "POSTYPE")
  {
    if (value == "SHORT")
    {
      data.position_type = data.OFX_POSITION_SHORT;
      data.position_type_valid = true;
    }
    else if (value == "LONG")
    {
      data.position_type = data.OFX_POSITION_LONG;
      data.position_type_valid = true;
    }
  }
  else if (identifier == "UNITS")
  {
    data.units = ofxamount_to_double(value);
    data.units_valid = true;
  }
  else if (identifier == "UNITPRICE")
  {
    data.unit_price = ofxamount_to_double(value);
    data.unit_price_valid = true;
  }
  else if (identifier == "MKTVAL")
  {
    data.market_value = ofxamount_to_double(value);
    data.market_value_valid = true;
  }
  else if (identifier == "DTPRICEASOF")
  {
    data.date_unit_price = ofxdate_to_time_t(value);
    data.date_unit_price_valid = true;
  }
  else if (identifier == "CURRATE")
  {
    data.currency_ratio = ofxamount_to_double(value);
    data.currency_ratio_valid = true;
  }
  else if (identifier == "CURSYM")
  {
    strncpy(data.currency, value.c_str(), sizeof(data.currency));
    data.currency_valid = true;
  }
  else if (identifier == "CURRENCY")
  {
    data.amounts_are_foreign_currency = false;
    data.amounts_are_foreign_currency_valid = true;
  }
  else if (identifier == "ORIGCURRENCY")
  {
    data.amounts_are_foreign_currency = true;
    data.amounts_are_foreign_currency_valid = true;
  }
  else if (identifier == "MEMO")
  {
    strncpy(data.memo, value.c_str(), sizeof(data.memo));
    data.memo_valid = true;
  }
  else if (identifier == "INV401KSOURCE")
  {
    if (value == "PRETAX")
    {
      data.inv_401k_source = data.OFX_401K_SOURCE_PRETAX;
      data.inv_401k_source_valid = true;
    }
    else if (value == "AFTERTAX")
    {
      data.inv_401k_source = data.OFX_401K_SOURCE_AFTERTAX;
      data.inv_401k_source_valid = true;
    }
    else if (value == "MATCH")
    {
      data.inv_401k_source = data.OFX_401K_SOURCE_MATCH;
      data.inv_401k_source_valid = true;
    }
    else if (value == "PROFITSHARING")
    {
      data.inv_401k_source = data.OFX_401K_SOURCE_PROFITSHARING;
      data.inv_401k_source_valid = true;
    }
    else if (value == "ROLLOVER")
    {
      data.inv_401k_source = data.OFX_401K_SOURCE_ROLLOVER;
      data.inv_401k_source_valid = true;
    }
    else if (value == "OTHERVEST")
    {
      data.inv_401k_source = data.OFX_401K_SOURCE_OTHERVEST;
      data.inv_401k_source_valid = true;
    }
    else if (value == "OTHERNONVEST")
    {
      data.inv_401k_source = data.OFX_401K_SOURCE_OTHERNONVEST;
      data.inv_401k_source_valid = true;
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
    strncpy(data.account_id, account_data->account_id, OFX_ACCOUNT_ID_LENGTH);
    data.account_id_valid = true;
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

