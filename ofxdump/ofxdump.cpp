/***************************************************************************
                          ofxdump.cpp
                             -------------------
    copyright            : (C) 2002 by Benoit Grégoire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
 * \brief Code for ofxdump utility.  C++ example code
 *
 * ofxdump prints to stdout, in human readable form, everything the library 
 understands about a particular ofx response file, and sends errors to 
 stderr.  To know exactly what the library understands about of a particular
 ofx response file, just call ofxdump on that file.
 *
 * ofxdump is meant as both a C++ code example and a developper/debuging
 tool.  It uses every function and every structure of the LibOFX API.  By
 default, WARNING, INFO, ERROR and STATUS messages are enabled.  You can
 change these defaults at the top of ofxdump.cpp
 *
 * usage: ofxdump path_to_ofx_file/ofx_filename
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
#include <iomanip>
#include <string>
#include "libofx.h"
#include <stdio.h>		/* for printf() */
#include <config.h>		/* Include config constants, e.g., VERSION TF */


using namespace std;

int main (int argc, char *argv[])
{
  /** Tell ofxdump what you want it to send to stderr.  See messages.cpp for more details */
  extern int ofx_PARSER_msg;
  extern int ofx_DEBUG_msg;
  extern int ofx_WARNING_msg;
  extern int ofx_ERROR_msg;
  extern int ofx_INFO_msg;
  extern int ofx_STATUS_msg;

  ofx_PARSER_msg = false;
  ofx_DEBUG_msg = false;
  ofx_WARNING_msg = true;
  ofx_ERROR_msg = true;
  ofx_INFO_msg = true;
  ofx_STATUS_msg = true;

  int special_options (char *argv[]);
  int count, ret = 0;;
 
  //printf ("This program was called with \"%s\".\n",argv[0]);
  if (argc > 1) 
    { 
      for (count = 1; count < argc; count++) 
	{
	  //printf("argv[%d] = %s\n", count, argv[count]); 
	}
      if  (0 == special_options(argv)) ;
      else{
        ofx_prep_cb(
                    ofx_proc_statement_cb,
                    ofx_proc_account_cb,
                    ofx_proc_transaction_cb,
                    ofx_proc_security_cb,
                    ofx_proc_status_cb
                    );
	ofx_proc_file(argc, argv);    /* Special option not found */
      }
    } 
  else
    {
      char *spoptar[] =           {"ofxdump","--help"};
      special_options(spoptar);   /* No arguments             */
    }
  return 0; 
}


int special_options (char *argv[])
{
  int count;
  
  /* Define case numbers */
  const int spoptver = 0;
  const int spopthelp = 1; 
  
  /* Define option and case arrays */
  const char *spoptar[] =           {"--version", "-V", "--help"};
  const int spoptdim = sizeof(spoptar)/sizeof(spoptar[0]);
  const int spoptcasear[spoptdim] = {spoptver, spoptver, spopthelp};
  
  /* Define help array  */
  const char *helpar[] =
    {
      "ofxdump command synopsis:\n",
      "ofxdump special_option\n",
      "ofxdump ofx_file\n",
      "\n",
      "Special options are:\n",
      "--version, V     libofx version\n",
      "--help           help text\n",
      "\n",
      "If the first argument is a special option, it is processed and any remaining \
arguments are ignored; otherwise, control goes to ofx_proc_file.\n\n"
    };
  const int helpardim = sizeof(helpar)/sizeof(helpar[1]);

  /* Scan for special option */  
  for (count = 0; (count < spoptdim) && (strcmp(argv[1], spoptar[count]) != 0) ; count++);

  if (count < spoptdim)               /* If found */
    {
      switch (spoptcasear[count])
	{ 
	case spoptver:  printf ("libofx version:  %s \n", VERSION ); return 0;
	case spopthelp: 
	  {
	    for (int i=0; i < helpardim; i++)  printf (helpar[i]); 
	    return 0;
	  }
	default: 
	  {
	    printf("Impossible special option case\n");
	    return -1;              /* Program error */
	  }
	}
    }
  return 1;    
}




int ofx_proc_status_cb(struct OfxStatusData data)
{
  cout<<"ofx_proc_status():\n";
  if(data.ofx_element_name_valid==true){
    cout<<"    Ofx entity this status is relevent to: "<< data.ofx_element_name<<" \n";
  }
  if(data.severity_valid==true){
    cout<<"    Severity: ";
    switch(data.severity){
    case OfxStatusData::INFO : cout<<"INFO\n";
      break;
    case OfxStatusData::WARN : cout<<"WARN\n";
      break;
    case OfxStatusData::ERROR : cout<<"ERROR\n";
      break;
    default: cout<<"WRITEME: Unknown status severity!\n";
    }
  }
  if(data.code_valid==true){
    cout<<"    Code: "<<data.code<<", name: "<<data.name<<"\n    Description: "<<data.description<<"\n";
  }
  if(data.server_message_valid==true){
    cout<<"    Server Message: "<<data.server_message<<"\n";
  }
  cout<<"\n";
  return 0;
}

int ofx_proc_security_cb(struct OfxSecurityData data)
{
  char dest_string[255];
  cout<<"ofx_proc_security():\n";
  if(data.unique_id_valid==true){
    cout<<"    Unique ID of the security being traded: "<<data.unique_id<<"\n";
  }
  if(data.unique_id_type_valid==true){
    cout<<"    Format of the Unique ID: "<<data.unique_id_type<<"\n";
  }
  if(data.secname_valid==true){
    cout<<"    Name of the security: "<<data.secname<<"\n";
  }
  if(data.ticker_valid==true){
    cout<<"    Ticker symbol: "<<data.ticker<<"\n";
  }
  if(data.unitprice_valid==true){
    cout<<"    Price of each unit of the security: "<<data.unitprice<<"\n";
  }
  if(data.date_unitprice_valid==true){
    strftime(dest_string,sizeof(dest_string),"%c %Z",localtime(&(data.date_unitprice)));
    cout<<"    Date as of which the unitprice is valid: "<<dest_string<<"\n";
  }
  if(data.currency_valid==true){
    cout<<"    Currency of the unitprice: "<<data.currency<<"\n";
  }
  if(data.memo_valid==true){
    cout<<"    Extra transaction information (memo): "<<data.memo<<"\n";
  }
  cout<<"\n";
  return 0;
}

int ofx_proc_transaction_cb(struct OfxTransactionData data)
{
  char dest_string[255];
  cout<<"ofx_proc_transaction():\n";
  
  if(data.account_id_valid==true){
    cout<<"    Account ID : "<<data.account_id<<"\n";
  }
  
  if(data.transactiontype_valid==true){
    switch(data.transactiontype){
    case data.OFX_CREDIT: strncpy(dest_string, "CREDIT: Generic credit", sizeof(dest_string));
      break;
    case data.OFX_DEBIT: strncpy(dest_string, "DEBIT: Generic debit", sizeof(dest_string));
      break;
    case data.OFX_INT: strncpy(dest_string, "INT: Interest earned or paid (Note: Depends on signage of amount)", sizeof(dest_string));
      break;
    case data.OFX_DIV: strncpy(dest_string, "DIV: Dividend", sizeof(dest_string));
      break;
    case data.OFX_FEE: strncpy(dest_string, "FEE: FI fee", sizeof(dest_string));
      break;
    case data.OFX_SRVCHG: strncpy(dest_string, "SRVCHG: Service charge", sizeof(dest_string));
      break;
    case data.OFX_DEP: strncpy(dest_string, "DEP: Deposit", sizeof(dest_string));
      break;
    case data.OFX_ATM: strncpy(dest_string, "ATM: ATM debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
      break;
    case data.OFX_POS: strncpy(dest_string, "POS: Point of sale debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
      break;
    case data.OFX_XFER: strncpy(dest_string, "XFER: Transfer", sizeof(dest_string));
      break;
    case data.OFX_CHECK: strncpy(dest_string, "CHECK: Check", sizeof(dest_string));
      break;
    case data.OFX_PAYMENT: strncpy(dest_string, "PAYMENT: Electronic payment", sizeof(dest_string));
      break;
    case data.OFX_CASH: strncpy(dest_string, "CASH: Cash withdrawal", sizeof(dest_string));
      break;
    case data.OFX_DIRECTDEP: strncpy(dest_string, "DIRECTDEP: Direct deposit", sizeof(dest_string));
      break;
    case data.OFX_DIRECTDEBIT: strncpy(dest_string, "DIRECTDEBIT: Merchant initiated debit", sizeof(dest_string));
      break;
    case data.OFX_REPEATPMT: strncpy(dest_string, "REPEATPMT: Repeating payment/standing order", sizeof(dest_string));
      break;
    case data.OFX_OTHER: strncpy(dest_string, "OTHER: Other", sizeof(dest_string));
      break;
    default : strncpy(dest_string, "Unknown transaction type", sizeof(dest_string));
      break;
    }
    cout<<"    Transaction type: "<<dest_string<<"\n";
  }
  if(data.date_initiated_valid==true){
    strftime(dest_string,sizeof(dest_string),"%c %Z",localtime(&(data.date_initiated)));
    cout<<"    Date initiated: "<<dest_string<<"\n";
  }
  if(data.date_posted_valid==true){
    strftime(dest_string,sizeof(dest_string),"%c %Z",localtime(&(data.date_posted)));
    cout<<"    Date posted: "<<dest_string<<"\n";
  }
  if(data.date_funds_available_valid==true){
    strftime(dest_string,sizeof(dest_string),"%c %Z",localtime(&(data.date_funds_available)));
    cout<<"    Date funds are available: "<<dest_string<<"\n";
  }
  if(data.amount_valid==true){
    cout<<"    Total money amount: "<<setiosflags(ios::fixed)<<setiosflags(ios::showpoint)<<setprecision(2)<<data.amount<<"\n";
  }
  if(data.units_valid==true){
    cout<<"    # of units: "<<setiosflags(ios::fixed)<<setiosflags(ios::showpoint)<<setprecision(2)<<data.units<<"\n";
  }
  if(data.unitprice_valid==true){
    cout<<"    Unit price: "<<setiosflags(ios::fixed)<<setiosflags(ios::showpoint)<<setprecision(2)<<data.unitprice<<"\n";
  }
  if(data.fi_id_valid==true){
    cout<<"    Financial institution's ID for this transaction: "<<data.fi_id<<"\n";
  }
  if(data.fi_id_corrected_valid==true){
    cout<<"    Financial institution ID replaced or corrected by this transaction: "<<data.fi_id_corrected<<"\n";
  }
  if(data.fi_id_correction_action_valid==true){
    cout<<"    Action to take on the corrected transaction: ";
    switch(data.fi_id_correction_action){
    case data.DELETE : cout<<"DELETE\n";
      break;
    case data.REPLACE : cout<<"REPLACE\n";
      break;
    default:
      cout<<"ofx_proc_transaction(): This should not happen!\n";
    }
  }
  if(data.invtransactiontype_valid==true){
    cout<<"    Investment transaction type: ";
    switch(data.invtransactiontype){
    case data.OFX_BUYDEBT: strncpy(dest_string, "BUYDEBT (Buy debt security)", sizeof(dest_string));
      break;
     case data.OFX_BUYMF: strncpy(dest_string, "BUYMF (Buy mutual fund)", sizeof(dest_string));
      break;
    case data.OFX_BUYOPT: strncpy(dest_string, "BUYOPT (Buy option)", sizeof(dest_string));
      break;
    case data.OFX_BUYOTHER: strncpy(dest_string, "BUYOTHER (Buy other security type)", sizeof(dest_string));
      break;
    case data.OFX_BUYSTOCK: strncpy(dest_string, "BUYSTOCK (Buy stock))", sizeof(dest_string));
      break;
    case data.OFX_CLOSUREOPT: strncpy(dest_string, "CLOSUREOPT (Close a position for an option)", sizeof(dest_string));
      break;
    case data.OFX_INCOME: strncpy(dest_string, "INCOME (Investment income is realized as cash into the investment account)", sizeof(dest_string));
      break;
    case data.OFX_INVEXPENSE: strncpy(dest_string, "INVEXPENSE (Misc investment expense that is associated with a specific security)", sizeof(dest_string));
      break;
    case data.OFX_JRNLFUND: strncpy(dest_string, "JRNLFUND (Journaling cash holdings between subaccounts within the same investment account)", sizeof(dest_string));
      break;
    case data.OFX_MARGININTEREST: strncpy(dest_string, "MARGININTEREST (Margin interest expense)", sizeof(dest_string));
      break;
    case data.OFX_REINVEST: strncpy(dest_string, "REINVEST (Reinvestment of income)", sizeof(dest_string));
      break;
    case data.OFX_RETOFCAP: strncpy(dest_string, "RETOFCAP (Return of capital)", sizeof(dest_string));
      break;
    case data.OFX_SELLDEBT: strncpy(dest_string, "SELLDEBT (Sell debt security.  Used when debt is sold, called, or reached maturity)", sizeof(dest_string));
      break;
    case data.OFX_SELLMF: strncpy(dest_string, "SELLMF (Sell mutual fund)", sizeof(dest_string));
      break;
    case data.OFX_SELLOPT: strncpy(dest_string, "SELLOPT (Sell option)", sizeof(dest_string));
      break;
    case data.OFX_SELLOTHER: strncpy(dest_string, "SELLOTHER (Sell other type of security)", sizeof(dest_string));
      break;
    case data.OFX_SELLSTOCK: strncpy(dest_string, "SELLSTOCK (Sell stock)", sizeof(dest_string));
      break;
    case data.OFX_SPLIT: strncpy(dest_string, "SPLIT (Stock or mutial fund split)", sizeof(dest_string));
      break;
    case data.OFX_TRANSFER: strncpy(dest_string, "TRANSFER (Transfer holdings in and out of the investment account)", sizeof(dest_string));
      break;
    default: strncpy(dest_string, "ERROR, this investment transaction type is unknown.  This is a bug in ofxdump", sizeof(dest_string));
      break;
    }
    cout<<dest_string<<"\n";
  }
  if(data.unique_id_valid==true){
    cout<<"    Unique ID of the security being traded: "<<data.unique_id<<"\n";
  }
  if(data.unique_id_type_valid==true){
    cout<<"    Format of the Unique ID: "<<data.unique_id_type<<"\n";
  }
  if(data.security_data_valid==true){
    cout<<"    Security data is available:\n    START security_data content----------\n";
    ofx_proc_security_cb(*(data.security_data_ptr));
    cout<<"    END security_data content----------\n";

  }

  if(data.server_transaction_id_valid==true){
    cout<<"    Server's transaction ID (confirmation number): "<<data.server_transaction_id<<"\n";
  }
  if(data.check_number_valid==true){
    cout<<"    Check number: "<<data.check_number<<"\n";
  }
  if(data.reference_number_valid==true){
    cout<<"    Reference number: "<<data.reference_number<<"\n";
  }
  if(data.standard_industrial_code_valid==true){
    cout<<"    Standard Industrial Code: "<<data.standard_industrial_code<<"\n";
  }
  if(data.payee_id_valid==true){
    cout<<"    Payee_id: "<<data.payee_id<<"\n";
  }
  if(data.name_valid==true){
    cout<<"    Name of payee or transaction description: "<<data.name<<"\n";
  }
  if(data.memo_valid==true){
    cout<<"    Extra transaction information (memo): "<<data.memo<<"\n";
  }
  cout<<"\n";
  return 0;
}//end ofx_proc_transaction()

int ofx_proc_statement_cb(struct OfxStatementData data)
{
  char dest_string[255];
  cout<<"ofx_proc_statement():\n";
  if(data.currency_valid==true){
    cout<<"    Currency: "<<data.currency<<"\n";
  }
  if(data.account_id_valid==true){
    cout<<"    Account ID: "<<data.account_id<<"\n";
  }
  if(data.date_start_valid==true){
    strftime(dest_string,sizeof(dest_string),"%c %Z",localtime(&(data.date_start)));
    cout<<"    Start date of this statement: "<<dest_string<<"\n";
  }
  if(data.date_end_valid==true){
    strftime(dest_string,sizeof(dest_string),"%c %Z",localtime(&(data.date_end)));
    cout<<"    End date of this statement: "<<dest_string<<"\n";
  }
  if(data.ledger_balance_valid==true){
    cout<<"    Ledger balance: "<<setiosflags(ios::fixed)<<setiosflags(ios::showpoint)<<setprecision(2)<<data.ledger_balance<<"\n";
  }
  if(data.ledger_balance_date_valid==true){
    strftime(dest_string,sizeof(dest_string),"%c %Z",localtime(&(data.ledger_balance_date)));
    cout<<"    Ledger balance date: "<<dest_string<<"\n";
  }
  if(data.available_balance_valid==true){
    cout<<"    Available balance: "<<setiosflags(ios::fixed)<<setiosflags(ios::showpoint)<<setprecision(2)<<data.available_balance<<"\n";
  }
  if(data.available_balance_date_valid==true){
    strftime(dest_string,sizeof(dest_string),"%c %Z",localtime(&(data.available_balance_date)));
    cout<<"    Ledger balance date: "<<dest_string<<"\n";
  }
  if(data.marketing_info_valid==true){
    cout<<"    Marketing information: "<<data.marketing_info<<"\n";
  }
  cout<<"\n";
  return 0;
}//end ofx_proc_statement()

int ofx_proc_account_cb(struct OfxAccountData data)
{
  cout<<"ofx_proc_account():\n";
  if(data.account_id_valid==true){
    cout<<"    Account ID: "<<data.account_id<<"\n";
    cout<<"    Account name: "<<data.account_name<<"\n";
  }
   if(data.account_type_valid==true){
    cout<<"    Account type: ";
    switch(data.account_type){
    case OfxAccountData::OFX_CHECKING : cout<<"CHECKING\n";
      break;
    case OfxAccountData::OFX_SAVINGS : cout<<"SAVINGS\n";
      break;
    case OfxAccountData::OFX_MONEYMRKT : cout<<"MONEYMRKT\n";
      break;
    case OfxAccountData::OFX_CREDITLINE : cout<<"CREDITLINE\n";
      break;
    case OfxAccountData::OFX_CMA : cout<<"CMA\n";
      break;
    case OfxAccountData::OFX_CREDITCARD : cout<<"CREDITCARD\n";
      break;
    case OfxAccountData::OFX_INVESTMENT : cout<<"INVESTMENT\n";
      break;
    default: cout<<"ofx_proc_account() WRITEME: This is an unknown account type!";
    }
  }
  if(data.currency_valid==true){
    cout<<"    Currency: "<<data.currency<<"\n";
  }

  cout<<"\n";
  return 0;
}//end ofx_proc_account()
