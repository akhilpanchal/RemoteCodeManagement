///////////////////////////////////////////////////////////////////
// XmlElement.cpp - Defines XML Element types                     //
// ver 1.4                                                       //
// Application: Help for CSE687 Pr#2, Spring 2015                //
// Platform:    Dell XPS 2720, Win 8.1 Pro, Visual Studio 2013   //
// Source:      Jim Fawcett, CST 4-187, 443-3948                 //
//              jfawcett@twcny.rr.com                            //
// Author:      Akhil Panchal, Syracuse University			     //
//              (408) 921-0731, ahpancha@syr.edu	             //
///////////////////////////////////////////////////////////////////

#include "XmlElement.h"
#include <iostream>

using namespace XmlProcessing;

size_t AbstractXmlElement::count = 0;
size_t AbstractXmlElement::tabSize = 2;

//----< doc element constructor >-----------------------------
DocElement::DocElement()
{

}

//----< private helper for DocElement::addChild(...) >-----------------------
bool DocElement::hasXmlRoot()
{
	for (auto pElement : children_)
	{
		if (dynamic_cast<TaggedElement*>(pElement.get()) != nullptr)
			return true;
	}
	return false;
}
//----< private helper for DocElement::addChild(...) >-----------------------
bool DocElement::hasXmlDeclaration()
{
	for (auto pElement : children_)
	{
		if (dynamic_cast<XmlDeclarElement*>(pElement.get()) != nullptr)
			return true;
	}
	return false;
}
//----< add only one child to doc element using pointer to child >-----------

bool DocElement::addChild(std::shared_ptr<AbstractXmlElement> pChild)
{
	// don't add TextElements
	TextElement* pTxEl = dynamic_cast<TextElement*>(pChild.get());
	if (pTxEl != 0)  // don't add text elements to DocElement
		return false;

	// don't add another DocElement
	DocElement* pDcEl = dynamic_cast<DocElement*>(pChild.get());
	if (pDcEl != 0)  // don't add text elements to DocElement
		return false;

	// add ProcInstrElements and CommentElements 
	TaggedElement* te = dynamic_cast<TaggedElement*>(pChild.get());
	if (te == nullptr) // is not a TaggedElement
	{
		children_.push_back(pChild);
		return true;
	}

	// add only one XmlDeclrElement
	if (!hasXmlDeclaration())
	{
		children_.push_back(pChild);
		return true;
	}
	// add only one TaggedElement
	if (!hasXmlRoot())
	{
		children_.push_back(pChild);
		return true;
	}
	return false;
}
//----< remove child from doc element using pointer to child >---------------

bool DocElement::removeChild(std::shared_ptr<AbstractXmlElement> pChild)
{
	auto iter = std::find(begin(children_), end(children_), pChild);
	if (iter != end(children_))
	{
		children_.erase(iter);
		return true;
	}
	return false;
}
//----< return value = tag from doc element >-----------------------------

std::string DocElement::value() { return std::string(""); }

//----< generate xml string for tagged element >-----------------------------

std::string DocElement::toString()
{
	std::string rtn;
	for (auto elem : children_)
		rtn += elem->toString();
	return rtn;
}
//----< add child to tagged element using pointer to child >-----------------

bool TaggedElement::addChild(std::shared_ptr<AbstractXmlElement> pChild)
{
	children_.push_back(pChild);
	return true;
}
//----< remove child from tagged element using pointer to child >------------

bool TaggedElement::removeChild(std::shared_ptr<AbstractXmlElement> pChild)
{
	auto iter = std::find(begin(children_), end(children_), pChild);
	if (iter != end(children_))
	{
		children_.erase(iter);
		return true;
	}
	return false;
}
//----< remove child from tagged element using child's value >---------------

bool TaggedElement::removeChild(const std::string& value)
{
	for (auto iter = begin(children_); iter != end(children_); ++iter)
	{
		if ((*iter)->value() == value)
		{
			children_.erase(iter);
			return true;
		}
	}
	return false;
}
//----< add attribute to tagged element >------------------------------------

bool TaggedElement::addAttrib(const std::string& name, const std::string& value)
{
	std::pair<std::string, std::string> atPair(name, value);
	attribs_.push_back(atPair);
	return true;
}
//----< remove attribute from tagged element >-------------------------------

bool TaggedElement::removeAttrib(const std::string& name)
{
	for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
	{
		if (iter->first == name)
		{
			attribs_.erase(iter);
			return true;
		}
	}
	return false;
}
//----< return value = tag from tagged element >-----------------------------

std::string TaggedElement::value() { return tag_; }

//----< generate xml string for tagged element >-----------------------------

std::string TaggedElement::toString()
{
	std::string spacer(tabSize*++count, ' ');
	std::string xml = "\n" + spacer + "<" + tag_;
	for (auto at : attribs_)
	{
		xml += " ";
		xml += at.first;
		xml += "=\"";
		xml += at.second;
		xml += "\"";
	}
	xml += ">";
	for (auto pChild : children_)
		xml += pChild->toString();
	xml += "\n" + spacer + "</" + tag_ + ">";
	--count;
	return xml;
}

//----< generate xml string for text element >-------------------------------

std::string TextElement::toString()
{
	std::string spacer(tabSize * ++count, ' ');
	std::string xml = "\n" + spacer + text_;
	--count;
	return xml;
}
//----< add attribute to ProcInstElement >-----------------------------------

bool ProcInstrElement::addAttrib(const std::string& name, const std::string& value)
{
	std::pair<std::string, std::string> atPair(name, value);
	attribs_.push_back(atPair);
	return true;
}
//----< remove attribute from ProcInstElement >------------------------------

bool ProcInstrElement::removeAttrib(const std::string& name)
{
	for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
	{
		if (iter->first == name)
		{
			attribs_.erase(iter);
			return true;
		}
	}
	return false;
}
//----< generate xml string for text element >-------------------------------

std::string XmlDeclarElement::toString()
{
	std::string spacer(tabSize * ++count, ' ');
	std::string xml = "\n" + spacer + "<?xml";
	for (auto at : attribs_)
	{
		xml += " ";
		xml += at.first;
		xml += "=\"";
		xml += at.second;
		xml += "\"";
	}
	xml += "?>";
	--count;
	return xml;
}
//----< add attribute to ProcInstElement >-----------------------------------

bool XmlDeclarElement::addAttrib(const std::string& name, const std::string& value)
{
	std::pair<std::string, std::string> atPair(name, value);
	attribs_.push_back(atPair);
	return true;
}
//----< remove attribute from ProcInstElement >------------------------------

bool XmlDeclarElement::removeAttrib(const std::string& name)
{
	for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
	{
		if (iter->first == name)
		{
			attribs_.erase(iter);
			return true;
		}
	}
	return false;
}
//----< helper function displays titles >------------------------------------

void XmlProcessing::title(const std::string& title, char underlineChar)
{
	std::cout << "\n  " << title;
	std::cout << "\n " << std::string(title.size() + 2, underlineChar);
}

//----< generate xml string for processing instruction element >-------------------------------

std::string ProcInstrElement::toString()
{
	std::string spacer(tabSize*++count, ' ');
	std::string xml = "\n" + spacer + "<?" + type_;
	for (auto at : attribs_)
	{
		xml += " ";
		xml += at.first;
		xml += "=\"";
		xml += at.second;
		xml += "\"";
	}
	xml += "?>";
	--count;
	return xml;
}

//----< generate xml string for Comment element >-------------------------------

std::string CommentElement::toString()
{
	std::string spacer(tabSize*++count, ' ');
	std::string xml = "\n" + spacer + "<!--" + commentText_ + "-->";
	--count;
	return xml;
}

//----< test stub >----------------------------------------------------------

#ifdef TEST_XMLELEMENT

#include "XmlElementFactory.h"

int main()
{
	title("Testing XmlElement Package", '=');
	std::cout << "\n";

	using sPtr = std::shared_ptr < AbstractXmlElement >;

	sPtr root = XmlElementFactory::makeTaggedElement("root");
	root->addChild(XmlElementFactory::makeTextElement("this is a test"));

	sPtr child = XmlElementFactory::makeTaggedElement("child");
	child->addChild(XmlElementFactory::makeTextElement("this is another test"));
	child->addAttrib("first", "test");
	root->addChild(child);

	sPtr docEl = XmlElementFactory::makeDocElement();
	std::cout << "  " << docEl->toString();
	std::cout << "\n\n";
}

#endif


