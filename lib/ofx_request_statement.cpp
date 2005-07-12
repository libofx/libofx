/***************************************************************************
         ofx_request_statement.cpp 
                             -------------------
    copyright            : (C) 2005 by Ace Jones
    email                : acejones@users.sourceforge.net
***************************************************************************/
/**@file
 * \brief Implementation of libofx_request_statement to create an OFX file
 *   containing a request for a statement.
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
#include "libofx.h"
#include "ofx_utilities.hh"
#include "ofx_request_statement.hh"

using namespace std;

char* libofx_request_statement( const OfxFiLogin* login, const OfxAccountInfo* account, time_t date_from )
{
  OfxStatementRequest strq( *login, *account, date_from );
  string request = OfxHeader() + strq.Output();

  unsigned size = request.size();
  char* result = (char*)malloc(size + 1);
  request.copy(result,size);
  result[size] = 0;
  
  return result;
}

OfxStatementRequest::OfxStatementRequest( const OfxFiLogin& fi, const OfxAccountInfo& account, time_t from ):
  OfxRequest(fi),
  m_account(account),
  m_date_from(from)
{
  Add( SignOnRequest() );

  if ( account.type == OFX_CREDITCARD_ACCOUNT )
    Add(CreditCardStatementRequest());
  else if ( account.type == OFX_INVEST_ACCOUNT )
    Add(InvestmentStatementRequest());
  else
    Add(BankStatementRequest());
}

OfxAggregate OfxStatementRequest::BankStatementRequest(void) const
{
  OfxAggregate bankacctfromTag("BANKACCTFROM");
  bankacctfromTag.Add( "BANKID", m_account.bankid );
  bankacctfromTag.Add( "ACCTID", m_account.accountid );
  bankacctfromTag.Add( "ACCTTYPE", "CHECKING" );
  // FIXME "CHECKING" should not be hard-coded

  OfxAggregate inctranTag("INCTRAN");
  inctranTag.Add( "DTSTART", time_t_to_ofxdate( m_date_from ) );
  inctranTag.Add( "INCLUDE","Y" );
  
  OfxAggregate stmtrqTag("STMTRQ");
  stmtrqTag.Add( bankacctfromTag );
  stmtrqTag.Add( inctranTag );

  return RequestMessage("BANK","STMT", stmtrqTag);
}

OfxAggregate OfxStatementRequest::CreditCardStatementRequest(void) const
{/*
  QString dtstart_string = _dtstart.toString(Qt::ISODate).remove(QRegExp("[^0-9]"));

  return message("CREDITCARD","CCSTMT",Tag("CCSTMTRQ")
    .subtag(Tag("CCACCTFROM").element("ACCTID",accountnum()))
    .subtag(Tag("INCTRAN").element("DTSTART",dtstart_string).element("INCLUDE","Y")));
}
*/
  OfxAggregate ccacctfromTag("CCACCTFROM");
  ccacctfromTag.Add( "ACCTID", m_account.accountid );

  OfxAggregate inctranTag("INCTRAN");
  inctranTag.Add( "DTSTART", time_t_to_ofxdate( m_date_from ) );
  inctranTag.Add( "INCLUDE","Y" );
  
  OfxAggregate ccstmtrqTag("CCSTMTRQ");
  ccstmtrqTag.Add( ccacctfromTag );
  ccstmtrqTag.Add( inctranTag );

  return RequestMessage("CREDITCARD","CCSTMT", ccstmtrqTag);
}

OfxAggregate OfxStatementRequest::InvestmentStatementRequest(void) const
{
  OfxAggregate invacctfromTag("INVACCTFROM");
  
  invacctfromTag.Add( "BROKERID", m_account.brokerid );
  invacctfromTag.Add( "ACCTID", m_account.accountid );

  OfxAggregate inctranTag("INCTRAN");
  inctranTag.Add( "DTSTART", time_t_to_ofxdate( m_date_from ) );
  inctranTag.Add( "INCLUDE","Y" );

  OfxAggregate incposTag("INCPOS");
  incposTag.Add( "DTASOF", time_t_to_ofxdatetime( time(NULL) ) );
  incposTag.Add( "INCLUDE","Y" );
    
  OfxAggregate invstmtrqTag("INVSTMTRQ");
  invstmtrqTag.Add( invacctfromTag );
  invstmtrqTag.Add( inctranTag );
  invstmtrqTag.Add( "INCOO","Y" );
  invstmtrqTag.Add( incposTag );  
  invstmtrqTag.Add( "INCBAL","Y" );

  return RequestMessage("INVSTMT","INVSTMT", invstmtrqTag);
}

// vim:cin:si:ai:et:ts=2:sw=2:

