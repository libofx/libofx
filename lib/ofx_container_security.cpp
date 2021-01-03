/***************************************************************************
         ofx_container_security.cpp
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : benoitg@coeus.ca
***************************************************************************/
/**@file
 * \brief Implementation of OfxSecurityContainer for stocks, bonds, mutual
 funds, etc.
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
 *                     OfxSecurityContainer                                *
 ***************************************************************************/

OfxSecurityContainer::OfxSecurityContainer(LibofxContext *p_libofx_context, OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(p_libofx_context, para_parentcontainer, para_tag_identifier)
{
  memset(&data, 0, sizeof(data));
  type = "SECURITY";

  if (para_tag_identifier == "STOCKINFO")
    data.security_type = data.OFX_STOCK_SECURITY;
  else if (para_tag_identifier == "MFINFO")
    data.security_type = data.OFX_FUND_SECURITY;
  else if (para_tag_identifier == "OPTINFO")
    data.security_type = data.OFX_OPTION_SECURITY;
  else if (para_tag_identifier == "DEBTINFO")
    data.security_type = data.OFX_DEBT_SECURITY;
  else
    ASSIGN(data.security_type, data.OFX_OTHER_SECURITY);
}
OfxSecurityContainer::~OfxSecurityContainer()
{
}

void OfxSecurityContainer::add_attribute(const string identifier, const string value)
{
  if (identifier == "UNIQUEID")
  {
    /* With <OPTINFO>, there can be two <UNIQUEID> elements,  */
    /* The one under <SECINFO> is mandatory and is for the option. */
    /* The one under <OPTINFO> is optional and is for the underlying. */
    /* Assume the first one in the file is the mandatory one and put it in unique_id */
    if (data.unique_id_valid == false)
    {
      ASSIGN_STRNCPY(data.unique_id, value);
    }
    else
    {
      /* If unique_id is already set, use unique_id2 */
      ASSIGN_STRNCPY(data.unique_id2, value);
    }
  }
  else if (identifier == "UNIQUEIDTYPE")
  {
    /* With <OPTINFO>, there can be two <UNIQUEIDTYPE> elements,  */
    /* The one under <SECINFO> is mandatory and is for the option. */
    /* The one under <OPTINFO> is optional and is for the underlying. */
    /* Assume the first one in the file is the mandatory one and put it in unique_id */
    if (data.unique_id_type_valid == false)
    {
      ASSIGN_STRNCPY(data.unique_id_type, value);
    }
    else
    {
      /* If unique_id_type is already set, use unique_id2_type */
      ASSIGN_STRNCPY(data.unique_id2_type, value);
    }
  }
  else if (identifier == "SECNAME")
  {
    ASSIGN_STRNCPY(data.secname, value);
  }
  else if (identifier == "TICKER")
  {
    ASSIGN_STRNCPY(data.ticker, value);
  }
  else if (identifier == "RATING")
  {
    ASSIGN_STRNCPY(data.rating, value);
  }
  else if (identifier == "UNITPRICE")
  {
    ASSIGN(data.unitprice, ofxamount_to_double(value));
  }
  else if (identifier == "DTASOF")
  {
    ASSIGN(data.date_unitprice, ofxdate_to_time_t(value));
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
  else if (identifier == "MEMO" || identifier == "MEMO2")
  {
    ASSIGN_STRNCPY(data.memo, value);
  }
  else if (identifier == "FIID")
  {
    ASSIGN_STRNCPY(data.fiid, value);
  }
  else if (identifier == "ASSETCLASS")
  {
    if (value == "DOMESTICBOND")
    {
      ASSIGN(data.asset_class, data.OFX_ASSET_CLASS_DOMESTICBOND);
    }
    else if (value == "INTLBOND")
    {
      ASSIGN(data.asset_class, data.OFX_ASSET_CLASS_INTLBOND);
    }
    else if (value == "LARGESTOCK")
    {
      ASSIGN(data.asset_class, data.OFX_ASSET_CLASS_LARGESTOCK);
    }
    else if (value == "SMALLSTOCK")
    {
      ASSIGN(data.asset_class, data.OFX_ASSET_CLASS_SMALLSTOCK);
    }
    else if (value == "INTLSTOCK")
    {
      ASSIGN(data.asset_class, data.OFX_ASSET_CLASS_INTLSTOCK);
    }
    else if (value == "MONEYMRKT")
    {
      ASSIGN(data.asset_class, data.OFX_ASSET_CLASS_MONEYMRKT);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.asset_class, data.OFX_ASSET_CLASS_OTHER);
    }
  }
  else if (identifier == "PARVALUE")
  {
    ASSIGN(data.par_value, ofxamount_to_double(value));
  }
  else if (identifier == "DEBTTYPE")
  {
    if (value == "COUPON")
    {
      ASSIGN(data.debt_type, data.OFX_DEBT_TYPE_COUPON);
    }
    else if (value == "ZERO")
    {
      ASSIGN(data.debt_type, data.OFX_DEBT_TYPE_ZERO);
    }
  }
  else if (identifier == "DEBTCLASS")
  {
    if (value == "TREASURY")
    {
      ASSIGN(data.debt_class, data.OFX_DEBTCLASS_TREASURY);
    }
    else if (value == "MUNICIPAL")
    {
      ASSIGN(data.debt_class, data.OFX_DEBTCLASS_MUNICIPAL);
    }
    else if (value == "CORPORATE")
    {
      ASSIGN(data.debt_class, data.OFX_DEBTCLASS_CORPORATE);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.debt_class, data.OFX_DEBTCLASS_OTHER);
    }
  }
  else if (identifier == "COUPONRT")
  {
    ASSIGN(data.coupon_rate, ofxamount_to_double(value));
  }
  else if (identifier == "DTCOUPON")
  {
    ASSIGN(data.date_coupon, ofxdate_to_time_t(value));
  }
  else if (identifier == "COUPONFREQ")
  {
    if (value == "MONTHLY")
    {
      ASSIGN(data.coupon_freq, data.OFX_COUPON_FREQ_MONTHLY);
    }
    else if (value == "QUARTERLY")
    {
      ASSIGN(data.coupon_freq, data.OFX_COUPON_FREQ_QUARTERLY);
    }
    else if (value == "SEMIANNUAL")
    {
      ASSIGN(data.coupon_freq, data.OFX_COUPON_FREQ_SEMIANNUAL);
    }
    else if (value == "ANNUAL")
    {
      ASSIGN(data.coupon_freq, data.OFX_COUPON_FREQ_ANNUAL);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.coupon_freq, data.OFX_COUPON_FREQ_OTHER);
    }
  }
  else if (identifier == "CALLPRICE")
  {
    ASSIGN(data.call_price, ofxamount_to_double(value));
  }
  else if (identifier == "YIELDTOCALL")
  {
    ASSIGN(data.yield_to_call, ofxamount_to_double(value));
  }
  else if (identifier == "DTCALL")
  {
    ASSIGN(data.call_date, ofxdate_to_time_t(value));
  }
  else if (identifier == "CALLTYPE")
  {
    if (value == "CALL")
    {
      ASSIGN(data.call_type, data.OFX_CALL_TYPE_CALL);
    }
    else if (value == "PUT")
    {
      ASSIGN(data.call_type, data.OFX_CALL_TYPE_PUT);
    }
    else if (value == "PREFUND")
    {
      ASSIGN(data.call_type, data.OFX_CALL_TYPE_PREFUND);
    }
    else if (value == "MATURITY")
    {
      ASSIGN(data.call_type, data.OFX_CALL_TYPE_MATURITY);
    }
  }
  else if (identifier == "YIELDTOMAT")
  {
    ASSIGN(data.yield_to_maturity, ofxamount_to_double(value));
  }
  else if (identifier == "DTMAT")
  {
    ASSIGN(data.maturity_date, ofxdate_to_time_t(value));
  }
  else if (identifier == "MFTYPE")
  {
    if (value == "OPENEND")
    {
      ASSIGN(data.mutual_fund_type, data.OFX_MFTYPE_OPENEND);
    }
    else if (value == "CLOSEEND")
    {
      ASSIGN(data.mutual_fund_type, data.OFX_MFTYPE_CLOSEEND);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.mutual_fund_type, data.OFX_MFTYPE_OTHER);
    }
  }
  else if (identifier == "STOCKTYPE")
  {
    if (value == "COMMON")
    {
      ASSIGN(data.stock_type, data.OFX_STOCKTYPE_COMMON);
    }
    else if (value == "PREFERRED")
    {
      ASSIGN(data.stock_type, data.OFX_STOCKTYPE_PREFERRED);
    }
    else if (value == "CONVERTIBLE")
    {
      ASSIGN(data.stock_type, data.OFX_STOCKTYPE_CONVERTIBLE);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.stock_type, data.OFX_STOCKTYPE_OTHER);
    }
  }
  else if (identifier == "YIELD")
  {
    ASSIGN(data.yield, ofxamount_to_double(value));
  }
  else if (identifier == "DTYIELDASOF")
  {
    ASSIGN(data.yield_asof_date, ofxdate_to_time_t(value));
  }
  else if (identifier == "OPTTYPE")
  {
    if (value == "CALL" || value == "Call")
    {
      ASSIGN(data.option_type, data.OFX_OPTION_TYPE_CALL);
    }
    else if (value == "PUT" || value == "Put")
    {
      ASSIGN(data.option_type, data.OFX_OPTION_TYPE_PUT);
    }
  }
  else if (identifier == "STRIKEPRICE")
  {
    ASSIGN(data.strike_price, ofxamount_to_double(value));
  }
  else if (identifier == "DTEXPIRE")
  {
    ASSIGN(data.date_expire, ofxdate_to_time_t(value));
  }
  else if (identifier == "SHPERCTRCT")
  {
    ASSIGN(data.shares_per_cont, ofxamount_to_double(value));
  }
  else
  {
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}
int  OfxSecurityContainer::gen_event()
{
  libofx_context->securityCallback(data);
  return true;
}

int  OfxSecurityContainer::add_to_main_tree()
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

