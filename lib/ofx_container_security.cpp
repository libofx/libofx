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
    data.security_type = data.OFX_OTHER_SECURITY;
  data.security_type_valid = true;
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
      STRNCPY(data.unique_id, value);
      data.unique_id_valid = true;
    }
    else
    {
      /* If unique_id is already set, use unique_id2 */
      STRNCPY(data.unique_id2, value);
      data.unique_id2_valid = true;
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
      STRNCPY(data.unique_id_type, value);
      data.unique_id_type_valid = true;
    }
    else
    {
      /* If unique_id_type is already set, use unique_id2_type */
      STRNCPY(data.unique_id2_type, value);
      data.unique_id2_type_valid = true;
    }
  }
  else if (identifier == "SECNAME")
  {
    STRNCPY(data.secname, value);
    data.secname_valid = true;
  }
  else if (identifier == "TICKER")
  {
    STRNCPY(data.ticker, value);
    data.ticker_valid = true;
  }
  else if (identifier == "RATING")
  {
    STRNCPY(data.rating, value);
    data.rating_valid = true;
  }
  else if (identifier == "UNITPRICE")
  {
    data.unitprice = ofxamount_to_double(value);
    data.unitprice_valid = true;
  }
  else if (identifier == "DTASOF")
  {
    data.date_unitprice = ofxdate_to_time_t(value);
    data.date_unitprice_valid = true;
  }
  else if (identifier == "CURRATE")
  {
    data.currency_ratio = ofxamount_to_double(value);
    data.currency_ratio_valid = true;
  }
  else if (identifier == "CURSYM")
  {
    STRNCPY(data.currency, value);
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
  else if (identifier == "MEMO" || identifier == "MEMO2")
  {
    STRNCPY(data.memo, value);
    data.memo_valid = true;
  }
  else if (identifier == "FIID")
  {
    STRNCPY(data.fiid, value);
    data.fiid_valid = true;
  }
  else if (identifier == "ASSETCLASS")
  {
    if (value == "DOMESTICBOND")
    {
      data.asset_class = data.OFX_ASSET_CLASS_DOMESTICBOND;
      data.asset_class_valid = true;
    }
    else if (value == "INTLBOND")
    {
      data.asset_class = data.OFX_ASSET_CLASS_INTLBOND;
      data.asset_class_valid = true;
    }
    else if (value == "LARGESTOCK")
    {
      data.asset_class = data.OFX_ASSET_CLASS_LARGESTOCK;
      data.asset_class_valid = true;
    }
    else if (value == "SMALLSTOCK")
    {
      data.asset_class = data.OFX_ASSET_CLASS_SMALLSTOCK;
      data.asset_class_valid = true;
    }
    else if (value == "INTLSTOCK")
    {
      data.asset_class = data.OFX_ASSET_CLASS_INTLSTOCK;
      data.asset_class_valid = true;
    }
    else if (value == "MONEYMRKT")
    {
      data.asset_class = data.OFX_ASSET_CLASS_MONEYMRKT;
      data.asset_class_valid = true;
    }
    else if (value == "OTHER")
    {
      data.asset_class = data.OFX_ASSET_CLASS_OTHER;
      data.asset_class_valid = true;
    }
  }
  else if (identifier == "PARVALUE")
  {
    data.par_value = ofxamount_to_double(value);
    data.par_value_valid = true;
  }
  else if (identifier == "DEBTTYPE")
  {
    if (value == "COUPON")
    {
      data.debt_type = data.OFX_DEBT_TYPE_COUPON;
      data.debt_type_valid = true;
    }
    else if (value == "ZERO")
    {
      data.debt_type = data.OFX_DEBT_TYPE_ZERO;
      data.debt_type_valid = true;
    }
  }
  else if (identifier == "DEBTCLASS")
  {
    if (value == "TREASURY")
    {
      data.debt_class = data.OFX_DEBTCLASS_TREASURY;
      data.debt_class_valid = true;
    }
    else if (value == "MUNICIPAL")
    {
      data.debt_class = data.OFX_DEBTCLASS_MUNICIPAL;
      data.debt_class_valid = true;
    }
    else if (value == "CORPORATE")
    {
      data.debt_class = data.OFX_DEBTCLASS_CORPORATE;
      data.debt_class_valid = true;
    }
    else if (value == "OTHER")
    {
      data.debt_class = data.OFX_DEBTCLASS_OTHER;
      data.debt_class_valid = true;
    }
  }
  else if (identifier == "COUPONRT")
  {
    data.coupon_rate = ofxamount_to_double(value);
    data.coupon_rate_valid = true;
  }
  else if (identifier == "DTCOUPON")
  {
    data.date_coupon = ofxdate_to_time_t(value);
    data.date_coupon_valid = true;
  }
  else if (identifier == "COUPONFREQ")
  {
    if (value == "MONTHLY")
    {
      data.coupon_freq = data.OFX_COUPON_FREQ_MONTHLY;
      data.coupon_freq_valid = true;
    }
    else if (value == "QUARTERLY")
    {
      data.coupon_freq = data.OFX_COUPON_FREQ_QUARTERLY;
      data.coupon_freq_valid = true;
    }
    else if (value == "SEMIANNUAL")
    {
      data.coupon_freq = data.OFX_COUPON_FREQ_SEMIANNUAL;
      data.coupon_freq_valid = true;
    }
    else if (value == "ANNUAL")
    {
      data.coupon_freq = data.OFX_COUPON_FREQ_ANNUAL;
      data.coupon_freq_valid = true;
    }
    else if (value == "OTHER")
    {
      data.coupon_freq = data.OFX_COUPON_FREQ_OTHER;
      data.coupon_freq_valid = true;
    }
  }
  else if (identifier == "CALLPRICE")
  {
    data.call_price = ofxamount_to_double(value);
    data.call_price_valid = true;
  }
  else if (identifier == "YIELDTOCALL")
  {
    data.yield_to_call = ofxamount_to_double(value);
    data.yield_to_call_valid = true;
  }
  else if (identifier == "DTCALL")
  {
    data.call_date = ofxdate_to_time_t(value);
    data.call_date_valid = true;
  }
  else if (identifier == "CALLTYPE")
  {
    if (value == "CALL")
    {
      data.call_type = data.OFX_CALL_TYPE_CALL;
      data.call_type_valid = true;
    }
    else if (value == "PUT")
    {
      data.call_type = data.OFX_CALL_TYPE_PUT;
      data.call_type_valid = true;
    }
    else if (value == "PREFUND")
    {
      data.call_type = data.OFX_CALL_TYPE_PREFUND;
      data.call_type_valid = true;
    }
    else if (value == "MATURITY")
    {
      data.call_type = data.OFX_CALL_TYPE_MATURITY;
      data.call_type_valid = true;
    }
  }
  else if (identifier == "YIELDTOMAT")
  {
    data.yield_to_maturity = ofxamount_to_double(value);
    data.yield_to_maturity_valid = true;
  }
  else if (identifier == "DTMAT")
  {
    data.maturity_date = ofxdate_to_time_t(value);
    data.maturity_date_valid = true;
  }
  else if (identifier == "MFTYPE")
  {
    if (value == "OPENEND")
    {
      data.mutual_fund_type = data.OFX_MFTYPE_OPENEND;
      data.mutual_fund_type_valid = true;
    }
    else if (value == "CLOSEEND")
    {
      data.mutual_fund_type = data.OFX_MFTYPE_CLOSEEND;
      data.mutual_fund_type_valid = true;
    }
    else if (value == "OTHER")
    {
      data.mutual_fund_type = data.OFX_MFTYPE_OTHER;
      data.mutual_fund_type_valid = true;
    }
  }
  else if (identifier == "STOCKTYPE")
  {
    if (value == "COMMON")
    {
      data.stock_type = data.OFX_STOCKTYPE_COMMON;
      data.stock_type_valid = true;
    }
    else if (value == "PREFERRED")
    {
      data.stock_type = data.OFX_STOCKTYPE_PREFERRED;
      data.stock_type_valid = true;
    }
    else if (value == "CONVERTIBLE")
    {
      data.stock_type = data.OFX_STOCKTYPE_CONVERTIBLE;
      data.stock_type_valid = true;
    }
    else if (value == "OTHER")
    {
      data.stock_type = data.OFX_STOCKTYPE_OTHER;
      data.stock_type_valid = true;
    }
  }
  else if (identifier == "YIELD")
  {
    data.yield = ofxamount_to_double(value);
    data.yield_valid = true;
  }
  else if (identifier == "DTYIELDASOF")
  {
    data.yield_asof_date = ofxdate_to_time_t(value);
    data.yield_asof_date_valid = true;
  }
  else if (identifier == "OPTTYPE")
  {
    if (value == "CALL" || value == "Call")
    {
      data.option_type = data.OFX_OPTION_TYPE_CALL;
      data.option_type_valid = true;
    }
    else if (value == "PUT" || value == "Put")
    {
      data.option_type = data.OFX_OPTION_TYPE_PUT;
      data.option_type_valid = true;
    }
  }
  else if (identifier == "STRIKEPRICE")
  {
    data.strike_price = ofxamount_to_double(value);
    data.strike_price_valid = true;
  }
  else if (identifier == "DTEXPIRE")
  {
    data.date_expire = ofxdate_to_time_t(value);
    data.date_expire_valid = true;
  }
  else if (identifier == "SHPERCTRCT")
  {
    data.shares_per_cont = ofxamount_to_double(value);
    data.shares_per_cont_valid = true;
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

