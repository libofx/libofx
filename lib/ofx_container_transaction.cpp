/***************************************************************************
         ofx_container_account.cpp
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : benoitg@coeus.ca
***************************************************************************/
/**@file
 * \brief Implementation of  OfxTransactionContainer,
 OfxBankTransactionContainer and OfxInvestmentTransactionContainer.
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

#include <cstdlib>
#include <string>
#include "messages.hh"
#include "libofx.h"
#include "ofx_containers.hh"
#include "ofx_utilities.hh"

extern OfxMainContainer * MainContainer;

/***************************************************************************
 *                      OfxTransactionContainer                            *
 ***************************************************************************/

OfxTransactionContainer::OfxTransactionContainer(LibofxContext *p_libofx_context, OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(p_libofx_context, para_parentcontainer, para_tag_identifier)
{
  OfxGenericContainer * tmp_parentcontainer = parentcontainer;

  memset(&data, 0, sizeof(data));
  type = "TRANSACTION";
  /* Find the parent statement container*/
  while (tmp_parentcontainer != NULL && tmp_parentcontainer->type != "STATEMENT")
  {
    tmp_parentcontainer = tmp_parentcontainer->parentcontainer;
  }
  if (tmp_parentcontainer != NULL)
  {
    parent_statement = (OfxStatementContainer*)tmp_parentcontainer;
  }
  else
  {
    parent_statement = NULL;
    message_out(ERROR, "Unable to find the enclosing statement container this transaction");
  }
  if (parent_statement != NULL && parent_statement->data.account_id_valid == true)
  {
    strncpy(data.account_id, parent_statement->data.account_id, OFX_ACCOUNT_ID_LENGTH);
    data.account_id_valid = true;
  }
}
OfxTransactionContainer::~OfxTransactionContainer()
{

}

int OfxTransactionContainer::gen_event()
{
  if (data.unique_id_valid == true && MainContainer != NULL)
  {
    data.security_data_ptr = MainContainer->find_security(data.unique_id);
    if (data.security_data_ptr != NULL)
    {
      data.security_data_valid = true;
    }
  }
  libofx_context->transactionCallback(data);
  return true;
}

int  OfxTransactionContainer::add_to_main_tree()
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


void OfxTransactionContainer::add_attribute(const string identifier, const string value)
{

  if (identifier == "DTPOSTED")
  {
    data.date_posted = ofxdate_to_time_t(value);
    data.date_posted_valid = true;
  }
  else if (identifier == "DTUSER")
  {
    data.date_initiated = ofxdate_to_time_t(value);
    data.date_initiated_valid = true;
  }
  else if (identifier == "DTAVAIL")
  {
    data.date_funds_available = ofxdate_to_time_t(value);
    data.date_funds_available_valid = true;
  }
  else if (identifier == "FITID")
  {
    STRNCPY(data.fi_id, value);
    data.fi_id_valid = true;
  }
  else if (identifier == "CORRECTFITID")
  {
    STRNCPY(data.fi_id_corrected, value);
    data.fi_id_corrected_valid = true;
  }
  else if (identifier == "CORRECTACTION")
  {
    data.fi_id_correction_action_valid = true;
    if (value == "REPLACE")
    {
      data.fi_id_correction_action = REPLACE;
    }
    else if (value == "DELETE")
    {
      data.fi_id_correction_action = DELETE;
    }
    else
    {
      data.fi_id_correction_action_valid = false;
    }
  }
  else if ((identifier == "SRVRTID") || (identifier == "SRVRTID2"))
  {
    STRNCPY(data.server_transaction_id, value);
    data.server_transaction_id_valid = true;
  }
  else if (identifier == "MEMO" || identifier == "MEMO2")
  {
    STRNCPY(data.memo, value);
    data.memo_valid = true;
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
  else
  {
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}// end OfxTransactionContainer::add_attribute()

void OfxTransactionContainer::add_account(OfxAccountData * account_data)
{
  if (account_data->account_id_valid == true)
  {
    data.account_ptr = account_data;
    strncpy(data.account_id, account_data->account_id, OFX_ACCOUNT_ID_LENGTH);
    data.account_id_valid = true;
  }
}

/***************************************************************************
 *                      OfxBankTransactionContainer                        *
 ***************************************************************************/

OfxBankTransactionContainer::OfxBankTransactionContainer(LibofxContext *p_libofx_context, OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxTransactionContainer(p_libofx_context, para_parentcontainer, para_tag_identifier)
{
  ;
}
void OfxBankTransactionContainer::add_attribute(const string identifier, const string value)
{
  if ( identifier == "TRNTYPE")
  {
    data.transactiontype_valid = true;
    if (value == "CREDIT")
    {
      data.transactiontype = OFX_CREDIT;
    }
    else if (value == "DEBIT")
    {
      data.transactiontype = OFX_DEBIT;
    }
    else if (value == "INT")
    {
      data.transactiontype = OFX_INT;
    }
    else if (value == "DIV")
    {
      data.transactiontype = OFX_DIV;
    }
    else if (value == "FEE")
    {
      data.transactiontype = OFX_FEE;
    }
    else if (value == "SRVCHG")
    {
      data.transactiontype = OFX_SRVCHG;
    }
    else if (value == "DEP")
    {
      data.transactiontype = OFX_DEP;
    }
    else if (value == "ATM")
    {
      data.transactiontype = OFX_ATM;
    }
    else if (value == "POS")
    {
      data.transactiontype = OFX_POS;
    }
    else if (value == "XFER")
    {
      data.transactiontype = OFX_XFER;
    }
    else if (value == "CHECK")
    {
      data.transactiontype = OFX_CHECK;
    }
    else if (value == "PAYMENT")
    {
      data.transactiontype = OFX_PAYMENT;
    }
    else if (value == "CASH")
    {
      data.transactiontype = OFX_CASH;
    }
    else if (value == "DIRECTDEP")
    {
      data.transactiontype = OFX_DIRECTDEP;
    }
    else if (value == "DIRECTDEBIT")
    {
      data.transactiontype = OFX_DIRECTDEBIT;
    }
    else if (value == "REPEATPMT")
    {
      data.transactiontype = OFX_REPEATPMT;
    }
    else if (value == "OTHER")
    {
      data.transactiontype = OFX_OTHER;
    }
    else
    {
      data.transactiontype_valid = false;
    }
  }//end TRANSTYPE
  else if (identifier == "TRNAMT")
  {
    data.amount = ofxamount_to_double(value);
    data.amount_valid = true;
    data.units = -data.amount;
    data.units_valid = true;
    data.unitprice = 1.00;
    data.unitprice_valid = true;
  }
  else if (identifier == "CHECKNUM")
  {
    STRNCPY(data.check_number, value);
    data.check_number_valid = true;
  }
  else if (identifier == "REFNUM")
  {
    STRNCPY(data.reference_number, value);
    data.reference_number_valid = true;
  }
  else if (identifier == "SIC")
  {
    data.standard_industrial_code = atoi(value.c_str());
    data.standard_industrial_code_valid = true;
  }
  else if ((identifier == "PAYEEID") || (identifier == "PAYEEID2"))
  {
    STRNCPY(data.payee_id, value);
    data.payee_id_valid = true;
  }
  else if (identifier == "NAME")
  {
    STRNCPY(data.name, value);
    data.name_valid = true;
  }
  else
  {
    /* Redirect unknown identifiers to base class */
    OfxTransactionContainer::add_attribute(identifier, value);
  }
}//end OfxBankTransactionContainer::add_attribute


/***************************************************************************
 *                    OfxInvestmentTransactionContainer                    *
 ***************************************************************************/

OfxInvestmentTransactionContainer::OfxInvestmentTransactionContainer(LibofxContext *p_libofx_context, OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxTransactionContainer(p_libofx_context, para_parentcontainer, para_tag_identifier)
{
  type = "INVESTMENT";
  data.transactiontype = OFX_OTHER;
  data.transactiontype_valid = true;

  data.invtransactiontype_valid = true;
  if (para_tag_identifier == "BUYDEBT")
  {
    data.invtransactiontype = OFX_BUYDEBT;
  }
  else if (para_tag_identifier == "BUYMF")
  {
    data.invtransactiontype = OFX_BUYMF;
  }
  else if (para_tag_identifier == "BUYOPT")
  {
    data.invtransactiontype = OFX_BUYOPT;
  }
  else if (para_tag_identifier == "BUYOTHER")
  {
    data.invtransactiontype = OFX_BUYOTHER;
  }
  else if (para_tag_identifier == "BUYSTOCK")
  {
    data.invtransactiontype = OFX_BUYSTOCK;
  }
  else if (para_tag_identifier == "CLOSUREOPT")
  {
    data.invtransactiontype = OFX_CLOSUREOPT;
  }
  else if (para_tag_identifier == "INCOME")
  {
    data.invtransactiontype = OFX_INCOME;
  }
  else if (para_tag_identifier == "INVEXPENSE")
  {
    data.invtransactiontype = OFX_INVEXPENSE;
  }
  else if (para_tag_identifier == "JRNLFUND")
  {
    data.invtransactiontype = OFX_JRNLFUND;
  }
  else if (para_tag_identifier == "JRNLSEC")
  {
    data.invtransactiontype = OFX_JRNLSEC;
  }
  else if (para_tag_identifier == "MARGININTEREST")
  {
    data.invtransactiontype = OFX_MARGININTEREST;
  }
  else if (para_tag_identifier == "REINVEST")
  {
    data.invtransactiontype = OFX_REINVEST;
  }
  else if (para_tag_identifier == "RETOFCAP")
  {
    data.invtransactiontype = OFX_RETOFCAP;
  }
  else if (para_tag_identifier == "SELLDEBT")
  {
    data.invtransactiontype = OFX_SELLDEBT;
  }
  else if (para_tag_identifier == "SELLMF")
  {
    data.invtransactiontype = OFX_SELLMF;
  }
  else if (para_tag_identifier == "SELLOPT")
  {
    data.invtransactiontype = OFX_SELLOPT;
  }
  else if (para_tag_identifier == "SELLOTHER")
  {
    data.invtransactiontype = OFX_SELLOTHER;
  }
  else if (para_tag_identifier == "SELLSTOCK")
  {
    data.invtransactiontype = OFX_SELLSTOCK;
  }
  else if (para_tag_identifier == "SPLIT")
  {
    data.invtransactiontype = OFX_SPLIT;
  }
  else if (para_tag_identifier == "TRANSFER")
  {
    data.invtransactiontype = OFX_TRANSFER;
  }
  else if (para_tag_identifier == "INVBANKTRAN")
  {
    data.invtransactiontype = OFX_INVBANKTRAN;
  }
  else
  {
    message_out(ERROR, "This should not happen, " + para_tag_identifier + " is an unknown investment transaction type");
    data.invtransactiontype_valid = false;
  }
}

void OfxInvestmentTransactionContainer::add_attribute(const string identifier, const string value)
{
  if (identifier == "UNIQUEID")
  {
    STRNCPY(data.unique_id, value);
    data.unique_id_valid = true;
  }
  else if (identifier == "UNIQUEIDTYPE")
  {
    STRNCPY(data.unique_id_type, value);
    data.unique_id_type_valid = true;
  }
  else if (identifier == "UNITS")
  {
    data.units = ofxamount_to_double(value);
    data.units_valid = true;
  }
  else if (identifier == "UNITPRICE")
  {
    data.unitprice = ofxamount_to_double(value);
    data.unitprice_valid = true;
  }
  else if (identifier == "MKTVAL")
  {
    data.market_value = ofxamount_to_double(value);
    data.market_value_valid = true;
  }
  else if (identifier == "TOTAL")
  {
    data.amount = ofxamount_to_double(value);
    data.amount_valid = true;
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
  else if (identifier == "DTSETTLE")
  {
    data.date_posted = ofxdate_to_time_t(value);
    data.date_posted_valid = true;
  }
  else if (identifier == "DTTRADE")
  {
    data.date_initiated = ofxdate_to_time_t(value);
    data.date_initiated_valid = true;
  }
  else if (identifier == "COMMISSION")
  {
    data.commission = ofxamount_to_double(value);
    data.commission_valid = true;
  }
  else if (identifier == "FEES")
  {
    data.fees = ofxamount_to_double(value);
    data.fees_valid = true;
  }
  else if (identifier == "OLDUNITS")
  {
    data.oldunits = ofxamount_to_double(value);
    data.oldunits_valid = true;
  }
  else if (identifier == "NEWUNITS")
  {
    data.newunits = ofxamount_to_double(value);
    data.newunits_valid = true;
  }
  else if (identifier == "ACCRDINT")
  {
    data.accrued_interest = ofxamount_to_double(value);
    data.accrued_interest_valid = true;
  }
  else if (identifier == "AVGCOSTBASIS")
  {
    data.avg_cost_basis = ofxamount_to_double(value);
    data.avg_cost_basis_valid = true;
  }
  else if (identifier == "BUYTYPE" || identifier == "OPTBUYTYPE")
  {
    if (value == "BUY")
    {
      data.buy_type = data.OFX_BUY_TYPE_BUY;
      data.buy_type_valid = true;
    }
    else if (value == "BUYTOCOVER")
    {
      data.buy_type = data.OFX_BUY_TYPE_BUYTOCOVER;
      data.buy_type_valid = true;
    }
    else if (value == "BUYTOOPEN")
    {
      data.buy_type = data.OFX_BUY_TYPE_BUYTOOPEN;
      data.buy_type_valid = true;
    }
    else if (value == "BUYTOCLOSE")
    {
      data.buy_type = data.OFX_BUY_TYPE_BUYTOCLOSE;
      data.buy_type_valid = true;
    }
  }
  else if (identifier == "DENOMINATOR")
  {
    data.denominator = ofxamount_to_double(value);
    data.denominator_valid = true;
  }
  else if (identifier == "DTPAYROLL")
  {
    data.date_payroll = ofxdate_to_time_t(value);
    data.date_payroll_valid = true;
  }
  else if (identifier == "DTPURCHASE")
  {
    data.date_purchase = ofxdate_to_time_t(value);
    data.date_purchase_valid = true;
  }
  else if (identifier == "GAIN")
  {
    data.gain = ofxamount_to_double(value);
    data.gain_valid = true;
  }
  else if (identifier == "FRACCASH")
  {
    data.cash_for_fractional = ofxamount_to_double(value);
    data.cash_for_fractional_valid = true;
  }
  else if (identifier == "INCOMETYPE")
  {
    if (value == "CGLONG")
    {
      data.income_type = data.OFX_CGLONG;
      data.income_type_valid = true;
    }
    else if (value == "CGSHORT")
    {
      data.income_type = data.OFX_CGSHORT;
      data.income_type_valid = true;
    }
    else if (value == "DIV")
    {
      data.income_type = data.OFX_DIVIDEND;
      data.income_type_valid = true;
    }
    else if (value == "INTEREST")
    {
      data.income_type = data.OFX_INTEREST;
      data.income_type_valid = true;
    }
    else if (value == "MISC")
    {
      data.income_type = data.OFX_MISC;
      data.income_type_valid = true;
    }
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
  else if (identifier == "LOAD")
  {
    data.load = ofxamount_to_double(value);
    data.load_valid = true;
  }
  else if (identifier == "LOANID")
  {
    STRNCPY(data.loan_id, value);
    data.loan_id_valid = true;
  }
  else if (identifier == "LOANINTEREST")
  {
    data.loan_interest = ofxamount_to_double(value);
    data.loan_interest_valid = true;
  }
  else if (identifier == "LOANPRINCIPAL")
  {
    data.loan_principal = ofxamount_to_double(value);
    data.loan_principal_valid = true;
  }
  else if (identifier == "MARKDOWN")
  {
    data.markdown = ofxamount_to_double(value);
    data.markdown_valid = true;
  }
  else if (identifier == "MARKUP")
  {
    data.markup = ofxamount_to_double(value);
    data.markup_valid = true;
  }
  else if (identifier == "NUMERATOR")
  {
    data.numerator = ofxamount_to_double(value);
    data.numerator_valid = true;
  }
  else if (identifier == "OPTACTION")
  {
    if (value == "EXERCISE")
    {
      data.opt_action = data.OFX_OPTACTION_EXERCISE;
      data.opt_action_valid = true;
    }
    else if (value == "ASSIGN")
    {
      data.opt_action = data.OFX_OPTACTION_ASSIGN;
      data.opt_action_valid = true;
    }
    else if (value == "EXPIRE")
    {
      data.opt_action = data.OFX_OPTACTION_EXPIRE;
      data.opt_action_valid = true;
    }
  }
  else if (identifier == "PENALTY")
  {
    data.penalty = ofxamount_to_double(value);
    data.penalty_valid = true;
  }
  else if (identifier == "POSTYPE")
  {
    if (value == "LONG")
    {
      data.pos_type = data.OFX_POSTYPE_LONG;
      data.pos_type_valid = true;
    }
    else if (value == "SHORT")
    {
      data.pos_type = data.OFX_POSTYPE_SHORT;
      data.pos_type_valid = true;
    }
  }
  else if (identifier == "PRIORYEARCONTRIB")
  {
    if (value == "Y")
    {
      data.prior_year_contrib = true;
      data.prior_year_contrib_valid = true;
    }
    else if (value == "N")
    {
      data.prior_year_contrib = false;
      data.prior_year_contrib_valid = true;
    }
  }
  else if (identifier == "RELFITID")
  {
    STRNCPY(data.related_fi_tid, value);
    data.related_fi_tid_valid = true;
  }
  else if (identifier == "RELTYPE")
  {
    if (value == "SPREAD")
    {
      data.related_type = data.OFX_RELTYPE_SPREAD;
      data.related_type_valid = true;
    }
    else if (value == "STRADDLE")
    {
      data.related_type = data.OFX_RELTYPE_STRADDLE;
      data.related_type_valid = true;
    }
    else if (value == "NONE")
    {
      data.related_type = data.OFX_RELTYPE_NONE;
      data.related_type_valid = true;
    }
    else if (value == "OTHER")
    {
      data.related_type = data.OFX_RELTYPE_OTHER;
      data.related_type_valid = true;
    }
  }
  else if (identifier == "SECURED")
  {
    if (value == "NAKED")
    {
      data.option_secured = data.OFX_SECURED_NAKED;
      data.option_secured_valid = true;
    }
    else if (value == "COVERED")
    {
      data.option_secured = data.OFX_SECURED_COVERED;
      data.option_secured_valid = true;
    }
  }
  else if (identifier == "SELLREASON")
  {
    if (value == "CALL")
    {
      data.sell_reason = data.OFX_SELLREASON_CALL;
      data.sell_reason_valid = true;
    }
    else if (value == "SELL")
    {
      data.sell_reason = data.OFX_SELLREASON_SELL;
      data.sell_reason_valid = true;
    }
    else if (value == "MATURITY")
    {
      data.sell_reason = data.OFX_SELLREASON_MATURITY;
      data.sell_reason_valid = true;
    }
  }
  else if (identifier == "SELLTYPE" || identifier == "OPTSELLTYPE")
  {
    if (value == "SELL")
    {
      data.sell_type = data.OFX_SELL_TYPE_SELL;
      data.sell_type_valid = true;
    }
    else if (value == "SELLSHORT")
    {
      data.sell_type = data.OFX_SELL_TYPE_SELLSHORT;
      data.sell_type_valid = true;
    }
    else if (value == "SELLTOOPEN")
    {
      data.sell_type = data.OFX_SELL_TYPE_SELLTOOPEN;
      data.sell_type_valid = true;
    }
    else if (value == "SELLTOCLOSE")
    {
      data.sell_type = data.OFX_SELL_TYPE_SELLTOCLOSE;
      data.sell_type_valid = true;
    }
  }
  else if (identifier == "SHPERCTRCT")
  {
    data.shares_per_cont = ofxamount_to_double(value);
    data.shares_per_cont_valid = true;
  }
  else if (identifier == "STATEWITHHOLDING")
  {
    data.state_withholding = ofxamount_to_double(value);
    data.state_withholding_valid = true;
  }
  else if (identifier == "SUBACCTFROM")
  {
    if (value == "CASH")
    {
      data.subacct_from = data.OFX_SUBACCT_CASH;
      data.subacct_from_valid = true;
    }
    else if (value == "MARGIN")
    {
      data.subacct_from = data.OFX_SUBACCT_MARGIN;
      data.subacct_from_valid = true;
    }
    else if (value == "SHORT")
    {
      data.subacct_from = data.OFX_SUBACCT_SHORT;
      data.subacct_from_valid = true;
    }
    else if (value == "OTHER")
    {
      data.subacct_from = data.OFX_SUBACCT_OTHER;
      data.subacct_from_valid = true;
    }
  }
  else if (identifier == "SUBACCTFUND")
  {
    if (value == "CASH")
    {
      data.subacct_funding = data.OFX_SUBACCT_CASH;
      data.subacct_funding_valid = true;
    }
    else if (value == "MARGIN")
    {
      data.subacct_funding = data.OFX_SUBACCT_MARGIN;
      data.subacct_funding_valid = true;
    }
    else if (value == "SHORT")
    {
      data.subacct_funding = data.OFX_SUBACCT_SHORT;
      data.subacct_funding_valid = true;
    }
    else if (value == "OTHER")
    {
      data.subacct_funding = data.OFX_SUBACCT_OTHER;
      data.subacct_funding_valid = true;
    }
  }
  else if (identifier == "SUBACCTSEC")
  {
    if (value == "CASH")
    {
      data.subacct_security = data.OFX_SUBACCT_CASH;
      data.subacct_security_valid = true;
    }
    else if (value == "MARGIN")
    {
      data.subacct_security = data.OFX_SUBACCT_MARGIN;
      data.subacct_security_valid = true;
    }
    else if (value == "SHORT")
    {
      data.subacct_security = data.OFX_SUBACCT_SHORT;
      data.subacct_security_valid = true;
    }
    else if (value == "OTHER")
    {
      data.subacct_security = data.OFX_SUBACCT_OTHER;
      data.subacct_security_valid = true;
    }
  }
  else if (identifier == "SUBACCTTO")
  {
    if (value == "CASH")
    {
      data.subacct_to = data.OFX_SUBACCT_CASH;
      data.subacct_to_valid = true;
    }
    else if (value == "MARGIN")
    {
      data.subacct_to = data.OFX_SUBACCT_MARGIN;
      data.subacct_to_valid = true;
    }
    else if (value == "SHORT")
    {
      data.subacct_to = data.OFX_SUBACCT_SHORT;
      data.subacct_to_valid = true;
    }
    else if (value == "OTHER")
    {
      data.subacct_to = data.OFX_SUBACCT_OTHER;
      data.subacct_to_valid = true;
    }
  }
  else if (identifier == "TAXES")
  {
    data.taxes = ofxamount_to_double(value);
    data.taxes_valid = true;
  }
  else if (identifier == "TAXEXEMPT")
  {
    if (value == "Y")
    {
      data.tax_exempt = true;
      data.tax_exempt_valid = true;
    }
    else if (value == "N")
    {
      data.tax_exempt = false;
      data.tax_exempt_valid = true;
    }
  }
  else if (identifier == "TFERACTION")
  {
    if (value == "IN")
    {
      data.transfer_action = data.OFX_TFERACTION_IN;
      data.transfer_action_valid = true;
    }
    else if (value == "OUT")
    {
      data.transfer_action = data.OFX_TFERACTION_OUT;
      data.transfer_action_valid = true;
    }
  }
  else if (identifier == "UNITTYPE")
  {
    if (value == "SHARES")
    {
      data.unit_type = data.OFX_UNITTYPE_SHARES;
      data.unit_type_valid = true;
    }
    else if (value == "CURRENCY")
    {
      data.unit_type = data.OFX_UNITTYPE_CURRENCY;
      data.unit_type_valid = true;
    }
  }
  else if (identifier == "WITHHOLDING")
  {
    data.withholding = ofxamount_to_double(value);
    data.withholding_valid = true;
  }
  /* the following fields are <STMTTRN> elements for <INVBANKTRAN> */
  else if ( identifier == "TRNTYPE")
  {
    data.transactiontype_valid = true;
    if (value == "CREDIT")
    {
      data.transactiontype = OFX_CREDIT;
    }
    else if (value == "DEBIT")
    {
      data.transactiontype = OFX_DEBIT;
    }
    else if (value == "INT")
    {
      data.transactiontype = OFX_INT;
    }
    else if (value == "DIV")
    {
      data.transactiontype = OFX_DIV;
    }
    else if (value == "FEE")
    {
      data.transactiontype = OFX_FEE;
    }
    else if (value == "SRVCHG")
    {
      data.transactiontype = OFX_SRVCHG;
    }
    else if (value == "DEP")
    {
      data.transactiontype = OFX_DEP;
    }
    else if (value == "ATM")
    {
      data.transactiontype = OFX_ATM;
    }
    else if (value == "POS")
    {
      data.transactiontype = OFX_POS;
    }
    else if (value == "XFER")
    {
      data.transactiontype = OFX_XFER;
    }
    else if (value == "CHECK")
    {
      data.transactiontype = OFX_CHECK;
    }
    else if (value == "PAYMENT")
    {
      data.transactiontype = OFX_PAYMENT;
    }
    else if (value == "CASH")
    {
      data.transactiontype = OFX_CASH;
    }
    else if (value == "DIRECTDEP")
    {
      data.transactiontype = OFX_DIRECTDEP;
    }
    else if (value == "DIRECTDEBIT")
    {
      data.transactiontype = OFX_DIRECTDEBIT;
    }
    else if (value == "REPEATPMT")
    {
      data.transactiontype = OFX_REPEATPMT;
    }
    else if (value == "OTHER")
    {
      data.transactiontype = OFX_OTHER;
    }
    else
    {
      data.transactiontype_valid = false;
    }
  }//end TRANSTYPE
  else if (identifier == "TRNAMT")
  {
    data.amount = ofxamount_to_double(value);
    data.amount_valid = true;
    data.units = -data.amount;
    data.units_valid = true;
    data.unitprice = 1.00;
    data.unitprice_valid = true;
  }
  else if (identifier == "CHECKNUM")
  {
    STRNCPY(data.check_number, value);
    data.check_number_valid = true;
  }
  else if (identifier == "REFNUM")
  {
    STRNCPY(data.reference_number, value);
    data.reference_number_valid = true;
  }
  else if (identifier == "SIC")
  {
    data.standard_industrial_code = atoi(value.c_str());
    data.standard_industrial_code_valid = true;
  }
  else if ((identifier == "PAYEEID") || (identifier == "PAYEEID2"))
  {
    STRNCPY(data.payee_id, value);
    data.payee_id_valid = true;
  }
  else if (identifier == "NAME")
  {
    STRNCPY(data.name, value);
    data.name_valid = true;
  }
  else
  {
    /* Redirect unknown identifiers to the base class */
    OfxTransactionContainer::add_attribute(identifier, value);
  }
}//end OfxInvestmentTransactionContainer::add_attribute

