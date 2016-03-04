#ifndef REGISTEREDCOMMUNICATORS_H
#define REGISTEREDCOMMUNICATORS_H

#include"Message.h"
#include"Communication.h"
#include"FileMgr.h"
#include"DataStore.h"
#include"Catalog.h"
#include"XmlDocument.h"
#include"IXmlElement.h"
#include"XmlElementFactory.h"

using namespace XmlProcessing;

//-----------< Registered Communicator Interface >-----------
class RegisteredCommunicator
{
public:
	virtual void handleRequest(IMessage *msg, Sender *send) = 0;
};

//-----------< Concrete Registered Communicators >-----------

class FileUploader : public RegisteredCommunicator
{
public:
	void handleRequest(IMessage *msg, Sender *send);
};

class SendDirectoryStructure : public RegisteredCommunicator
{
public:
	void handleRequest(IMessage *msg, Sender *send);
};

class FileDownload : public RegisteredCommunicator
{
public:
	void handleRequest(IMessage *msg, Sender *send);
};

class SearchFile : public RegisteredCommunicator
{
public:
	void handleRequest(IMessage *msg, Sender *send);
};

class SearchText : public RegisteredCommunicator
{
public:
	void handleRequest(IMessage *msg, Sender *send);
};

class XmlSearchText : public RegisteredCommunicator
{
public:
	void handleRequest(IMessage *msg, Sender *send);
	XmlDocument buildXmlText(vector<string> results);
};

class XmlSearchFile : public RegisteredCommunicator
{
public:
	void handleRequest(IMessage *msg, Sender *send);
	XmlDocument buildXmlFile(vector<string> results, IMessage* msg);
};

#endif