/////////////////////////////////////////////////////////////////////////////////////////
// FileMgr.cpp - Find all files on specified path or tree matching specified patterns  //
//                                                                                     //
// Application:   Project #1, CSE687 - Object Oriented Design, Spring 2014             //
// Platform:      Dell XPS 2720, Win 8.1, Visual Studio 2013                           //
// Source:        Jim Fawcett, CST 4-187, Syracuse University                          //
//                (315) 443-3948, jfawcett@twcny.rr.com                                //
// Author:		 Akhil Panchal, Syracuse University									   //
//              (408) 921-0731, ahpancha@syr.edu									   //
/////////////////////////////////////////////////////////////////////////////////////////

#include "FileMgr.h"
#include "FileSystem.h"
#define STATIC_LIB
#include "Utilities.h"
#include <iostream>
#include <string>

//----------< FileMgr constructor: Manages Directories and Files at the specified path and stores info in specified DataStore >---------
FileMgr::FileMgr(const std::string& path, DataStore& ds) : path_(path), store_(ds)
{
  patterns_.push_back("*.*");
}

//----------< Change managed path of the FileMgr >---------
bool FileMgr::changePath(const std::string& path)
{
  if (FileSystem::Directory::exists(path))
  {
    path_ = path;
    return true;
  }
  return false;
}

//----------< Adds file pattern to the Manager >---------
void FileMgr::addPattern(const std::string& patt)
{
  if (patterns_.size() == 1 && patterns_[0] == "*.*")
    patterns_.pop_back();
  patterns_.push_back(patt);
}

//----------< Searches for filepatterns in the managed subdirectory tree recursivly/non-recursively based on the bool value passed >---------
void FileMgr::search(bool recurse)
{
  std::string fullPath = path_;
  if (recurse)
    find(fullPath);
  else
  {
    for (auto patt : patterns_)
    {
      std::vector<std::string> files = FileSystem::Directory::getFiles(fullPath, patt);
      for (auto file : files)
      {
        store_.save(file, fullPath);
      }
    }
  }
}

//----------< finds the files in the specified path >---------
void FileMgr::find(const std::string& path)
{
  for (auto patt : patterns_)
  {
    std::vector<std::string> files = FileSystem::Directory::getFiles(path, patt);
    for (auto f : files)
    {
      store_.save(f, path);
    }
  }
  std::vector<std::string> subdirs = FileSystem::Directory::getDirectories(path);
  for (auto d : subdirs)
  {
    if (d != "." && d != "..")
    {
      find(path + "\\" + d);
    }
  }
}


//-----< File Manager TEST STUB>-----
#ifdef TEST_FILEMGR

int main()
{
  title("Testing FileMgr", '=');
  std::cout << "\n";

  DataStore ds;
  FileMgr fm("../..", ds);

  title("Non-recursive search");
  fm.search();
  for (auto fs : ds)
  {
    std::string file = fs.first;
    std::cout << "\n  " << file;
    DataStore::PathCollection pc = ds.getPaths(file);
    for (auto path : pc)
    {
      std::cout << "\n    " << path;
    }
    ///////////////////////////////////
    // This works too:
    //  for (auto iter : fs.second)
    //  {
    //    std::cout << "\n    " << *iter;
    //  }
  }
  std::cout << "\n";

  title("Recursive search");
  fm.search(true);
  for (auto fs : ds)
  {
    std::string file = fs.first;
    std::cout << "\n  " << file;
    DataStore::PathCollection pc = ds.getPaths(file);
    for (auto path : pc)
    {
      std::cout << "\n    " << path;
    }
  }
  std::cout << "\n\n";
  return 0;
}

#endif
