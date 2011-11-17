/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// CTK includes
#include "ctkDependencyGraph.h"
#include "ctkDependencyGraphTestHelper.h"

// STL includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDependencyGraphTest1(int argc, char * argv [] )
{
  if (argc > 1)
    {
    std::cerr << argv[0] << " expects zero arguments" << std::endl;
    }

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
//  graph.printGraph();  // printAdditionalInfo also prints graph.
  
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

  std::list<int> path;
  std::list<int> expectedPath;

  graph.findPath( 8, 7, path );
  expectedPath.push_back(8);
  expectedPath.push_back(6);
  expectedPath.push_back(7);

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
  expectedPath.push_back(1);
  expectedPath.push_back(5);
  expectedPath.push_back(7);

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
  expectedPath.push_back(3);
  expectedPath.push_back(4);
  expectedPath.push_back(1);
  expectedPath.push_back(5);
  expectedPath.push_back(7);

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
  expectedPath.push_back(2);
  expectedPath.push_back(1);
  expectedPath.push_back(5);

  if (path != expectedPath)
    {
    std::cerr << "Problem with findPath()" << std::endl;
    printIntegerList("current:", path);
    printIntegerList("expected:", expectedPath);
    return EXIT_FAILURE;
    }

  path.clear();
  expectedPath.clear();

  std::list<std::list<int>* > paths;
  std::list<int> expectedPath1;
  std::list<int> expectedPath2;
  std::list<int> expectedPath3;

  graph.findPaths(14, 5, paths);

  expectedPath1.push_back(14);
  expectedPath1.push_back(9);
  expectedPath1.push_back(3);
  expectedPath1.push_back(4);
  expectedPath1.push_back(1);
  expectedPath1.push_back(5);

  expectedPath2.push_back(14);
  expectedPath2.push_back(9);
  expectedPath2.push_back(2);
  expectedPath2.push_back(1);
  expectedPath2.push_back(5);

  std::list<std::list<int>* >::const_iterator pathsIterator;
  for(pathsIterator = paths.begin(); pathsIterator != paths.end(); pathsIterator++)
    {
    if (*(*pathsIterator) != expectedPath1 && *(*pathsIterator) != expectedPath2)
      {
      printIntegerList("current:", *(*pathsIterator));
      printIntegerList("expected:", expectedPath1, false);
      printIntegerList(" or ", expectedPath2);
      return EXIT_FAILURE;
      }
    }

  expectedPath1.clear();
  expectedPath2.clear();

  graph.findPaths(14, 7, paths);

  expectedPath1.push_back(14);
  expectedPath1.push_back(9);
  expectedPath1.push_back(3);
  expectedPath1.push_back(4);
  expectedPath1.push_back(1);
  expectedPath1.push_back(5);
  expectedPath1.push_back(7);

  expectedPath2.push_back(14);
  expectedPath2.push_back(9);
  expectedPath2.push_back(2);
  expectedPath2.push_back(1);
  expectedPath2.push_back(5);
  expectedPath2.push_back(7);

  expectedPath3.push_back(14);
  expectedPath3.push_back(13);
  expectedPath3.push_back(12);
  expectedPath3.push_back(11);
  expectedPath3.push_back(10);
  expectedPath3.push_back(7);

  for(pathsIterator = paths.begin(); pathsIterator != paths.end(); pathsIterator++)
    {
    if (*(*pathsIterator) != expectedPath1 && *(*pathsIterator) != expectedPath2 && *(*pathsIterator) != expectedPath3)
      {
      printIntegerList("current:", *(*pathsIterator));
      printIntegerList("expected:", expectedPath1, false);
      printIntegerList(" or ", expectedPath2, false);
      printIntegerList(" or ", expectedPath3);
      return EXIT_FAILURE;
      }
    }

  return EXIT_SUCCESS;
}
