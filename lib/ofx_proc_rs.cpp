/***************************************************************************
         ofx_proc_rs.cpp 
                             -------------------
    copyright            : (C) 2002 by Benoit Grégoire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
 * \brief LibOFX internal object code.
 *
 * These objects will process the elements returned by ofx_sgml.cpp and add them to their data members.
 * \warning Object documentation is not yet done.
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
#include <stdlib.h>
#include <string>
#include "ParserEventGeneratorKit.h"
#include "messages.h"
#include "libofx.h"
#include "ofx_error_msg.h"
#include "ofx_utilities.h"
#include "ofx_proc_rs.h"

/***************************************************************************
 *                         OfxGenericContainer                             *
 ***************************************************************************/

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
};

/***************************************************************************
 *                         OfxDummyContainer                               *
 ***************************************************************************/

OfxDummyContainer::OfxDummyContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  type="DUMMY";
  message_out(INFO, "Created OfxDummyContainer to hold unsupported aggregate "+para_tag_identifier);
}
void OfxDummyContainer::add_attribute(const string identifier, const string value)
{
  message_out(DEBUG, "OfxDummyContainer for "+tag_identifier+" ignored a "+identifier+" ("+value+")");
}

/***************************************************************************
 *                         OfxPushUpContainer                              *
 ***************************************************************************/

OfxPushUpContainer::OfxPushUpContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  type="PUSHUP";
  message_out(DEBUG, "Created OfxPushUpContainer to hold aggregate "+tag_identifier);
}
void OfxPushUpContainer::add_attribute(const string identifier, const string value)
{
  //message_out(DEBUG, "OfxPushUpContainer for "+tag_identifier+" will push up a "+identifier+" ("+value+") to a "+ parentcontainer->type + " container");
  parentcontainer->add_attribute(identifier, value);
}

/***************************************************************************
 *                         OfxStatusContainer                              *
 ***************************************************************************/

OfxStatusContainer::OfxStatusContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  memset(&data,0,sizeof(data));
  type="STATUS";
  if (parentcontainer!=NULL){
    strncpy(data.ofx_element_name, parentcontainer->tag_identifier.c_str(), OFX_ELEMENT_NAME_LENGTH);
    data.ofx_element_name_valid=true;
  }
  
}
OfxStatusContainer::~OfxStatusContainer()
{
  ofx_proc_status (data);
}
void OfxStatusContainer::add_attribute(const string identifier, const string value)
{
  ErrorMsg error_msg;

  if( identifier=="CODE"){
    data.code=atoi(value.c_str());
    error_msg = find_error_msg(data.code);
    data.name = error_msg.name;//memory is already allocated
    data.description = error_msg.description;//memory is already allocated
    data.code_valid = true;
  }
  else if(identifier=="SEVERITY"){
    data.severity_valid = true;
    if(value=="INFO") {
      data.severity=OfxStatusData::INFO;
    }
    else if(value=="WARN") {
      data.severity=OfxStatusData::WARN;
    }
    else if(value=="ERROR") {
      data.severity=OfxStatusData::ERROR;
    }
    else{
      message_out(ERROR, "WRITEME: Unknown severity "+value+" inside a "+type+" container");
      data.severity_valid = false;
    }
  }
  else if((identifier=="MESSAGE")||(identifier=="MESSAGE2")){
    data.server_message=new char[value.length()];
    strcpy(data.server_message,value.c_str());
    data.server_message_valid=true;
  }
  else{
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}

/***************************************************************************
 *                        OfxSecurityContainer                             *
 ***************************************************************************/

  void add_attribute(const string identifier, const string value);

OfxSecurityContainer::OfxSecurityContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  OfxGenericContainer * tmp_parentcontainer=parentcontainer;

  memset(&data,0,sizeof(data));
  type="SECURITY";
}
OfxSecurityContainer::~OfxSecurityContainer()
{
 // parent_statement->add_security(data);
  ofx_proc_security(data);
}
void OfxSecurityContainer::add_attribute(const string identifier, const string value)
{
  if(identifier=="UNIQUEID"){
    strncpy(data.unique_id,value.c_str(), sizeof(data.unique_id));
    data.unique_id_valid = true;
  }
  else if(identifier=="UNIQUEIDTYPE"){
    strncpy(data.unique_id_type,value.c_str(), sizeof(data.unique_id_type));
    data.unique_id_type_valid = true;
  }
  else if(identifier=="SECNAME"){
    strncpy(data.secname,value.c_str(), sizeof(data.secname));
    data.secname_valid = true;
  }
  else if(identifier=="TICKER"){
    strncpy(data.ticker,value.c_str(), sizeof(data.ticker));
    data.ticker_valid = true;
  }
  else if(identifier=="UNITPRICE"){
    data.unitprice=ofxamount_to_double(value);
    data.unitprice_valid = true;
  }
  else if(identifier=="DTASOF"){
    data.date_unitprice = ofxdate_to_time_t(value);
    data.date_unitprice_valid = true;
  }
  else if(identifier=="CURDEF"){
    strncpy(data.currency,value.c_str(),OFX_CURRENCY_LENGTH);
    data.currency_valid=true;
  }
  else if(identifier=="MEMO" || identifier=="MEMO2"){
    strncpy(data.memo,value.c_str(), sizeof(data.memo));
    data.memo_valid = true;
 }
  else{
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}

/***************************************************************************
 *                      OfxTransactionContainer                            *
 ***************************************************************************/

OfxTransactionContainer::OfxTransactionContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  OfxGenericContainer * tmp_parentcontainer=parentcontainer;

  memset(&data,0,sizeof(data));
  type="TRANSACTION";
  /* Find the parent statement container*/
  while(tmp_parentcontainer!=NULL&&tmp_parentcontainer->type!="STATEMENT")
    {
      tmp_parentcontainer=parentcontainer->parentcontainer;
    }  
  if (tmp_parentcontainer!=NULL){
    parent_statement=(OfxStatementContainer*)tmp_parentcontainer;
  }
  else{
    parent_statement=NULL;
    message_out(ERROR,"Unable to find the enclosing statement container this transaction");
  }
  if (parent_statement!=NULL&&parent_statement->data.account_id_valid==true){
    strncpy(data.account_id,parent_statement->data.account_id,OFX_ACCOUNT_ID_LENGTH);
    data.account_id_valid = true;
  }
}
OfxTransactionContainer::~OfxTransactionContainer()
{
  parent_statement->add_transaction(data);
}
void OfxTransactionContainer::add_attribute(const string identifier, const string value)
{

  if(identifier=="DTPOSTED"){
    data.date_posted = ofxdate_to_time_t(value);
    data.date_posted_valid = true;
  }
  else if(identifier=="DTUSER"){
    data.date_initiated = ofxdate_to_time_t(value);
    data.date_initiated_valid = true;
  }
  else if(identifier=="DTAVAIL"){
    data.date_funds_available = ofxdate_to_time_t(value);
    data.date_funds_available_valid = true;
  }
  else if(identifier=="FITID"){
    strncpy(data.fi_id,value.c_str(), sizeof(data.fi_id));
    data.fi_id_valid = true;
  }
  else if(identifier=="CORRECTFITID"){
    strncpy(data.fi_id_corrected,value.c_str(), sizeof(data.fi_id));
    data.fi_id_corrected_valid = true;
  }
  else if(identifier=="CORRECTACTION"){
    data.fi_id_correction_action_valid=true;
    if(value=="REPLACE"){
      data.fi_id_correction_action=data.REPLACE;
    }
    else if(value=="DELETE"){
      data.fi_id_correction_action=data.DELETE;
    }
    else{
      data.fi_id_correction_action_valid=false;
    }
  }
  else if((identifier=="SRVRTID")||(identifier=="SRVRTID2")){
    strncpy(data.server_transaction_id,value.c_str(), sizeof(data.server_transaction_id));
    data.server_transaction_id_valid = true;
  }
  else if(identifier=="MEMO" || identifier=="MEMO2"){
    strncpy(data.memo,value.c_str(), sizeof(data.memo));
    data.memo_valid = true;
  }
  else{
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}// end OfxTransactionContainer::add_attribute()


OfxBankTransactionContainer::OfxBankTransactionContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier): 
  OfxTransactionContainer(para_parentcontainer, para_tag_identifier)
{
  ;
}
void OfxBankTransactionContainer::add_attribute(const string identifier, const string value)
{
  if( identifier=="TRNTYPE"){
    data.transactiontype_valid=true;
    if(value=="CREDIT"){
      data.transactiontype=data.OFX_CREDIT;
    }
    else if(value=="DEBIT"){
      data.transactiontype=data.OFX_DEBIT;
    }
    else if(value=="INT"){
      data.transactiontype=data.OFX_INT;
    }
    else if(value=="DIV"){
      data.transactiontype=data.OFX_DIV;
    }
    else if(value=="FEE"){
      data.transactiontype=data.OFX_FEE;
    }
    else if(value=="SRVCHG"){
      data.transactiontype=data.OFX_SRVCHG;
    }
    else if(value=="DEP"){
      data.transactiontype=data.OFX_DEP;
    }
    else if(value=="ATM"){
      data.transactiontype=data.OFX_ATM;
    }
    else if(value=="POS"){
      data.transactiontype=data.OFX_POS;
    }
    else if(value=="XFER"){
      data.transactiontype=data.OFX_XFER;
    }
    else if(value=="CHECK"){
      data.transactiontype=data.OFX_CHECK;
    }
    else if(value=="PAYMENT"){
      data.transactiontype=data.OFX_PAYMENT;
    }
    else if(value=="CASH"){
      data.transactiontype=data.OFX_CASH;
    }
    else if(value=="DIRECTDEP"){
      data.transactiontype=data.OFX_DIRECTDEP;
    }
    else if(value=="DIRECTDEBIT"){
      data.transactiontype=data.OFX_DIRECTDEBIT;
    }
    else if(value=="REPEATPMT"){
      data.transactiontype=data.OFX_REPEATPMT;
    }
    else if(value=="OTHER"){
      data.transactiontype=data.OFX_OTHER;
    }
    else {
      data.transactiontype_valid=false;
    }
  }//end TRANSTYPE
  else if(identifier=="TRNAMT"){
    data.amount=ofxamount_to_double(value);
    data.amount_valid=true;
    data.units = -data.amount;
    data.units_valid=true;
    data.unitprice = 1,00;
    data.unitprice_valid=true;
  }
  else if(identifier=="CHECKNUM"){
    strncpy(data.check_number,value.c_str(), sizeof(data.check_number));
    data.check_number_valid = true;
  }
  else if(identifier=="REFNUM"){
    strncpy(data.reference_number,value.c_str(), sizeof(data.reference_number));
    data.reference_number_valid = true;
  }
  else if(identifier=="SIC"){
    data.standard_industrial_code = atoi(value.c_str());
    data.standard_industrial_code_valid = true;
  }
  else if((identifier=="PAYEEID")||(identifier=="PAYEEID2")){
    strncpy(data.payee_id,value.c_str(), sizeof(data.payee_id));
    data.payee_id_valid = true;
  }
  else if(identifier=="NAME"){
    strncpy(data.name,value.c_str(), sizeof(data.name));
    data.name_valid = true;
  }
  else{
    /* Redirect unknown identifiers to base class */
    OfxTransactionContainer::add_attribute(identifier, value);
  }
}//end OfxBankTransactionContainer::add_attribute
 
OfxInvestmentTransactionContainer::OfxInvestmentTransactionContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxTransactionContainer(para_parentcontainer, para_tag_identifier)
{
  type="INVESTMENT";
  data.transactiontype=data.OFX_OTHER;
  data.transactiontype_valid=true;
  
  data.invtranstype_valid=true; 
  strncpy(data.invtranstype,para_tag_identifier.c_str(),sizeof(data.invtranstype));
}

void OfxInvestmentTransactionContainer::add_attribute(const string identifier, const string value)
{
  if(identifier=="UNIQUEID"){
    strncpy(data.unique_id,value.c_str(), sizeof(data.unique_id));
    data.unique_id_valid = true;
  }
  else if(identifier=="UNIQUEIDTYPE"){
    strncpy(data.unique_id_type,value.c_str(), sizeof(data.unique_id_type));
    data.unique_id_type_valid = true;
  }
  else if(identifier=="UNITS"){
    data.units=ofxamount_to_double(value);
    data.units_valid = true;
  }
  else if(identifier=="UNITPRICE"){
    data.unitprice=ofxamount_to_double(value);
    data.unitprice_valid = true;
  }
  else if(identifier=="TOTAL"){
    data.amount=ofxamount_to_double(value);
    data.amount_valid = true;
  }
  else if(identifier=="DTSETTLE"){
    data.date_posted = ofxdate_to_time_t(value);
    data.date_posted_valid = true;
  }
  else if(identifier=="DTTRADE"){
    data.date_initiated = ofxdate_to_time_t(value);
    data.date_initiated_valid = true;
  }
  else{
    /* Redirect unknown identifiers to the base class */
    OfxTransactionContainer::add_attribute(identifier, value);
  }
}//end OfxInvestmentTransactionContainer::add_attribute


/***************************************************************************
 *                      OfxAccountContainer                                *
 ***************************************************************************/

OfxAccountContainer::OfxAccountContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  memset(&data,0,sizeof(data));
  type="ACCOUNT";
  strcpy(bankid,"");
  strcpy(branchid,"");
  strcpy(acctid,"");
  strcpy(acctkey,"");
  strcpy(brokerid,"");
  if(para_tag_identifier== "CCACCTFROM")
    {
      /*Set the type for a creditcard account.  Bank account specific 
	OFX elements will set this attribute elsewhere */
      data.account_type = data.OFX_CREDITCARD;
      data.account_type_valid = true;
    }
  if(para_tag_identifier== "INVACCTFROM")
    {
      /*Set the type for an investment account.  Bank account specific 
	OFX elements will set this attribute elsewhere */
      data.account_type = data.OFX_INVESTMENT;
      data.account_type_valid = true;
    }
  if (parentcontainer!=NULL&&((OfxStatementContainer*)parentcontainer)->data.currency_valid==true){
    strncpy(data.currency,((OfxStatementContainer*)parentcontainer)->data.currency,OFX_CURRENCY_LENGTH); /* In ISO-4217 format */
    data.currency_valid=true;
  }
}
OfxAccountContainer::~OfxAccountContainer()
{
  gen_account_id ();
  if (parentcontainer->type == "STATEMENT")
    {
      ((OfxStatementContainer*)parentcontainer)->add_account(data);
    }
  ofx_proc_account (data);
}

void OfxAccountContainer::add_attribute(const string identifier, const string value)
{
  if( identifier=="BANKID"){
    strncpy(bankid,value.c_str(),OFX_BANKID_LENGTH);
  }
  else if( identifier=="BRANCHID"){
    strncpy(branchid,value.c_str(),OFX_BRANCHID_LENGTH);
  }
  else if( identifier=="ACCTID"){
    strncpy(acctid,value.c_str(),OFX_ACCTID_LENGTH);
  }
  else if( identifier=="ACCTKEY"){
    strncpy(acctkey,value.c_str(),OFX_ACCTKEY_LENGTH);
  }
  else if( identifier=="BROKERID"){ /* For investment accounts */
    strncpy(brokerid,value.c_str(),OFX_BROKERID_LENGTH);
  }
  else if((identifier=="ACCTTYPE")||(identifier=="ACCTTYPE2")){
    data.account_type_valid=true;
    if(value=="CHECKING"){
      data.account_type=data.OFX_CHECKING;
    }
    else if(value=="SAVINGS"){
      data.account_type=data.OFX_SAVINGS;
    }
    else if(value=="MONEYMRKT"){
      data.account_type=data.OFX_MONEYMRKT;
    }
    else if(value=="CREDITLINE"){
      data.account_type=data.OFX_CREDITLINE;
    }
    else if(value=="CMA"){
      data.account_type=data.OFX_CMA;
    }
    /* AccountType CREDITCARD is set at object creation, if appropriate */
    else{
      data.account_type_valid=false;
    }
  }
  else{
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}//end OfxAccountContainer::add_attribute()

void OfxAccountContainer::gen_account_id(void)
{
  if(data.account_type==OfxAccountData::OFX_CREDITCARD){
    strncat(data.account_id,acctid,OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));
    strncat(data.account_id," ",OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));
    strncat(data.account_id,acctkey,OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));

    strncat(data.account_name,"Credit card ",OFX_ACCOUNT_NAME_LENGTH-strlen(data.account_name));
    strncat(data.account_name,acctid,OFX_ACCOUNT_NAME_LENGTH-strlen(data.account_name));
  }
  else if(data.account_type==OfxAccountData::OFX_INVESTMENT){
    strncat(data.account_id,brokerid,OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));
    strncat(data.account_id," ",OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));
    strncat(data.account_id,acctid,OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));

    strncat(data.account_name,"Investment account ",OFX_ACCOUNT_NAME_LENGTH-strlen(data.account_name));
    strncat(data.account_name,acctid,OFX_ACCOUNT_NAME_LENGTH-strlen(data.account_name));
    strncat(data.account_name," at broker ",OFX_ACCOUNT_NAME_LENGTH-strlen(data.account_name));
    strncat(data.account_name,brokerid,OFX_ACCOUNT_NAME_LENGTH-strlen(data.account_name));
  }
  else{
    strncat(data.account_id,bankid,OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));
    strncat(data.account_id," ",OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));
    strncat(data.account_id,branchid,OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));
    strncat(data.account_id," ",OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));
    strncat(data.account_id,acctid,OFX_ACCOUNT_ID_LENGTH-strlen(data.account_id));

    strncat(data.account_name,"Bank account ",OFX_ACCOUNT_NAME_LENGTH-strlen(data.account_name));
    strncat(data.account_name,acctid,OFX_ACCOUNT_NAME_LENGTH-strlen(data.account_name));
  }
  if(strlen(data.account_id)>=0){
    data.account_id_valid=true;
  }
}//end OfxAccountContainer::gen_account_id()

/***************************************************************************
 *                    OfxStatementContainer                                *
 ***************************************************************************/

OfxStatementContainer::OfxStatementContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  memset(&data,0,sizeof(data));
  type="STATEMENT";
}
OfxStatementContainer::~OfxStatementContainer()
{
  ofx_proc_statement(data);
  while(transaction_queue.empty()!=true)
    {
      ofx_proc_transaction(transaction_queue.front());
      transaction_queue.pop();
    }
}
void OfxStatementContainer::add_attribute(const string identifier, const string value)
{
  if(identifier=="CURDEF"){
    strncpy(data.currency,value.c_str(),OFX_CURRENCY_LENGTH);
    data.currency_valid=true;
  }
  else if(identifier=="MKTGINFO"){
    strncpy(data.marketing_info,value.c_str(),OFX_MARKETING_INFO_LENGTH);
    data.marketing_info_valid=true;
  }
  else if(identifier=="DTSTART"){
    data.date_start = ofxdate_to_time_t(value);
    data.date_start_valid = true;
  }
  else if(identifier=="DTEND"){
    data.date_end = ofxdate_to_time_t(value);
    data.date_end_valid = true;
  }
  else{
    OfxGenericContainer::add_attribute(identifier, value);
  }
}//end OfxStatementContainer::add_attribute()

void OfxStatementContainer::add_balance(OfxBalanceContainer* ptr_balance_container)
{
  if(ptr_balance_container->tag_identifier=="LEDGERBAL"){
    data.ledger_balance=ptr_balance_container->amount;
    data.ledger_balance_valid=ptr_balance_container->amount_valid;
  }
  else if(ptr_balance_container->tag_identifier=="AVAILBAL"){
    data.available_balance=ptr_balance_container->amount;
    data.available_balance_valid=ptr_balance_container->amount_valid;
  }
  else{
    message_out(ERROR,"OfxStatementContainer::add_balance(): the balance has unknown tag_identifier: "+ptr_balance_container->tag_identifier);
  }
}

void OfxStatementContainer::add_account(OfxAccountData const account_data)
{
  if(account_data.account_id_valid==true)
    {
      data.account = account_data;
      strncpy(data.account_id,account_data.account_id,OFX_ACCOUNT_ID_LENGTH);
      data.account_id_valid = true;
    }
}
void OfxStatementContainer::add_transaction(const OfxTransactionData transaction_data)
{
  transaction_queue.push(transaction_data);
}
void OfxStatementContainer::add_security(const OfxSecurityData security_data)
{
  ofx_proc_security(security_data);
}

/***************************************************************************
 * OfxBalanceContainer  (does not directly abstract a object in libofx.h)  *
 ***************************************************************************/

OfxBalanceContainer::OfxBalanceContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  amount_valid=false;
  date_valid=false;
  type="BALANCE";
}

OfxBalanceContainer::~OfxBalanceContainer()
{
  if (parentcontainer->type == "STATEMENT")
    {
      ((OfxStatementContainer*)parentcontainer)->add_balance(this);
    }
  else
    {
      message_out (ERROR,"I completed a " + type + " element, but I havent found a suitable parent to save it");
    }
}
void OfxBalanceContainer::add_attribute(const string identifier, const string value)
{
  if(identifier=="BALAMT"){
    amount=ofxamount_to_double(value);
    amount_valid=true;
  }
  else if(identifier=="DTASOF"){
    date = ofxdate_to_time_t(value);
    date_valid = true;
  }
  else{
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }  
}
