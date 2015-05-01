#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H

/////////////////////////////////////////////////////////////////////////////
// MockChannel.h - An Interface File For the C++ classes that need to be   //
//					used in the .NET environment. All the C++ files are	   //
//					build as a dll and are added as a reference to the	   //
//					.NET project.										   //
// version 1.0                                                             //
// ----------------------------------------------------------------------- //
// Copyright © Akhil Panchal, 2015                                         //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2013 Ultimate                    //
// Platform:    Dell Inspiron 17R 5721, Intel Core i5, Windows 8.1		   //
// Application: CSE 687 Project #4, Spring 2015                            //
// Source:      Jim Fawcett, CST 4-187, 443-3948						   //
//              jfawcett@twcny.rr.com									   //
// Author:      Akhil Panchal, Syracuse University						   //
//              (408) 921-0731, ahpancha@syr.edu						   //
/////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* -------------------
* This module consists of an Interface and an Object Factory that enable us
* to use the C++ objects in the .NET environment using Managed C++/CLI
* Interface pointers. We can see the Liskov's substitution at work here.
*
* Public Interface:
* ---------------
*
* IMessage Interface
*---------------------------------
*	virtual void constructMessage(string cmd, string val, string recvIP, int recvPort, string sendIP, int sendPort) = 0;
*		Constructs Message Object, given the string paramenters.
*	virtual string getCommand() = 0;
*		Gets the command from the message header
*	virtual string getValue() = 0;
*		Gets the value field from the message header
*	virtual string getRecvIP() = 0;
*		Gets the receivers IP from the message header
*	virtual string getRecvPort() = 0;
*		Gets the receivers port from the message header
*	virtual string getSendIP() = 0;
*		Gets the senders IP from the message header
*	virtual string getSendPort() = 0;
*		Gets the senders port from the message header
*	virtual string getContentLength() = 0;
*		Gets the gets messages content length from the message header
*	virtual string getBody() = 0;
*		Gets the body from the message header
*	virtual string getTime() = 0;
*		Gets the processing time from the message header
*	virtual void setContentLength(int s) = 0;
*		Sets the content length from the message header
*	virtual void setValue(string s) = 0;
*		Sets the value of the message header
*	virtual void setBody(string s) = 0;
*		Sets the body of the message
*	virtual void setTime(string t) = 0;
*		Sets the processing time taken in the message header
*
* ISendr Interface
*	virtual void postMessage(IMessage* msg) = 0
*		Posts the Message in the Send Queue.
*
* IRecvr Interface
*-------------------------------
*	virtual IMessage* getMessage() = 0
*		Gets the Message from the Receive Queue
*
* ObjectFactory Interface
*-------------------------------
*	IMessage* createMessage()
*		Returns a user defined C++ Message object wrapped in an IMessage Interace pointer.
*   ISendr* createSendr()
*		Returns a user defined C++ Sender object wrapped in an ISendr Interace pointer.
*   IRecvr* createRecvr(int port)
*		Returns a user defined C++ Receiver object wrapped in an IRecvr Interace pointer.
*	IMockChannel* createMockChannel(ISendr* pISendr, IRecvr* pIRecvr, IMessage* pIMessage)
*		Returns a user defined C++ Peer object wrapped in an IMockChannel Interace pointer.
*
* Required Files:
* ---------------
*   Message.h, Message.cpp
*
* Build Process:
* --------------
*	devenv Sockets.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.1 : 20 April 2015
* - Added IMessage Interface
* ver 1.0 : 06 April 2015
* - first release
*
*/

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>
#include <vector>

using namespace std;

struct IMessage
{
	virtual void constructMessage(string cmd, string val, string recvIP, int recvPort, string sendIP, int sendPort) = 0;
									//Constructs Message Object, given the string paramenters.
	virtual void constructMessage1(string cmd, string val, string recvIP, string recvPort, string sendIP, string sendPort) = 0;
									
	virtual void parseMessage(vector<string>) = 0;
	virtual string getCommand() = 0;	//Gets the command from the message header
	virtual string getValue() = 0;		//Gets the value field from the message header
	virtual string getRecvIP() = 0;		//Gets the receivers IP from the message header
	virtual string getRecvPort() = 0;	//Gets the receivers port from the message header
	virtual string getSendIP() = 0;		//Gets the senders IP from the message header
	virtual string getSendPort() = 0;	//Gets the sender port from the message header
	virtual string getContentLength() = 0;	//Gets the content length from the message header
	virtual string getBody() = 0;		//Gets the body from the message 
	virtual string getTime() = 0;		//Gets the processing time from the message

	virtual void setContentLength(int s) = 0;	//Sets the content length from the message
	virtual void setValue(string s) = 0;		//Sets the value from the message header
	virtual void setBody(string s) = 0;			//Sets the body of the message
	virtual void setTime(string t) = 0;			//Sets the time taken int message 
};

struct ISendr
{
	virtual void postMessage(IMessage* msg) = 0;	// post Message to Send Queue
};

struct IRecvr
{
	virtual IMessage* getMessage() = 0;				// get Message from Receive Queue
};

struct IMockChannel
{
public:
  virtual void start() = 0;							// Starts the Server Peer
  virtual void stop() = 0;							// Stops the Server Peer
};

extern "C" {
  struct ObjectFactory
  {
	DLL_DECL IMessage* createMessage();
    DLL_DECL ISendr* createSendr();
    DLL_DECL IRecvr* createRecvr(int port);
    DLL_DECL IMockChannel* createMockChannel(ISendr* pISendr, IRecvr* pIRecvr, IMessage* pIMessage);
	
  };
}

#endif


