/***************************************************************************
                          ofx_proc_rs.h 
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
 * \brief LibOFX internal object code.
 *
 * These objects will process the elements returned by ofx_sgml.cpp and add them to their data members.
 * \warning Object documentation is not yet complete.
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef OFX_PROC_H
#define OFX_PROC_H
#include "libofx.h"
using namespace std;

/** \brief A generic container for an OFX SGML element.  Every container inherits from OfxGenericContainer.
 *
 A hierarchy of containers is built as the file is parsed.  The supported OFX elements all have a matching container.  The others are assigned a OfxDummyContainer, so every OFX element creates a container as the file is par Note however that containers are destroyed as soon as the corresponding SGML element is closed.
*/
class OfxGenericContainer {
 public:
  string type;/**< The type of the object, often == tag_identifier */
  string tag_identifier; /**< The identifer of the creating tag */
  OfxGenericContainer *parentcontainer;
  
  OfxGenericContainer();
  OfxGenericContainer(OfxGenericContainer *para_parentcontainer);
  OfxGenericContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier);
  
  /** \brief Add data to a container object.
   *
   Must be called once completed parsing an OFX SGML data element.  The parent container should know what to do with it.
   \param identifier The name of the data element
   \param value The concatenated string of the data
  */ 
  virtual void add_attribute(const string identifier, const string value) = 0;
  virtual ~OfxGenericContainer(){};

  /// Returns the parent container object (the one representing the containing OFX SGML element)
    OfxGenericContainer* getparent()
      {
	return parentcontainer;
      };
};//End class OfxGenericObject

/** \brief A container to holds OFX SGML elements that LibOFX knows nothing about
 *
 The OfxDummyContainer is used for elements (not data elements) that are not recognised.  Note that recognised objects may very well be a children of an OfxDummyContainer.
*/
class OfxDummyContainer:public OfxGenericContainer {
 public:
  
  OfxDummyContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):OfxGenericContainer(para_parentcontainer, para_tag_identifier)
    {
      type="DUMMY";
      message_out(INFO, "Created OfxDummyContainer to hold unsupported aggregate "+para_tag_identifier);
    }
  void add_attribute(const string identifier, const string value);
};

/** \brief Represents the <STATUS> OFX SGML entity */
class OfxStatusContainer:public OfxGenericContainer {
 public:
  OfxStatusData data;
  
  OfxStatusContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):OfxGenericContainer(para_parentcontainer, para_tag_identifier)
    {
      memset(&data,0,sizeof(data));
      type="STATUS";
      if (parentcontainer!=NULL){
	strncpy(data.ofx_element_name, parentcontainer->tag_identifier.c_str(), OFX_ELEMENT_NAME_LENGTH);
	data.ofx_element_name_valid=true;
      }
      
    }
  void add_attribute(const string identifier, const string value);
};

/** \brief Represents the <BALANCE> OFX SGML entity 
 *
 OfxBalanceContainer is an auxiliary container (there is no matching data object in libofx.h)
*/
class OfxBalanceContainer:public OfxGenericContainer {
 public:
  /* Not yet complete see spec 1.6 p.63 */
  //char name[OFX_BALANCE_NAME_LENGTH];
  //char description[OFX_BALANCE_DESCRIPTION_LENGTH];
  //enum BalanceType{DOLLAR, PERCENT, NUMBER} balance_type;
  double amount; /**< Interpretation depends on balance_type */
  int amount_valid;
  time_t date; /**< Effective date of the given balance */
  int date_valid;
  
  OfxBalanceContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):OfxGenericContainer(para_parentcontainer, para_tag_identifier)
    {
      amount_valid=false;
      date_valid=false;
      type="BALANCE";
    }
  void add_attribute(const string identifier, const string value);
};

/** \brief  Represents a statement for either a bank account or a credit card account.
 *
 Can be built from either a <STMTRS> or a <CCSTMTRS> OFX SGML entity 
 */
class OfxStatementContainer:public OfxGenericContainer {
 public:
  OfxStatementData data;
  
  OfxStatementContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):OfxGenericContainer(para_parentcontainer, para_tag_identifier)
    {
      memset(&data,0,sizeof(data));
      type="STATEMENT";
    }
  void add_attribute(const string identifier, const string value);
  void add_account(OfxAccountData const account_data);
  void add_balance(OfxBalanceContainer* ptr_balance_container);
};

/** \brief  Represents a bank account or a credit card account.
 *
 Can be built from either a <BANKACCTFROM> or <CCACCTFROM> OFX SGML entity 
 */
class OfxAccountContainer:public OfxGenericContainer {
 public:
  OfxAccountData data;
  
  OfxAccountContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):OfxGenericContainer(para_parentcontainer, para_tag_identifier)
    {
      memset(&data,0,sizeof(data));
      type="ACCOUNT";
      strcpy(bankid,"");
      strcpy(branchid,"");
      strcpy(acctid,"");
      strcpy(acctkey,"");
      if (parentcontainer!=NULL&&((OfxStatementContainer*)parentcontainer)->data.currency_valid==true){
	strncpy(data.currency,((OfxStatementContainer*)parentcontainer)->data.currency,OFX_CURRENCY_LENGTH); /* In ISO-4217 format */
	data.currency_valid=true;
      }
    }
  void add_attribute(const string identifier, const string value);
  void gen_account_id(void);
 private:
  char bankid[OFX_BANKID_LENGTH];
  char branchid[OFX_BRANCHID_LENGTH];
  char acctid[OFX_ACCTID_LENGTH];/**< This field is used by both <BANKACCTFROM> and <CCACCTFROM> */
  char acctkey[OFX_ACCTKEY_LENGTH];
  
};

/** \brief  Represents a transaction.
 *
 Built from <STMTTRN> OFX SGML entity 
 */
class OfxTransactionContainer:public OfxGenericContainer {
 public:
  OfxTransactionData data;
  
  OfxTransactionContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):OfxGenericContainer(para_parentcontainer, para_tag_identifier)
    {
      memset(&data,0,sizeof(data));
      type="STMTTRN";
      if (parentcontainer!=NULL&&((OfxStatementContainer*)parentcontainer)->data.account_id_valid==true){
	strncpy(data.account_id,((OfxStatementContainer*)parentcontainer)->data.account_id,OFX_ACCOUNT_ID_LENGTH);
	data.account_id_valid = true;
      }
    }
  void add_attribute(const string identifier, const string value);

};


#endif
