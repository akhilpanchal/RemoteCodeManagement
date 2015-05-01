#ifndef XMLELEMENT_H
#define XMLELEMENT_H
///////////////////////////////////////////////////////////////////
// XmlElement.h - define XML Element types                       //
// ver 1.6                                                       //
// Application: Help for CSE687 Pr#2, Spring 2015                //
// Platform:    Dell XPS 2720, Win 8.1 Pro, Visual Studio 2013   //
// Author:      Jim Fawcett, CST 4-187, 443-3948                 //
//              jfawcett@twcny.rr.com                            //
// Author:      Akhil Panchal, Syracuse University				 //
//              (408) 921-0731, ahpancha@syr.edu				 //
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
* Abstract Syntax Trees are unordered trees with two types of nodes:
*   Terminal nodes     - nodes with no children
*   Non-Terminal nodes - nodes which may have a finite number of children
* They are often used to contain the results of parsing some language.
*
* The elements defined in this package will be used in the AST defined in
* the AbstractSyntaxTree package, included in this Visual Studio 2013
* solution.
*   AbstractXmlElement - base for all the XML Element types
*   DocElement         - XML element with children designed to hold prologue, Xml root, and epilogue
*   TaggedElement      - XML element with tag, attributes, child elements
*   TextElement        - XML element with only text, no markup
*   CommentElement     - XML element with comment markup and text
*   ProcInstrElement   - XML element with markup and attributes but no children
*   XmlDeclarElement   - XML declaration
*
* Public Interface:
* ---------------
* Accessed via AbstractXmlElement abstract class. Refer to IXmlElement.h for public interface
*
* Required Files:
* ---------------
*   - XmlElement.h, XmlElement.cpp, IXmlElement.h
*
* Build Process:
* --------------
*   cl /EHa /DTEST_XMLELEMENT XmlElement.cpp
*
* Maintenance History:
* --------------------
* ver 1.6 : 16 Mar 15
*	Moved the AbstactXmlElement abstract class and its interface definitions
*	to IXmlElement.h
*	Created a separate Factory package for concrete xml element types defined here.
* ver 1.5 : 22 Feb 15
* - brought comments up to date
* ver 1.4 : 21 Feb 15
* - Added XmlDeclElement.  It has the syntax of a ProcInstrElement but has the
*   constraint that if present it must be the first child of the document.  The
*   best way to handle this seemed to be to create anther element type and have
*   the XmlDocument constructor always add as the first child.
* - Fixed a bug in DocElement::addChild found by Venkat Subha Rao Cheedella.
*   addChild now correctly handles allowing as many comments and ProcInstructions
*   as needed but only one tagged element as the root of the XML information.
* ver 1.3 : 17 Feb 15
* - Removed move ctor and move assign declar comments in all classes.
*   Move construction and move assignment is needed only by the XmlDocument class
* - added addAttribute and removeAttribute to the ProcInstrElement class
* - fixed a bug in the DocElement::addChild method that prevented multiple
*   ProcInstrElements and CommentElements from being added.
* - Scott Constable pointed out two of these three problems to me earlier today.
*   Thanks Scott!
* ver 1.2 : 14 Feb 15
* - removed removeChild(const std::string& value);
* - declared deleted copy constructor and copy assignment
* - added comment declaring move constructor and move assignment
* - added DocElement - I decided that Scott Constable was right about this
* - fixed flaw in declaration of ProcInstrElement attribs_ member
* - return string by value from AbstractXmlElement::value() and all derived elements
* ver 1.1 : 12 Feb 15
* - modified Element::toString() to improve whitespace handling
* - added attribute test in test stub
* Ver 1.0 : 11 Feb 15
* - first release
*/

#include <memory>
#include <string>
#include <vector>
#include "IXmlElement.h"

namespace XmlProcessing
{
	
	/////////////////////////////////////////////////////////////////////////////
	// DocElement - holds the document prologue, XML tree, and epilog

	class DocElement : public AbstractXmlElement
	{
	public:
		DocElement();
		~DocElement() {}
		DocElement(const DocElement& doc) = delete;
		DocElement& operator=(const DocElement& doc) = delete;
		virtual bool addChild(std::shared_ptr<AbstractXmlElement> pChild);
		virtual bool removeChild(std::shared_ptr<AbstractXmlElement> pChild);
		virtual std::string value();
		virtual std::string toString();
		virtual std::vector<std::shared_ptr<AbstractXmlElement>> getChildren(){ return children_; }
	private:
		bool hasXmlRoot();
		bool hasXmlDeclaration();
		std::vector<std::shared_ptr<AbstractXmlElement>> children_;
	};

	//std::shared_ptr<AbstractXmlElement> makeDocElement(std::shared_ptr<AbstractXmlElement> pRoot = nullptr);

	/////////////////////////////////////////////////////////////////////////////
	// TextElement - represents the text part of an XML element

	class TextElement : public AbstractXmlElement
	{
	public:
		TextElement(const std::string& text) : text_(text) {}
		virtual ~TextElement() {}
		TextElement(const TextElement& te) = delete;
		TextElement& operator=(const TextElement& te) = delete;
		virtual std::string value();
		virtual std::string toString();
	private:
		std::string text_;
	};

	inline std::string TextElement::value() { return text_; }

	//std::shared_ptr<AbstractXmlElement> makeTextElement(const std::string& text);

	/////////////////////////////////////////////////////////////////////////////
	// Element - represents a tagged element with attributes and child elements

	class TaggedElement : public AbstractXmlElement
	{
	public:
		TaggedElement(const std::string& tag) : tag_(tag) {};
		TaggedElement(const TaggedElement& te) = delete;
		virtual ~TaggedElement() {}
		TaggedElement& operator=(const TaggedElement& te) = delete;
		virtual bool addChild(std::shared_ptr<AbstractXmlElement> pChild);
		virtual bool removeChild(std::shared_ptr<AbstractXmlElement> pChild);
		virtual bool removeChild(const std::string& value);
		virtual bool addAttrib(const std::string& name, const std::string& value);
		virtual bool removeAttrib(const std::string& name);
		virtual std::string value();
		virtual std::string toString();
		virtual std::vector<std::shared_ptr<AbstractXmlElement>> getChildren(){ return children_; }
		virtual std::vector<std::pair<std::string, std::string>> getAttributes(){ return attribs_; }
	private:
		std::string tag_;
		std::vector<std::shared_ptr<AbstractXmlElement>> children_;
		std::vector<std::pair<std::string, std::string>> attribs_;
	};


	/////////////////////////////////////////////////////////////////////////////
	// CommentElement - represents XML comments, e.g., <!-- comment text -->
	class CommentElement : public AbstractXmlElement
	{
	public:
		CommentElement(const std::string& text) : commentText_(text) {}
		CommentElement(const CommentElement& ce) = delete;
		virtual ~CommentElement() {}
		CommentElement& operator=(const CommentElement& ce) = delete;
		virtual std::string value() { return commentText_; }
		virtual std::string toString();
	private:
		std::string commentText_ = "to be defined";
	};

	////////////////////////////////////////////////////////////////////////////////////
	// ProcInstrElement - represents XML Processing Instructions, e.g., <?xml-stylesheet version="1.0"?>
	
	class ProcInstrElement : public AbstractXmlElement
	{
	public:
		ProcInstrElement(const ProcInstrElement& pe) = delete;
		ProcInstrElement(const std::string& type) : type_(type) {}
		ProcInstrElement& operator=(const ProcInstrElement& pe) = delete;
		virtual bool addAttrib(const std::string& name, const std::string& value);
		virtual bool removeAttrib(const std::string& name);
		virtual std::string value() { return type_; }
		virtual std::string toString();
		//"changes"
		virtual std::vector<std::pair<std::string, std::string>> getAttributes(){ return attribs_; }
	private:
		std::vector<std::pair<std::string, std::string>> attribs_;
		std::string type_ = "xml declaration";
		std::string text;
	};

	//std::shared_ptr<AbstractXmlElement> makeProcInstrElement(const std::string& type);

	/////////////////////////////////////////////////////////////////////////////
	// XmlDeclarElement - <?xml version="1.0"?>

	class XmlDeclarElement : public AbstractXmlElement
	{
	public:
		XmlDeclarElement(const ProcInstrElement& pe) = delete;
		XmlDeclarElement() {}
		XmlDeclarElement& operator=(const ProcInstrElement& pe) = delete;
		virtual bool addAttrib(const std::string& name, const std::string& value);
		virtual bool removeAttrib(const std::string& name);
		virtual std::string value() { return ""; }
		virtual std::string toString();
		virtual std::vector<std::pair<std::string, std::string>> getAttributes(){ return attribs_; }
	private:
		std::vector<std::pair<std::string, std::string>> attribs_;
		std::string type_ = "xml declaration";
	};

	void title(const std::string& title, char underlineChar = '-');
}
#endif
