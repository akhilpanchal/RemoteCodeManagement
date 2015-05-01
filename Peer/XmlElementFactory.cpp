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

#include"XmlElementFactory.h"

using namespace XmlProcessing;

//----< factory for doc elements >----------------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeDocElement()
{
	std::shared_ptr<AbstractXmlElement> ptr(new DocElement());
	return ptr;
}

//----< factory for tagged elements >----------------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeTaggedElement(const std::string& tag)
{
	std::shared_ptr<AbstractXmlElement> ptr(new TaggedElement(tag));
	return ptr;
}
//----< factory for text elements >------------------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeTextElement(const std::string& text)
{
	std::shared_ptr<AbstractXmlElement> ptr(new TextElement(text));
	return ptr;
}
//----< factory for comment elements >---------------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeCommentElement(const std::string& text)
{
	std::shared_ptr<AbstractXmlElement> ptr(new CommentElement(text));
	return ptr;
}
//----< factory for processing instruction elements >------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeProcInstrElement(const std::string& text)
{
	std::shared_ptr<AbstractXmlElement> ptr(new ProcInstrElement(text));
	return ptr;
}
//----< factory for XML Declaration elements >-------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeXmlDeclarElement()
{
	std::shared_ptr<AbstractXmlElement> ptr(new XmlDeclarElement());
	return ptr;
}

#ifdef TEST_XMLELEMENTFACTORY

#include "XmlElementFactory.h"

int main()
{
	title("Testing XmlElementFactory Package", '=');
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