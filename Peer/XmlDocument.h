#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H
///////////////////////////////////////////////////////////////////
// XmlDocument.cpp - a container of XmlElement nodes             //
// Ver 1.3                                                       //
// Application: Help for CSE687 Pr#2, Spring 2015                //
// Platform:    Dell XPS 2720, Win 8.1 Pro, Visual Studio 2013   //
// Source:      Jim Fawcett, CST 4-187, 443-3948                 //
//              jfawcett@twcny.rr.com                            //
// Author:      Akhil Panchal, Syracuse University			     //
//              (408) 921-0731, ahpancha@syr.edu	             //
///////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is intended to help students in CSE687 - Object Oriented Design
* get started with Project #2 - XML Document Model.  It uses C++11 constructs,
* most noteably std::shared_ptr.  The XML Document Model is essentially
* a program-friendly wrapper around an Abstract Syntax Tree (AST) used to
* contain the results of parsing XML markup.
*
* Abstract Syntax Trees, defined in this package, are unordered trees with
* two types of nodes:
*   Terminal nodes     - nodes with no children
*   Non-Terminal nodes - nodes which may have a finite number of children
* They are often used to contain the results of parsing some language.
*
* The elements defined in the companion package, XmlElement, will be used in
* the AST defined in this package.  They are:
*   AbstractXmlElement - base for all the XML Element types
*   DocElement         - XML element with children designed to hold prologue, Xml root, and epilogue
*   TaggedElement      - XML element with tag, attributes, child elements
*   TextElement        - XML element with only text, no markup
*   CommentElement     - XML element with comment markup and text
*   ProcInstrElement   - XML element with markup and attributes but no children
*   XmlDeclarElement   - XML declaration element with attributes
*
* Public Interface:
* ---------------
* XmlDocument();													
*	Creates an empty XmlDocument
*
* XmlDocument(const std::string& src, bool srcIsFile = true)
*	Creates XmlDocument from given file or string
*
* XmlDocument(XmlDocument&& xDoc);	
*	move constructor for XmlDocument
*
* XmlDocument& operator=(XmlDocument&& xDoc)						
* 	move assignment for XmlDocument
*
* XmlDocument& element(const std::string& tag)
*	finds first element (DFS order) with given tag
*
* XmlDocument& elements(const std::string& tag)
*	finds all elements with given tag
*
* XmlDocument& children(const std::string& tag = "")     
*	finds sPtrs to children of XDocument with given tag
*
* XmlDocument& descendents(const std::string& tag = "") 
*	finds sPtrs to descendents of XDocument with given tag
*
* XmlDocument& getElementById(const std::string& attrib = "", const std::string& value = "")
*	Returns sPtr to the unique element with the given id 
*	attribute value if exists.Else null
*
* std::vector<sPtr> select()			
*	return reference to found_.  Use std::move(found_) to clear found_
*
* bool addRootElement(const std::string& tag)
*	adds root element with tagname to empty xmldocument
*
* bool addRootElement(sPtr root);						
*	adds root element by node pointer specified to empty xmldocument
*
* bool addChild(sPtr child, sPtr parent = nullptr)	
*	adds child to a node
*
* bool removeChild(const std::string& tag="")	
*	removes child with specified tag from a node
*
* bool removeChild(sPtr child)		
*	removes child pointer specified from a node
*
* bool addAttribute(const std::string& attrib, const std::string& value, sPtr element = nullptr)
*	adds attribute to a node int tree
*
* bool removeAttribute(const std::string& attrib, sPtr element = nullptr)
*	remove attribute from a node
*
* std::vector<std::pair<std::string, std::string>> attributes(sPtr elem)
*	Returns attributes of a node in tree
*
* std::vector<sPtr> children(sPtr elem)
*	Returns children of a node in tree.
*
* std::string toString()					
*	returns xml string of the XmlDocument
*
* bool toFile(std::string filename)						
*	stores the XmlDocument tree in the specified file
*
* Required Files:
* ---------------
*   - XmlDocument.h, XmlDocument.cpp, IXmlElement.h,
*	  XmlElementfactory.h, XmlElementfactory.cpp,
*	  xmlElementParts.h, xmlElementParts.cpp
*
* Build Process:
* --------------
*   cl / EHa / DTEST_XMLDOCUMENT XmlDocument.cpp
*
* Maintenance History:
* --------------------
* ver 1.3 : 19 Mar 15
* - added public interface implementation for all requirements listed on
*	http://ecs.syr.edu/faculty/fawcett/handouts/CSE687/projects/pr2s15.pdf
* ver 1.2 : 21 Feb 15
* - modified these comments
* ver 1.1 : 14 Feb 15
* - minor changes to comments, method arguments
* Ver 1.0 : 11 Feb 15
* - first release
*
*/

#include <stack>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "xmlElementParts.h"
#include "IXmlElement.h"
#include "XmlElementFactory.h"

namespace XmlProcessing
{
	class XmlDocument
	{
	public:
		using sPtr = std::shared_ptr < AbstractXmlElement >;
		XmlDocument();													// Default constructor for programmatic creation of xml files
		XmlDocument(const std::string& src, bool srcIsFile = true);		// Parameterized constructor
		XmlDocument(XmlDocument&& xDoc);								// move constructor for XmlDocument
		XmlDocument& operator=(XmlDocument&& xDoc);						// move assignment for XmlDocument
		
		virtual XmlDocument& element(const std::string& tag);           // found_[0] contains first element (DFS order) with tag
		virtual XmlDocument& elements(const std::string& tag);          // found_ contains all elements with tag
		virtual XmlDocument& children(const std::string& tag = "");     // found_ contains sPtrs to children of prior found_[0] 
		virtual XmlDocument& descendents(const std::string& tag = "");  // found_ contains sPtrs to descendents of prior found_[0]
		virtual XmlDocument& getElementById(const std::string& attrib = "", const std::string& value = "");
																		// returns sPtr to the unique element with the given id 
																		// attribute value if exists.Else null
		virtual std::vector<sPtr> select();								// return reference to found_.  Use std::move(found_) to clear found_
		virtual bool addRootElement(const std::string& tag);			// adds root element with tagname to empty xmldocument
		virtual bool addRootElement(sPtr root);							// adds root element by node pointer specified to empty xmldocument
		virtual bool addChild(sPtr child, sPtr parent = nullptr);		// adds child to a node
		virtual bool removeChild(const std::string& tag="");			// removes child with specified tag from a node
		virtual bool removeChild(sPtr child);							// removes child pointer specified from a node
		virtual bool addAttribute(const std::string& attrib, const std::string& value, sPtr element = nullptr);
																		// adds attribute to a node int tree
		virtual bool removeAttribute(const std::string& attrib, sPtr element = nullptr);
																		// remove attribute from a node
		virtual std::vector<std::pair<std::string, std::string>> attributes(sPtr elem);
																		// get attributes of a node in tree
		virtual std::vector<sPtr> children(sPtr elem);					// get children of a node in tree.
		virtual std::string toString();									// returns xml string of the XmlDocument
		virtual bool toFile(std::string filename);						// stores the XmlDocument tree in the specified file

	private:
		sPtr pDocElement_;												// AST that holds procInstr, comments, XML root, and more comments
		std::vector<sPtr> found_;										// query results

		sPtr addXmlDeclaration();										// helper function to tree generation: XmlDocument constructor
		sPtr addCommentElement();										// helper function to tree generation: XmlDocument constructor
		sPtr addProcInstrElement();										// helper function to tree generation: XmlDocument constructor
		sPtr addTaggedElement();										// helper function to tree generation: XmlDocument constructor
		sPtr addTextElement();											// helper function to tree generation: XmlDocument constructor

		void dfs(sPtr node, const std::string& tag);					// helper function to elements()
		void dfs2(sPtr node, const std::string& tag, bool &tagFound);	// helper function to element()
		sPtr dfs3(sPtr node, const std::string& attrib, const std::string& value, sPtr elemFound);
																		// helper function to getElementById()
		void findDescendants(sPtr node, const std::string& tag);
		std::vector < std::string > xmlTokens;							// vector of tokens from xml string
		size_t i;														// iterator over xml tokens vector
	};
}
#endif
