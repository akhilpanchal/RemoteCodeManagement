#ifndef IXMLELEMENT_H
#define IXMLELEMENT_H

/////////////////////////////////////////////////////////////////////////////
// IXmlElement.h - Abstract base class for different Xml element types	   //
// version 1.0                                                             //
// ----------------------------------------------------------------------- //
// Copyright © Akhil Panchal, 2015                                         //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2013 Ultimate                    //
// Platform:    Dell Inspiron 17R 5721, Intel Core i5, Windows 8.1		   //
// Application: CSE 687 Project #1, Spring 2015                            //
// Author:      Akhil Panchal, Syracuse University			               //
//              (408) 921-0731, ahpancha@syr.edu	                       //
/////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* -------------------
* This module defines the AbstractXmlElement base class.
* It defines some of the common default operations for Xml Elements.
*
* Public Interface:
* ---------------
* virtual bool addChild(std::shared_ptr<AbstractXmlElement> pChild);
*	accepts a shared pointer to the abstract base class and adds the 
*	pointer to the appropriate concrete xml element pointed to by the
*	calling shared pointer.
*	If the concrete node cannot hold children, the default behavior
*	is seen as defined in this abstract base class: returns false
*
* virtual bool removeChild(std::shared_ptr<AbstractXmlElement> pChild);
*	accepts a shared pointer to the abstract base class and removes that
*	pointer from the children of the concrete xml element pointed to by
*	the calling shared pointer.
*	If the concrete node doesn't hold any children, the default behavior
*	is seen as defined in this abstract base class: returns false

* virtual bool removeChild(const std::string& tag);
*	accepts a string tagname and searches for the child with that tagname
*	in the list of children of the concrete xml element pointed to by
*	the calling shared pointer.
*	If the concrete node doesn't hold any children, the default behavior
*	is seen as defined in this abstract base class: returns false
*
* virtual bool addAttrib(const std::string& name, const std::string& value);
*	adds the attribute-value pair provided as parameters to the concrete xml 
*	element pointed to by the calling shared pointer.
*	If the concrete node cannot hold any attributes, the default behavior
*	is seen as defined in this abstract base class: returns false
*
* virtual bool removeAttrib(const std::string& name);
*	removes the attribute provided as paramenter to the concrete xml
*	element pointed to by the calling shared pointer.
*	If the concrete node cannot hold any attributes, the default behavior
*	is seen as defined in this abstract base class: returns false
*
* virtual std::string value() = 0;
*	Pure virtual function that the derived classes MUST redefine
*	returns a string thaat represents the tag for a tagged Xml Element
*	and returns the entire text for text and comment elements.
*
* virtual std::string toString() = 0
*	Pure virtual function that the derived classes MUST redefine
*	returns a string representation of the Xml Element.
*
* virtual std::vector<std::shared_ptr<AbstractXmlElement>> getChildren();
*	returns a vector of smart pointers to the children of the concrete xml
*	element.
*	If the xml element type cannot hold children, executes default behavior
*	as defined in this file. returns empty vector.
*
* virtual std::vector<std::pair<std::string, std::string>> getAttributes();
*	returns a vector of attribute-value pairs of the concrete xml
*	element.
*	If the xml element type cannot hold attributes, executes default behavior
*	as defined in this file. returns empty vector.
*
* Required Files:
* ---------------
*   IXmlElement.h
*
* Build Process:
* --------------
*   Abstract base class only so no build.
*
* Maintenance History:
* --------------------
* ver 1.0 : 16 Mar 15
* - first release
* Moved AbstractXmlElement class from XmlElement.h to this package and
* added the following interfaces and their definitions:
*	getChildren();
*	getAttributes();
*/

/////////////////////////////////////////////////////////////////////////////
// AbstractXmlElement - base class for all concrete element types

class AbstractXmlElement
{
public:
	virtual bool addChild(std::shared_ptr<AbstractXmlElement> pChild);			// adds child to a node in xml
	virtual bool removeChild(std::shared_ptr<AbstractXmlElement> pChild);		// removes given child (smartpointer) from a node in xml
	virtual bool removeChild(const std::string& tag);							// removes given child (tagname) from a node in xml
	virtual bool addAttrib(const std::string& name, const std::string& value);	// adds attribute to an element in xml
	virtual bool removeAttrib(const std::string& name);							// removes attribute to an element in xml
	virtual std::string value() = 0;											// returns the tagname or the entire text for an xml element.
	virtual std::string toString() = 0;											// returns the entire string representation of an xml element
	virtual ~AbstractXmlElement();												// destroys xml element
	virtual std::vector<std::shared_ptr<AbstractXmlElement>> getChildren();		// returns children of an xml element
	virtual std::vector<std::pair<std::string, std::string>> getAttributes();	// returns attributes of an element
protected:
	static size_t count;
	static size_t tabSize;
};

//------< Default Behavior for Xml Element : Returns false or Empty results >------

inline bool AbstractXmlElement::addChild(std::shared_ptr<AbstractXmlElement> pChild) { return false; }
inline bool AbstractXmlElement::removeChild(std::shared_ptr<AbstractXmlElement> pChild) { return false; }
inline bool AbstractXmlElement::removeChild(const std::string& tag) { return false; }
inline bool AbstractXmlElement::addAttrib(const std::string& name, const std::string& value) { return false; }
inline bool AbstractXmlElement::removeAttrib(const std::string& name) { return false; }
inline AbstractXmlElement::~AbstractXmlElement() {}
inline std::vector<std::shared_ptr<AbstractXmlElement>> AbstractXmlElement::getChildren(){ std::vector<std::shared_ptr<AbstractXmlElement>> x; return x; }
inline std::vector<std::pair<std::string, std::string>> AbstractXmlElement::getAttributes(){ std::vector<std::pair<std::string, std::string>> x; return x; }
#endif