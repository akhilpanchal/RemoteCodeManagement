
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/////////////////////////////////////////////////////////////////////////////
// Communication.h - Consists of a Sender and a Receiver that transfer or  //
//					 receive messages over the communication channel.	   //
// version 1.1                                                             //
// ----------------------------------------------------------------------- //
// Copyright © Akhil Panchal, 2015                                         //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:		Visual C++, Visual Studio 2013 Ultimate                //
// Platform:		Dell Inspiron 17R 5721, Intel Core i5, Windows 8.1	   //
// Application:		CSE 687 Project #4, Spring 2015                        //
// Author:			Akhil Panchal, Syracuse University			           //
//					(408) 921-0731, ahpancha@syr.edu	                   //
/////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* -------------------
* This module defines the Sender, Receiver and Client Handler for the peer 
* that manages the communication with other peers.
*
*	Sender:		Sends the requests and the body/content through the socket
*				communication channel.
*
*	Receiver:	Receives the requests and body/content from the listening
*				socket and initiates a Client handler that serves the 
*				request.
*
*	ClientHandler:
*				Serves the client request  on a worker thread so that the 
*				Receiver can go back to accepting connections from other 
*				clients.
*
* Public Interface:
* ---------------
*
*	=======Reciever's Interface:=======
*
* Receiver(int portNo=9080, Socket::IpVer ipVer = Socket::IP6)
*	Initiates a socket that handles IP addresses of type specified
*	(IP6 by default), that listens on the port specified at the specified.
*
* static BlockingQueue<Message>& getQueueInstance()
*	Returns the instance of the Recieve Queue to the calling method.
*
*	=======Sender's Interface:=======
*
* bool connectToPeer(string ip, int portNo)
*	Attempts to connect the peer to another peer on the specified 
*	IP address and port number.
*
* static BlockingQueue<Message>& getQueueInstance()
*	Returns the instance of the Send Queue to the calling method.
*
* void sendHeader(Message msg)
*	Sends the Header specified in the argument to the connected peer.
*	
* bool sendFile(Message msg)
*	Sends the file specified in the Message header specified block by 
*	block to the connected peer.
*
* void sendBody(IMessage *msg)
*	Sends the body of the Message Reference passed.
*
*	=======Client Handler's Interface:=======
*
* Message receiveHeader(Socket& socket)
*	Receives the header content sent by the connected peer,
*	constructs the message and returns it.
*
* void receiveBody(Socket& socket_, IMessage* body)
*	Receives the body content sent by the connected peer,
*	stores in the message reference passed.
*
*
* bool acceptFile(Message msg, Socket& socket)
*	Attempts to receive a file specified in the header of the 
*	specified message.
*
* Required Files:
* ---------------
*   Communication.h, Communication.cpp
*	Sockets.h, Sockets.cpp,
*	AppHelpers.h, AppHelpers.cpp,
*	WindowsHelpers.h, WindowsHelpers.cpp
*	BlockingQueue.h,
*	Message.h, ,Message.cpp
*	FileSystem.h, FileSystem.cpp
*
* Build Process:
* --------------
*	devenv Sockets.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.1 : 20 April 2015
* - Added sendBody() and receiveBody() methods.
* ver 1.0 : 06 April 2015
* - first release
*
*/

#define IN_DLL
#include<fstream>
#include<iostream>
#include<chrono>
#include"Sockets.h"
#include"AppHelpers.h"
#include"BlockingQueue.h"
#include"Message.h"
#include"FileSystem.h"
#include"MockChannel.h"

using namespace std;
using namespace ApplicationHelpers;

/* CLIENT-HANDLER = Functor that serves each client on a separate thread and port */
class Receiver;

class ClientHandler
{
public:
	void operator()(Socket& socket_);				// Accepts a new socket connection and serves a single client request.
	bool acceptFile(IMessage *msg, Socket& socket);	// Accepts file specified in the header block by block throught the socket specified.
	IMessage* receiveHeader(Socket& socket);			// Receives a header from the connected client through the socket specified.
	void receiveBody(Socket& socket_, IMessage* body);	// Receives body from the connected client through the socket specified.
	void setReceiver(Receiver *rec)	{ recvr = rec; }
private:
	Receiver *recvr;
};



/* RECEIVER =	Represents a receiver in a peer to peer communication model
				Receives data from the peer client via the socket communication channel.
					
*/

class Receiver :public IRecvr
{
public:
	Receiver(int portNo=9080, Socket::IpVer ipVer = Socket::IP6);
												// starts the receiver that listens to the specifed port and type of IPAddrs
	BlockingQueue<IMessage*>& getQueueInstance(){	return recvQ; }
												// returns the shared receive queue instance to the caller
	IMessage* getMessage() { IMessage *m = new Message(); m = recvQ.deQ(); return m; }
	void postMessage(IMessage *msg)	{ recvQ.enQ(msg); }
private:
	ClientHandler clientHandler;
	BlockingQueue<IMessage*> recvQ;
	SocketSystem ss;
	SocketListener *recvr;
	IMessage *msg;
};

/* SENDER =	Represents a sender in a peer to peer communication model
			Sends data to the peer server via the socket communication channel.
*/

class Sender : public ISendr
{
public:
	Sender(){};
	bool connectToPeer(string ip, int portNo);	// attempts to connect to the peer with specified IP and port number.
	bool disconnectFromPeer();
	BlockingQueue<IMessage*>& getQueueInstance(){ return sendQ; }
												// returns the shared send queue instance to the caller
	void postMessage(IMessage *msg)	{ sendQ.enQ(msg); }
	void sendHeader(IMessage *msg);				// send the header of the Message object specified.
	void sendBody(IMessage *msg);				// send the body of the Message object specified.
	bool sendFile(IMessage *msg);				// sends the File specified in the message attribute block by block.
	void startSending();
private:
	BlockingQueue<IMessage*> sendQ;
	SocketSystem ss;
	SocketConnecter sender;
	IMessage *msg;
};

#endif