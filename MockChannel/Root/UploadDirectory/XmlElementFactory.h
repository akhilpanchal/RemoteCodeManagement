#ifndef XMLELEMENTFACTORY_H
#define XMLELEMENTFACTORY_H
/////////////////////////////////////////////////////////////////////////////
// XmlElementFactory.cpp - Provides Object Factory for different types of  //
//							Xml Elements								   //
// version 1.0                                                             //
// ----------------------------------------------------------------------- //
// Copyright © Akhil Panchal, 2015                                         //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2013 Ultimate                    //
// Platform:    Dell Inspiron 17R 5721, Core i5, Windows 8.1			   //
// Application: CSE 687 Project #1, Spring 2015                            //
// Author:      Akhil Panchal, Syracuse University			               //
//              (408) 921-0731, ahpancha@syr.edu	                       //
/////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is a factory that creates concrete Xml Element objects,
* wraps them around a smart pointer of the AbstractXmlElement base class
* and returns smart pointer to the caller.
*
* Public Interface:
* ---------------
* static std::shared_ptr<AbstractXmlElement> makeDocElement();
*	creates Document Element which is the root of the XML Tree respresentation.
*
* static std::shared_ptr<AbstractXmlElement> makeTaggedElement(const std::string& tag);
*	creates a Xml Tagged element.
*
* static std::shared_ptr<AbstractXmlElement> makeTextElement(const std::string& text);
*	creates a Xml Text element.
*
* static std::shared_ptr<AbstractXmlElement> makeCommentElement(const std::string& text);
*	creates a Xml Comment element.
*
* static std::shared_ptr<AbstractXmlElement> makeProcInstrElement(const std::string& text);
*	creates a Xml Processing Instruction element.
*
* static std::shared_ptr<AbstractXmlElement> makeXmlDeclarElement();
*	creates a Xml Declaration element.
*
* Required Files:
* ---------------
*   XmlElement.h, XmlElement.cpp
*	XmlElementFactory.h, XmlElementFactory.cpp
*
* Build Process:
* --------------
* cl /EHa /DTEST_XMLELEMENTFACTORY XmlElementFactory.cpp
*
* Maintenance History:
* --------------------
* ver 1.0 : 16 Mar 15
* - first release
* Moved global factory methods from XmlElement.h and encapsulated them 
* in XmlElementFactory class in this package.
*
*/


#include"../XmlElement/XmlElement.h"

class XmlElementFactory
{
public:
	//----< factory for doc elements >----------------------------------------
	static std::shared_ptr<AbstractXmlElement> makeDocElement();

	//----< factory for tagged elements >----------------------------------------
	static std::shared_ptr<AbstractXmlElement> makeTaggedElement(const std::string& tag);

	//----< factory for text elements >------------------------------------------
	static std::shared_ptr<AbstractXmlElement> makeTextElement(const std::string& text);

	//----< factory for comment elements >---------------------------------------
	static std::shared_ptr<AbstractXmlElement> makeCommentElement(const std::string& text);
	
	//----< factory for processing instruction elements >------------------------
	static std::shared_ptr<AbstractXmlElement> makeProcInstrElement(const std::string& text);

	//----< factory for XML Declaration elements >-------------------------------
	static std::shared_ptr<AbstractXmlElement> makeXmlDeclarElement();

};

#endif