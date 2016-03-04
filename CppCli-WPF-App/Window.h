#ifndef WINDOW_H
#define WINDOW_H
///////////////////////////////////////////////////////////////////////////
// Window.h - C++\CLI implementation of WPF Application                  //
//          - Demo for CSE 687 Project #4                                //
// ver 2.0                                                               //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015             //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package demonstrates how to build a C++\CLI WPF application.  It 
*  provides one class, WPFCppCliDemo, derived from System::Windows::Window
*  that is compiled with the /clr option to run in the Common Language
*  Runtime, and another class MockChannel written in native C++ and compiled
*  as a DLL with no Common Language Runtime support.
*
*  The window class hosts, in its window, a tab control with three views, two
*  of which are provided with functionality that you may need for Project #4.
*  It loads the DLL holding MockChannel.  MockChannel hosts a send queue, a
*  receive queue, and a C++11 thread that reads from the send queue and writes
*  the deQ'd message to the receive queue.
*
*  The Client can post a message to the MockChannel's send queue.  It hosts
*  a receive thread that reads the receive queue and dispatches any message
*  read to a ListBox in the Client's FileList tab.  So this Demo simulates
*  operations you will need to execute for Project #4.
*
*  Required Files:
*  ---------------
*  Window.h, Window.cpp, MockChannel.h, MockChannel.cpp,
*  Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp
*
*  Build Command:
*  --------------
*  devenv Sockets.sln /rebuild debug
*  - this builds C++\CLI client application and native mock channel DLL
*
*  Maintenance History:
*  --------------------
*  ver 2.0 : 15 Apr 2015
*  - completed message passing demo with moch channel
*  - added FileBrowserDialog to show files in a selected directory
*  ver 1.0 : 13 Apr 2015
*  - incomplete demo with GUI but not connected to mock channel
*/
/*
* Create C++/CLI Console Application
* Right-click project > Properties > Common Language Runtime Support > /clr
* Right-click project > Add > References
*   add references to :
*     System
*     System.Windows.Presentation
*     WindowsBase
*     PresentatioCore
*     PresentationFramework
*/
using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;
using namespace System::Diagnostics;
using namespace cli;
#include "../MockChannel/MockChannel.h"
#include <iostream>
#include <chrono>


namespace CppCliWindows
{
	ref class WPFCppCliDemo : Window
	{
		// MockChannel references
		IMessage* pMessage_;
		ISendr* pSendr_;
		IRecvr* pRecvr_;
		IMockChannel* pChann_;

		// Controls for Window
		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
		Grid^ hGrid = gcnew Grid();                    
		TabControl^ hTabControl = gcnew TabControl();
		TabItem^ hHomeTab = gcnew TabItem();
		TabItem^ hFileListTab = gcnew TabItem();
		TabItem^ hSearchTab = gcnew TabItem();
		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();

		// Controls for HomeTab View
		Label^ hLabelServers = gcnew Label();
		Label^ hLabelDirectory = gcnew Label();
		Label^ hLabelInstruction = gcnew Label();
		Grid^ hSendMessageGrid = gcnew Grid();
		Button^ hSendButton = gcnew Button();
		Button^ hClearButton = gcnew Button();
		ListBox^ hServerList = gcnew ListBox();
		ListBox^ hDirList = gcnew ListBox();
		ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
		ScrollViewer^ hScrollViewer2 = gcnew ScrollViewer();
		StackPanel^ hStackPanel1 = gcnew StackPanel();

		// Controls for FileListView View
		Grid^ hFileListGrid = gcnew Grid();
		StackPanel^ hStackPanel2 = gcnew StackPanel();
		Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		ListBox^ hListBox = gcnew ListBox();
		Button^ hFolderBrowseButton = gcnew Button();
		Button^ hUploadButton = gcnew Button();
		Grid^ hGrid2 = gcnew Grid();
		Label^ hLabelInstruction2 = gcnew Label();

		// Controls for Search View
		ComboBox^ hComboBox = gcnew ComboBox();
		Grid^ hSearchGrid = gcnew Grid();
		TextBox^ hText = gcnew TextBox();
		TextBox^ hTextWildCard = gcnew TextBox();
		Button^ hAddCardButton = gcnew Button();
		Button^ hSearchButton = gcnew Button();
		Button^ hClearCardsButton = gcnew Button();
		ListBox^ hWildCards = gcnew ListBox();
		ListBox^ hSearchResults = gcnew ListBox();
		Label^ hLabelResults = gcnew Label();
		Label^ hLabelWildCards = gcnew Label();
		Label^ hInstruction3 = gcnew Label();
		Label^ hLabelSearchType = gcnew Label();
		Label^ hLabelSearchText = gcnew Label();
		CheckBox^ xmlView = gcnew CheckBox();
		StackPanel^ hStackMain = gcnew StackPanel();
		StackPanel^ hStackInner1 = gcnew StackPanel();
		StackPanel^ hStackInner2 = gcnew StackPanel();
		StackPanel^ hStackInner2_1 = gcnew StackPanel();
		StackPanel^ hStackInner2_2 = gcnew StackPanel();
		StackPanel^ hStackInner2_3 = gcnew StackPanel();
		StackPanel^ hStackInner2_4 = gcnew StackPanel();
		StackPanel^ hStackInner2_5 = gcnew StackPanel();
		StackPanel^ hStackInner3 = gcnew StackPanel();
		Stopwatch^ stopWatch = gcnew Stopwatch();

		// receive thread
		Thread^ recvThread;	
	public:
		WPFCppCliDemo(System::String^, System::String^);// Parameterized Constructor for Client GUI.
														// Accepts IP and Port no. where the Client must listen
		~WPFCppCliDemo();								// Client GUI destructor
		void setUpStatusBar();							// Sets up Status Bar
		void setUpTabControl();							// Sets up Tab Control
		void setUpHomeView();							// Sets up Home View Tab (FileDownload tab)
		void setUpUploadView();							// Sets up Upload File Tab
		void setUpSearchView();							// Sets up Search Tab
		void serverListChanged(Object^ sender, SelectionChangedEventArgs^ args);
														// selection event handler for Servers list box.
		void fileSelectionChanged(Object^ sender, SelectionChangedEventArgs^ args);
														// selection event handler for download file list box.
		void uploadFileSelectionChanged(Object^ sender, SelectionChangedEventArgs^ args);
														// selection event handler for upload file list box.
		void searchTypeChanged(Object^ sender, SelectionChangedEventArgs^ args);
														// Event Handler for Search type Combo Box.
		void downloadSelectedFile(Object^ obj, RoutedEventArgs^ args);
														// Download button click event handler
														// Posts a download message in Send Queue
		void addWildCard(Object^ obj, RoutedEventArgs^ args);
														// Add Card button click event handler
														// add the wild card to the list
		void clearWildCard(Object^ obj, RoutedEventArgs^ args);
														// Clear Card button click event handler
														// clear the wild cards list
		void sendSearchQuery(Object^ obj, RoutedEventArgs^ args);
														// Search Button Click event Handler.
														// Posts a search message in the Send Queue

		void addText(String^ msg);						// Adds text to the Status Bar
		void getMessage();
														// Dispatches received messages from the Receive Queue
		void clear(Object^ sender, RoutedEventArgs^ args);
														// Clears Data on the File Download Tab
		void uploadSelectedFile(Object^ sender, RoutedEventArgs^ args);
														// Posts an upload message in the Send Queue
		void browseForFolder(Object^ sender, RoutedEventArgs^ args);
														// shows the browse folder dialog box
		void OnLoaded(Object^ sender, RoutedEventArgs^ args);
														// handle events when window pops up
		void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);
														// handle events when Window closes

		void getDirectories(IMessage*);					// show directories in Results listbox
		void getSearchResults(IMessage*);				// show search results in Results listbox
		void getSearchResultsXml(IMessage*);			// show search results in XML format in Results listbox
		void showDirectory(String^ arg);				// Show directory in Listbox
		void showResult(String^ arg);					// Show result in Result Listbox
		private:
		std::string toStdString(String^ pStr);			// converts System::String^ to std::string
		String^ toSystemString(std::string& str);		// converts std::string to SYstem::String^
		void setUpHomeTabProperties();					// sets textbox properties in File Download tab
		void setButtonsProperties();					// sets Button properties in File Download tab
		void setUpStackPanels();						// sets up stack panels in Search Tab control
		void setUpSearchComponents();					// set up Search tab components
		void setUpPanelsHelper();						// setup helper to setUpStackPanels method
		std::string parseIp(System::String^ item);		// parse IP from GUI
		std::string parsePort(System::String^ item);	// parse Port from GUI

		String^ portNo = "";
		String^ ip = "";
		String^ serverPort = "";
		String^ serverIp = "";
		String^ downloadFile = "";
		String^ uploadFile = "";
		String^ defaultStatus = "Client Listening on port: ";
		String^ searchType = "";
	};
}


#endif
