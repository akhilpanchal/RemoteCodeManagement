///////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application		 //
// ver 3.0														 //
// Application: Client GUI using WPF in .NET.					 //
//				CSE 687 Project #4								 //
// Platform:    Dell XPS 2720, Win 8.1 Pro, Visual Studio 2013   //
// Source:      Jim Fawcett, CST 4-187, 443-3948                 //
//              jfawcett@twcny.rr.com                            //
// Author:      Akhil Panchal, Syracuse University			     //
//              (408) 921-0731, ahpancha@syr.edu	             //
///////////////////////////////////////////////////////////////////
/*
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*/
#include "Window.h"
using namespace CppCliWindows;

//----------< Constructor for WPFCppCliDemo: Initiates Sender, Receiver for the Client >----------

WPFCppCliDemo::WPFCppCliDemo(System::String^ ipAdd, System::String^ port)
{	// set up channel
	portNo = port;
	ip = ipAdd;
	ObjectFactory* pObjFact = new ObjectFactory;
	pMessage_ = pObjFact->createMessage();
	pSendr_ = pObjFact->createSendr();
	pRecvr_ = pObjFact->createRecvr(System::Convert::ToInt32(portNo));
	Console::WriteLine("\n Client Listening on port: " + portNo);
	hStatus->Text = defaultStatus + portNo;
	pChann_ = pObjFact->createMockChannel(pSendr_, pRecvr_, pMessage_);
	pChann_->start();
	delete pObjFact;
	recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFCppCliDemo::getMessage));	// client's receive thread
	recvThread->Start();
	this->Loaded += gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);// set event handlers
	this->Closing += gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
	hSendButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::downloadSelectedFile);
	hClearButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
	hUploadButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::uploadSelectedFile);
	hAddCardButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addWildCard);
	hClearCardsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clearWildCard);
	hSearchButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendSearchQuery);
	hServerList->SelectionChanged += gcnew SelectionChangedEventHandler(this, &WPFCppCliDemo::serverListChanged);
	hDirList->SelectionChanged += gcnew SelectionChangedEventHandler(this, &WPFCppCliDemo::fileSelectionChanged);
	hListBox->SelectionChanged += gcnew SelectionChangedEventHandler(this, &WPFCppCliDemo::uploadFileSelectionChanged);
	hComboBox->SelectionChanged += gcnew SelectionChangedEventHandler(this, &WPFCppCliDemo::searchTypeChanged);
	// set Window properties
	this->Title = "WPF C++/CLI Demo";
	this->Width = 1000;
	this->Height = 700;
	// attach dock panel to Window
	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);
	// setup Window controls and views
	setUpTabControl();
	setUpStatusBar();
	setUpHomeView();
	setUpUploadView();
	setUpSearchView();
}

//----------< Destructor for WPFCppCliDemo: Deletes Sender and Receiver for the Client >----------
WPFCppCliDemo::~WPFCppCliDemo()
{
  delete pChann_;
  delete pSendr_;
  delete pRecvr_;
}

//----------< Set up Status Bar in the Client GUI >----------
void WPFCppCliDemo::setUpStatusBar()
{
  hStatusBar->Items->Add(hStatusBarItem);
  hStatus->Text += ".\tVery important messages will appear here";
  hStatus->FontWeight = FontWeights::Bold;
  hStatusBarItem->Content = hStatus;
  hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

//----------< Sets us Tab Control in GUI >----------
void WPFCppCliDemo::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hGrid->Height = 800;
	hHomeTab->Header = "File Download";
	hFileListTab->Header = "File Upload";
	hSearchTab->Header = "Search";
	hTabControl->Items->Add(hHomeTab);
	hTabControl->Items->Add(hFileListTab);
	hTabControl->Items->Add(hSearchTab);
	// Read from Server Info File
	String^ filecontent = System::IO::File::ReadAllText("MockChannel/Root/ServerInfo.txt");
	String^ servername = "";
	for (int i = 0; i < filecontent->Length; i++)
	{
		if (filecontent[i]!='\n')
			servername += filecontent[i];
		else
		{
			hServerList->Items->Add(servername);
			servername = "";
		}
	}
	if (servername!="")
	hServerList->Items->Add(servername);
}

//----------< Sets up Home Tab Components >----------
void WPFCppCliDemo::setUpHomeTabProperties()
{
	hLabelServers->Content = "Server List";
	hLabelDirectory->Content = "Files Hosted";
	hLabelInstruction->Content = "To Download a file(Requirement #3):\n1. Select a Server from the list, select one of the hosted files and Click Download!\n";
	hLabelInstruction->Content += "2. All downloaded files will be Stored in the MockChannel/Root/DownloadDirectory folder.\n";
	hLabelInstruction->Content += "3. (Requirement #4 and bonus)The \"Performance Information (Reqs Processing time and End to End time)";
	hLabelInstruction->Content += "\"\n\twill be showed in the Status bar below!.";
	hLabelInstruction->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hLabelInstruction->FontWeight = FontWeights::Bold;
	hLabelInstruction->FontSize = 12;
	hLabelInstruction->Height = 120;
	hSendMessageGrid->Children->Add(hLabelInstruction);
	hSendMessageGrid->Height = 700;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(30);
	hSendMessageGrid->RowDefinitions->Add(hRow1Def);
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(350);
	hSendMessageGrid->RowDefinitions->Add(hRow2Def);
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(50);
	hSendMessageGrid->RowDefinitions->Add(hRow3Def);
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(20);
	hSendMessageGrid->RowDefinitions->Add(hRow4Def);
	ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
	hSendMessageGrid->ColumnDefinitions->Add(hCol1Def);
	hCol1Def->Width = GridLength(300);
	ColumnDefinition^ hCol2Def = gcnew ColumnDefinition();
	hSendMessageGrid->ColumnDefinitions->Add(hCol2Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hServerList;
	Border^ hBorder2 = gcnew Border();
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hDirList;
	hServerList->Padding = Thickness(15);
	hServerList->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hServerList->FontWeight = FontWeights::Bold;
	hServerList->FontSize = 12;
	hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer1->Content = hBorder1;
	hScrollViewer2->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer2->Content = hBorder2;
}

//----------< Sets up Home Tab Buttons >----------
void WPFCppCliDemo::setButtonsProperties()
{
  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(75);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def);
  hSendButton->Content = "Download File";
  Border^ hBorder2 = gcnew Border();
  hBorder2->Width = 120;
  hBorder2->Height = 30;
  hBorder2->BorderThickness = Thickness(1);
  hBorder2->BorderBrush = Brushes::Black;
  hClearButton->Content = "Clear";
  hBorder2->Child = hSendButton;
  Border^ hBorder3 = gcnew Border();
  hBorder3->Width = 120;
  hBorder3->Height = 30;
  hBorder3->BorderThickness = Thickness(1);
  hBorder3->BorderBrush = Brushes::Black;
  hBorder3->Child = hClearButton;
  hStackPanel1->Children->Add(hBorder2);
  TextBlock^ hSpacer = gcnew TextBlock();
  hSpacer->Width = 10;
  hStackPanel1->Children->Add(hSpacer);
  hStackPanel1->Children->Add(hBorder3);
  hStackPanel1->Orientation = Orientation::Horizontal;
  hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hSendMessageGrid->SetRow(hStackPanel1, 2);
  hSendMessageGrid->Children->Add(hStackPanel1);
}

//----------< Sets up Home Tab View >----------
void WPFCppCliDemo::setUpHomeView()
{
	Console::Write("\n  setting up sendMessage view");
	hSendMessageGrid->Margin = Thickness(20);
	hHomeTab->Content = hSendMessageGrid;
	hServerList->SelectionMode = System::Windows::Controls::SelectionMode::Multiple;
	setUpHomeTabProperties();
	setButtonsProperties();
	hSendMessageGrid->SetRow(hLabelServers, 0);
	hSendMessageGrid->SetColumn(hLabelServers, 0);
	hSendMessageGrid->SetRow(hLabelDirectory, 0);
	hSendMessageGrid->SetColumn(hLabelDirectory, 1);
	hSendMessageGrid->SetRow(hScrollViewer1, 1);
	hSendMessageGrid->SetColumn(hScrollViewer1, 0);
	hSendMessageGrid->SetRow(hScrollViewer2, 1);
	hSendMessageGrid->SetColumn(hScrollViewer2, 1);
	hSendMessageGrid->SetRow(hLabelInstruction, 4);
	hSendMessageGrid->SetColumn(hLabelInstruction, 0);
	hSendMessageGrid->SetColumnSpan(hLabelInstruction, 2);
	hSendMessageGrid->Children->Add(hLabelServers);
	hSendMessageGrid->Children->Add(hLabelDirectory);
	hSendMessageGrid->Children->Add(hScrollViewer1);
	hSendMessageGrid->Children->Add(hScrollViewer2);
	
}

//----------< Converts .NET's System::String^ to C++ std::String >----------
std::string WPFCppCliDemo::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
	dst += (char)pStr[i];
	return dst;
}

//----------< Server List selection changed event-handler >----------
void WPFCppCliDemo::serverListChanged(Object^ sender, SelectionChangedEventArgs^ args)
{
	stopWatch->Reset();
	stopWatch->Start();
	hDirList->Items->Clear();
	std::string sIp;
	std::string sPort;
	for each (Object^ item in hServerList->SelectedItems)
	{
		sIp = parseIp(item->ToString());
		sPort = parsePort(item->ToString());
		serverIp = toSystemString(sIp);
		serverPort = toSystemString(sPort);
		pMessage_->constructMessage1("get_dir", "", sIp, sPort , toStdString(ip), toStdString(portNo));
		pMessage_->setBody("null");
		pMessage_->setTime("null");
		pSendr_->postMessage(pMessage_);
		Console::WriteLine("\n  Sending get_dir reqs:");
	}
	if (hServerList->SelectedIndex == -1)
	{
		serverIp = "";
		serverPort = "";
	}
}

//----------< Converts C++ std::String to .NET's System::String^ >----------
String^ WPFCppCliDemo::toSystemString(std::string& str)
{
  StringBuilder^ pStr = gcnew StringBuilder();
  for (size_t i = 0; i < str.size(); ++i)
    pStr->Append((Char)str[i]);
  return pStr->ToString();
}

//----------< Adds content to Status Bar >----------
void WPFCppCliDemo::addText(String^ msg)
{
	hStatus->Text = defaultStatus + portNo + "\t" + msg;
}

//----------< Dispatches Messages from Receive Queue to WPF GUI >----------
void WPFCppCliDemo::getMessage()
{
	// recvThread runs this function
	while (true)
	{
		std::cout << "\n  receive thread calling getMessage()";
		pMessage_ = pRecvr_->getMessage();

		stopWatch->Stop();
		String^ sMsg;
		if (pMessage_->getCommand() == "ack_get_dir")
		{
			getDirectories(pMessage_);
			sMsg = "Directories and Files listed from: " + toSystemString(pMessage_->getSendIP() + " " + pMessage_->getSendPort());
		}
		if (pMessage_->getCommand() == "ack_search_text" || pMessage_->getCommand() == "ack_search_file")
		{
			getSearchResults(pMessage_);
			sMsg = "Search Results for '" + toSystemString(pMessage_->getValue()) + "'";
		}
		if (pMessage_->getCommand() == "file_upload")
		{
			sMsg = "File Downloaded: " + toSystemString(pMessage_->getValue());
			if (pMessage_->getValue() == "searchRes.xml")
				getSearchResultsXml(pMessage_);
		}
		if (pMessage_->getCommand() == "ack_file_upload")
		{
			sMsg = "File Uploaded: " + toSystemString(pMessage_->getValue());
		}
		sMsg += ". Request Processing Time: " + toSystemString(pMessage_->getTime()) + " milliseconds!";
		sMsg += ". End to End Processing Time: " + stopWatch->ElapsedMilliseconds.ToString() + " milliseconds!";

		array<String^>^ args = gcnew array<String^>(1);
		args[0] = sMsg;
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
	}
}

//----------< Clears content on the Home tab >----------
void WPFCppCliDemo::clear(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  cleared message text");
  hStatus->Text = "Cleared message";
  hServerList->Items->Clear();
}

//----------< Sets up Upload tab view >----------
void WPFCppCliDemo::setUpUploadView()
{
	Console::Write("\n  setting up FileList view");
	hLabelInstruction2->Content = "To Upload a file (Requirement #3):\n\n1: Select a Server from the 'File Download' Tab.\n2: ";
	hLabelInstruction2->Content += "Browse a directory by clicking on 'Browse Directory' button above.\n";
	hLabelInstruction2->Content += "3. Select a file from the directory from the listbox.\n";
	hLabelInstruction2->Content += "4: Click on the 'Upload' Button! All acknowledgements and\n";
	hLabelInstruction2->Content += "5. Performance Information will be displayed on the status Bar.";
	hLabelInstruction2->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hLabelInstruction2->FontWeight = FontWeights::Bold;	hLabelInstruction2->FontSize = 12;
	hFileListGrid->Margin = Thickness(20);
	hFileListTab->Content = hFileListGrid;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(400);
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(50);
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hFileListGrid->RowDefinitions->Add(hRow1Def);
	hFileListGrid->RowDefinitions->Add(hRow2Def);
	hFileListGrid->RowDefinitions->Add(hRow3Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hListBox;  
	hFileListGrid->Children->Add(hBorder1);
	hFileListGrid->Children->Add(hLabelInstruction2);
	hFolderBrowseButton->Content = "Browse Directory";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 120;
	hFolderBrowseButton->BorderThickness = Thickness(2);
	hFolderBrowseButton->BorderBrush = Brushes::Black;
	hUploadButton->Content = "Upload File";
	hUploadButton->Height = 30;
	hUploadButton->Width = 120;
	hUploadButton->BorderThickness = Thickness(2);
	hUploadButton->BorderBrush = Brushes::Black;
	hStackPanel2->Children->Add(hFolderBrowseButton);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 10;
	hStackPanel2->Children->Add(hSpacer2);
	hStackPanel2->Children->Add(hUploadButton);
	hStackPanel2->Orientation = Orientation::Horizontal;
	hStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hFileListGrid->SetRow(hLabelInstruction2, 2);
	hFileListGrid->SetRow(hBorder1, 0);
	hFileListGrid->SetRow(hStackPanel2,1);
	hFileListGrid->Children->Add(hStackPanel2);
	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}

//----------< Sets up Search tab view >----------
void WPFCppCliDemo::setUpSearchView()
{
	Console::Write("\n  setting up Connection view");
	hSearchGrid->Margin = Thickness(20);
	hSearchTab->Content = hSearchGrid;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(400);
	hSearchGrid->RowDefinitions->Add(hRow1Def);
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(200);
	hSearchGrid->RowDefinitions->Add(hRow2Def);
	ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
	hSearchGrid->ColumnDefinitions->Add(hCol1Def);
	hCol1Def->Width = GridLength(300);
	ColumnDefinition^ hCol2Def = gcnew ColumnDefinition();
	hSearchGrid->ColumnDefinitions->Add(hCol2Def);
	hAddCardButton->Content = "Add Card";	hSearchButton->Content = "Search";
	hClearCardsButton->Content = "Clear";
	hLabelResults->Content = "Search Results";
	hLabelWildCards->Content = "WildCards:\t       ";
	hInstruction3->Content = "Requirement #5, #6, #7, #8 & #10:\n1. Select one or more servers from the File Download Tab.\n";
	hInstruction3->Content += "2. Select Search Type from drop-down menu.\n3. Enter File or Text to be searched.\n";
	hInstruction3->Content += "4. Add Wild Cards/Patterns by entering it in the provided Text-box and Clicking on 'Add Card/Pattern' Button.\n";
	hInstruction3->Content += "\t4.1 You can clear cards by selecting the ones you want to remove and clicking on 'Clear' button.\n";
	hInstruction3->Content += "\t4.2 If No cards are selected, all cards will be removed from the Search Query.\n";
	hInstruction3->Content += "5. You can check the 'VIEW IN XML' check box to view the result in XML.: (Requirement 8)\n";
	hInstruction3->Content += "6. After completing the search Query, click on the 'Search' button and wait for the results in the Text Box.\n";
	hInstruction3->Content += "7. Like always, Performance Information and acknowledgements will be displayed on the status bar.\n";
	hInstruction3->Content += "8. For Requirement #10 confirmation, please see the file Dispatcher.cpp, Line #88 & #113 in ./Peer folder.\n";
	hInstruction3->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hInstruction3->FontWeight = FontWeights::Bold;	hInstruction3->FontSize = 12;	hInstruction3->Height = 200;
	setUpSearchComponents();	setUpStackPanels();
	hSearchGrid->Children->Add(hInstruction3);
	hSearchGrid->Children->Add(hSearchResults);
	hSearchGrid->Children->Add(hStackInner2);
	hSearchGrid->Children->Add(xmlView);
	hSearchGrid->SetRow(hInstruction3,1);
	hSearchGrid->SetColumn(hInstruction3, 0);
	hSearchGrid->SetColumnSpan(hInstruction3, 2);
	xmlView->Content = "VIEW IN XML";	xmlView->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	xmlView->FontWeight = FontWeights::Bold;	xmlView->FontSize = 14;
	hSearchGrid->SetRow(xmlView, 1);
	hSearchGrid->SetColumn(xmlView, 1);
	hSearchGrid->SetRow(hSearchResults, 0);
	hSearchGrid->SetColumn(hSearchResults, 1);
	hSearchGrid->SetRow(hStackInner2, 0);
	hSearchGrid->SetColumn(hStackInner2, 0);
	hStackInner2->Orientation = Orientation::Vertical;
}

//----------< Sets up Search Tab components >----------
void WPFCppCliDemo::setUpSearchComponents()
{
	hLabelSearchType->Content = "Select Search Type";
	hLabelSearchText->Content = "Enter Text:\t";
	hComboBox->Items->Add("Text Search");
	hComboBox->Items->Add("File Search");
	hTextWildCard->Text = "Enter Card Here";
	hWildCards->SelectionMode = System::Windows::Controls::SelectionMode::Multiple;
	hStackInner2_2->Orientation = Orientation::Horizontal;
	hStackInner2_1->Orientation = Orientation::Horizontal;
	hStackInner2_3->Orientation = Orientation::Horizontal;
	hStackInner2_4->Orientation = Orientation::Horizontal;
	hStackInner2_5->Orientation = Orientation::Horizontal;
}

//----------< Sets up Search tab stack panels >----------
void WPFCppCliDemo::setUpStackPanels()
{
	Border^ hBorder1 = gcnew Border();
	hBorder1->Width = 120;
	hBorder1->Height = 30;
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hSearchButton;
	setUpPanelsHelper();
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 180;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hText;
	Border^ hBorder5 = gcnew Border();
	hBorder5->Width = 180;
	hBorder5->Height = 30;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	hBorder5->Child = hComboBox;
	Border^ hBorder7 = gcnew Border();
	hBorder7->Width = 120;
	hBorder7->Height = 30;
	hBorder7->BorderThickness = Thickness(1);
	hBorder7->BorderBrush = Brushes::Black;
	hBorder7->Child = hClearCardsButton;
	hStackInner2_3->Children->Add(hLabelSearchText);
	hStackInner2_3->Children->Add(hBorder4);
	hStackInner2_4->Children->Add(hLabelSearchType);
	hStackInner2_4->Children->Add(hBorder5);
	TextBlock^ hSpacer4 = gcnew TextBlock();
	hSpacer4->Width = 10;
	hSpacer4->Height = 50;
	hStackInner2_5->Children->Add(hBorder7);
	hStackInner2_5->Children->Add(hSpacer4);
	hStackInner2_5->Children->Add(hBorder1);
}

//----------< Sets up Stack panels in Search Tab helper >----------
void WPFCppCliDemo::setUpPanelsHelper()
{
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 100;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hWildCards;
	hStackInner2_1->Children->Add(hLabelWildCards);
	hStackInner2_1->Children->Add(hBorder3);
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hAddCardButton;
	Border^ hBorder6 = gcnew Border();
	hBorder6->Width = 120;
	hBorder6->Height = 30;
	hBorder6->BorderThickness = Thickness(1);
	hBorder6->BorderBrush = Brushes::Black;
	hBorder6->Child = hTextWildCard;
	hStackInner2_2->Children->Add(hBorder6);
	TextBlock^ hSpacer5 = gcnew TextBlock();
	hSpacer5->Width = 10;
	hSpacer5->Height = 50;
	hStackInner2_2->Children->Add(hSpacer5);
	hStackInner2_2->Children->Add(hBorder2);
	hStackInner2->Children->Add(hStackInner2_4);
	TextBlock^ hSpacer1 = gcnew TextBlock();
	hSpacer1->Width = 20;
	hSpacer1->Height = 50;
	hStackInner2->Children->Add(hSpacer1);
	hStackInner2->Children->Add(hStackInner2_3);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 10;
	hSpacer2->Height = 50;
	hStackInner2->Children->Add(hSpacer2);
	hStackInner2->Children->Add(hStackInner2_2);
	TextBlock^ hSpacer3 = gcnew TextBlock();
	hSpacer3->Width = 10;
	hSpacer3->Height = 10;
	hStackInner2->Children->Add(hSpacer3);
	hStackInner2->Children->Add(hStackInner2_1);
	TextBlock^ hSpacer6 = gcnew TextBlock();
	hSpacer6->Width = 10;
	hSpacer6->Height = 10;
	hStackInner2->Children->Add(hSpacer6);
	hStackInner2->Children->Add(hStackInner2_5);
}

//----------< Sets up Browse Folder Button in Upload tab >----------
void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
  std::cout << "\n  Browsing for folder";
  hListBox->Items->Clear();
  System::Windows::Forms::DialogResult result;
  result = hFolderBrowserDialog->ShowDialog();
  if (result == System::Windows::Forms::DialogResult::OK)
  {
    String^ path = hFolderBrowserDialog->SelectedPath;
    std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
    array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
    for (int i = 0; i < files->Length; ++i)
      hListBox->Items->Add(files[i]);
    array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
    for (int i = 0; i < dirs->Length; ++i)
      hListBox->Items->Add(L"<> " + dirs[i]);
  }
}

//----------< Parses IP from GUI >----------
std::string WPFCppCliDemo::parseIp(System::String^ item)
{
	std::string ip = "";
	for (int i = 0; i < item->Length && item[i] != ':'; i++)
	{
		ip += (char)item[i];
	}
	return ip;
}

//----------< parses Port from GUI >----------
std::string WPFCppCliDemo::parsePort(System::String^ item)
{
	std::string port = "";
	int i;
	for (i = 0; i < item->Length && item[i] != ':'; i++)
	{
	}
	for (i = i + 1; i < item->Length; i++)
	{
		port += (char)item[i];
	}
	return port;
}

//----------< Posts a get directories message in the send queue with the selected server >----------
void WPFCppCliDemo::getDirectories(IMessage* imsg)
{
	string body = imsg->getBody();
	std::string filename = "";
	array<String^>^ args = gcnew array<String^>(1);
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::showDirectory);
	for (unsigned int i = 0; i < body.size(); i++)
	{
		if (body[i] == '@')
		{
			Dispatcher->Invoke(act, toSystemString(filename));  // must call addText on main UI thread
			filename = "";
			i++;
		}
		filename += body[i];
	}
}

//----------< Shows directory in the listbox in file download tab >----------
void WPFCppCliDemo::showDirectory(String^ arg)
{
	hDirList->Items->Add(arg);
}

//----------< Sets up Upload tab view >----------
void WPFCppCliDemo::fileSelectionChanged(Object^ sender, SelectionChangedEventArgs^ args)
{
	for each (Object^ item in hDirList->SelectedItems)
	{
		downloadFile = item->ToString();
	}
}

//----------< Upload File Listbox selection changed event-handler >----------
void WPFCppCliDemo::uploadFileSelectionChanged(Object^ sender, SelectionChangedEventArgs^ args)
{
	for each (Object^ item in hListBox->SelectedItems)
	{
		uploadFile = item->ToString();
	}
}

//----------< Posts a download message int the send queue with the selected item in the listbox >----------
void WPFCppCliDemo::downloadSelectedFile(Object^ obj, RoutedEventArgs^ args)
{
	stopWatch->Reset();
	stopWatch->Start();

	if (downloadFile != "" && downloadFile[0] != '<')
	{
		pMessage_->constructMessage1("file_download", toStdString(downloadFile->ToString()), toStdString(serverIp), toStdString(serverPort), toStdString(ip), toStdString(portNo));
		pMessage_->setBody("null");
		pMessage_->setTime("null");
		pSendr_->postMessage(pMessage_);
		Console::WriteLine("\n1=" + downloadFile->ToString() + "=1" + "Sending file download reqs:\n");
	}
	else
	{
		hStatus->Text = defaultStatus + portNo + "\t Please Select a Server and a File before clicking Download!";
	}
}

//----------< Posts a upload message int the send queue with the selected item in the listbox >----------
void WPFCppCliDemo::uploadSelectedFile(Object^ sender, RoutedEventArgs^ args)
{
	stopWatch->Reset();
	stopWatch->Start();

	if (uploadFile != "" && serverIp != "")
	{
		pMessage_->constructMessage1("file_upload", toStdString(uploadFile->ToString()), toStdString(serverIp), toStdString(serverPort), toStdString(ip), toStdString(portNo));
		pMessage_->setBody("null");
		pMessage_->setTime("null");
		pSendr_->postMessage(pMessage_);
		Console::WriteLine("\n1=" + downloadFile->ToString() + "=1" + "Sending file upload reqs:\n");
	}
	else
	{
		hStatus->Text = defaultStatus + portNo + "\t Please Select a Server and a File before clicking Upload!";
	}
}

//----------< Adds a wild card to the listbox >----------
void WPFCppCliDemo::addWildCard(Object^ obj, RoutedEventArgs^ args)
{
	if (hTextWildCard->Text == "")
	{
		hStatus->Text = defaultStatus + portNo + " Please Enter a valid Wildcard before clicking 'Add Card'.";
	}
	else
		hWildCards->Items->Add(hTextWildCard->Text);
}

//----------< Clears the selected wild cards from the listbox >----------
void WPFCppCliDemo::clearWildCard(Object^ obj, RoutedEventArgs^ args)
{
	if (hWildCards->SelectedIndex == -1)
		hWildCards->Items->Clear();
	else
	{
		while (hWildCards->SelectedItems->Count > 0)
			hWildCards->Items->Remove(hWildCards->SelectedItem);
	}
}

//----------< Drop down menu selection changed event handler >----------
void WPFCppCliDemo::searchTypeChanged(Object^ sender, SelectionChangedEventArgs^ args)
{
	searchType = hComboBox->SelectedItem->ToString();
	if (searchType == "Text Search")
	{
		hLabelSearchText->Content = "Enter Text:\t";
		hLabelWildCards->Content = "WildCards:\t       ";
		hAddCardButton->Content = "Add Card";
	}
	if (searchType == "File Search")
	{
		hLabelSearchText->Content = "Enter File Name:    ";
		hLabelWildCards->Content = "File Patterns:\t       ";
		hAddCardButton->Content = "Add Pattern";
	}
}

//----------< Posts a search message int the send queue with the information entered in the Search tab >----------
void WPFCppCliDemo::sendSearchQuery(Object^ obj, RoutedEventArgs^ args)
{
	stopWatch->Reset();	stopWatch->Start();
	hSearchResults->Items->Clear();
	if (searchType == "")
		hStatus->Text = defaultStatus + portNo + "\tPlease Select a Search Type from the Drop-Down menu!";
	else if (hText->Text == "" || hText->Text == "   Enter Text\t")
	{
		hStatus->Text = defaultStatus + portNo + "\tPlease Enter a Keyword to search in the Textbox provided!";
		hText->Text = "   Enter Text\t";
	}
	else
	{
		if (serverIp == "")
			hStatus->Text = defaultStatus + portNo + "\tPlease select a server first on the File Download Tab!";
		else
		{
			if (xmlView->IsChecked && searchType == "Text Search")
			{
				pMessage_->constructMessage1("xml_search_text", toStdString(hText->Text), toStdString(serverIp), toStdString(serverPort), toStdString(ip), toStdString(portNo));
			}
			else if (xmlView->IsChecked && searchType == "File Search")
			{
				pMessage_->constructMessage1("xml_search_file", toStdString(hText->Text), toStdString(serverIp), toStdString(serverPort), toStdString(ip), toStdString(portNo));
			}
			else if (searchType == "Text Search")
			{
				pMessage_->constructMessage1("search_text", toStdString(hText->Text), toStdString(serverIp), toStdString(serverPort), toStdString(ip), toStdString(portNo));

			}
			else if (searchType == "File Search")
			{
				pMessage_->constructMessage1("search_file", toStdString(hText->Text), toStdString(serverIp), toStdString(serverPort), toStdString(ip), toStdString(portNo));
			}
		}
		String^ body = "";
		if (hWildCards->Items->Count == 0)
			body += "*.*";
		else
		{
			for (int i = hWildCards->Items->Count - 1; i >= 0; i--)
				body += hWildCards->Items->GetItemAt(i)->ToString() + "#";
		}
		Console::WriteLine("\n" + body);
		pMessage_->setBody(toStdString(body));
		pMessage_->setTime("null");
		pSendr_->postMessage(pMessage_);
	}
}

//----------< Displays the received Search results on the GUI >----------
void WPFCppCliDemo::getSearchResults(IMessage* imsg)
{
	string body = imsg->getBody();
	std::string filename = "";
	array<String^>^ args = gcnew array<String^>(1);
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::showResult);
	Dispatcher->Invoke(act, toSystemString(imsg->getSendIP() + " " + imsg->getSendPort()));  // must call addText on main UI thread
	for (unsigned int i = 0; i < body.size(); i++)
	{
		if (body[i] == '@')
		{
			Dispatcher->Invoke(act, toSystemString(filename));  // must call addText on main UI thread
			filename = "";
			i++;
		}
		filename += body[i];
	}
}

//----------< Displays the received search results in XML format on the GUI >----------
void WPFCppCliDemo::getSearchResultsXml(IMessage* imsg)
{
	String^ filecontent = System::IO::File::ReadAllText("MockChannel/Root/DownloadDirectory/searchRes.xml");
	array<String^>^ args = gcnew array<String^>(1);
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::showResult);
	Dispatcher->Invoke(act, toSystemString(imsg->getSendIP() + " " + imsg->getSendPort()));  // must call addText on main UI thread
	Dispatcher->Invoke(act, filecontent);  // must call addText on main UI thread
}

//----------< Shows the passed result to the GUI >----------
void WPFCppCliDemo::showResult(String^ arg)
{
	hSearchResults->Items->Add(arg);
}

//----------< Window onload event handler >----------
void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  Window loaded");
}

//----------< Window closed event handler >----------
void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
  Console::Write("\n  Window closing");
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
	try{
		Console::WriteLine(L"\n Starting WPFCppCliDemo");
		Application^ app = gcnew Application();
		app->Run(gcnew WPFCppCliDemo(args[0], args[1]));
		Console::WriteLine(L"\n\n");
	}
	catch (System::Exception^ ex)
	{
		Console::WriteLine("Exception Caught: '{0}'", ex);
	}
}