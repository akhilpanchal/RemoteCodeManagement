
#include"RegisteredCommunicators.h"

//----------< File Upload Communicator >-------------
void FileUploader::handleRequest(IMessage *msg, Sender *send)
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

//----------< Send Directories Communicator >-------------
void SendDirectoryStructure::handleRequest(IMessage *msg, Sender *send)
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

//----------< File Download Communicator >-------------
void FileDownload::handleRequest(IMessage *msg, Sender *send)
{
	IMessage *m = new Message();
	m->constructMessage("file_upload", msg->getValue(), msg->getSendIP(), stoi(msg->getSendPort()), msg->getRecvIP(), stoi(msg->getRecvPort()));
	m->setBody("null");
	m->setTime("0");
	send->postMessage(m);
}

//----------< File Search Communicator >-------------
void SearchFile::handleRequest(IMessage *msg, Sender *send)
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

//----------< Text Search Communicator >-------------
void SearchText::handleRequest(IMessage *msg, Sender *send)
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
void XmlSearchText::handleRequest(IMessage *msg, Sender *send)
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
XmlDocument XmlSearchText::buildXmlText(vector<string> results)
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
	return xmlBody;
}

//----------< File Search XML Results Communicator >-------------
void XmlSearchFile::handleRequest(IMessage *msg, Sender *send)
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

//----------< File Search Xml Results Communicator Helper >-------------
XmlDocument XmlSearchFile::buildXmlFile(vector<string> results, IMessage* msg)
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
