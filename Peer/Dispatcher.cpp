/////////////////////////////////////////////////////////////////////////////
// Dispatcher.cpp:	Dispatches received messages to the appropriate		   //
//					Registered Communicator								   //
// version 1.0                                                             //
// ----------------------------------------------------------------------- //
// Copyright © Akhil Panchal, 2015                                         //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2013 Ultimate                    //
// Platform:    Dell Inspiron 17R 5721, Core i5, Windows 8.1			   //
// Application: CSE 687 Project #4, Spring 2015                            //
// Author:      Akhil Panchal, Syracuse University			               //
//              (408) 921-0731, ahpancha@syr.edu	                       //
/////////////////////////////////////////////////////////////////////////////

#include"Dispatcher.h"

//----------< Starts the dispatcher on a worker thread >-------------

void Dispatcher::start()
{
	thread DispatchThread(
		[&]()
	{
		while (true)
		{
			try
			{
				IMessage *msg = recvr->getMessage();
				ReqHandler reqHandler = lookUp[msg->getCommand()];
				if (msg->getCommand() == "dispatcher_stop")
					break;
				else
					reqHandler(msg, sender);
			}
			catch (std::exception& ex)
			{
				Verbose::show("  Exception caught:", always);
				Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
			}
		}
	}
	);
	DispatchThread.detach();		// Peer wont access this thread again. Will terminate when encounters a send_stop message
}

//----------< Informs the Dispatcher about the registered communicator functions available >-------------

Dispatcher::Dispatcher()
{
	ReqHandler req = fileUpload;
	lookUp["file_upload"] = fileUpload;
	lookUp["get_dir"] = sendDirectories;
	lookUp["file_download"] = fileDownload;
	lookUp["search_file"] = searchFile;
	lookUp["search_text"] = searchText;
	lookUp["xml_search_text"] = xmlSearchText;
	lookUp["xml_search_file"] = xmlSearchFile;
}

//----------< File Upload Communicator >-------------
void fileUpload(IMessage *msg, Sender* send)
{
	string s = "\n  File " + msg->getValue() + " received successfully from ";
	s = s + msg->getSendIP() + " " + msg->getSendPort();
	s = s + "\n  Stored in Peer/DownloadDirectory";
	Verbose::show(s);
	IMessage *m = new Message();
	m->constructMessage("ack_file_upload", msg->getValue(), msg->getSendIP(), stoi(msg->getSendPort()), msg->getRecvIP(), stoi(msg->getRecvPort()));
	m->setBody("null");
	m->setTime(msg->getTime());
	send->postMessage(m);
}

//----------< File Download Communicator >-------------
void fileDownload(IMessage *msg, Sender *send)
{
	IMessage *m = new Message();
	m->constructMessage("file_upload", msg->getValue(), msg->getSendIP(), stoi(msg->getSendPort()), msg->getRecvIP(), stoi(msg->getRecvPort()));
	m->setBody("null");
	m->setTime("0");
	send->postMessage(m);
}

//----------< Send Directories Communicator >-------------
void sendDirectories(IMessage *msg, Sender* send)
{
	using HighResolutionClock = chrono::high_resolution_clock;
	HighResolutionClock::time_point t1 = HighResolutionClock::now();

	DataStore ds;
	FileMgr fm("Peer/Root", ds);

	string body = "";
	fm.search(true);
	for (auto path : ds.getPaths())
	{
		body += "<dir>" + path;
		body += "@";
	}
	for (auto fs : ds)
	{
		std::string file = fs.first;
		std::cout << "\n  " << file;
		DataStore::PathCollection pc = ds.getPaths(file);
		for (auto path : pc)
		{
			body += path + "\\" + fs.first;
			body += "@";
		}
	}
	HighResolutionClock::time_point t2 = HighResolutionClock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	IMessage *m = new Message();
	m->constructMessage("ack_get_dir", msg->getValue(), msg->getSendIP(), stoi(msg->getSendPort()), msg->getRecvIP(), stoi(msg->getRecvPort()));
	m->setBody(body);
	m->setTime(to_string(duration));
	send->postMessage(m);
}

//----------< File Search Communicator >-------------
void searchFile(IMessage *msg, Sender *send)
{
	using HighResolutionClock = chrono::high_resolution_clock;
	HighResolutionClock::time_point t1 = HighResolutionClock::now();
	vector<string> results;
	Catalog cat("Peer/Root");
	string body = msg->getBody();
	string pattern = "";
	bool flag = false;
	for (unsigned int i = 0; i < body.size(); i++)
	{
		if (body[i] == '#')
		{
			flag = true;
			cat.addPattern(pattern);
			pattern = "";
			continue;
		}
		pattern += body[i];
	}
	if (flag == false)
		cat.addPattern(pattern);
	cat.buildCatalog(true);
	body = "";
	for (auto item : cat.getFiles())
	{
		size_t pos = item.find(msg->getValue());
		if (pos != string::npos)
		{
			body += item + "@";
		}
	}
	if (body == "")
		body = "No Results Found!@";
	HighResolutionClock::time_point t2 = HighResolutionClock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	IMessage *m = new Message();
	m->constructMessage("ack_search_file", msg->getValue(), msg->getSendIP(), stoi(msg->getSendPort()), msg->getRecvIP(), stoi(msg->getRecvPort()));
	m->setBody(body);
	m->setTime(to_string(duration));
	send->postMessage(m);
}

//----------< File Search XML Results Communicator >-------------
void xmlSearchFile(IMessage *msg, Sender *send)
{
	using HighResolutionClock = chrono::high_resolution_clock;
	HighResolutionClock::time_point t1 = HighResolutionClock::now();
	vector<string> results;
	Catalog cat("Peer/Root");
	string body = msg->getBody();
	string pattern = "";
	bool flag = false;
	for (unsigned int i = 0; i < body.size(); i++)
	{
		if (body[i] == '#')
		{
			flag = true;
			cat.addPattern(pattern);
			pattern = "";
			continue;
		}
		pattern += body[i];
	}
	if (flag == false)
		cat.addPattern(pattern);
	cat.buildCatalog(true);

	body = "";
	XmlDocument searchResult = buildXmlFile(cat.getFiles(), msg);
	
	searchResult.toFile("searchRes.xml");

	HighResolutionClock::time_point t2 = HighResolutionClock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	IMessage *m = new Message();
	m->constructMessage("file_upload", "searchRes.xml", msg->getSendIP(), stoi(msg->getSendPort()), msg->getRecvIP(), stoi(msg->getRecvPort()));
	m->setBody(body);
	m->setTime(to_string(duration));
	send->postMessage(m);
}

//----------< Text Search Communicator >-------------
void searchText(IMessage *msg, Sender *send)
{
	using HighResolutionClock = chrono::high_resolution_clock;
	HighResolutionClock::time_point t1 = HighResolutionClock::now();

	vector<string> results;
	Catalog cat("Peer/Root");
	string body = msg->getBody();
	string pattern = "";
	bool flag = false;
	for (unsigned int i = 0; i < body.size(); i++)
	{
		if (body[i] == '#')
		{
			flag = true;
			cat.addPattern(pattern);
			pattern = "";
			continue;
		}
		pattern += body[i];

	}
	if (flag==false)
		cat.addPattern(pattern);
	cat.buildCatalog(true);
	results = cat.getSearchResults(msg->getValue());
	body = "";
	for (auto item : results)
	{
		body += item + "@";
	}
	if (body == "")
		body = "No Results Found!@";

	HighResolutionClock::time_point t2 = HighResolutionClock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	IMessage *m = new Message();
	m->constructMessage("ack_search_text", msg->getValue(), msg->getSendIP(), stoi(msg->getSendPort()), msg->getRecvIP(), stoi(msg->getRecvPort()));
	m->setBody(body);
	m->setTime(to_string(duration));
	send->postMessage(m);
}

//----------< Text Search Xml Results Communicator >-------------
void xmlSearchText(IMessage *msg, Sender *send)
{
	using HighResolutionClock = chrono::high_resolution_clock;
	HighResolutionClock::time_point t1 = HighResolutionClock::now();
	vector<string> results;
	Catalog cat("Peer/Root");
	string body = msg->getBody();
	string pattern = "";
	bool flag = false;
	for (unsigned int i = 0; i < body.size(); i++)
	{
		if (body[i] == '#')
		{
			flag = true;
			cat.addPattern(pattern);
			pattern = "";
			continue;
		}
		pattern += body[i];

	}
	if (flag == false)
		cat.addPattern(pattern);
	cat.buildCatalog(true);
	results = cat.getSearchResults(msg->getValue());
	body = "";
	XmlDocument xmlResult = buildXmlText(results);
	xmlResult.toFile("searchRes.xml");
	HighResolutionClock::time_point t2 = HighResolutionClock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	IMessage *m = new Message();
	m->constructMessage("file_upload", "searchRes.xml", msg->getSendIP(), stoi(msg->getSendPort()), msg->getRecvIP(), stoi(msg->getRecvPort()));
	m->setBody(body);
	m->setTime(to_string(duration));
	send->postMessage(m);
}

//----------< Text Search Xml Results Communicator Helper >-------------
XmlDocument buildXmlText(vector<string> results)
{
	using sPtr = std::shared_ptr < AbstractXmlElement >;
	XmlDocument xmlBody;
	sPtr root = XmlElementFactory::makeTaggedElement("root");
	xmlBody.addRootElement(root);
	stack<sPtr> stack_;
	stack_.push(root);
	sPtr curr;
	for (auto item : results)
	{
		string elem = "";
		for (auto c : item)
		{
			if (c == '\\')
			{
				if (xmlBody.element(elem).select().size()!=0)
				{
					curr = xmlBody.element(elem).select()[0];
				}
				else
				{
					sPtr element = XmlElementFactory::makeTaggedElement(elem);
					sPtr parent = stack_.top();
					if (xmlBody.addChild(element, parent))
					{
						stack_.push(element);
						curr = element;
					}
				}
				elem = "";
				continue;
			}
			elem += c;
		}
		sPtr filename = XmlElementFactory::makeTextElement(elem);
		xmlBody.addChild(filename, curr);
	}
	return xmlBody;
}

//----------< File Search Xml Results Communicator Helper >-------------
XmlDocument buildXmlFile(vector<string> results, IMessage* msg)
{
	using sPtr = std::shared_ptr < AbstractXmlElement >;
	XmlDocument xmlBody;
	sPtr root = XmlElementFactory::makeTaggedElement("root");
	xmlBody.addRootElement(root);
	stack<sPtr> stack_;
	stack_.push(root);
	sPtr curr;
	for (auto item : results)
	{
		size_t pos = item.find(msg->getValue());
		if (pos != string::npos)
		{
			string elem = "";
			for (auto c : item)
			{
				if (c == '\\')
				{
					if (xmlBody.element(elem).select().size() != 0)
					{
						curr = xmlBody.element(elem).select()[0];
					}
					else
					{
						sPtr element = XmlElementFactory::makeTaggedElement(elem);
						sPtr parent = stack_.top();
						if (xmlBody.addChild(element, parent))
						{
							stack_.push(element);
							curr = element;
						}
					}
					elem = "";
					continue;
				}
				elem += c;
			}
			sPtr filename = XmlElementFactory::makeTextElement(elem);
			xmlBody.addChild(filename, curr);
		}
	}
	return xmlBody;
}

//----------< Test stub Dispatcher Package. >-------------
#ifdef TEST_DISPATCHER

int main()
{
	Sender send;
	IMessage *imsg = new Message();
	imsg->constructMessage("file_upload","test","localhost","9070","localhost","9080");
	send.postMessage(imsg);
	imsg->constructMessage("file_download","test","localhost","9070","localhost","9080");
	send.postMessage(imsg);
	imsg->constructMessage("file_search","test","localhost","9070","localhost","9080");
	send.postMessage(imsg);
	imsg->constructMessage("text_search","test","localhost","9070","localhost","9080");
	send.postMessage(imsg);
	Dispatcher dispatcher_;
	dispatcher_.start();
	return 0;
}

#endif