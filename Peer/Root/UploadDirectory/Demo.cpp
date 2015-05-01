/////////////////////////////////////////////////////////////////////////////
// Demo.cpp:	Demonstrates usage of XmlDocument package				   //
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
#include"Demo.h"
using namespace std;

//----< Demonstrates usage of XmlDocument class >-----

void Demo::demonstrateXmlDocument(int argc, char** args)
{
	try
	{
		cout << "\n====================TESTING REQUIREMENT 3====================\n\n";
		string xmlstring = "< ? xml version = \"1.0\" ?> <root id= \"873\">Some Text\n<university id=\"12\">Syracuse</university>";
		xmlstring += "<region id =\"\">Upstate New York</region></root>";
		cout << "Building xml tree from String: " << xmlstring << endl;
		XmlDocument xdoc1(xmlstring);
		std::cout << "\nxDoc = " << xdoc1.toString() << endl << endl;

		cout << "\nBuilding xml tree from File: " << args[1] << endl;
		XmlDocument xdoc(args[1]);				// works for xml files
		std::cout << "\nxDoc = " << xdoc.toString() << endl << endl;

		moveSemantics(xdoc);
		findElementbyID(xdoc);
		findElements(xdoc);
		addRemoveChild(xdoc);
		getAttribs(xdoc);
		addRemoveAttribs(xdoc);
		writeXmltoFile(xdoc);
		extraReqs(xdoc);
	} catch (exception ex)
	{
		std::cout << "\n  " << ex.what() << "\n\n";
	}
}

//----< Helper function to the demostrateXmlDocument public interface. Demonstrates move semantics >-----

void Demo::moveSemantics(XmlDocument& xdoc)
{
	cout << "\n====================TESTING REQUIREMENT 4====================\n\n";
	cout << "Move constructing 'x' with 'xdoc'\n";
	XmlDocument x = std::move(xdoc);	//move construction
	cout<<"'x' = "<<x.toString()<<endl;
	cout << "\n'xdoc' = " << xdoc.toString()<<endl;
	cout << "Move assigning 'xdoc' with 'x'\n";
	xdoc = std::move(x);	//moved assignment (moved back to xdoc for further demonstration)
	cout << "\n'xdoc' = " << xdoc.toString() << endl;
	cout << "\n'x' = " << x.toString() << endl;
}

//----< Helper function to the demostrateXmlDocument public interface. Demonstrates searching an element by a given id >-----

void Demo::findElementbyID(XmlDocument& xdoc)
{
	cout << "\n====================TESTING REQUIREMENT 5====================\n\n";
	cout << "getElementById(): Searching for element with a given id = 4.\n";

	XElement temp = xdoc.getElementById("id", "4").select().front();

	if (temp != nullptr)
		cout << "Element found:\n" << temp->toString() << endl;
	else
		cout << "Element not found\n";

	cout << "\ngetElementById(): Searching for element with a given id = 7.\n";
	temp = xdoc.getElementById("id", "7").select().front();

	if (temp != nullptr)
		cout << "Element found:\n" << temp->toString() << endl;
	else
		cout << "Element not found\n";
}

//----< Helper function to the demostrateXmlDocument public interface. Demonstrates finding all elements with specified tagname >-----

void Demo::findElements(XmlDocument& xdoc)
{
	//Testing elements(tag) method
	cout << "\n====================TESTING REQUIREMENT 6====================\n\n";
	cout << "elements(tag): Searching for elements with a given tag = title.";
	int count = 0;
	xdoc = xdoc.elements("title");
	for (auto res : xdoc.select())
	{
		cout << res->toString() << endl;
		count++;
	}
	cout <<endl<< count << " elements found!" << endl;
}

//----< Helper function to the demostrateXmlDocument public interface. Demonstrates adding a child node to a node in tree >-----

void Demo::addRemoveChild(XmlDocument& xdoc)
{
	//Testing addChild()
	cout << "\n====================TESTING REQUIREMENT 7====================\n\n";
	cout << "creating a Tagged Element named 'NewChild' and adding it to the element with tagname = 'author'\n";
	XElement child = XmlElementFactory::makeTaggedElement("NewChild");
	child->addAttrib("firstname","John");
	child->addAttrib("lastname","Doe");
	cout << "New Child: "<<child->toString()<<endl;
	if (xdoc.element("author").addChild(child))
		cout << "\nChild added successfully!\n";
	else
		cout << "\nCannot add child! Parent not Found\n";
	cout << "\nUpdated Xml Tree:\n" << xdoc.toString()<<endl;
	cout << "\nRemoving 'NewChild' from the element with tagname = 'author'\n";
	if (xdoc.element("author").removeChild("NewChild"))
		cout << "Child removed successfully!\n";
	else
		cout << "\nUnable to remove Child. Child not found!\n";
	cout << "\nUpdated Xml Tree:\n" << xdoc.toString() << endl;

	cout << "\nProgrammatic creation of XML:\n";
	XmlDocument doc;
	XElement c = XmlElementFactory::makeTaggedElement("child");
	cout << "Adding root element with given tagname = 'root' with attribute 'firstname' = 'john' to an empty XmlDocument.\n";
	doc.addRootElement("root") ? cout <<"Root added!\n" :cout<<"Unable to add root element!\n";
	doc.element("root").addAttribute("firstname","John");
	c->addAttrib("lastname","Doe");
	cout << "Adding child to root element with attribute 'lastname' = 'Doe'.\n";
	doc.element("root").addChild(c);
	cout << "Created Xml:\n" << doc.toString()<<endl;
}

//----< Helper function to the demostrateXmlDocument public interface. Demonstrates getting attribute-value pairs of elements in tree >-----

void Demo::getAttribs(XmlDocument& xdoc)
{
	cout << "\n====================TESTING REQUIREMENT 8====================\n\n";
	cout << "Finding attributes of element with a given 'id' attribute = 2\n\n";

	vector<pair<string, string>> attribs = xdoc.attributes((xdoc.getElementById("id", "2").select().front()));
	if (attribs.size() != 0)
		for (auto a : attribs)
			cout << "\tattribute = " << a.first << "\tvalue = " << a.second << endl;
	else
		cout << "No attributes found!";

	cout << "\nFinding children of element whose parent has a tagname = 'reference'.\n\n";
	std::vector<XElement> children_;
	xdoc = xdoc.element("reference");
	if (!(children_ = xdoc.select()).empty())
	{
		std::vector<XElement> children_ = xdoc.children((xdoc.element("reference").select().front()));
		for (auto c : children_)
			cout << c->toString() << endl;
	}
	else
		cout << "No children found!\n";
}

//----< Helper function to the demostrateXmlDocument public interface. Demonstrates adding and removing attributes >-----

void Demo::addRemoveAttribs(XmlDocument& xdoc)
{
	cout << "\n====================TESTING REQUIREMENT 9====================\n\n";
	cout << "Adding attribute 'firstname' with value 'John' to the element whose 'id' is given as '2'.\n\n";
	if (xdoc.getElementById("id", "2").addAttribute("firstname", "John"))
		cout << "Attribute added successfully!\n";
	else
		cout << "Attribute cannot be added!\n";
	cout <<"Updated Xml Tree:\n"<<xdoc.toString()<<endl;
	cout << "\nRemoving attribute 'course' from the element whose 'id' is given as '2'.\n\n";
	if (xdoc.getElementById("id", "2").removeAttribute("course"))
		cout << "Attribute added successfully!\n";
	else
		cout << "Attribute cannot be removed!\n";
	cout << "Updated Xml Tree:\n" << xdoc.toString() << endl;
}

//----< Helper function to the demostrateXmlDocument public interface. Demonstrates writing of the Xml tree to a file >-----

void Demo::writeXmltoFile(XmlDocument& xdoc)
{
	cout << "\n====================TESTING REQUIREMENT 10====================\n\n";
	cout << "Tree Generation from string and file demonstrated in Req 3.\n\n";
	string file = "Xtree.xml";
	cout << "Storing XmlDocument in a file "<<file<<" :\n\n";
	if ( xdoc.toFile(file))
		cout << "XDocument successfully stored in " << file << "!\n";
	else
		cout << "Unable to store XmlDocument to File!\n";

	cout << "\n====================REQUIREMENT 11====================\n\n";
	cout << "The Executive package accepts the command line arguments and the Demo package demonstrates the requirements.\n ";

}

//----< Helper function to the demostrateXmlDocument public interface. Demonstrates finding descendents of a node in Xml tree >-----

void Demo::extraReqs(XmlDocument& xdoc)
{
	//testing descendants method
	cout << "\n====================TESTING EXTRA REQUIREMENTS====================\n\nTesting descendents() method";
	cout << "Finding descendents of the element with tagname 'reference':\n";
	xdoc = xdoc.element("reference").descendents();
	for (auto res : xdoc.select())
		cout << res->toString() << endl;
}

//----< test stub >----------------------------------------------------------

#ifdef TEST_DEMO

int main()
{
	Demo d;
	d.demostrateXmlDocument(2,"../Test/LectureNote.xml");
	XmlDocument d;
	d.addRootElement("tagname1");
	cout << "d = " << d.toString();
}

#endif