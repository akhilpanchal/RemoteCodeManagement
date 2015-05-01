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

#include "XmlDocument.h"

using namespace XmlProcessing;

//-----<XmlDocument default constructor>---------------------------
XmlProcessing::XmlDocument::XmlDocument()
{
	pDocElement_ = XmlElementFactory::makeDocElement();
	sPtr xmlDec = XmlElementFactory::makeXmlDeclarElement();
	xmlDec->addAttrib("version", "1.0");
	xmlDec->addAttrib("encoding", "UTF-8");
	pDocElement_->addChild(xmlDec);
}

//-----<XmlDocument parameterized constructor>---------------------

XmlProcessing::XmlDocument::XmlDocument(const std::string& src, bool srcIsFile)
{
	src[0] == '<' ? srcIsFile = false : srcIsFile = true;
	std::string currTag; std::stack<sPtr> xmlElements;
	pDocElement_ = XmlElementFactory::makeDocElement();
	sPtr child, parent = pDocElement_; bool closeTag;
	try
	{
		
		Toker toker(src, srcIsFile); toker.setMode(Toker::xml);
		XmlParts parts(&toker);
		while (parts.get())
		{
			xmlTokens.clear();
			xmlTokens = parts.getTokens();
			for (i = 0; i < xmlTokens.size(); i++)
			{
				closeTag = false;
				if (i+3 < xmlTokens.size() && xmlTokens[i] == "<" && xmlTokens[i + 1] == "?" && xmlTokens[i + 2] == "xml" && xmlTokens[i + 3] != "-")	//Check if XML Declaration
					child = addXmlDeclaration();					
				else if (i+1 < xmlTokens.size() && xmlTokens[i] == "<" && xmlTokens[i + 1] == "!")	//Check if comment
					child = addCommentElement();
				else if (i+1 < xmlTokens.size() && xmlTokens[i] == "<" && xmlTokens[i + 1] == "?")	//Check if Proc Instruction				
					child = addProcInstrElement();
				else if (i+1 < xmlTokens.size() && xmlTokens[i] == "<" && xmlTokens[i + 1] == "/")	//Check if Closing Tag
				{
					if (xmlTokens[i + 2] != xmlElements.top()->value())
						throw std::exception("MALFORMED XML");
					closeTag = true; i += 4;
					xmlElements.pop();
					if (!xmlElements.empty())
						parent = xmlElements.top();
				}					
				else if (i < xmlTokens.size() && xmlTokens[i] == "<")								//Check if Tagged Element
				{
					closeTag = true; sPtr tagElem = addTaggedElement();
					parent->addChild(tagElem);
					xmlElements.push(tagElem);
					parent = tagElem;						
				}
				else														//If nothing, then Text Element				
					child = addTextElement();
				if (!closeTag)
					parent->addChild(child);
			}
		}
	}catch (std::exception ex)
	{
		throw std::exception(ex.what());
	}
}
//----< Helper function to the XmlDocument constructor. Adds XmlElement to the XML tree >-----
XmlDocument::sPtr XmlProcessing::XmlDocument::addXmlDeclaration()
{
	sPtr xmlDec = XmlElementFactory::makeXmlDeclarElement();
	i += 3;
	while (i < xmlTokens.size() && xmlTokens[i] != "?")
	{
		std::string value;
		for (size_t j = 0; j < xmlTokens[i + 2].length(); j++)
			if (xmlTokens[i + 2][j] != '\"' && xmlTokens[i + 2][j] != '\'')
				value += xmlTokens[i + 2][j];
		xmlDec->addAttrib(xmlTokens[i], value);
		i += 3;
	}
	i += 2;
	return xmlDec;
}
//----< Helper function to the XmlDocument constructor. Adds CommentElement to the XML tree >-----
XmlDocument::sPtr XmlProcessing::XmlDocument::addCommentElement()
{
	i += 3;
	std::string commText = "";
	while (i < xmlTokens.size() && xmlTokens[i] != "--")
	{
		commText += xmlTokens[i++];
		commText += " ";
	}
	commText[commText.size() - 1] = '\0';
	i += 2;
	sPtr commentElem = XmlElementFactory::makeCommentElement(commText);
	return commentElem;
}
//----< Helper function to the XmlDocument constructor. Adds ProcInstructElement to the XML tree >-----
XmlDocument::sPtr XmlProcessing::XmlDocument::addProcInstrElement()
{
	i += 2;
	std::string pIType = "";
	do
	{
		pIType += xmlTokens[i++];
	} while (i < xmlTokens.size() && xmlTokens[i + 1] != "=");
	sPtr pIElem = XmlElementFactory::makeProcInstrElement(pIType);
	while (i < xmlTokens.size() && xmlTokens[i] != "?")
	{
		std::string value;
		std::string attrib;
		size_t k;
		for (k = i; xmlTokens[k] != "="; k++)
		{
			attrib += xmlTokens[k];
			i++;
		}
		i++;

		for (size_t j = 0; j < xmlTokens[i].length(); j++)
			if (xmlTokens[i][j] != '\"' && xmlTokens[i][j] != '\'')
				value += xmlTokens[i][j];
		pIElem->addAttrib(attrib, value);
		i++;
	}
	i++;
	return pIElem;
}
//----< Helper function to the XmlDocument constructor. Adds TaggedElement to the XML tree >-----
XmlDocument::sPtr XmlProcessing::XmlDocument::addTaggedElement()
{
	i++;
	sPtr tagElem = XmlElementFactory::makeTaggedElement(xmlTokens[i++]);
	while (i < xmlTokens.size() && xmlTokens[i] != ">")
	{
		std::string value;
		std::string attrib;
		size_t k;
		for (k = i; i < xmlTokens.size() && xmlTokens[k] != "="; k++)
		{
			attrib += xmlTokens[k];
			i++;
		}
		i++;
		for (size_t j = 0; i < xmlTokens.size() && j < xmlTokens[i].length(); j++)
			if (xmlTokens[i][j] != '\"' && xmlTokens[i][j] != '\'')
				value += xmlTokens[i][j];
		tagElem->addAttrib(attrib, value);
		i++;
	}
	i++;
	return tagElem;
}

//----< Helper function to the XmlDocument constructor. Adds TextElement to the XML tree >-----

XmlDocument::sPtr XmlProcessing::XmlDocument::addTextElement()
{
	std::string text = "";
	while (i < xmlTokens.size() && xmlTokens[i] != "<")
	{
		text += xmlTokens[i++];
		text += " ";
	}
	text[text.size() - 1] = '\0';
	sPtr textElem = XmlElementFactory::makeTextElement(text);
	return textElem;
}

//----< Returns the XmlDocument Object Model as a string >--------

std::string XmlProcessing::XmlDocument::toString()
{
	if (pDocElement_ != nullptr)
		return pDocElement_->toString();
	return "Invalid XmlDocument";
}

//----------------------<XmlDocument move constructor>---------------------------

XmlProcessing::XmlDocument::XmlDocument(XmlDocument&& xDoc) : pDocElement_(xDoc.pDocElement_), found_(std::move(xDoc.found_))
{
	std::cout << "\nXDocument Move Constructor invoked!\n";
	xDoc.pDocElement_ = nullptr;
}

//-----------------<XmlDocument move assignment operator>---------------------

XmlDocument& XmlProcessing::XmlDocument::operator=(XmlDocument&& xDoc)
{
	if (this == &xDoc)
		return *this;
	found_ = xDoc.found_;
	pDocElement_.reset();
	pDocElement_ = xDoc.pDocElement_;
	xDoc.pDocElement_.reset();
	return *this;
}

//----< Returns the results of the prior operation performed on the XmlDocument>-----
std::vector<XmlDocument::sPtr> XmlProcessing::XmlDocument::select()
{
	return std::move(found_);
}

//----< Returns Elements with the specified tag >-----
XmlDocument& XmlProcessing::XmlDocument::elements(const std::string& tag)
{
	found_.clear();
	//Depth First Search to find all tags
	dfs(pDocElement_,tag);
	return *this;
	
}
//----< Helper function to elements(). Performs DFS on the XML tree >-----
void XmlProcessing::XmlDocument::dfs(sPtr node, const std::string& tag)
{
	for (auto child : node->getChildren())
	{
		if (child->value() == tag)
			found_.push_back(child);
		dfs(child, tag);
	}
}
//----< Returns first element in the Tree with the specified tag >-----
XmlDocument& XmlProcessing::XmlDocument::element(const std::string& tag)
{
	found_.clear();
	static bool tagFound;
	tagFound = false;
	bool &tf = tagFound;
	//Depth First Search to find the first tag
	dfs2(pDocElement_,tag, tf);
	return *this;
}
//----< Helper function to element(). Performs DFS on the XML tree >-----
void XmlProcessing::XmlDocument::dfs2(sPtr node, const std::string& tag, bool &tagFound)
{
	if (!tagFound)
		for (auto child : node->getChildren())
		{
			if (child->value() == tag && tagFound==false)
			{
				found_.push_back(child);
				tagFound = true;
				break;
			}
			else
				dfs2(child, tag, tagFound);
		}
}

//----< Returns children of the element in the Tree specified with the tag >-----
XmlDocument& XmlProcessing::XmlDocument::children(const std::string& tag)
{
	if (found_.empty())
	{
		return *this;
	}
	std::vector<sPtr> found = found_;
	found_.clear();
	for (auto f1 : found)
		for (auto f2 : f1->getChildren())
		{
			if (tag=="")
				found_.push_back(f2);
			else if(f2->value()==tag)
				found_.push_back(f2);
		}
	return *this;
}

//----< Returns descendents of the element in the Tree specified with the tag >-----
XmlDocument& XmlProcessing::XmlDocument::descendents(const std::string& tag)
{
	if (found_.empty())
	{
		std::cout << "Cannot find descendents! Element cannot hold descendents or Parent not specified!\n";
		return *this;
	}
	std::vector<sPtr> found = found_;
	found_.clear();

	for (auto f1 : found)
	{
		findDescendants(f1,tag);
	}
	return *this;
}
//----< Helper function to descendents(). Searches recursively for all descendents of the found children>-----
void XmlProcessing::XmlDocument::findDescendants(sPtr node, const std::string& tag)
{
	for (auto child : node->getChildren())
	{
		if (tag=="")
			found_.push_back(child);
		else if (child->value()==tag)
			found_.push_back(child);
		findDescendants(child, tag);
	}
}

//-------<Checks for the id attribute of the xml elements and stores it in found_[0]>-----------------
XmlDocument& XmlProcessing::XmlDocument::getElementById(const std::string& attrib, const std::string& value)
{
	found_.clear();
	sPtr elemFound;
	elemFound = dfs3(pDocElement_, attrib, value, nullptr);
	found_.push_back(elemFound);
	return *this;
}

//----< Helper function to getElementById(). Performs DFS on the XML tree >-----
XmlDocument::sPtr XmlProcessing::XmlDocument::dfs3(sPtr node, const std::string& attrib, const std::string& value, sPtr elemFound)
{
	for (auto child : node->getChildren())
	{
		std::vector<std::pair<std::string, std::string>> attribs = child->getAttributes();
		for (auto attr = attribs.begin(); attr != attribs.end(); attr++)
			//id attribute is unique across the Xml Document
			if (attr->first == attrib && attr->second == value /*&& elemFound == nullptr*/)
			{
				elemFound = child;
				break;
			}
		if (elemFound==nullptr)
			elemFound = dfs3(child, attrib, value, elemFound);
	}
	return elemFound;
}

//-------< Adds a root element to the XmlDocument with the tagname as specfied in the parameter >-----------------

bool XmlDocument::addRootElement(const std::string& tag)
{
	if (tag != "")
		return pDocElement_->addChild(XmlElementFactory::makeTaggedElement(tag));
	return false;
}

//-------< Adds a root element to the XmlDocument represented by the smart pointer parameter >-----------------

bool XmlDocument::addRootElement(sPtr root)
{
	if (root != nullptr)
		return pDocElement_->addChild(root);
	return false;
}

//-------< Adds a child to the parent specified by pointers in the parameter or from the prior found_[0] result>-----------------

bool XmlDocument::addChild(sPtr child, sPtr parent)
{
	if (parent == nullptr && !found_.empty())
		if (found_[0]!=nullptr)
			return found_[0]->addChild(child);
	else if(parent)
		return parent->addChild(child);
	return false;
}

//-------< Removes the child  with the tagname as specified in the parameter of the parent found in the previous found_[0] result >-----------------

bool XmlDocument::removeChild(const std::string& tag)
{
	if (!found_.empty())
	{
		if (found_[0] != nullptr && tag == "")	//remove all children
		{
			for (auto child : found_[0]->getChildren())
				found_[0]->removeChild(child);
			return true;
		}
		else if (found_[0] != nullptr && tag != "")
			return found_[0]->removeChild(tag);
		return false;
	}
	return false;
}

//--< Removes the child  represented by the smart pointer specified in the parameter of the parent found in the previous found_[0] result >--

bool XmlDocument::removeChild(sPtr child)
{
	if (child==nullptr || found_.empty())
		return false;
	else if (found_[0]!=nullptr)
		return found_[0]->removeChild(child);
	return false;
}

//----< Returns a vector consisting of attribute-value pair of a element pointed to by the smart pointer passed in the parameter >----

std::vector<std::pair<std::string, std::string>> XmlDocument::attributes(sPtr elem)
{
	if (elem != nullptr)
		return elem->getAttributes();
	else
	{
		std::vector<std::pair<std::string, std::string>> x;
		return x;
	}
}

//----< Returns a vector consisting of smartpointers to the children of the element pointed to by the smart pointer passed in the parameter >----

std::vector<XmlDocument::sPtr> XmlDocument::children(sPtr elem)
{
	if (elem != nullptr)
		return elem->getChildren();
	else
	{
		std::vector<sPtr> x;
		return x;
	}
}

//----< Adds a given attribute-value pair to an element pointed to by the smart pointer passed in the parameter >----

bool XmlDocument::addAttribute(const std::string& attrib, const std::string& value, sPtr element)
{
	if (!found_.empty())
		if (found_[0] != nullptr)
			return found_[0]->addAttrib(attrib, value);
	else if (element)
		return element->addAttrib(attrib, value);
	return false;
}

//----< Removes a given attribute of an element pointed to by the smart pointer passed in the parameter >----

bool XmlDocument::removeAttribute(const std::string& attrib, sPtr element)
{
	if (!found_.empty())
		if (found_[0] != nullptr)
			return found_[0]->removeAttrib(attrib);
	else if (element)
		return element->removeAttrib(attrib);
	return false;
}

//----< Writes XmlDocument into a file specified in the parameter >----
bool XmlDocument::toFile(std::string filename)
{
	std::ofstream myfile;
	if (&filename != nullptr && filename != "")
	try
	{
		myfile.open(filename);
		myfile << this->toString();
		myfile.close();
		return true;
	}
	catch (std::exception ex)
	{
		std::cout << "Exception caught: " << ex.what();
		return false;
	}
	return false;
}



#ifdef TEST_XMLDOCUMENT

int main()
{
	title("Testing XmlDocument class");

	XmlDocument doc("../Test/LectureNote.xml");
	d.addRootElement("tagname1");
	cout << "d = " << d.toString();
	std::cout << "\n\n";
}

#endif