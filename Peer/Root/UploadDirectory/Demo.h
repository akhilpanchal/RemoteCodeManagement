#ifndef DEMO_H
#define DEMO_H

/////////////////////////////////////////////////////////////////////////////
// Demo.h:		Demonstrates usage of XmlDocument package				   //
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
/* Package Operations :
*------------------ -
* This package acts as a Test client which uses the XmlDocument package.
* This package demostrates the ways in which the public interfaces of the
* XmlDocument class can be used and thus demostrates the requirements for
* the Project.
*
* Public Interface :
*---------------
* void demostrateXmlDocument(int argc, char** args);
*	demostrates the requirements of Project 2.
*	Accepts command linearguments passed by the Executive.
*
* Required Files :
*-------------- -
*	XmlDocument.h, XmlDocument.cpp
*	Demo.h, Demo.cpp
*
* Build Process :
*--------------
* cl / EHa / DTEST_DEMO Demo.cpp
*
* Maintenance History :
*--------------------
* ver 1.0 : 16 Mar 15
* -first release
*
*/

#include<iostream>
#include<memory>
#include"XmlDocument.h"

using namespace XmlProcessing;
	class Demo
	{
		using XElement = std::shared_ptr < AbstractXmlElement >;

	public:
		void demonstrateXmlDocument(int argc, char** args);	// demonstrates XmlDocument class usage
	private:
		void moveSemantics(XmlDocument& xdoc);				// demonstrates move semantics
		void findElementbyID(XmlDocument& xdoc);			// demonstrates finding element in Xml tree
		void findElements(XmlDocument& xdoc);				// demonstrates finding a collection of elements in tree with tagname
		void addRemoveChild(XmlDocument& xdoc);				// demonstrates addition and removal of child from a tree
		void getAttribs(XmlDocument& xdoc);					// demonstrates retrieving attributes of a element in tree
		void addRemoveAttribs(XmlDocument& xdoc);			// demonstrates addition and removal of attributes from a node in tree
		void writeXmltoFile(XmlDocument& xdoc);				// demonstrates writing XmlDocument to a file.
		void extraReqs(XmlDocument& xdoc);					// demonstrates descendents() method: finding descendents of a node in tree
		XmlDocument x;
	};

#endif