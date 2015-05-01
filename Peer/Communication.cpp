/////////////////////////////////////////////////////////////////////////////
// Communication.cpp:	Consists of a Sender, Receiver and a Client Handler//
//						that transfer or receive messages over the		   //
//						communication channel.							   //
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

#include"Communication.h"

//-------< RECEIVER MEMBERS >-------
//BlockingQueue<Message> Receiver::recvQ;
//BlockingQueue<Message> Sender::sendQ;

//----< Starts a listener at specified port no. of Type specifed (IP6 by default) >----

Receiver::Receiver(int portNo, Socket::IpVer ipVer)
{
	recvr = new SocketListener(portNo, ipVer);
	clientHandler.setReceiver(this);
	recvr->start(clientHandler);
	Verbose::show("\n  ==========PEER LISTENING AT PORT : " + to_string(portNo) + "==========\n");
	//Verbose::show("press key to exit: ", always);
}

//-------< SENDER MEMBERS >-------

//----------< Starts a sender worker thread >-------------

void Sender::startSending()
{
	std::thread SendThread(
		[&]()
	{
		do
		{
			try
			{
				IMessage *msg = sendQ.deQ();
				/*Message* msg = dynamic_cast<Message*>(imsg);
				map<string, string> header = msg->getHeader();*/

				if (msg->getCommand() == "send_stop")
					break;
				else if (msg->getCommand() == "file_upload")
				{
					if (connectToPeer(msg->getRecvIP(), stoi(msg->getRecvPort())))
					{
						if (sendFile(msg))
						{	}
					}
					else
						Verbose::show("Connection failed!\n");
				}
				else if (msg->getCommand() == "ack")
				{
					if (connectToPeer(msg->getRecvIP(), stoi(msg->getRecvPort())))
						sendHeader(msg);
				}
				else
				{
					if (connectToPeer(msg->getRecvIP(), stoi(msg->getRecvPort())))
					{
						sendHeader(msg);
						sendBody(msg);
					}
				}
			}
			catch (exception ex)
			{
				string s = ex.what();
				Verbose::show("\n In send Thread: " + s);
			}
		} while (1);
	}
	);
	SendThread.detach();
}


//----------< Sends the Message header specified >-------------

void Sender::sendHeader(IMessage *imsg)
{
	Message* msg = dynamic_cast<Message*>(imsg);
	for (auto iter : msg->getHeader())
	{
		string str = "";
		str += iter.first + "#" + iter.second;
		sender.sendString(str, '\n');
	}
	sender.sendString("\n", '\n');	// marks the end of Header
}

void Sender::sendBody(IMessage *imsg)
{
	Message* msg = dynamic_cast<Message*>(imsg);
	sender.sendString(msg->getBody(),'\n');
	sender.sendString(msg->getTime(),'\n');
	sender.sendString("\n", '\n');
}

//----------< Sends the File specified in header >-------------

bool Sender::sendFile(IMessage *msg)
{
	using HighResolutionClock = chrono::high_resolution_clock;
	HighResolutionClock::time_point t1 = HighResolutionClock::now();
	string filename = msg->getValue();
	FileSystem::FileInfo fn(filename);
	if (fn.good())
		filename = fn.name();
	/*else
	{
		Verbose::show("\nFile to be uploaded is not accessible!\n");
		return false;
	}*/
	Socket::byte buffer[1024];
	streamoff bytesRead;
	ifstream file(msg->getValue(), ios::in | ios::binary);
	if (file.is_open())
	{
		msg->setValue(filename);
		file.seekg(0, ios::end);
		streamoff remaining = file.tellg();
		file.seekg(0, ios::beg);
		while (file.good())
		{

			file.read(buffer, min(1024, remaining));
			bytesRead = min(1024, remaining);
			// set content length, send header and then send the buffer
			msg->setContentLength((int)bytesRead);
			HighResolutionClock::time_point t2 = HighResolutionClock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
			msg->setTime(to_string(duration));
			sendHeader(msg);
			sender.send((size_t)bytesRead, buffer);
			remaining -= bytesRead;
			if (bytesRead < 1024)
				break;
		}
		file.close();
		return true;
	}
	return false;
}

//----------< Connects to the specified IP and port no. >-------------

bool Sender::connectToPeer(string ip, int portNo)
{
	while (!sender.connect(ip, portNo))
	{
		::Sleep(100);
	}
	Verbose::show("Connected to Peer at " + ip + " " + to_string(portNo));
	return true;
}

//----------< disonnects the sender port >-------------

bool Sender::disconnectFromPeer()
{
	return sender.shutDown();
}

//-------< CLIENT HANDLER MEMBERS >-------

//----< Client Handler thread starts running this function, Passed as a functor by the Listen thread >-----------------

void ClientHandler::operator()(Socket& socket_)
{
	try
	{
		IMessage *msg = receiveHeader(socket_);
		if (msg->getCommand() == "send_string")
			Verbose::show("\nString received: " + msg->getValue() + "\n");
		else if (msg->getCommand() == "file_upload")
		{
			using HighResolutionClock = chrono::high_resolution_clock;
			HighResolutionClock::time_point t1 = HighResolutionClock::now();

			if (acceptFile(msg, socket_))
			{	}
			else
				Verbose::show("\nFile Upload Failed!\n");
			HighResolutionClock::time_point t2 = HighResolutionClock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
			msg->setTime(to_string(duration));
		}
		else if (msg->getCommand() == "ack")
		{
			string s = "\n  Reply from " + msg->getSendIP() + msg->getSendPort();
			s += ": " + msg->getValue() + " uploaded successfully!\n";
			Verbose::show(s);
		}
		else //(msg->getCommand() == "ack_get_dir")
		{
			receiveBody(socket_,msg);
		}
		recvr->postMessage(msg);
	}
	catch (std::exception& ex)
	{
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
	}
	socket_.shutDown();
	socket_.close();
}

//----------< Receives the header from the specified socket. >-------------

IMessage* ClientHandler::receiveHeader(Socket& socket_)
{
	vector<string> received;
	while (true)
	{
		string command = socket_.recvString('\n');
		// interpret test command
		if (command == "\n" || command.size() == 0)
		{
			command = socket_.recvString('\n');
			break;			//header received
		}
		received.push_back(command);
	}
	IMessage *msg = new Message();
	msg->parseMessage(received);
	return msg;
}

void ClientHandler::receiveBody(Socket& socket_, IMessage* msg)
{
	string body = "";
	string time = "";
	while (true)
	{
		string item = socket_.recvString('\n');
		if (item.size() != 0)
		{
			body = item;
			break;
		}
		else
			break;
	}
	while (true)
	{
		string item = socket_.recvString('\n');
		if (item.size() != 0)
		{
			time = item;
			break;
		}
		else
			break;
	}
	msg->setBody(body);
	msg->setTime(time);
}


//----------< Receives the entire file specified in the header from the socket.>----------

bool ClientHandler::acceptFile(IMessage *msg, Socket& socket_)
{
	string filepath = "Peer/Root/DownloadDirectory/";
	std::ofstream outfile(filepath + msg->getValue(), ios::binary);
	size_t BufLen = 1024;
	Socket::byte buffer[1024];
	BufLen = stoi(msg->getContentLength());
	bool ok;
	if (socket_.bytesWaiting() == 0)
	{
		return false;
	}
	while (true)
	{
		ok = socket_.recv(BufLen, buffer);

		if (socket_ == INVALID_SOCKET || !ok)
			return false;

		outfile.write(buffer, BufLen);

		if (BufLen < 1024)
		{
			outfile.close();
			//recvr->postMessage(msg);
			return true;
		}
		else if (socket_.bytesWaiting() != 0)
		{
			msg = receiveHeader(socket_);
			BufLen = stoi(msg->getContentLength());
		}
		else
		{
			//recvr->postMessage(msg);
			return true;
		}
	}
	return false;
}


//----------< Test Stub for Communication Package. >-------------

#ifdef TEST_COMMUNICATION

int main()
{
	Sender s;
	Receiver r(9060);
	Message m;
	m.constructMessage("file_upload", "../Peer/UploadDirectory/test.txt", "localhost", 9050, "localhost", 9060);
	s.postMessage(m);
	return 0;
}

#endif