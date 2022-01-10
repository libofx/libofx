/***************************************************************************
         ofx_container_statement.cpp
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : benoitg@coeus.ca
***************************************************************************/
/**@file
 * \brief Implementation of OfxStatementContainer for bank statements,
credit cart statements, etc.
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
 *                    OfxStatementContainer                                *
 ***************************************************************************/

OfxStatementContainer::OfxStatementContainer(LibofxContext *p_libofx_context, OfxGenericContainer *para_parentcontainer, std::string para_tag_identifier):
  OfxGenericContainer(p_libofx_context, para_parentcontainer, para_tag_identifier)
{
  memset(&data, 0, sizeof(data));
  type = "STATEMENT";
}
OfxStatementContainer::~OfxStatementContainer()
{
  /*  while(transaction_queue.empty()!=true)
      {
        ofx_proc_transaction_cb(transaction_queue.front());
        transaction_queue.pop();
      }*/
}
void OfxStatementContainer::add_attribute(const std::string identifier, const std::string value)
{
  if (identifier == "CURDEF")
  {
    STRNCPY(data.currency, value);
    data.currency_valid = !value.empty();
  }
  else if (identifier == "MKTGINFO")
  {
    ASSIGN_STRNCPY(data.marketing_info, value);
  }
  else if (identifier == "DTASOF")
  {
    ASSIGN(data.date_asof, ofxdate_to_time_t(value));
  }
  else if (identifier == "DTSTART")
  {
    ASSIGN(data.date_start, ofxdate_to_time_t(value));
  }
  else if (identifier == "DTEND")
  {
    ASSIGN(data.date_end, ofxdate_to_time_t(value));
  }
  else
  {
    OfxGenericContainer::add_attribute(identifier, value);
  }
}//end OfxStatementContainer::add_attribute()

void OfxStatementContainer::add_balance(OfxBalanceContainer* ptr_balance_container)
{
  if (ptr_balance_container->tag_identifier == "LEDGERBAL")
  {
    data.ledger_balance = ptr_balance_container->amount;
    data.ledger_balance_valid = ptr_balance_container->amount_valid;
    data.ledger_balance_date = ptr_balance_container->date;
    data.ledger_balance_date_valid = ptr_balance_container->date_valid;
  }
  else if (ptr_balance_container->tag_identifier == "AVAILBAL"
           || ptr_balance_container->tag_identifier == "INV401KBAL")
  {
    data.available_balance = ptr_balance_container->amount;
    data.available_balance_valid = ptr_balance_container->amount_valid;
    data.available_balance_date = ptr_balance_container->date;
    data.available_balance_date_valid = ptr_balance_container->date_valid;
  }
  else if (ptr_balance_container->tag_identifier == "INVBAL")
  {
    data.available_balance = ptr_balance_container->amount;
    data.available_balance_valid = ptr_balance_container->amount_valid;
    data.available_balance_date = ptr_balance_container->date;
    data.available_balance_date_valid = ptr_balance_container->date_valid;
    data.margin_balance = ptr_balance_container->margin_balance;
    data.margin_balance_valid = ptr_balance_container->margin_balance_valid;
    data.short_balance = ptr_balance_container->short_balance;
    data.short_balance_valid = ptr_balance_container->short_balance_valid;
    data.buying_power = ptr_balance_container->buying_power;
    data.buying_power_valid = ptr_balance_container->buying_power_valid;
  }
  else
  {
    message_out(ERROR, "OfxStatementContainer::add_balance(): the balance has unknown tag_identifier: " + ptr_balance_container->tag_identifier);
  }
}


int  OfxStatementContainer::add_to_main_tree()
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

int  OfxStatementContainer::gen_event()
{
  libofx_context->statementCallback(data);
  return true;
}


void OfxStatementContainer::add_account(OfxAccountData * account_data)
{
  if (account_data->account_id_valid == true)
  {
    data.account_ptr = account_data;
    ASSIGN_STRNCPY(data.account_id, std::string(account_data->account_id));
  }
}
/*void OfxStatementContainer::add_transaction(const OfxTransactionData transaction_data)
{
  transaction_queue.push(transaction_data);
}*/
