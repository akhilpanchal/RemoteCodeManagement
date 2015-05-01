#ifndef FILEMGR_H
#define FILEMGR_H
///////////////////////////////////////////////////////////////////////////////////////
// FileMgr.h - Find all files on specified path or tree matching specified patterns  //
//                                                                                   //
// Application:   Project #1, CSE687 - Object Oriented Design, Spring 2014           //
// Platform:      Dell XPS 2720, Win 8.1, Visual Studio 2013                         //
// Source:        Jim Fawcett, CST 4-187, Syracuse University						 //
//                (315) 443-3948, jfawcett@twcny.rr.com								 //
// Author:			Akhil Panchal, Syracuse University								 //
//					(408) 921-0731, ahpancha@syr.edu								 //
///////////////////////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * FileMgr uses the services of FileSystem to find files and stores its findings
 * efficiently in DataStore.
 * - Finds all files, matching a set of patterns, along with their paths.
 * - Stores each filename and path only once, using DataStore.
 *
 * User Interface:
 * changePath("/aPath");
 * addPattern("*.h");
 * search();
 *
 * Required Files:
 * ---------------
 * FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp,
 * DataStore.h, DataStore.cpp, Utilities.h, Utilities.cpp
 *
 * Build Process:
 * --------------
 * From the Visual Studio Developer's Command Prompt:
 * devenv Sockets.sln /rebuild debug
 *
 * Maintenance History:
 * --------------------
 * - Ver 1.0 : 1 Mar 2015
 *   first release
 */

#include "DataStore.h"
#include <iostream>

class FileMgr
{
public:
  using iterator = DataStore::iterator;
  using patterns = std::vector<std::string>;

  FileMgr(const std::string& path, DataStore& ds);
  bool FileMgr::changePath(const std::string& path);
  void addPattern(const std::string& patt);
  void search(bool recurse = false);
  void find(const std::string& path);
private:
  std::string path_;
  DataStore& store_;
  patterns patterns_;
};

#endif
