#ifndef DATASTORE_H
#define DATASTORE_H
///////////////////////////////////////////////////////////////////////////////
// DataStore.h - Efficiently store file and path information from Dir tree   //
//                                                                           //
// Application:   Project #1, CSE687 - Object Oriented Design, Spring 2014   //
// Platform:      Dell XPS 2720, Win 8.1, Visual Studio 2013                 //
// Author:        Jim Fawcett, CST 4-187, Syracuse University                //
//                (315) 443-3948, jfawcett@twcny.rr.com                      //
///////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* DataStore is a stand-alone package that has no dependencies other than on
* The utilities package.  Its purpose is to efficiently store directory 
* information.
* - Stores each filename and path only once.
*
* User Interface:
* save(file, path)
* DataStore::iterator iter = ds.begin();
* std::vector<std::string> paths = getPaths();
* size_t nFiles = ds.numberOfFiles();
* size_t nPaths = ds.numberOfPaths();
*
* Required Files:
* ---------------
* DataStore.h, DataStore.cpp
*
* Build Process:
* --------------
* From the Visual Studio Developer's Command Prompt:
* devenv VSHelp.sln /rebuild debug
*
* Maintenance History:
* --------------------
* - Ver 1.0 : 1 Mar 2015
*   first release
*/

#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>

class DataStore
{
public:
  using Path = std::string;
  using Paths = std::set < Path > ;
  using PathIter = Paths::iterator;
  using ListOfIters = std::list < PathIter > ;
  using File = std::string;
  using Store = std::map < File, ListOfIters > ;
  using iterator = Store::iterator;
  using PathCollection = std::vector < Path > ;

  void save(const std::string& filespec);
  void save(const std::string& filename, const std::string& path);
  Paths getPaths()	{ return paths; }
  PathCollection getPaths(const File& file);
  size_t numberOfFiles();
  size_t numberOfPaths();
  iterator begin() { return store.begin(); }
  iterator end() { return store.end(); }
private:
  Store store;
  Paths paths;
};

inline size_t DataStore::numberOfFiles() { return store.size(); }

inline size_t DataStore::numberOfPaths() { return paths.size(); }

#endif
