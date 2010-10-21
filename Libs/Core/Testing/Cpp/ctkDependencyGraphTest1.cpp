/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// CTK includes
#include "ctkDependencyGraph.h"

// STL includes
#include <cstdlib>
#include <iostream>

namespace
{
void printIntegerList(const char* msg, const QList<int>& list, bool endl = true)
{
  std::cerr << msg; 
  foreach(int l, list)
    {
    std::cerr << l << " "; 
    }
  if (endl)
    {
    std::cerr << std::endl;
    }
}
}

//-----------------------------------------------------------------------------
int ctkDependencyGraphTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);  

  const int numberOfVertices = 14;

  ctkDependencyGraph  graph(numberOfVertices);

  graph.setVerbose(true);
  graph.setVerbose(false);
  //graph.setVerbose(true);

  //
  // 6 - 8
  // |
  // 7 - 5 - 1 - 2 - 
  // |       |     | - 9
  // |       4 - 3 -   |
  // |                 |
  // 10 - 11 - 12 - 13 - 14
  //
  graph.insertEdge(3,4);
  graph.insertEdge(4,1);
  graph.insertEdge(1,5);
  graph.insertEdge(5,7);
  graph.insertEdge(2,1);
  graph.insertEdge(8,6);
  graph.insertEdge(6,7);
  graph.insertEdge(9,2);
  graph.insertEdge(9,3);
  graph.insertEdge(14,9);
  graph.insertEdge(14,13);
  graph.insertEdge(13,12);
  graph.insertEdge(12,11);
  graph.insertEdge(11,10);
  graph.insertEdge(10,7);

  int expectedNumberOfEdge = 15;
  

  graph.printAdditionalInfo();
  graph.printGraph();
  
  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();
  
  if( cfc == true )
    {
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == true )
    {
    return EXIT_FAILURE;
    }

  //int corigin = graph.cycleOrigin();

  //int cend = graph.cycleEnd();

  QList<int> path;
  QList<int> expectedPath;

  graph.findPath( 8, 7, path );
  expectedPath << 8 << 6 << 7;
  if (path != expectedPath)
    {
    std::cerr << "Problem with findPath()" << std::endl;
    printIntegerList("current:", path);
    printIntegerList("expected:", expectedPath);
    return EXIT_FAILURE;
    }

  path.clear();
  expectedPath.clear();
  
  graph.findPath( 1, 7, path );
  expectedPath << 1 << 5 << 7;
  if (path != expectedPath)
    {
    std::cerr << "Problem with findPath()" << std::endl;
    printIntegerList("current:", path);
    printIntegerList("expected:", expectedPath);
    return EXIT_FAILURE;
    }

  path.clear();
  expectedPath.clear();
  
  graph.findPath( 3, 7, path );
  expectedPath << 3 << 4 << 1 << 5 << 7;
  if (path != expectedPath)
    {
    std::cerr << "Problem with findPath()" << std::endl;
    printIntegerList("current:", path);
    printIntegerList("expected:", expectedPath);
    return EXIT_FAILURE;
    }

  path.clear();
  expectedPath.clear();
  
  graph.findPath( 2, 5, path );
  expectedPath << 2 << 1 << 5;
  if (path != expectedPath)
    {
    std::cerr << "Problem with findPath()" << std::endl;
    printIntegerList("current:", path);
    printIntegerList("expected:", expectedPath);
    return EXIT_FAILURE;
    }

  path.clear();
  expectedPath.clear();

  QList<QList<int>* > paths;
  QList<int> expectedPath1;
  QList<int> expectedPath2;
  QList<int> expectedPath3;

  graph.findPaths(14, 5, paths);
  expectedPath1 << 14 << 9 << 3 << 4 << 1 << 5;
  expectedPath2 << 14 << 9 << 2 << 1 << 5;
  foreach(QList<int>* p, paths)
    {
    if (*p != expectedPath1 && *p != expectedPath2)
      {
      printIntegerList("current:", *p);
      printIntegerList("expected:", expectedPath1, false);
      printIntegerList(" or ", expectedPath2);
      return EXIT_FAILURE;
      }
    }

  expectedPath1.clear();
  expectedPath2.clear();

  graph.findPaths(14, 7, paths);
  expectedPath1 << 14 << 9 << 3 << 4 << 1 << 5 << 7;
  expectedPath2 << 14 << 9 << 2 << 1 << 5 << 7;
  expectedPath3 << 14 << 13 << 12 << 11 << 10 << 7;
  foreach(QList<int>* p, paths)
    {
    if (*p != expectedPath1 && *p != expectedPath2 && *p != expectedPath3)
      {
      printIntegerList("current:", *p);
      printIntegerList("expected:", expectedPath1, false);
      printIntegerList(" or ", expectedPath2, false);
      printIntegerList(" or ", expectedPath3);
      return EXIT_FAILURE;
      }
    }

//   QList<int> list;
//   graph.setEdgeListToExclude( list );
// 
//   graph.shouldExcludeEdge(2);
// 
//   QList<int> sortedlist;
//   graph.topologicalSort( sortedlist );

  return EXIT_SUCCESS;
}
