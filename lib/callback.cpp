/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/**@file ofx_callback.cpp
 * \brief Callback function handling 
 * \author  (C) 2003 by Benoit Grégoire (bock@step.polymtl.ca)
 */

#include <config.h>
#include "libofx.h"
#include "context.hh"

using namespace std;

/*
class OfxCallbackHandler
{
public:
  OfxCallbackHandler( OfxCallbackFunc param_func, void * param_user_data )
;
  OfxCallbackHandler( OfxCallbackFunc param_func): OfxCallbackHandler(param_func,NULL);

private:
  OfxCallbackFunc  func;
  void *	   func_data;
};
*/

CFCT void ofx_prep_cb(LibofxContextPtr libofx_context_param,
		      LibofxProcStatementCallback ofx_statement_cb,
		      LibofxProcAccountCallback ofx_account_cb,
		      LibofxProcTransactionCallback ofx_transaction_cb,
		      LibofxProcSecurityCallback ofx_security_cb,
		      LibofxProcStatusCallback ofx_status_cb
		      )
{
  LibofxContext *libofx_context = ( LibofxContext *)libofx_context_param;
  /* assign callbacks...these can be overridden in client code */
  libofx_context->cb_registry.ofx_statement_cb=ofx_statement_cb;
  libofx_context->cb_registry.ofx_transaction_cb=ofx_transaction_cb;
  libofx_context->cb_registry.ofx_security_cb=ofx_security_cb;
  libofx_context->cb_registry.ofx_account_cb=ofx_account_cb;
  libofx_context->cb_registry.ofx_status_cb=ofx_status_cb;
};
