/////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp - An ObjectFactory for the C++ classes that need to be  //
//					used in the .NET environment. All the C++ files are	   //
//					build as a dll and are added as a reference to the	   //
//					.NET project.										   //
// version 1.1                                                             //
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

#define IN_DLL
#include "MockChannel.h"
#include "Communication.h"
#include "Peer.h"
#include "Message.h"
#include "BlockingQueue.h"
#include <string>
#include <thread>
#include <iostream>

using BQueue = BlockingQueue < IMessage* >;

//----< factory functions >--------------------------------------------------

//----< Creates a Sender >--------------------------------------------------
ISendr* ObjectFactory::createSendr() { return new Sender(); }

//----< Creates a Receiver >--------------------------------------------------
IRecvr* ObjectFactory::createRecvr(int port) { return new Receiver(port); }

//----< Creates a Message >--------------------------------------------------
IMessage* ObjectFactory::createMessage()  { return new Message(); }

//----< Creates a Peer >--------------------------------------------------
IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr, IMessage* pIMessage)
{ 
	return new Peer(pISendr, pIRecvr, pIMessage); 
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr();
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->start();
  pSendr->postMessage("Hello World");
  pSendr->postMessage("CSE687 - Object Oriented Design");
  Message msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pSendr->postMessage("stopping");
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pMockChannel->stop();
  pSendr->postMessage("quit");
  std::cin.get();
}
#endif
