/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/**@file ofx_callback.hh
 * \brief Callback function handling 
 * \author  (C) 2003 by Benoit Grégoire (bock@step.polymtl.ca)
 */
#ifndef OFX_CALLBACK_H
#define OFX_CALLBACK_H

using namespace std;

struct OfxCallbackRegistry{
  void * statement_data;
  int (*ofx_statement_cb)( const struct OfxStatementData data, void *);
  void * account_data;
  int (*ofx_account_cb)( const struct OfxAccountData data, void *);
  void * transaction_data;
  int (*ofx_transaction_cb)( const struct OfxTransactionData data, void *);
  void * security_data;
  int (*ofx_security_cb)( const struct OfxSecurityData data, void *);
  void * status_data;
  int (*ofx_status_cb)(const struct OfxStatusData data, void *);
};

#endif
