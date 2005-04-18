/***************************************************************************
                          ofx_request_statement.hh 
                             -------------------
    copyright            : (C) 2005 by Ace Jones
    email                : acejones@users.sourceforge.net
***************************************************************************/
/**@file
 * \brief Declaration of libofx_request_statement to create an OFX file
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

#ifndef OFX_REQ_STATEMENT_H
#define OFX_REQ_STATEMENT_H

#include <string>
#include "libofx.h"
#include "ofx_request.hh"

using namespace std;

/**
 * \brief A statement request
 *
 * This is an entire OFX aggregate, with all subordinate aggregates needed to log onto
 * the OFX server of a single financial institution and download a statement for
 * a single account.
*/

class OfxStatementRequest: public OfxRequest
{
public:
  /**
   * Creates the request aggregate to obtain a statement from this @p fi for
   * this @p account, starting on this @p start date, ending today.
   *
   * @param fi The information needed to log on user into one financial
   *   institution
   * @param account The account for which a statement is desired
   * @param start The beginning time of the statement period desired
   */
  OfxStatementRequest( const OfxFiLogin& fi, const OfxAccountInfo& account, time_t from );

protected:
  /**
   * Creates a bank statement request aggregate, <BANKMSGSRQV1>, <STMTTRNRQ>
   * & <STMTRQ> for this account.  Should only be used if this account is a
   * BANK account.
   *
   * @return The request aggregate created
   */
  OfxAggregate BankStatementRequest(void) const;
  
  /**
   * Creates a credit card statement request aggregate, <CREDITCARDMSGSRQV1>,
   * <CCSTMTTRNRQ> & <CCSTMTRQ> for this account.  Should only be used if this
   * account is a CREDIT CARD account.
   *
   * @return The request aggregate created
   */
  OfxAggregate CreditCardStatementRequest(void) const;
  
  /**
   * Creates an investment statement request aggregate, <INSTMTMSGSRQV1>,
   * <INVSTMTTRNRQ> & <INVSTMTRQ> for this account.  Should only be used if this
   * account is an INVESTMENT account.
   *
   * @return The request aggregate created
   */
  OfxAggregate InvestmentStatementRequest(void) const;
  
private:
  OfxAccountInfo m_account;
  time_t m_date_from;
};

#endif // OFX_REQ_STATEMENT_H
