/***************************************************************************
         ofx_connect.cpp
                             -------------------
    copyright            : (C) 2005 by Ace Jones
    email                : acejones@users.sourceforge.net
***************************************************************************/
/**@file
 * \brief Code for ofxconnect utility.  C++ example code
 *
 * the purpose of the ofxconnect utility is to server as example code for
 * ALL functions of libOFX that have to do with creating OFX files.
 *
 * ofxconnect prints to stdout the created OFX file based on the options
 * you pass it
 *
 * currently it will only create the statement request file.  you can POST
 * this to an OFX server to request a statement from that financial
 * institution for that account.
 *
 * In the hopefully-not-to-distant future, ofxconnect will also make the
 * connection to the OFX server, post the data, and call ofxdump itself.
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
#include <fstream>
#include <string>
#include "libofx.h"
#include <config.h>		/* Include config constants, e.g., VERSION TF */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <string.h>
#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#endif

#include "cmdline.h" /* Gengetopt generated parser */

#include "nodeparser.h"
#include "ofxpartner.h"

#ifdef HAVE_LIBCURL
bool post(const char* request, const char* url, const char* filename)
{
  CURL *curl = curl_easy_init();
  if (! curl)
    return false;

  remove("tmpout");
  FILE* file = fopen(filename, "wb");
  if (! file )
  {
    curl_easy_cleanup(curl);
    return false;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);

  struct curl_slist *headerlist = NULL;
  headerlist = curl_slist_append(headerlist, "Content-type: application/x-ofx");
  headerlist = curl_slist_append(headerlist, "Accept: */*, application/x-ofx");

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);

  CURLcode res = curl_easy_perform(curl);

  curl_easy_cleanup(curl);
  curl_slist_free_all (headerlist);

  fclose(file);

  return true;
}
#else
bool post(const char*, const char*, const char*)
{
  std::cerr << "ERROR: libox must be configured with libcurl to post this request directly" << std::endl;
  return false;
}
#endif

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& strvect)
{
  for ( std::vector<std::string>::const_iterator it = strvect.begin(); it != strvect.end(); ++it)
  {
    os << (*it) << std::endl;
  }
  return os;
}

int main (int argc, char *argv[])
{
  gengetopt_args_info args_info;

  if (cmdline_parser (argc, argv, &args_info) != 0)
    exit(1) ;

  if ( argc == 1 )
  {
    cmdline_parser_print_help();
    exit(1);
  }

  if ( args_info.statement_req_given || args_info.accountinfo_req_given )
  {
    if ( (args_info.inputs_num > 0) )
    {
      std::cout << "file " << args_info.inputs[0] << std::endl;
    }
    else
    {
      std::cerr << "ERROR: You must specify an output file" << std::endl;
      exit(1);
    }
  }
  else if ( args_info.bank_fipid_given || args_info.bank_services_given )
  {
    if ( (args_info.inputs_num > 0) )
    {
      std::cout << "bank " << args_info.inputs[0] << std::endl;
    }
    else
    {
      std::cerr << "ERROR: You must specify an bank" << std::endl;
      exit(1);
    }
  }

  OfxFiLogin fi;
  memset(&fi, 0, sizeof(OfxFiLogin));
  bool ok = true;
  std::string url;

  if ( args_info.statement_req_given || args_info.accountinfo_req_given || args_info.payment_req_given || args_info.paymentinquiry_req_given )
  {
    // Get the FI Login information
    //

    if ( args_info.fipid_given )
    {
      std::cerr << "fipid " <<  args_info.fipid_arg << std::endl;
      std::cerr << "contacting partner server..." << std::endl;
      OfxFiServiceInfo svcinfo = OfxPartner::ServiceInfo(args_info.fipid_arg);
      std::cout << "fid " << svcinfo.fid << std::endl;
      strncpy(fi.fid, svcinfo.fid, OFX_FID_LENGTH - 1);
      std::cout << "org " << svcinfo.org << std::endl;
      strncpy(fi.org, svcinfo.org, OFX_ORG_LENGTH - 1);
      std::cout << "url " << svcinfo.url << std::endl;
      url = svcinfo.url;
    }
    if ( args_info.fid_given )
    {
      std::cerr << "fid " <<  args_info.fid_arg << std::endl;
      strncpy(fi.fid, args_info.fid_arg, OFX_FID_LENGTH - 1);
    }
    else if ( ! args_info.fipid_given )
    {
      std::cerr << "ERROR: --fid is required" << std::endl;
      ok = false;
    }

    if ( args_info.org_given )
    {
      std::cerr << "org " << args_info.org_arg << std::endl;
      strncpy(fi.org, args_info.org_arg, OFX_ORG_LENGTH - 1);
    }
    else if ( ! args_info.fipid_given )
    {
      std::cerr << "ERROR: --org is required" << std::endl;
      ok = false;
    }

    if ( args_info.user_given )
    {
      std::cerr << "user " << args_info.user_arg << std::endl;
      strncpy(fi.userid, args_info.user_arg, OFX_USERID_LENGTH - 1);
    }
    else
    {
      std::cerr << "ERROR: --user is required" << std::endl;
      ok = false;
    }

    if ( args_info.pass_given )
    {
      std::cerr << "pass " << args_info.pass_arg << std::endl;
      strncpy(fi.userpass, args_info.pass_arg, OFX_USERPASS_LENGTH - 1);
    }
    else
    {
      std::cerr << "ERROR: --pass is required" << std::endl;
      ok = false;
    }

    if ( args_info.url_given )
      url = args_info.url_arg;
  }

  if ( args_info.statement_req_given )
  {
    std::cerr << "Statement request" << std::endl;

    OfxAccountData account;
    memset(&account, 0, sizeof(OfxAccountData));

    if ( args_info.bank_given )
    {
      std::cerr << "bank " << args_info.bank_arg << std::endl;
      strncpy(account.bank_id, args_info.bank_arg, OFX_BANKID_LENGTH - 1);
    }
    else
    {
      if ( args_info.type_given && args_info.type_arg == 1 )
      {
        std::cerr << "ERROR: --bank is required for a bank request" << std::endl;
        ok = false;
      }
    }

    if ( args_info.broker_given )
    {
      std::cerr << "broker " << args_info.broker_arg << std::endl;
      strncpy(account.broker_id, args_info.broker_arg, OFX_BROKERID_LENGTH - 1);
    }
    else
    {
      if ( args_info.type_given && args_info.type_arg == 2 )
      {
        std::cerr << "ERROR: --broker is required for an investment statement request" << std::endl;
        ok = false;
      }
    }

    if ( args_info.acct_given )
    {
      std::cerr << "acct " << args_info.acct_arg << std::endl;
      strncpy(account.account_number, args_info.acct_arg, OFX_ACCTID_LENGTH - 1);
    }
    else
    {
      std::cerr << "ERROR: --acct is required for a statement request" << std::endl;
      ok = false;
    }

    if ( args_info.type_given )
    {
      std::cerr << "type " << args_info.type_arg << std::endl;
      switch (args_info.type_arg)
      {
      case 1:
        account.account_type = account.OFX_CHECKING;
        break;
      case 2:
        account.account_type = account.OFX_INVESTMENT;
        break;
      case 3:
        account.account_type = account.OFX_CREDITCARD ;
        break;
      default:
        std::cerr << "ERROR: --type is not valid.  Must be between 1 and 3" << std::endl;
        ok = false;
      }
    }
    else
    {
      std::cerr << "ERROR: --type is required for a statement request" << std::endl;
      ok = false;
    }

    if ( args_info.past_given )
    {
      std::cerr << "past " << args_info.past_arg << std::endl;
    }
    else
    {
      std::cerr << "ERROR: --past is required for a statement request" << std::endl;
      ok = false;
    }

    if ( ok )
    {
      char* request = libofx_request_statement( &fi, &account, time(NULL) - args_info.past_arg * 86400L );

      if ( url.length() && args_info.inputs_num > 0 )
        post(request, url.c_str(), args_info.inputs[0]);
      else
        std::cout << request;

      free(request);
    }
  }

  if ( args_info.paymentinquiry_req_given )
  {
    char tridstr[33];
    memset(tridstr, 0, 33);

    bool ok = true;

    if ( args_info.trid_given )
    {
      std::cerr << "trid " << args_info.trid_arg << std::endl;
      snprintf(tridstr, 32, "%i", args_info.trid_arg);
    }
    else
    {
      std::cerr << "ERROR: --trid is required for a payment inquiry request" << std::endl;
      ok = false;
    }

    if ( ok )
    {
      char* request = libofx_request_payment_status( &fi, tridstr );

      std::filebuf fb;
      fb.open ("query", std::ios::out);
      std::ostream os(&fb);
      os << request;
      fb.close();

      if ( url.length() && args_info.inputs_num > 0 )
        post(request, url.c_str(), args_info.inputs[0]);
      else
        std::cout << request;

      free(request);
    }
  }

  if ( args_info.payment_req_given )
  {
    OfxAccountData account;
    memset(&account, 0, sizeof(OfxAccountData));
    OfxPayee payee;
    memset(&payee, 0, sizeof(OfxPayee));
    OfxPayment payment;
    memset(&payment, 0, sizeof(OfxPayment));

    strcpy(payee.name, "MARTIN PREUSS");
    strcpy(payee.address1, "1 LAUREL ST");
    strcpy(payee.city, "SAN CARLOS");
    strcpy(payee.state, "CA");
    strcpy(payee.postalcode, "94070");
    strcpy(payee.phone, "866-555-1212");

    strcpy(payment.amount, "200.00");
    strcpy(payment.account, "1234");
    strcpy(payment.datedue, "20060301");
    strcpy(payment.memo, "This is a test");

    bool ok = true;

    if ( args_info.bank_given )
    {
      std::cerr << "bank " << args_info.bank_arg << std::endl;
      strncpy(account.bank_id, args_info.bank_arg, OFX_BANKID_LENGTH - 1);
    }
    else
    {
      if ( args_info.type_given && args_info.type_arg == 1 )
      {
        std::cerr << "ERROR: --bank is required for a bank request" << std::endl;
        ok = false;
      }
    }

    if ( args_info.broker_given )
    {
      std::cerr << "broker " << args_info.broker_arg << std::endl;
      strncpy(account.broker_id, args_info.broker_arg, OFX_BROKERID_LENGTH - 1);
    }
    else
    {
      if ( args_info.type_given && args_info.type_arg == 2 )
      {
        std::cerr << "ERROR: --broker is required for an investment statement request" << std::endl;
        ok = false;
      }
    }

    if ( args_info.acct_given )
    {
      std::cerr << "acct " << args_info.acct_arg << std::endl;
      strncpy(account.account_number, args_info.acct_arg, OFX_ACCTID_LENGTH - 1);
    }
    else
    {
      std::cerr << "ERROR: --acct is required for a statement request" << std::endl;
      ok = false;
    }

    if ( args_info.type_given )
    {
      std::cerr << "type " << args_info.type_arg << std::endl;
      switch (args_info.type_arg)
      {
      case 1:
        account.account_type = account.OFX_CHECKING;
        break;
      case 2:
        account.account_type = account.OFX_INVESTMENT;
        break;
      case 3:
        account.account_type = account.OFX_CREDITCARD ;
        break;
      default:
        std::cerr << "ERROR: --type is not valid.  Must be between 1 and 3" << std::endl;
        ok = false;
      }
    }
    else
    {
      std::cerr << "ERROR: --type is required for a statement request" << std::endl;
      ok = false;
    }

    if ( ok )
    {
      char* request = libofx_request_payment( &fi, &account, &payee, &payment );

      std::filebuf fb;
      fb.open ("query", std::ios::out);
      std::ostream os(&fb);
      os << request;
      fb.close();

      if ( url.length() && args_info.inputs_num > 0 )
        post(request, url.c_str(), args_info.inputs[0]);
      else
        std::cout << request;

      free(request);
    }

  }

  if ( args_info.accountinfo_req_given )
  {
    if ( ok )
    {
      char* request = libofx_request_accountinfo( &fi );

      if ( url.length() && args_info.inputs_num > 0 )
        post(request, url.c_str(), args_info.inputs[0]);
      else
        std::cout << request;

      free(request);
    }
  }

  if ( args_info.bank_list_given )
  {
    std::cout << OfxPartner::BankNames();
  }

  if ( args_info.bank_fipid_given && args_info.inputs_num > 0 )
  {
    std::cout << OfxPartner::FipidForBank(args_info.inputs[0]);
  }

  if ( args_info.bank_services_given && args_info.inputs_num > 0 )
  {
    OfxFiServiceInfo svcinfo = OfxPartner::ServiceInfo(args_info.inputs[0]);
    std::cout << "Account List? " << (svcinfo.accountlist ? "Yes" : "No") << std::endl;
    std::cout << "Statements? " << (svcinfo.statements ? "Yes" : "No") << std::endl;
    std::cout << "Billpay? " << (svcinfo.billpay ? "Yes" : "No") << std::endl;
    std::cout << "Investments? " << (svcinfo.investments ? "Yes" : "No") << std::endl;
  }

  if ( args_info.allsupport_given )
  {
    std::vector<std::string> banks = OfxPartner::BankNames();
    std::vector<std::string>::const_iterator it_bank = banks.begin();
    while ( it_bank != banks.end() )
    {
      std::vector<std::string> fipids = OfxPartner::FipidForBank(*it_bank);
      std::vector<std::string>::const_iterator it_fipid = fipids.begin();
      while ( it_fipid != fipids.end() )
      {
        if ( OfxPartner::ServiceInfo(*it_fipid).accountlist )
          std::cout << *it_bank << std::endl;
        ++it_fipid;
      }
      ++it_bank;
    }
  }

  return 0;
}


// vim:cin:si:ai:et:ts=2:sw=2:

