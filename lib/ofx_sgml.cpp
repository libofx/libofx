/***************************************************************************
                          ofx_sgml.cpp
                          -------------------
    copyright            : (C) 2002 by Benoit Grégoire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
   \brief OFX/SGML parsing functionnality.
   *
   Almost all of the SGML parser specific code is contained in this file (some is in messages.cpp and ofx_utilities.cpp).  To understand this file you must read the documentation of OpenSP's generic interface: see http://openjade.sourceforge.net/
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

#include <iostream>
#include <stdlib.h>
#include <string>
#include "ParserEventGeneratorKit.h"
#include "libofx.h"
#include "ofx_utilities.h"
#include "messages.h"
#include "ofx_proc_rs.h"

using namespace std;


SGMLApplication::OpenEntityPtr entity_ptr;
SGMLApplication::Position position;


/** \brief This object is driven by OpenSP as it parses the SGML from the ofx file(s)
*/
class OutlineApplication : public SGMLApplication
{
public:
  OfxGenericContainer *curr_container_element; /**< The currently open object from ofx_proc_rs.cpp */
  OfxGenericContainer *tmp_container_element;
  bool is_data_element; /**< If the SGML element contains data, this flag is raised */
  string incoming_data; /**< The raw data from the SGML data element */
  bool osp134workaround; /**< If the OpenSP < 1.3.4 bug is encountered, the flag is rased by startElement and lowered by endElement */
  string  osp134workaround_data; /**< The name of the previous element identifier */

  
  OutlineApplication ()
  {
    curr_container_element = NULL;
    is_data_element = false;
  }
  
  /** \brief Callback: Start of an OFX element
   *
   An OpenSP callback, get's called when the opening tag of an OFX element appears in the file
  */
  void startElement (const StartElementEvent & event)
  {
    string identifier;
    CharStringtostring (event.gi, identifier);
    
    position = event.pos;
    switch (event.contentType)
      {
      case StartElementEvent::empty:	cerr <<"StartElementEvent::empty\n";
	break;
      case StartElementEvent::cdata:	cerr <<"StartElementEvent::cdata\n";
	break;
      case StartElementEvent::rcdata:	cerr <<"StartElementEvent::rcdata\n";
	break;
      case StartElementEvent::mixed:	//cerr <<"mixed\n";
	is_data_element = true;
	break;
      case StartElementEvent::element:	//cerr <<"element\n";
	is_data_element = false;
	break;
      default:
	cerr << "unknow SGML content type ?!?!??";
      }
    /*       cout<<")\n";*/
    
    if (is_data_element == false)
      {
	/*------- The following are OFX entities ---------------*/
	if (identifier == "STATUS")
	  {
	    message_out (PARSER, "Element " + identifier + " found");
	    curr_container_element = new OfxStatusContainer (curr_container_element, identifier);
	  }
	else if (identifier == "STMTRS" || identifier == "CCSTMTRS")
	  {
	    message_out (PARSER, "Element " + identifier + " found");
	    curr_container_element = new OfxStatementContainer (curr_container_element, identifier);
	  }
	else if (identifier == "BANKTRANLIST")
	  {
	    message_out (PARSER, "Element " + identifier + " found");
	    //BANKTRANLIST ignored, we will process it's attributes directly inside the STATEMENT,
	    if(curr_container_element->type!="STATEMENT")
	      {
		message_out(ERROR,"Element " + identifier + " found while not inside a STATEMENT container");
	      }
	    else
	      {
		curr_container_element = new OfxPushUpContainer (curr_container_element, identifier);
	      }
	  }
	else if (identifier == "STMTTRN")
	  {
	    message_out (PARSER, "Element " + identifier + " found");
	    curr_container_element = new OfxBankTransactionContainer (curr_container_element, identifier);
	  }
	else if(identifier == "BUYDEBT" ||
		identifier == "BUYMF" ||
		identifier == "BUYOPT" ||
		identifier == "BUYOTHER" ||
		identifier == "BUYSTOCK" ||
		identifier == "CLOSUREOPT" ||
		identifier == "INCOME" ||
		identifier == "INVEXPENSE" ||
		identifier == "JRNLFUND" ||
		identifier == "JRNLSEC" ||
		identifier == "MARGININTEREST" ||
		identifier == "REINVEST" ||
		identifier == "RETOFCAP" ||
		identifier == "SELLDEBT" ||
		identifier == "SELLMF" ||
		identifier == "SELLOPT" ||
		identifier == "SELLOTHER" ||
		identifier == "SELLSTOCK" ||
		identifier == "SPLIT" ||
		identifier == "TRANSFER" )
	  {
	    message_out (PARSER, "Element " + identifier + " found");
	    curr_container_element = new OfxInvestmentTransactionContainer (curr_container_element, identifier);
	  }
/*The following is a list of OFX elements whose attributes will be processed by the parent container*/
	else if (identifier == "INVBUY" ||
		 identifier == "INVSELL" ||
		 identifier == "INVTRAN" ||
		 identifier == "SECID")
	    {
	      message_out (PARSER, "Element " + identifier + " found");
	      curr_container_element = new OfxPushUpContainer (curr_container_element, identifier);
	    }

	/* The different types of accounts */
	else if (identifier == "BANKACCTFROM" || identifier == "CCACCTFROM")
	  {
	    message_out (PARSER, "Element " + identifier + " found");
	    curr_container_element = new OfxAccountContainer (curr_container_element, identifier);
	  }
	/* The different types of balances */
	else if (identifier == "LEDGERBAL" || identifier == "AVAILBAL")
	  {
	    message_out (PARSER, "Element " + identifier + " found");
	    curr_container_element = new OfxBalanceContainer (curr_container_element, identifier);
	  }
	else
	  {
	    /* We dont know this OFX element, so we create a dummy container */
	    curr_container_element = new OfxDummyContainer(curr_container_element, identifier);
	  }
      }
    else
      {
	/* The element was a data element.  OpenSP will call one or several data() callback with the data */
	message_out (PARSER, "Data element " + identifier + " found");
/* There is a bug in OpenSP 1.3.4, which won't send endElement Event for some elements, and will instead send an error like "document type does not allow element "MESSAGE" here".  Incoming_data should be empty in such a case, but it will not be if the endElement event was skiped. So we empty it, so at least the last element has a chance of having valid data */ 
	/*if (incoming_data != "")
	  {
	  message_out (WARNING, "startElement: incoming_data should be empty! You are probably using OpenSP <= 1.3.4.  The folowing data was lost: " + incoming_data );
	  //incoming_data.assign ("");
	  }*/

	/* This workaround for the bug.  It may not be maintained in future versions. */ 
	if (incoming_data != "")
	  {
	    message_out (WARNING, "startElement: The OpenSP <= 1.3.4 endElement bug workaround was used.  Upgrade your OpenSP, your data is NOT garanteed to be correct.");
	    osp134workaround = true;
	    EndElementEvent tmp_event;
	    tmp_event.pos=event.pos;
	    tmp_event.gi=event.gi;
	    endElement( tmp_event);
	  }
      }
    osp134workaround_data = identifier;
  }

  /** \brief Callback: End of an OFX element
   *
   An OpenSP callback, get's called at the end of an OFX element (the closing tags are not always present in OFX) in the file.
  */
  void endElement (const EndElementEvent & event)
  {
    string identifier;
    if( osp134workaround == true)
      {
	 identifier =  osp134workaround_data;
	 osp134workaround = false;
      }
    else
      {
	CharStringtostring (event.gi, identifier);
      }

    message_out(PARSER,"endElement event received from OpenSP");

    position = event.pos;
    if (is_data_element == true)
      {
	incoming_data = strip_whitespace(incoming_data);

	if (curr_container_element != NULL)
	  {
	    curr_container_element->add_attribute (identifier, incoming_data);
	    message_out (PARSER,"endElement: Added data '" + incoming_data + "' from " + identifier + " to " + curr_container_element->type + " container_element");
	    incoming_data.assign ("");
	    is_data_element = false;
	  }
	else
	  {
	    message_out (ERROR, "endElement: Trying to add data '" + incoming_data + "' from " + identifier + " to NULL container_element");
	    incoming_data.assign ("");
	    is_data_element = false;
	  }
      }
    else
      {
	if (curr_container_element != NULL)
	  {
	    if (identifier == curr_container_element->tag_identifier)
	      {
		tmp_container_element = curr_container_element;
		curr_container_element = curr_container_element->getparent ();
		/*The destructor called by delete will automatically take the corect action 
		  (such as calling a libofx.h callback) before destroying the container */
		delete tmp_container_element;
		message_out (PARSER, "Element " + identifier + " closed, object destroyed");
	      }
	    else
	      {
		message_out (ERROR, "Tried to close a "+identifier+" but a "+curr_container_element->type+" is currently open.");
	      }
	  }
	else
	  {
	    message_out (ERROR,"Tried to close a "+identifier+" without a open element (NULL pointer)");
	  }
      }
  }

  /** \brief Callback: Data from an OFX element
   *
   An OpenSP callback, get's called when the raw data of an OFX element appears in the file.  Is usually called more than once for a single element, so we must concatenate the data.
  */
  void data (const DataEvent & event)
  {
    string tmp;
    
    position = event.pos;
    /*cout << "data:"<<event.data<<"\n"; */
    AppendCharStringtostring (event.data, incoming_data);
  }

  /** \brief Callback: SGML parse error
   *
   An OpenSP callback, get's called when a parser error has occured.
  */
  void error (const ErrorEvent & event)
  {
    string message;
    string string_buf;
    OfxMsgType error_type = ERROR;

    position = event.pos;
    message = message + "OpenSP parser: ";
    switch (event.type){
    case SGMLApplication::ErrorEvent::quantity:
      message = message + "quantity (Exceeding a quantity limit)";
      error_type = ERROR;
      break;
    case SGMLApplication::ErrorEvent::idref:
      message = message + "idref (An IDREF to a non-existent ID)";
      error_type = ERROR;
      break;
    case SGMLApplication::ErrorEvent::capacity:
      message = message + "capacity (Exceeding a capacity limit)";
      error_type = ERROR;
      break;
    case SGMLApplication::ErrorEvent::otherError:
      message = message + "otherError (misc parse error)";
      error_type = ERROR;
      break;
    case SGMLApplication::ErrorEvent::warning:
      message = message + "warning (Not actually an error.)";
      error_type = WARNING;
      break;
    case SGMLApplication::ErrorEvent::info:
      message =  message + "info (An informationnal message.  Not actually an error)";
      error_type = INFO;
      break;
    default:
      message = message + "OpenSP sent an unknown error to LibOFX (You probably have a newer version of OpenSP)";
    }
    message =	message + "\n" + "Error msg: " + CharStringtostring (event.message, string_buf);
    message_out (error_type, message);
  }


  void OpenEntityChange (const OpenEntityPtr & para_entity_ptr)
  {
    cout << "\nOpenEntityChange()\n";
    entity_ptr = para_entity_ptr;

  };

private:
};

/**
   ofx_proc_sgml will take a list of files in command line format.  The first file must be the DTD, and then any number of OFX files.
*/
int ofx_proc_sgml(int argc, char *argv[])
{
  message_out(DEBUG,"Begin ofx_proc_sgml()");
  message_out(DEBUG,argv[0]);
  message_out(DEBUG,argv[1]);
  
  ParserEventGeneratorKit parserKit;
  parserKit.setOption (ParserEventGeneratorKit::showOpenEntities);
  EventGenerator *egp =	parserKit.makeEventGenerator (argc, argv);
  egp->inhibitMessages (true);	/* Error output is handled by libofx not OpenSP */
  OutlineApplication app;
  unsigned nErrors = egp->run (app); /* Begin parsing */
  delete egp;
  return nErrors > 0;
}
