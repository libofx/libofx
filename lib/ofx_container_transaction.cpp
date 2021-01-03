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
    ASSIGN(data.date_posted, ofxdate_to_time_t(value));
  }
  else if (identifier == "DTUSER")
  {
    ASSIGN(data.date_initiated, ofxdate_to_time_t(value));
  }
  else if (identifier == "DTAVAIL")
  {
    ASSIGN(data.date_funds_available, ofxdate_to_time_t(value));
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
    ASSIGN(data.amounts_are_foreign_currency, false);
  }
  else if (identifier == "ORIGCURRENCY")
  {
    ASSIGN(data.amounts_are_foreign_currency, true);
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
    ASSIGN(data.amount, ofxamount_to_double(value));
    ASSIGN(data.units, -data.amount);
    ASSIGN(data.unitprice, 1.00);
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
    ASSIGN(data.standard_industrial_code, atoi(value.c_str()));
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
  ASSIGN(data.transactiontype, OFX_OTHER);

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
    ASSIGN(data.units, ofxamount_to_double(value));
  }
  else if (identifier == "UNITPRICE")
  {
    ASSIGN(data.unitprice, ofxamount_to_double(value));
  }
  else if (identifier == "MKTVAL")
  {
    ASSIGN(data.market_value, ofxamount_to_double(value));
  }
  else if (identifier == "TOTAL")
  {
    ASSIGN(data.amount, ofxamount_to_double(value));
  }
  else if (identifier == "CURRATE")
  {
    ASSIGN(data.currency_ratio, ofxamount_to_double(value));
  }
  else if (identifier == "CURSYM")
  {
    STRNCPY(data.currency, value);
    data.currency_valid = true;
  }
  else if (identifier == "DTSETTLE")
  {
    ASSIGN(data.date_posted, ofxdate_to_time_t(value));
  }
  else if (identifier == "DTTRADE")
  {
    ASSIGN(data.date_initiated, ofxdate_to_time_t(value));
  }
  else if (identifier == "COMMISSION")
  {
    ASSIGN(data.commission, ofxamount_to_double(value));
  }
  else if (identifier == "FEES")
  {
    ASSIGN(data.fees, ofxamount_to_double(value));
  }
  else if (identifier == "OLDUNITS")
  {
    ASSIGN(data.oldunits, ofxamount_to_double(value));
  }
  else if (identifier == "NEWUNITS")
  {
    ASSIGN(data.newunits, ofxamount_to_double(value));
  }
  else if (identifier == "ACCRDINT")
  {
    ASSIGN(data.accrued_interest, ofxamount_to_double(value));
  }
  else if (identifier == "AVGCOSTBASIS")
  {
    ASSIGN(data.avg_cost_basis, ofxamount_to_double(value));
  }
  else if (identifier == "BUYTYPE" || identifier == "OPTBUYTYPE")
  {
    if (value == "BUY")
    {
      ASSIGN(data.buy_type, data.OFX_BUY_TYPE_BUY);
    }
    else if (value == "BUYTOCOVER")
    {
      ASSIGN(data.buy_type, data.OFX_BUY_TYPE_BUYTOCOVER);
    }
    else if (value == "BUYTOOPEN")
    {
      ASSIGN(data.buy_type, data.OFX_BUY_TYPE_BUYTOOPEN);
    }
    else if (value == "BUYTOCLOSE")
    {
      ASSIGN(data.buy_type, data.OFX_BUY_TYPE_BUYTOCLOSE);
    }
  }
  else if (identifier == "DENOMINATOR")
  {
    ASSIGN(data.denominator, ofxamount_to_double(value));
  }
  else if (identifier == "DTPAYROLL")
  {
    ASSIGN(data.date_payroll, ofxdate_to_time_t(value));
  }
  else if (identifier == "DTPURCHASE")
  {
    ASSIGN(data.date_purchase, ofxdate_to_time_t(value));
  }
  else if (identifier == "GAIN")
  {
    ASSIGN(data.gain, ofxamount_to_double(value));
  }
  else if (identifier == "FRACCASH")
  {
    ASSIGN(data.cash_for_fractional, ofxamount_to_double(value));
  }
  else if (identifier == "INCOMETYPE")
  {
    if (value == "CGLONG")
    {
      ASSIGN(data.income_type, data.OFX_CGLONG);
    }
    else if (value == "CGSHORT")
    {
      ASSIGN(data.income_type, data.OFX_CGSHORT);
    }
    else if (value == "DIV")
    {
      ASSIGN(data.income_type, data.OFX_DIVIDEND);
    }
    else if (value == "INTEREST")
    {
      ASSIGN(data.income_type, data.OFX_INTEREST);
    }
    else if (value == "MISC")
    {
      ASSIGN(data.income_type, data.OFX_MISC);
    }
  }
  else if (identifier == "INV401KSOURCE")
  {
    if (value == "PRETAX")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_SOURCE_PRETAX);
    }
    else if (value == "AFTERTAX")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_SOURCE_AFTERTAX);
    }
    else if (value == "MATCH")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_SOURCE_MATCH);
    }
    else if (value == "PROFITSHARING")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_SOURCE_PROFITSHARING);
    }
    else if (value == "ROLLOVER")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_SOURCE_ROLLOVER);
    }
    else if (value == "OTHERVEST")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_SOURCE_OTHERVEST);
    }
    else if (value == "OTHERNONVEST")
    {
      ASSIGN(data.inv_401k_source, data.OFX_401K_SOURCE_OTHERNONVEST);
    }
  }
  else if (identifier == "LOAD")
  {
    ASSIGN(data.load, ofxamount_to_double(value));
  }
  else if (identifier == "LOANID")
  {
    STRNCPY(data.loan_id, value);
    data.loan_id_valid = true;
  }
  else if (identifier == "LOANINTEREST")
  {
    ASSIGN(data.loan_interest, ofxamount_to_double(value));
  }
  else if (identifier == "LOANPRINCIPAL")
  {
    ASSIGN(data.loan_principal, ofxamount_to_double(value));
  }
  else if (identifier == "MARKDOWN")
  {
    ASSIGN(data.markdown, ofxamount_to_double(value));
  }
  else if (identifier == "MARKUP")
  {
    ASSIGN(data.markup, ofxamount_to_double(value));
  }
  else if (identifier == "NUMERATOR")
  {
    ASSIGN(data.numerator, ofxamount_to_double(value));
  }
  else if (identifier == "OPTACTION")
  {
    if (value == "EXERCISE")
    {
      ASSIGN(data.opt_action, data.OFX_OPTACTION_EXERCISE);
    }
    else if (value == "ASSIGN")
    {
      ASSIGN(data.opt_action, data.OFX_OPTACTION_ASSIGN);
    }
    else if (value == "EXPIRE")
    {
      ASSIGN(data.opt_action, data.OFX_OPTACTION_EXPIRE);
    }
  }
  else if (identifier == "PENALTY")
  {
    ASSIGN(data.penalty, ofxamount_to_double(value));
  }
  else if (identifier == "POSTYPE")
  {
    if (value == "LONG")
    {
      ASSIGN(data.pos_type, data.OFX_POSTYPE_LONG);
    }
    else if (value == "SHORT")
    {
      ASSIGN(data.pos_type, data.OFX_POSTYPE_SHORT);
    }
  }
  else if (identifier == "PRIORYEARCONTRIB")
  {
    if (value == "Y")
    {
      ASSIGN(data.prior_year_contrib, true);
    }
    else if (value == "N")
    {
      ASSIGN(data.prior_year_contrib, false);
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
      ASSIGN(data.related_type, data.OFX_RELTYPE_SPREAD);
    }
    else if (value == "STRADDLE")
    {
      ASSIGN(data.related_type, data.OFX_RELTYPE_STRADDLE);
    }
    else if (value == "NONE")
    {
      ASSIGN(data.related_type, data.OFX_RELTYPE_NONE);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.related_type, data.OFX_RELTYPE_OTHER);
    }
  }
  else if (identifier == "SECURED")
  {
    if (value == "NAKED")
    {
      ASSIGN(data.option_secured, data.OFX_SECURED_NAKED);
    }
    else if (value == "COVERED")
    {
      ASSIGN(data.option_secured, data.OFX_SECURED_COVERED);
    }
  }
  else if (identifier == "SELLREASON")
  {
    if (value == "CALL")
    {
      ASSIGN(data.sell_reason, data.OFX_SELLREASON_CALL);
    }
    else if (value == "SELL")
    {
      ASSIGN(data.sell_reason, data.OFX_SELLREASON_SELL);
    }
    else if (value == "MATURITY")
    {
      ASSIGN(data.sell_reason, data.OFX_SELLREASON_MATURITY);
    }
  }
  else if (identifier == "SELLTYPE" || identifier == "OPTSELLTYPE")
  {
    if (value == "SELL")
    {
      ASSIGN(data.sell_type, data.OFX_SELL_TYPE_SELL);
    }
    else if (value == "SELLSHORT")
    {
      ASSIGN(data.sell_type, data.OFX_SELL_TYPE_SELLSHORT);
    }
    else if (value == "SELLTOOPEN")
    {
      ASSIGN(data.sell_type, data.OFX_SELL_TYPE_SELLTOOPEN);
    }
    else if (value == "SELLTOCLOSE")
    {
      ASSIGN(data.sell_type, data.OFX_SELL_TYPE_SELLTOCLOSE);
    }
  }
  else if (identifier == "SHPERCTRCT")
  {
    ASSIGN(data.shares_per_cont, ofxamount_to_double(value));
  }
  else if (identifier == "STATEWITHHOLDING")
  {
    ASSIGN(data.state_withholding, ofxamount_to_double(value));
  }
  else if (identifier == "SUBACCTFROM")
  {
    if (value == "CASH")
    {
      ASSIGN(data.subacct_from, data.OFX_SUBACCT_CASH);
    }
    else if (value == "MARGIN")
    {
      ASSIGN(data.subacct_from, data.OFX_SUBACCT_MARGIN);
    }
    else if (value == "SHORT")
    {
      ASSIGN(data.subacct_from, data.OFX_SUBACCT_SHORT);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.subacct_from, data.OFX_SUBACCT_OTHER);
    }
  }
  else if (identifier == "SUBACCTFUND")
  {
    if (value == "CASH")
    {
      ASSIGN(data.subacct_funding, data.OFX_SUBACCT_CASH);
    }
    else if (value == "MARGIN")
    {
      ASSIGN(data.subacct_funding, data.OFX_SUBACCT_MARGIN);
    }
    else if (value == "SHORT")
    {
      ASSIGN(data.subacct_funding, data.OFX_SUBACCT_SHORT);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.subacct_funding, data.OFX_SUBACCT_OTHER);
    }
  }
  else if (identifier == "SUBACCTSEC")
  {
    if (value == "CASH")
    {
      ASSIGN(data.subacct_security, data.OFX_SUBACCT_CASH);
    }
    else if (value == "MARGIN")
    {
      ASSIGN(data.subacct_security, data.OFX_SUBACCT_MARGIN);
    }
    else if (value == "SHORT")
    {
      ASSIGN(data.subacct_security, data.OFX_SUBACCT_SHORT);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.subacct_security, data.OFX_SUBACCT_OTHER);
    }
  }
  else if (identifier == "SUBACCTTO")
  {
    if (value == "CASH")
    {
      ASSIGN(data.subacct_to, data.OFX_SUBACCT_CASH);
    }
    else if (value == "MARGIN")
    {
      ASSIGN(data.subacct_to, data.OFX_SUBACCT_MARGIN);
    }
    else if (value == "SHORT")
    {
      ASSIGN(data.subacct_to, data.OFX_SUBACCT_SHORT);
    }
    else if (value == "OTHER")
    {
      ASSIGN(data.subacct_to, data.OFX_SUBACCT_OTHER);
    }
  }
  else if (identifier == "TAXES")
  {
    ASSIGN(data.taxes, ofxamount_to_double(value));
  }
  else if (identifier == "TAXEXEMPT")
  {
    if (value == "Y")
    {
      ASSIGN(data.tax_exempt, true);
    }
    else if (value == "N")
    {
      ASSIGN(data.tax_exempt, false);
    }
  }
  else if (identifier == "TFERACTION")
  {
    if (value == "IN")
    {
      ASSIGN(data.transfer_action, data.OFX_TFERACTION_IN);
    }
    else if (value == "OUT")
    {
      ASSIGN(data.transfer_action, data.OFX_TFERACTION_OUT);
    }
  }
  else if (identifier == "UNITTYPE")
  {
    if (value == "SHARES")
    {
      ASSIGN(data.unit_type, data.OFX_UNITTYPE_SHARES);
    }
    else if (value == "CURRENCY")
    {
      ASSIGN(data.unit_type, data.OFX_UNITTYPE_CURRENCY);
    }
  }
  else if (identifier == "WITHHOLDING")
  {
    ASSIGN(data.withholding, ofxamount_to_double(value));
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
    ASSIGN(data.amount, ofxamount_to_double(value));
    ASSIGN(data.units, -data.amount);
    ASSIGN(data.unitprice, 1.00);
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
    ASSIGN(data.standard_industrial_code, atoi(value.c_str()));
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

