/***************************************************************************
                          ofxdump.cpp
                             -------------------
    copyright            : (C) 2002 by Benoit Grégoire
    email                : benoitg@coeus.ca
***************************************************************************/
/**@file
 * \brief Code for ofxdump utility.  C++ example code
 *
 * ofxdump prints to stdout, in human readable form, everything the library
 understands about a particular ofx response file, and sends errors to
 stderr.  To know exactly what the library understands about of a particular
 ofx response file, just call ofxdump on that file.
 *
 * ofxdump is meant as both a C++ code example and a developer/debugging
 tool.  It uses every function and every structure of the LibOFX API.  By
 default, WARNING, INFO, ERROR and STATUS messages are enabled.  You can
 change these defaults at the top of ofxdump.cpp
 *
 * usage: ofxdump path_to_ofx_file/ofx_filename
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include "libofx.h"
#include <stdio.h>		/* for printf() */
#include <config.h>		/* Include config constants, e.g., VERSION TF */
#include <errno.h>

#include "cmdline.h" /* Gengetopt generated parser */

using namespace std;


int ofx_proc_security_cb(struct OfxSecurityData data, void * security_data)
{
  char dest_string[255];
  cout << "ofx_proc_security():\n";
  if (data.unique_id_valid == true)
  {
    cout << "    Unique ID of the security: " << data.unique_id << "\n";
  }
  if (data.unique_id_type_valid == true)
  {
    cout << "    Format of the Unique ID: " << data.unique_id_type << "\n";
  }
  if (data.unique_id2_valid == true)
  {
    cout << "    Unique ID of the underlying security: " << data.unique_id2 << "\n";
  }
  if (data.unique_id2_type_valid == true)
  {
    cout << "    Format of the underlying Unique ID: " << data.unique_id2_type << "\n";
  }
  if (data.security_type_valid == true)
  {
    if (data.security_type == OfxSecurityData::OFX_DEBT_SECURITY)
      strncpy(dest_string, "DEBTINFO: Debt security", sizeof(dest_string));
    else if (data.security_type == OfxSecurityData::OFX_FUND_SECURITY)
      strncpy(dest_string, "MFINFO: Mutual fund security", sizeof(dest_string));
    else if (data.security_type == OfxSecurityData::OFX_OPTION_SECURITY)
      strncpy(dest_string, "OPTINFO: Option security", sizeof(dest_string));
    else if (data.security_type == OfxSecurityData::OFX_STOCK_SECURITY)
      strncpy(dest_string, "STOCKINFO: Stock security", sizeof(dest_string));
    else if (data.security_type == OfxSecurityData::OFX_OTHER_SECURITY)
      strncpy(dest_string, "OTHERINFO: Other type of security", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Security type: " << dest_string << "\n";
  }
  if (data.secname_valid == true)
  {
    cout << "    Name of the security: " << data.secname << "\n";
  }
  if (data.ticker_valid == true)
  {
    cout << "    Ticker symbol: " << data.ticker << "\n";
  }
  if (data.rating_valid == true)
  {
    cout << "    Rating of the security: " << data.rating << "\n";
  }
  if (data.unitprice_valid == true)
  {
    if (data.security_type_valid == true
       && data.security_type == OfxSecurityData::OFX_DEBT_SECURITY)
      cout << "    Price (percent of par): " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(4) << data.unitprice << "%\n";
    else
      cout << "    Price of each unit of the security: " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.unitprice << "\n";
  }
  if (data.date_unitprice_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_unitprice)));
    cout << "    Date as of which the unitprice is valid: " << dest_string << "\n";
  }
  if (data.amounts_are_foreign_currency_valid == true)
  {
    cout << "    Amounts are in foreign currency: " << (data.amounts_are_foreign_currency ? "Yes" : "No") << "\n";
  }
  if (data.currency_valid == true)
  {
    cout << "    Currency: " << data.currency << "\n";
  }
  if (data.currency_ratio_valid == true)
  {
    cout << "    Ratio of default currency to currency: " << data.currency_ratio << "\n";
  }
  if (data.memo_valid == true)
  {
    cout << "    Extra security information (memo): " << data.memo << "\n";
  }
  if (data.asset_class_valid == true)
  {
    if (data.asset_class == OfxSecurityData::OFX_ASSET_CLASS_DOMESTICBOND)
      strncpy(dest_string, "DOMESTICBOND: Domestic bond", sizeof(dest_string));
    else if (data.asset_class == OfxSecurityData::OFX_ASSET_CLASS_INTLBOND)
      strncpy(dest_string, "INTLBOND: International bond", sizeof(dest_string));
    else if (data.asset_class == OfxSecurityData::OFX_ASSET_CLASS_LARGESTOCK)
      strncpy(dest_string, "LARGESTOCK: Large cap stock", sizeof(dest_string));
    else if (data.asset_class == OfxSecurityData::OFX_ASSET_CLASS_SMALLSTOCK)
      strncpy(dest_string, "SMALLSTOCK: Small cap stock", sizeof(dest_string));
    else if (data.asset_class == OfxSecurityData::OFX_ASSET_CLASS_INTLSTOCK)
      strncpy(dest_string, "INTLSTOCK: International stock", sizeof(dest_string));
    else if (data.asset_class == OfxSecurityData::OFX_ASSET_CLASS_MONEYMRKT)
      strncpy(dest_string, "MONEYMRKT: Money market", sizeof(dest_string));
    else if (data.asset_class == OfxSecurityData::OFX_ASSET_CLASS_OTHER)
      strncpy(dest_string, "OTHER: Other", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Asset class: " << dest_string << "\n";
  }
  if (data.fiasset_class_valid == true)
  {
    cout << "    FI defined asset class: " << data.fiasset_class << "\n";
  }
  if (data.par_value_valid == true)
  {
    cout << "    Par value: " << data.par_value << "\n";
  }
  if (data.debt_type_valid == true)
  {
    if (data.debt_type == OfxSecurityData::OFX_DEBT_TYPE_COUPON)
      strncpy(dest_string, "COUPON: Coupon debt", sizeof(dest_string));
    else if (data.debt_type == OfxSecurityData::OFX_DEBT_TYPE_ZERO)
      strncpy(dest_string, "ZERO: Zero coupon debt", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Debt type: " << dest_string << "\n";
  }
  if (data.debt_class_valid == true)
  {
    if (data.debt_class == OfxSecurityData::OFX_DEBTCLASS_TREASURY)
      strncpy(dest_string, "TREASURY: Treasury debt", sizeof(dest_string));
    else if (data.debt_class == OfxSecurityData::OFX_DEBTCLASS_MUNICIPAL)
      strncpy(dest_string, "MUNICIPAL: Municipal debt", sizeof(dest_string));
    else if (data.debt_class == OfxSecurityData::OFX_DEBTCLASS_CORPORATE)
      strncpy(dest_string, "CORPORATE: Corporate debt", sizeof(dest_string));
    else if (data.debt_class == OfxSecurityData::OFX_DEBTCLASS_OTHER)
      strncpy(dest_string, "OTHER: Other debt class", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Debt class: " << dest_string << "\n";
  }
  if (data.coupon_rate_valid == true)
  {
    cout << "    Coupon rate: " << data.coupon_rate << "%\n";
  }
  if (data.date_coupon_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_coupon)));
    cout << "    Date for the next coupon: " << dest_string << "\n";
  }
  if (data.coupon_freq_valid == true)
  {
    if (data.coupon_freq == OfxSecurityData::OFX_COUPON_FREQ_MONTHLY)
      strncpy(dest_string, "MONTHLY: Monthly coupon", sizeof(dest_string));
    else if (data.coupon_freq == OfxSecurityData::OFX_COUPON_FREQ_QUARTERLY)
      strncpy(dest_string, "QUARTERLY: Quarterly coupon", sizeof(dest_string));
    else if (data.coupon_freq == OfxSecurityData::OFX_COUPON_FREQ_SEMIANNUAL)
      strncpy(dest_string, "SEMIANNUAL: Semiannual coupon", sizeof(dest_string));
    else if (data.coupon_freq == OfxSecurityData::OFX_COUPON_FREQ_ANNUAL)
      strncpy(dest_string, "ANNUAL: Annual coupon", sizeof(dest_string));
    else if (data.coupon_freq == OfxSecurityData::OFX_COUPON_FREQ_OTHER)
      strncpy(dest_string, "OTHER: Other frequency", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Coupon frequency: " << dest_string << "\n";
  }
  if (data.call_price_valid == true)
  {
    cout << "    Call price (unit price): " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.call_price << "\n";
  }
  if (data.yield_to_call_valid == true)
  {
    cout << "    Yield to next call (rate): " << data.yield_to_call << "%\n";
  }
  if (data.call_date_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.call_date)));
    cout << "    Date for the next call: " << dest_string << "\n";
  }
  if (data.call_type_valid == true)
  {
    if (data.call_type == OfxSecurityData::OFX_CALL_TYPE_CALL)
      strncpy(dest_string, "CALL: Call", sizeof(dest_string));
    else if (data.call_type == OfxSecurityData::OFX_CALL_TYPE_PUT)
      strncpy(dest_string, "PUT: Put", sizeof(dest_string));
    else if (data.call_type == OfxSecurityData::OFX_CALL_TYPE_PREFUND)
      strncpy(dest_string, "PREFUND: Prefund", sizeof(dest_string));
    else if (data.call_type == OfxSecurityData::OFX_CALL_TYPE_MATURITY)
      strncpy(dest_string, "MATURITY: Maturity", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Call type: " << dest_string << "\n";
  }
  if (data.yield_to_maturity_valid == true)
  {
    cout << "    Yield to maturity (rate): " << data.yield_to_maturity << "%\n";
  }
  if (data.maturity_date_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.maturity_date)));
    cout << "    Maturity date: " << dest_string << "\n";
  }
  if (data.mutual_fund_type_valid == true)
  {
    if (data.mutual_fund_type == OfxSecurityData::OFX_MFTYPE_OPENEND)
      strncpy(dest_string, "OPENEND: Open ended", sizeof(dest_string));
    else if (data.mutual_fund_type == OfxSecurityData::OFX_MFTYPE_CLOSEEND)
      strncpy(dest_string, "CLOSEEND: Closed ended", sizeof(dest_string));
    else if (data.mutual_fund_type == OfxSecurityData::OFX_MFTYPE_OTHER)
      strncpy(dest_string, "OTHER: Other type", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Mutual fund type: " << dest_string << "\n";
  if (data.stock_type_valid == true)
  {
    if (data.stock_type == OfxSecurityData::OFX_STOCKTYPE_COMMON)
      strncpy(dest_string, "COMMON: Common stock", sizeof(dest_string));
    else if (data.stock_type == OfxSecurityData::OFX_STOCKTYPE_PREFERRED)
      strncpy(dest_string, "PREFERRED: Preferred stock", sizeof(dest_string));
    else if (data.stock_type == OfxSecurityData::OFX_STOCKTYPE_CONVERTIBLE)
      strncpy(dest_string, "CONVERTIBLE: Convertable stock", sizeof(dest_string));
    else if (data.stock_type == OfxSecurityData::OFX_STOCKTYPE_OTHER)
      strncpy(dest_string, "OTHER: Other type", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Stock type: " << dest_string << "\n";
  }
  }
  if (data.yield_valid == true)
  {
    cout << "    Current yield (rate): " << data.yield << "%\n";
  }
  if (data.yield_asof_date_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.yield_asof_date)));
    cout << "    Date for which current yield is valid: " << dest_string << "\n";
  }
  if (data.option_type_valid == true)
  {
    if (data.option_type == OfxSecurityData::OFX_OPTION_TYPE_CALL)
      strncpy(dest_string, "CALL: Call option", sizeof(dest_string));
    else if (data.option_type == OfxSecurityData::OFX_OPTION_TYPE_PUT)
      strncpy(dest_string, "PUT: Put option", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Option type: " << dest_string << "\n";
  }
  if (data.strike_price_valid == true)
  {
    cout << "    Strike price: " << data.strike_price << "\n";
  }
  if (data.date_expire_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_expire)));
    cout << "    Expiration date: " << dest_string << "\n";
  }
  if (data.shares_per_cont_valid == true)
  {
    cout << "    Shares per contract: " << data.shares_per_cont << "\n";
  }
  cout << "\n";
  return 0;
}

int ofx_proc_transaction_cb(struct OfxTransactionData data, void * transaction_data)
{
  char dest_string[255];
  cout << "ofx_proc_transaction():\n";

  if (data.account_id_valid == true)
  {
    cout << "    Account ID : " << data.account_id << "\n";
  }

  if (data.transactiontype_valid == true)
  {
    if (data.transactiontype == OFX_CREDIT)
      strncpy(dest_string, "CREDIT: Generic credit", sizeof(dest_string));
    else if (data.transactiontype == OFX_DEBIT)
      strncpy(dest_string, "DEBIT: Generic debit", sizeof(dest_string));
    else if (data.transactiontype == OFX_INT)
      strncpy(dest_string, "INT: Interest earned or paid (Note: Depends on signage of amount)", sizeof(dest_string));
    else if (data.transactiontype == OFX_DIV)
      strncpy(dest_string, "DIV: Dividend", sizeof(dest_string));
    else if (data.transactiontype == OFX_FEE)
      strncpy(dest_string, "FEE: FI fee", sizeof(dest_string));
    else if (data.transactiontype == OFX_SRVCHG)
      strncpy(dest_string, "SRVCHG: Service charge", sizeof(dest_string));
    else if (data.transactiontype == OFX_DEP)
      strncpy(dest_string, "DEP: Deposit", sizeof(dest_string));
    else if (data.transactiontype == OFX_ATM)
      strncpy(dest_string, "ATM: ATM debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
    else if (data.transactiontype == OFX_POS)
      strncpy(dest_string, "POS: Point of sale debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
    else if (data.transactiontype == OFX_XFER)
      strncpy(dest_string, "XFER: Transfer", sizeof(dest_string));
    else if (data.transactiontype == OFX_CHECK)
      strncpy(dest_string, "CHECK: Check", sizeof(dest_string));
    else if (data.transactiontype == OFX_PAYMENT)
      strncpy(dest_string, "PAYMENT: Electronic payment", sizeof(dest_string));
    else if (data.transactiontype == OFX_CASH)
      strncpy(dest_string, "CASH: Cash withdrawal", sizeof(dest_string));
    else if (data.transactiontype == OFX_DIRECTDEP)
      strncpy(dest_string, "DIRECTDEP: Direct deposit", sizeof(dest_string));
    else if (data.transactiontype == OFX_DIRECTDEBIT)
      strncpy(dest_string, "DIRECTDEBIT: Merchant initiated debit", sizeof(dest_string));
    else if (data.transactiontype == OFX_REPEATPMT)
      strncpy(dest_string, "REPEATPMT: Repeating payment/standing order", sizeof(dest_string));
    else if (data.transactiontype == OFX_OTHER)
      strncpy(dest_string, "OTHER: Other", sizeof(dest_string));
    else
      strncpy(dest_string, "Unknown transaction type", sizeof(dest_string));
    cout << "    Transaction type: " << dest_string << "\n";
  }


  if (data.date_initiated_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_initiated)));
    cout << "    Date initiated: " << dest_string << "\n";
  }
  if (data.date_posted_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_posted)));
    cout << "    Date posted: " << dest_string << "\n";
  }
  if (data.date_funds_available_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_funds_available)));
    cout << "    Date funds are available: " << dest_string << "\n";
  }
  if (data.amount_valid == true)
  {
    cout << "    Total money amount: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.amount << "\n";
  }
  if (data.units_valid == true)
  {
    if (data.invtransactiontype_valid)
    {
      cout << "    # of units: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(4) << data.units;
      strncpy(dest_string, " (bonds: face value; options: contracts; all others: shares)", sizeof(dest_string));
      if (data.security_data_valid == true)
      {
        if (data.security_data_ptr->security_type_valid == true)
        {
          if (data.security_data_ptr->security_type == OfxSecurityData::OFX_DEBT_SECURITY)
            strncpy(dest_string, " (face value)", sizeof(dest_string));
          else if (data.security_data_ptr->security_type == OfxSecurityData::OFX_FUND_SECURITY ||
                   data.security_data_ptr->security_type == OfxSecurityData::OFX_STOCK_SECURITY ||
                   data.security_data_ptr->security_type == OfxSecurityData::OFX_OTHER_SECURITY)
            strncpy(dest_string, " (shares)", sizeof(dest_string));
          else if (data.security_data_ptr->security_type == OfxSecurityData::OFX_OPTION_SECURITY)
            strncpy(dest_string, " (contracts)", sizeof(dest_string));
        }
      }
      cout << dest_string;
    }
    else
      cout << "    # of units: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.units;
    cout << "\n";
  }
  if (data.oldunits_valid == true)
  {
    if (data.invtransactiontype_valid)
      cout << "    # of units before split: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(4) << data.oldunits << "\n";
    else
      cout << "    # of units before split: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.oldunits << "\n";
  }
  if (data.newunits_valid == true)
  {
    cout << "    # of units after split: " << setprecision(4) << data.newunits << "\n";
  }
  if (data.unitprice_valid == true)
  {
    if (data.invtransactiontype_valid)
    {
      cout << "    Unit price: " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(4) << data.unitprice;
      strncpy(dest_string, " (bonds: % of par; options: premium per share of underlying; all others: price per share)\n", sizeof(dest_string));
      if (data.security_data_valid == true)
      {
        if (data.security_data_ptr->security_type_valid == true)
        {
          if (data.security_data_ptr->security_type == OfxSecurityData::OFX_DEBT_SECURITY)
          {
            strncpy(dest_string, "% (% of par)\n", sizeof(dest_string));
          }
          else if (data.security_data_ptr->security_type == OfxSecurityData::OFX_FUND_SECURITY ||
                   data.security_data_ptr->security_type == OfxSecurityData::OFX_STOCK_SECURITY ||
                   data.security_data_ptr->security_type == OfxSecurityData::OFX_OTHER_SECURITY)
            strncpy(dest_string, " (price per share)\n", sizeof(dest_string));
          else if (data.security_data_ptr->security_type == OfxSecurityData::OFX_OPTION_SECURITY)
            strncpy(dest_string, " (premium per share of underlying)\n", sizeof(dest_string));
        }
      }
      cout << dest_string;
    }
    else
      cout << "    Unit price: " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.unitprice << "\n";
 }
  if (data.fees_valid == true)
  {
    cout << "    Fees: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.fees << "\n";
  }
  if (data.commission_valid == true)
  {
    cout << "    Commission: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.commission << "\n";
  }
  if (data.amounts_are_foreign_currency_valid == true)
  {
    cout << "    Amounts are in foreign currency: " << (data.amounts_are_foreign_currency ? "Yes" : "No") << "\n";
  }
  if (data.currency_valid == true)
  {
    cout << "    Currency: " << data.currency << "\n";
  }
  if (data.currency_ratio_valid == true)
  {
    cout << "    Ratio of default currency to currency: " << data.currency_ratio << "\n";
  }
  if (data.fi_id_valid == true)
  {
    cout << "    Financial institution's ID for this transaction: " << data.fi_id << "\n";
  }
  if (data.fi_id_corrected_valid == true)
  {
    cout << "    Financial institution ID replaced or corrected by this transaction: " << data.fi_id_corrected << "\n";
  }
  if (data.fi_id_correction_action_valid == true)
  {
    cout << "    Action to take on the corrected transaction: ";
    if (data.fi_id_correction_action == DELETE)
      cout << "DELETE\n";
    else if (data.fi_id_correction_action == REPLACE)
      cout << "REPLACE\n";
    else
      cout << "ofx_proc_transaction(): This should not happen!\n";
  }
  if (data.invtransactiontype_valid == true)
  {
    cout << "    Investment transaction type: ";
    if (data.invtransactiontype == OFX_BUYDEBT)
      strncpy(dest_string, "BUYDEBT (Buy debt security)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_BUYMF)
      strncpy(dest_string, "BUYMF (Buy mutual fund)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_BUYOPT)
      strncpy(dest_string, "BUYOPT (Buy option)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_BUYOTHER)
      strncpy(dest_string, "BUYOTHER (Buy other security type)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_BUYSTOCK)
      strncpy(dest_string, "BUYSTOCK (Buy stock)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_CLOSUREOPT)
      strncpy(dest_string, "CLOSUREOPT (Close a position for an option)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_INCOME)
      strncpy(dest_string, "INCOME (Investment income is realized as cash into the investment account)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_INVEXPENSE)
      strncpy(dest_string, "INVEXPENSE (Misc investment expense that is associated with a specific security)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_JRNLFUND)
      strncpy(dest_string, "JRNLFUND (Journaling cash holdings between subaccounts within the same investment account)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_MARGININTEREST)
      strncpy(dest_string, "MARGININTEREST (Margin interest expense)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_REINVEST)
      strncpy(dest_string, "REINVEST (Reinvestment of income)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_RETOFCAP)
      strncpy(dest_string, "RETOFCAP (Return of capital)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLDEBT)
      strncpy(dest_string, "SELLDEBT (Sell debt security.  Used when debt is sold, called, or reached maturity)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLMF)
      strncpy(dest_string, "SELLMF (Sell mutual fund)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLOPT)
      strncpy(dest_string, "SELLOPT (Sell option)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLOTHER)
      strncpy(dest_string, "SELLOTHER (Sell other type of security)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLSTOCK)
      strncpy(dest_string, "SELLSTOCK (Sell stock)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SPLIT)
      strncpy(dest_string, "SPLIT (Stock or mutial fund split)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_TRANSFER)
      strncpy(dest_string, "TRANSFER (Transfer holdings in and out of the investment account)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_INVBANKTRAN)
      strncpy(dest_string, "INVBANKTRAN (Transfer cash in and out of the investment account)", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR, this investment transaction type is unknown.  This is a bug in ofxdump", sizeof(dest_string));

    cout << dest_string << "\n";
  }
  if (data.unique_id_valid == true)
  {
    cout << "    Unique ID of the security being traded: " << data.unique_id << "\n";
  }
  if (data.unique_id_type_valid == true)
  {
    cout << "    Format of the Unique ID: " << data.unique_id_type << "\n";
  }
  if (data.server_transaction_id_valid == true)
  {
    cout << "    Server's transaction ID (confirmation number): " << data.server_transaction_id << "\n";
  }
  if (data.check_number_valid == true)
  {
    cout << "    Check number: " << data.check_number << "\n";
  }
  if (data.reference_number_valid == true)
  {
    cout << "    Reference number: " << data.reference_number << "\n";
  }
  if (data.standard_industrial_code_valid == true)
  {
    cout << "    Standard Industrial Code: " << data.standard_industrial_code << "\n";
  }
  if (data.payee_id_valid == true)
  {
    cout << "    Payee_id: " << data.payee_id << "\n";
  }
  if (data.name_valid == true)
  {
    cout << "    Name of payee or transaction description: " << data.name << "\n";
  }
  if (data.memo_valid == true)
  {
    cout << "    Extra transaction information (memo): " << data.memo << "\n";
  }
  if (data.accrued_interest_valid == true)
  {
    cout << "    Accrued Interest: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.accrued_interest << "\n";
  }
  if (data.avg_cost_basis_valid == true)
  {
    cout << "    Average cost basis: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.avg_cost_basis << "\n";
  }
  if (data.buy_type_valid == true)
  {
    if (data.buy_type == OfxTransactionData::OFX_BUY_TYPE_BUY)
      strncpy(dest_string, "BUY: Buy", sizeof(dest_string));
    else if (data.buy_type == OfxTransactionData::OFX_BUY_TYPE_BUYTOCOVER)
      strncpy(dest_string, "BUYTOCOVER: Buy to cover", sizeof(dest_string));
    else if (data.buy_type == OfxTransactionData::OFX_BUY_TYPE_BUYTOOPEN)
      strncpy(dest_string, "BUYTOOPEN: Buy to open", sizeof(dest_string));
    else if (data.buy_type == OfxTransactionData::OFX_BUY_TYPE_BUYTOCLOSE)
      strncpy(dest_string, "BUYTOCLOSE: Buy to close", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Buy type: " << dest_string << "\n";
  }
  if (data.denominator_valid == true)
  {
    cout << "    Stock split ratio denominator: " << data.denominator << "\n";
  }
  if (data.date_payroll_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_payroll)));
    cout << "    Date 401(k) funds were deducted from payroll: " << dest_string << "\n";
  }
  if (data.date_purchase_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_purchase)));
    cout << "    Original purchase date of the security: " << dest_string << "\n";
  }
  if (data.gain_valid == true)
  {
    cout << "    Average cost basis: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.gain << "\n";
  }
  if (data.cash_for_fractional_valid == true)
  {
    cout << "    Average cost basis: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.cash_for_fractional << "\n";
  }
  if (data.income_type_valid == true)
  {
    if (data.income_type == OfxTransactionData::OFX_CGLONG)
      strncpy(dest_string, "CGLONG: Long term capital gains", sizeof(dest_string));
    else if (data.income_type == OfxTransactionData::OFX_CGSHORT)
      strncpy(dest_string, "CGSHORT: Short term capital gains", sizeof(dest_string));
    else if (data.income_type == OfxTransactionData::OFX_DIVIDEND)
      strncpy(dest_string, "DIVIDEND", sizeof(dest_string));
    else if (data.income_type == OfxTransactionData::OFX_INTEREST)
      strncpy(dest_string, "INTEREST", sizeof(dest_string));
    else if (data.income_type == OfxTransactionData::OFX_MISC)
      strncpy(dest_string, "MISC: Miscellaneous", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Income type: " << dest_string << "\n";
  }
  if (data.inv_401k_source_valid == true)
  {
    if (data.inv_401k_source == OfxTransactionData::OFX_401K_SOURCE_PRETAX)
      strncpy(dest_string, "PRETAX", sizeof(dest_string));
    else if (data.inv_401k_source == OfxTransactionData::OFX_401K_SOURCE_AFTERTAX)
      strncpy(dest_string, "AFTERTAX", sizeof(dest_string));
    else if (data.inv_401k_source == OfxTransactionData::OFX_401K_SOURCE_MATCH)
      strncpy(dest_string, "MATCH", sizeof(dest_string));
    else if (data.inv_401k_source == OfxTransactionData::OFX_401K_SOURCE_PROFITSHARING)
      strncpy(dest_string, "PROFITSHARING", sizeof(dest_string));
    else if (data.inv_401k_source == OfxTransactionData::OFX_401K_SOURCE_ROLLOVER)
      strncpy(dest_string, "ROLLOVER", sizeof(dest_string));
    else if (data.inv_401k_source == OfxTransactionData::OFX_401K_SOURCE_OTHERVEST)
      strncpy(dest_string, "OTHERVEST", sizeof(dest_string));
    else if (data.inv_401k_source == OfxTransactionData::OFX_401K_SOURCE_OTHERNONVEST)
      strncpy(dest_string, "OTHERNONVEST", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Source of 401(k) money: " << dest_string << "\n";
  }
  if (data.load_valid == true)
  {
    cout << "    Load (amount): " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.load << "\n";
  }
  if (data.loan_id_valid == true)
  {
    cout << "    401(k) loan id: " << data.loan_id << "\n";
  }
  if (data.loan_interest_valid == true)
  {
    cout << "    401(k) loan interest (amount): " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.loan_interest << "\n";
  }
  if (data.loan_principal_valid == true)
  {
    cout << "    401(k) loan principle (amount): " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.loan_principal << "\n";
  }
  if (data.markdown_valid == true)
  {
    cout << "    Markdown (unitprice): " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.markdown << "\n";
  }
  if (data.markup_valid == true)
  {
    cout << "    Markup (unitprice): " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.markup << "\n";
  }
  if (data.numerator_valid == true)
  {
    cout << "    Stock split ratio numerator: " << data.numerator << "\n";
  }
  if (data.opt_action_valid == true)
  {
    if (data.opt_action == OfxTransactionData::OFX_OPTACTION_EXERCISE)
      strncpy(dest_string, "EXERCISE", sizeof(dest_string));
    else if (data.opt_action == OfxTransactionData::OFX_OPTACTION_ASSIGN)
      strncpy(dest_string, "ASSIGN", sizeof(dest_string));
    else if (data.opt_action == OfxTransactionData::OFX_OPTACTION_EXPIRE)
      strncpy(dest_string, "EXPIRE", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Option action: " << dest_string << "\n";
  }
  if (data.penalty_valid == true)
  {
    cout << "    Penalty withheld (amount): " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.penalty << "\n";
  }
  if (data.pos_type_valid == true)
  {
    if (data.pos_type == OfxTransactionData::OFX_POSTYPE_LONG)
      strncpy(dest_string, "LONG", sizeof(dest_string));
    else if (data.pos_type == OfxTransactionData::OFX_POSTYPE_SHORT)
      strncpy(dest_string, "SHORT", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Position Type: " << dest_string << "\n";
  }
  if (data.prior_year_contrib_valid == true)
  {
    cout << "    Prior year 401(k) contribution: " << (data.prior_year_contrib ? "Yes" : "No") << "\n";
  }
  if (data.related_fi_tid_valid == true)
  {
    cout << "    Related transaction TID: " << data.related_fi_tid << "\n";
  }
  if (data.related_type_valid == true)
  {
    if (data.related_type == OfxTransactionData::OFX_RELTYPE_SPREAD)
      strncpy(dest_string, "SPREAD", sizeof(dest_string));
    else if (data.related_type == OfxTransactionData::OFX_RELTYPE_STRADDLE)
      strncpy(dest_string, "STRADDLE", sizeof(dest_string));
    else if (data.related_type == OfxTransactionData::OFX_RELTYPE_NONE)
      strncpy(dest_string, "NONE", sizeof(dest_string));
    else if (data.related_type == OfxTransactionData::OFX_RELTYPE_OTHER)
      strncpy(dest_string, "OTHER", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Related Option Type: " << dest_string << "\n";
  }
  if (data.option_secured_valid == true)
  {
    if (data.option_secured == OfxTransactionData::OFX_SECURED_NAKED)
      strncpy(dest_string, "NAKED", sizeof(dest_string));
    else if (data.option_secured == OfxTransactionData::OFX_SECURED_COVERED)
      strncpy(dest_string, "COVERED: Cash covered", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    How is the option secured: " << dest_string << "\n";
  }
  if (data.sell_reason_valid == true)
  {
    if (data.sell_reason == OfxTransactionData::OFX_SELLREASON_CALL)
      strncpy(dest_string, "CALL: the debt was called", sizeof(dest_string));
    else if (data.sell_reason == OfxTransactionData::OFX_SELLREASON_SELL)
      strncpy(dest_string, "SELL: the debt was sold", sizeof(dest_string));
    else if (data.sell_reason == OfxTransactionData::OFX_SELLREASON_MATURITY)
      strncpy(dest_string, "MATURITY: the debt reached maturity", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Reason for the debt sell: " << dest_string << "\n";
  }
  if (data.sell_type_valid == true)
  {
    if (data.sell_type == OfxTransactionData::OFX_SELL_TYPE_SELL)
      strncpy(dest_string, "SELL: Sell", sizeof(dest_string));
    else if (data.sell_type == OfxTransactionData::OFX_SELL_TYPE_SELLSHORT)
      strncpy(dest_string, "SELLSHORT: Sell short", sizeof(dest_string));
    else if (data.sell_type == OfxTransactionData::OFX_SELL_TYPE_SELLTOOPEN)
      strncpy(dest_string, "SELLTOOPEN: Sell to open", sizeof(dest_string));
    else if (data.sell_type == OfxTransactionData::OFX_SELL_TYPE_SELLTOCLOSE)
      strncpy(dest_string, "SELLTOCLOSE: Sell to close", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Sell type: " << dest_string << "\n";
  }
  if (data.shares_per_cont_valid == true)
  {
    cout << "    Shares per option contract: " << data.shares_per_cont << "\n";
  }
  if (data.state_withholding_valid == true)
  {
    cout << "    State taxes withheld: " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.state_withholding << "\n";
  }
  if (data.subacct_from_valid == true)
  {
    if (data.subacct_from == OfxTransactionData::OFX_SUBACCT_CASH)
      strncpy(dest_string, "CASH", sizeof(dest_string));
    else if (data.subacct_from == OfxTransactionData::OFX_SUBACCT_MARGIN)
      strncpy(dest_string, "MARGIN", sizeof(dest_string));
    else if (data.subacct_from == OfxTransactionData::OFX_SUBACCT_SHORT)
      strncpy(dest_string, "SHORT", sizeof(dest_string));
    else if (data.subacct_from == OfxTransactionData::OFX_SUBACCT_OTHER)
      strncpy(dest_string, "OTHER", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    From sub account type: " << dest_string << "\n";
  }
  if (data.subacct_funding_valid == true)
  {
    if (data.subacct_funding == OfxTransactionData::OFX_SUBACCT_CASH)
      strncpy(dest_string, "CASH", sizeof(dest_string));
    else if (data.subacct_funding == OfxTransactionData::OFX_SUBACCT_MARGIN)
      strncpy(dest_string, "MARGIN", sizeof(dest_string));
    else if (data.subacct_funding == OfxTransactionData::OFX_SUBACCT_SHORT)
      strncpy(dest_string, "SHORT", sizeof(dest_string));
    else if (data.subacct_funding == OfxTransactionData::OFX_SUBACCT_OTHER)
      strncpy(dest_string, "OTHER", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Funding sub account type: " << dest_string << "\n";
  }
  if (data.subacct_security_valid == true)
  {
    if (data.subacct_security == OfxTransactionData::OFX_SUBACCT_CASH)
      strncpy(dest_string, "CASH", sizeof(dest_string));
    else if (data.subacct_security == OfxTransactionData::OFX_SUBACCT_MARGIN)
      strncpy(dest_string, "MARGIN", sizeof(dest_string));
    else if (data.subacct_security == OfxTransactionData::OFX_SUBACCT_SHORT)
      strncpy(dest_string, "SHORT", sizeof(dest_string));
    else if (data.subacct_security == OfxTransactionData::OFX_SUBACCT_OTHER)
      strncpy(dest_string, "OTHER", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Security sub account type: " << dest_string << "\n";
  }
  if (data.subacct_to_valid == true)
  {
    if (data.subacct_to == OfxTransactionData::OFX_SUBACCT_CASH)
      strncpy(dest_string, "CASH", sizeof(dest_string));
    else if (data.subacct_to == OfxTransactionData::OFX_SUBACCT_MARGIN)
      strncpy(dest_string, "MARGIN", sizeof(dest_string));
    else if (data.subacct_to == OfxTransactionData::OFX_SUBACCT_SHORT)
      strncpy(dest_string, "SHORT", sizeof(dest_string));
    else if (data.subacct_to == OfxTransactionData::OFX_SUBACCT_OTHER)
      strncpy(dest_string, "OTHER", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    To sub account type: " << dest_string << "\n";
  }
  if (data.taxes_valid == true)
  {
    cout << "    Tax on the trade (amount): " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.taxes << "\n";
  }
  if (data.tax_exempt_valid == true)
  {
    cout << "    Tax exempt: " << (data.tax_exempt ? "Yes" : "No") << "\n";
  }
  if (data.transfer_action_valid == true)
  {
    if (data.transfer_action == OfxTransactionData::OFX_TFERACTION_IN)
      strncpy(dest_string, "IN", sizeof(dest_string));
    else if (data.transfer_action == OfxTransactionData::OFX_TFERACTION_OUT)
      strncpy(dest_string, "OUT", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Action for the transfer: " << dest_string << "\n";
  }
  if (data.unit_type_valid == true)
  {
    if (data.unit_type == OfxTransactionData::OFX_UNITTYPE_SHARES)
      strncpy(dest_string, "SHARES", sizeof(dest_string));
    else if (data.unit_type == OfxTransactionData::OFX_UNITTYPE_CURRENCY)
      strncpy(dest_string, "CURRENCY", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Type of the Units value: " << dest_string << "\n";
  }
  if (data.withholding_valid == true)
  {
    cout << "    Federal tax withheld (amount): " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.withholding << "\n";
  }
  if (data.security_data_valid == true)
  {
    ofx_proc_security_cb(*(data.security_data_ptr), NULL );
  }
  cout << "\n";
  return 0;
}//end ofx_proc_transaction()

int ofx_proc_position_cb(struct OfxPositionData data, void * position_data)
{
  char dest_string[255];
  cout << "ofx_proc_position():\n";
  if (data.account_id_valid == true)
  {
    cout << "    Account ID: " << data.account_id << "\n";
  }
  if (data.unique_id_valid == true)
  {
    cout << "    Unique ID of the security: " << data.unique_id << "\n";
  }
  if (data.unique_id_type_valid == true)
  {
    cout << "    Format of the Unique ID: " << data.unique_id_type << "\n";
  }
  if (data.heldinaccount_type_valid == true)
  {
    if (data.heldinaccount_type == OfxPositionData::OFX_HELDINACCT_CASH)
      strncpy(dest_string, "CASH: Cash subaccount", sizeof(dest_string));
    else if (data.heldinaccount_type == OfxPositionData::OFX_HELDINACCT_MARGIN)
      strncpy(dest_string, "MARGIN: Margin subaccount", sizeof(dest_string));
    else if (data.heldinaccount_type == OfxPositionData::OFX_HELDINACCT_SHORT)
      strncpy(dest_string, "SHORT: Short subaccount", sizeof(dest_string));
    else if (data.heldinaccount_type == OfxPositionData::OFX_HELDINACCT_OTHER)
      strncpy(dest_string, "OTHER: Other subaccount", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Held in account type: " << dest_string << "\n";
  }
  if (data.position_type_valid == true)
  {
    if (data.position_type == OfxPositionData::OFX_POSITION_SHORT)
      strncpy(dest_string, "SHORT: Short (writer for options, short for all others)", sizeof(dest_string));
    else if (data.position_type == OfxPositionData::OFX_POSITION_LONG)
      strncpy(dest_string, "LONG: Long (holder for options, long for all others)", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR: unrecognized", sizeof(dest_string));
    cout << "    Position type: " << dest_string << "\n";
  }
  if (data.units_valid == true)
  {
    cout << "    Units: " << setprecision(4) << data.units;
    strncpy(dest_string, " (bonds: face value; options: contracts; all others: shares)\n", sizeof(dest_string));
    if (data.security_data_valid == true)
    {
      if (data.security_data_ptr->security_type_valid == true)
      {
        if (data.security_data_ptr->security_type == OfxSecurityData::OFX_DEBT_SECURITY)
          strncpy(dest_string, " (face value)\n", sizeof(dest_string));
        else if (data.security_data_ptr->security_type == OfxSecurityData::OFX_FUND_SECURITY ||
                 data.security_data_ptr->security_type == OfxSecurityData::OFX_STOCK_SECURITY ||
                 data.security_data_ptr->security_type == OfxSecurityData::OFX_OTHER_SECURITY)
          strncpy(dest_string, " (shares)\n", sizeof(dest_string));
        else if (data.security_data_ptr->security_type == OfxSecurityData::OFX_OPTION_SECURITY)
          strncpy(dest_string, " (contracts)\n", sizeof(dest_string));
      }
    }
    cout << dest_string;
  }
  if (data.unit_price_valid == true)
  {
    if (data.security_data_valid == true)
    {
      cout << "    Unit price: " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(4) << data.unit_price;
      strncpy(dest_string, " (bonds: % of par; options: premium per share of underlying; all others: price per share)\n", sizeof(dest_string));
      if (data.security_data_ptr->security_type_valid == true)
      {
        if (data.security_data_ptr->security_type == OfxSecurityData::OFX_DEBT_SECURITY)
          strncpy(dest_string, "% (% of par)\n", sizeof(dest_string));
        else if (data.security_data_ptr->security_type == OfxSecurityData::OFX_FUND_SECURITY ||
                 data.security_data_ptr->security_type == OfxSecurityData::OFX_STOCK_SECURITY ||
                 data.security_data_ptr->security_type == OfxSecurityData::OFX_OTHER_SECURITY)
          strncpy(dest_string, " (price per share)\n", sizeof(dest_string));
        else if (data.security_data_ptr->security_type == OfxSecurityData::OFX_OPTION_SECURITY)
          strncpy(dest_string, " (premium per share of underlying)\n", sizeof(dest_string));
      }
      cout << dest_string;
    }
    else
      cout << "    Unit price: " <<  setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.unit_price << "\n";
  }
  if (data.market_value_valid == true)
  {
    cout << "    Market Value: " << data.market_value << "\n";
  }
  if (data.amounts_are_foreign_currency_valid == true)
  {
    cout << "    Amounts are in foreign currency: " << (data.amounts_are_foreign_currency ? "Yes" : "No") << "\n";
  }
  if (data.currency_valid == true)
  {
    cout << "    Currency: " << data.currency << "\n";
  }
  if (data.currency_ratio_valid == true)
  {
    cout << "    Ratio of default currency to currency: " << data.currency_ratio << "\n";
  }
  if (data.date_unit_price_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_unit_price)));
    cout << "    Date of unit price: " << dest_string << "\n";
  }
  if (data.memo_valid == true)
  {
    cout << "    Extra position information (memo): " << data.memo << "\n";
  }
  if (data.inv_401k_source_valid == true)
  {
    if (data.inv_401k_source == OfxPositionData::OFX_401K_POSN_SOURCE_PRETAX)
      strncpy(dest_string, "PRETAX", sizeof(dest_string));
    else if (data.inv_401k_source == OfxPositionData::OFX_401K_POSN_SOURCE_AFTERTAX)
      strncpy(dest_string, "AFTERTAX", sizeof(dest_string));
    else if (data.inv_401k_source == OfxPositionData::OFX_401K_POSN_SOURCE_MATCH)
      strncpy(dest_string, "MATCH", sizeof(dest_string));
    else if (data.inv_401k_source == OfxPositionData::OFX_401K_POSN_SOURCE_PROFITSHARING)
      strncpy(dest_string, "PROFITSHARING", sizeof(dest_string));
    else if (data.inv_401k_source == OfxPositionData::OFX_401K_POSN_SOURCE_ROLLOVER)
      strncpy(dest_string, "ROLLOVER", sizeof(dest_string));
    else if (data.inv_401k_source == OfxPositionData::OFX_401K_POSN_SOURCE_OTHERVEST)
      strncpy(dest_string, "OTHERVEST", sizeof(dest_string));
    else if (data.inv_401k_source == OfxPositionData::OFX_401K_POSN_SOURCE_OTHERNONVEST)
      strncpy(dest_string, "OTHERNONVEST", sizeof(dest_string));
    else
      strncpy(dest_string,"ERROR: unrecognized", sizeof(dest_string));
    cout << "    Source of 401(k) money: " << dest_string << "\n";
  }
  if (data.security_data_valid == true)
  {
    ofx_proc_security_cb(*(data.security_data_ptr), NULL );
  }
  cout << "\n";
  return 0;
}// end of ofx_proc_position()

int ofx_proc_statement_cb(struct OfxStatementData data, void * statement_data)
{
  char dest_string[255];
  cout << "ofx_proc_statement():\n";
  if (data.currency_valid == true)
  {
    cout << "    Default Currency: " << data.currency << "\n";
  }
  if (data.account_id_valid == true)
  {
    cout << "    Account ID: " << data.account_id << "\n";
  }
  if (data.date_asof_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_asof)));
    cout << "    Statement as-of date: " << dest_string << "\n";
  }
  if (data.date_start_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_start)));
    cout << "    Start date of this statement: " << dest_string << "\n";
  }
  if (data.date_end_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_end)));
    cout << "    End date of this statement: " << dest_string << "\n";
  }
  if (data.ledger_balance_valid == true)
  {
    cout << "    Ledger balance: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.ledger_balance << "\n";
  }
  if (data.ledger_balance_date_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.ledger_balance_date)));
    cout << "    Ledger balance date: " << dest_string << "\n";
  }
  if (data.available_balance_valid == true)
  {
    cout << "    Available balance: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.available_balance << "\n";
  }
  if (data.available_balance_date_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.available_balance_date)));
    cout << "    Available balance date: " << dest_string << "\n";
  }
  if (data.margin_balance_valid == true)
  {
    cout << "    Margin balance: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.margin_balance << "\n";
  }
  if (data.short_balance_valid == true)
  {
    cout << "    Short balance: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.short_balance << "\n";
  }
  if (data.buying_power_valid == true)
  {
    cout << "    Buying power: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.buying_power << "\n";
  }
  if (data.marketing_info_valid == true)
  {
    cout << "    Marketing information: " << data.marketing_info << "\n";
  }
  cout << "\n";
  return 0;
}//end ofx_proc_statement()

int ofx_proc_account_cb(struct OfxAccountData data, void * account_data)
{
  cout << "ofx_proc_account():\n";
  if (data.account_id_valid == true)
  {
    cout << "    Account ID: " << data.account_id << "\n";
    cout << "    Account name: " << data.account_name << "\n";
  }
  if (data.account_type_valid == true)
  {
    cout << "    Account type: ";
    switch (data.account_type)
    {
    case OfxAccountData::OFX_CHECKING :
      cout << "CHECKING\n";
      break;
    case OfxAccountData::OFX_SAVINGS :
      cout << "SAVINGS\n";
      break;
    case OfxAccountData::OFX_MONEYMRKT :
      cout << "MONEYMRKT\n";
      break;
    case OfxAccountData::OFX_CREDITLINE :
      cout << "CREDITLINE\n";
      break;
    case OfxAccountData::OFX_CMA :
      cout << "CMA\n";
      break;
    case OfxAccountData::OFX_CREDITCARD :
      cout << "CREDITCARD\n";
      break;
    case OfxAccountData::OFX_INVESTMENT :
      cout << "INVESTMENT\n";
      break;
    default:
      cout << "ofx_proc_account() WRITEME: This is an unknown account type!";
    }
  }
  if (data.currency_valid == true)
  {
    cout << "    Default Currency: " << data.currency << "\n";
  }

  if (data.bank_id_valid)
    cout << "    Bank ID: " << data.bank_id << endl;;

  if (data.branch_id_valid)
    cout << "    Branch ID: " << data.branch_id << endl;

  if (data.account_number_valid)
    cout << "    Account #: " << data.account_number << endl;

  cout << "\n";
  return 0;
}//end ofx_proc_account()



int ofx_proc_status_cb(struct OfxStatusData data, void * status_data)
{
  cout << "ofx_proc_status():\n";
  if (data.ofx_element_name_valid == true)
  {
    cout << "    Ofx entity this status is relevant to: " << data.ofx_element_name << " \n";
  }
  if (data.severity_valid == true)
  {
    cout << "    Severity: ";
    switch (data.severity)
    {
    case OfxStatusData::INFO :
      cout << "INFO\n";
      break;
    case OfxStatusData::WARN :
      cout << "WARN\n";
      break;
    case OfxStatusData::ERROR :
      cout << "ERROR\n";
      break;
    default:
      cout << "WRITEME: Unknown status severity!\n";
    }
  }
  if (data.code_valid == true)
  {
    cout << "    Code: " << data.code << ", name: " << data.name << "\n    Description: " << data.description << "\n";
  }
  if (data.server_message_valid == true)
  {
    cout << "    Server Message: " << data.server_message << "\n";
  }
  cout << "\n";
  return 0;
}


int main (int argc, char *argv[])
{
  int rc = 0;
  gengetopt_args_info args_info;

  /* let's call our cmdline parser */
  if (cmdline_parser (argc, argv, &args_info) != 0)
    exit(1) ;

  //  if (args_info.msg_parser_given)
  //    cout << "The msg_parser option was given!" << endl;

  //  cout << "The flag is " << ( args_info.msg_parser_flag ? "on" : "off" ) <<
  //    "." << endl ;

  ofx_PARSER_msg = bool(args_info.msg_parser_flag);
  ofx_DEBUG_msg = bool(args_info.msg_debug_flag);
  ofx_WARNING_msg = bool(args_info.msg_warning_flag);
  ofx_ERROR_msg = bool (args_info.msg_error_flag);
  ofx_INFO_msg = bool(args_info.msg_info_flag);
  ofx_STATUS_msg = bool(args_info.msg_status_flag);

  bool skiphelp = false;

  if (args_info.list_import_formats_given)
  {
    skiphelp = true;
    cout << "The supported file formats for the 'input-file-format' argument are:" << endl;
    for (int i = 0; LibofxImportFormatList[i].format != LAST; i++)
    {
      cout << "     " << LibofxImportFormatList[i].description << endl;
    }
  }

  LibofxContextPtr libofx_context = libofx_get_new_context();

  //char **inputs ; /* unnamed options */
  //unsigned inputs_num ; /* unnamed options number */
  if (args_info.inputs_num  > 0)
  {
    //const char* filename = args_info.inputs[0];

    ofx_set_statement_cb(libofx_context, ofx_proc_statement_cb, 0);
    ofx_set_account_cb(libofx_context, ofx_proc_account_cb, 0);
    ofx_set_transaction_cb(libofx_context, ofx_proc_transaction_cb, 0);
    ofx_set_security_cb(libofx_context, ofx_proc_security_cb, 0);
    ofx_set_status_cb(libofx_context, ofx_proc_status_cb, 0);
    ofx_set_position_cb(libofx_context, ofx_proc_position_cb, 0);

    enum LibofxFileFormat file_format = libofx_get_file_format_from_str(LibofxImportFormatList, args_info.import_format_arg);
    /** @todo currently, only the first file is processed as the library can't deal with more right now.*/
    if (args_info.inputs_num  > 1)
    {
      cout << "Sorry, currently, only the first file is processed as the library can't deal with more right now.  The following files were ignored:" << endl;
      for ( unsigned i = 1 ; i < args_info.inputs_num ; ++i )
      {
        cout << "file: " << args_info.inputs[i] << endl ;
      }
    }
    rc = libofx_proc_file(libofx_context, args_info.inputs[0], file_format);
  }
  else
  {
    if ( !skiphelp )
      cmdline_parser_print_help();
  }

  libofx_free_context(libofx_context);
  cmdline_parser_free(&args_info);
  return rc;
}
