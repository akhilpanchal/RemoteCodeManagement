/////////////////////////////////////////////////////////////////////////////
// Executive.cpp - Entry point for Xml DOM CSE 687 Pr #2		           //
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
* This package is the entry point for the demonstration of CSE 687 Project 2
*
* Public Interface :
*---------------
* void demonstrateRequirements(int argc, char** args)
*	Initiates Demo for XmlDocument class usage
*	Accepts arguments from command line.
*
* Required Files :
*-------------- -
*	Executive.cpp,
*	Demo.h, Demo.cpp
*
* Build Process :
*--------------
* cl / EHa / DTEST_EXECUTIVE EXECUTIVE.cpp
*					OR
* devenv XmlDocument.sln /rebuild debug
*
* Maintenance History :
*--------------------
* ver 1.0 : 16 Mar 15
* -first release
*
*/
#include"Demo.h"

using namespace std;

class Executive
{
public:
	Executive();
	void demonstrateRequirements(int argc, char** args);	// Initiates Demo for XmlDocument Class
private:
	std::unique_ptr<Demo> demo_ = nullptr;
};

//----< Default constructor for Executive >----

Executive::Executive()
{
	demo_ = std::unique_ptr<Demo>(new Demo());
}

//----< Initiates Demonstration for Project 2 >----

void Executive::demonstrateRequirements(int argc, char** args)
{
	demo_->demonstrateXmlDocument(argc, args);
}

#ifdef TEST_EXECUTIVE

int main(int argc, char** args)
{
	Executive exec;
	exec.demonstrateRequirements(argc, args);
	return 0;
}

#endif