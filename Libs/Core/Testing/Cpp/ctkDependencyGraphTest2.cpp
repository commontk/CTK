/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

int ctkDependencyGraphTest2(int argc, char * argv [] )
{
  if (argc > 1)
    {
    std::cerr << argv[0] << " expects zero arguments" << std::endl;
    }

  // check that cycle detection works
  {
  const int numberOfVertices = 2;

  ctkDependencyGraph graph(numberOfVertices);

  //
  //  1  ->  2
  //   \    /
  //     <-
  //
  graph.insertEdge(1,2);
  graph.insertEdge(2,1);

  int expectedNumberOfEdge = 2;
  
  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    std::cerr << "Number of vertices does not match (expected" << numberOfVertices << " got " << nov << ")" << std::endl;
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    std::cerr << "Number of edges does not match (expected" << expectedNumberOfEdge << " got " << noe << ")" << std::endl;
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();
  
  if( cfc == false )
    {
    std::cerr << "Cycle detection failed" << std::endl;
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == false )
    {
    std::cerr << "Cycle detected flag wrong" << std::endl;
    return EXIT_FAILURE;
    }

  int co = graph.cycleOrigin();
  int ce = graph.cycleEnd();

  if (co != 2)
    {
    std::cerr << "Wrong cycle origin (expected" << 2 << " got " << co << " )" << std::endl;
    return EXIT_FAILURE;
    }

  if (ce != 1)
    {
    std::cerr << "Wrong cycle end (expected" << 1 << " got " << ce << " )" << std::endl;
    return EXIT_FAILURE;
    }

  }

  {
  const int numberOfVertices = 4;

  ctkDependencyGraph graph(numberOfVertices);

  //         ->  3
  //       /
  // 1 -> 2  ->  4
  //       \    /
  //         <-
  //
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);
  graph.insertEdge(2,4);
  graph.insertEdge(4,2);

  int expectedNumberOfEdge = 4;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    std::cerr << "Number of vertices does not match (expected" << numberOfVertices << " got " << nov << ")" << std::endl;
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    std::cerr << "Number of edges does not match (expected" << expectedNumberOfEdge << " got " << noe << ")" << std::endl;
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == false )
    {
    std::cerr << "Cycle detection failed" << std::endl;
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == false )
    {
    std::cerr << "Cycle detected flag wrong" << std::endl;
    return EXIT_FAILURE;
    }

  int co = graph.cycleOrigin();
  int ce = graph.cycleEnd();

  if (co != 2)
    {
    std::cerr << "Wrong cycle origin (expected" << 2 << " got " << co << ")" << std::endl;
    return EXIT_FAILURE;
    }

  if (ce != 4)
    {
    std::cerr << "Wrong cycle end (expected" << 4 << " got " << ce << ")" << std::endl;
    return EXIT_FAILURE;
    }

  }

  // check that cycle detection works on disconnected graphs
  {
  const int numberOfVertices = 8;

  ctkDependencyGraph graph(numberOfVertices);

  /* 1 -> 2  -> 3
   *       \
   *         -> 4
   *
   *         ->  7
   *       /
   * 5 -> 6  ->  8
   *  \         /
   *    ---<---
   */
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);
  graph.insertEdge(2,4);
  graph.insertEdge(5,6);
  graph.insertEdge(6,7);
  graph.insertEdge(6,8);
  graph.insertEdge(8,5);

  int expectedNumberOfEdge = 7;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    std::cerr << "Number of vertices does not match (expected" << numberOfVertices << " got " << nov << ")" << std::endl;
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    std::cerr << "Number of edges does not match (expected" << expectedNumberOfEdge << " got " << noe << ")" << std::endl;
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == false )
    {
    std::cerr << "Cycle detection failed" << std::endl;
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == false )
    {
    std::cerr << "Cycle detected flag wrong" << std::endl;
    return EXIT_FAILURE;
    }

  }

  // check that topological ordering and paths
  // work on disconnected graphs
  {
  const int numberOfVertices = 11;

  ctkDependencyGraph graph(numberOfVertices);

  /* 1 -> 2  -> 3
   *       \
   *         -> 4
   *
   *         ->  7 ->
   *       /          \
   * 5 -> 6  ->  8 ->  9
   *             ^
   *             |
   *            10 -> 11
   */
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);
  graph.insertEdge(2,4);
  graph.insertEdge(5,6);
  graph.insertEdge(6,7);
  graph.insertEdge(6,8);
  graph.insertEdge(7,9);
  graph.insertEdge(8,9);
  graph.insertEdge(10,8);
  graph.insertEdge(10,11);

  int expectedNumberOfEdge = 10;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    std::cerr << "Number of vertices does not match (expected" << numberOfVertices << " got " << nov << ")" << std::endl;
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    std::cerr << "Number of edges does not match (expected" << expectedNumberOfEdge << " got " << noe << ")" << std::endl;
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == true )
    {
    std::cerr << "Cycle detection failed" << std::endl;
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == true )
    {
    std::cerr << "Cycle detected flag wrong" << std::endl;
    return EXIT_FAILURE;
    }

  std::list<int> sources;
  graph.sourceVertices(sources);

  std::list<int> expectedSources;
  expectedSources.push_back(1);
  expectedSources.push_back(5);
  expectedSources.push_back(10);

  if (sources != expectedSources)
    {
    std::cerr << "Problem with sourceVertices()" << std::endl;
    printIntegerList("sources:", sources);
    printIntegerList("expectedSources:", expectedSources);
    return EXIT_FAILURE;
    }

  std::list<int> globalSort;
  graph.topologicalSort(globalSort);

  std::list<int> expectedGlobalSort;

  expectedGlobalSort.push_back(1);
  expectedGlobalSort.push_back(5);
  expectedGlobalSort.push_back(10);
  expectedGlobalSort.push_back(2);
  expectedGlobalSort.push_back(6);
  expectedGlobalSort.push_back(11);
  expectedGlobalSort.push_back(3);
  expectedGlobalSort.push_back(4);
  expectedGlobalSort.push_back(7);
  expectedGlobalSort.push_back(8);
  expectedGlobalSort.push_back(9);

  if (globalSort != expectedGlobalSort)
  {
    std::cerr << "Problem with topologicalSort(globalSort)" << std::endl;
    printIntegerList("globalSort:", globalSort);
    printIntegerList("expectedGlobalSort:", expectedGlobalSort);
    return EXIT_FAILURE;
  }

  std::list<int> subSort10;
  graph.topologicalSort(subSort10, 10);

  std::list<int> expectedSubSort10;
  expectedSubSort10.push_back(10);
  expectedSubSort10.push_back(8);
  expectedSubSort10.push_back(11);
  expectedSubSort10.push_back(9);

  if (subSort10 != expectedSubSort10)
  {
    std::cerr << "Problem with topologicalSort(subSort10, 10)" << std::endl;
    printIntegerList("subSort10:", subSort10);
    printIntegerList("expectedSubSort10:", expectedSubSort10);
    return EXIT_FAILURE;
  }

  }

  // check that topological ordering and paths
  // work on disconnected graphs and isolated vertices
  {
  const int numberOfVertices = 13;

  ctkDependencyGraph graph(numberOfVertices);

  /* 1 -> 2  -> 3
   *       \
   *         -> 4
   *
   *         ->  7 ->
   *       /          \
   * 5 -> 6  ->  8 ->  9
   *             ^
   *             |
   *            10 -> 11
   *
   * 12   13
   */
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);
  graph.insertEdge(2,4);
  graph.insertEdge(5,6);
  graph.insertEdge(6,7);
  graph.insertEdge(6,8);
  graph.insertEdge(7,9);
  graph.insertEdge(8,9);
  graph.insertEdge(10,8);
  graph.insertEdge(10,11);

  int expectedNumberOfEdge = 10;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    std::cerr << "Number of vertices does not match (expected" << numberOfVertices << " got " << nov << ")" << std::endl;
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    std::cerr << "Number of edges does not match (expected" << expectedNumberOfEdge << " got " << noe << ")" << std::endl;
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == true )
    {
    std::cerr << "Cycle detection failed" << std::endl;
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == true )
    {
    std::cerr << "Cycle detected flag wrong" << std::endl;
    return EXIT_FAILURE;
    }

  std::list<int> sources;
  graph.sourceVertices(sources);

  std::list<int> expectedSources;
  expectedSources.push_back(1);
  expectedSources.push_back(5);
  expectedSources.push_back(10);
  expectedSources.push_back(12);
  expectedSources.push_back(13);

  if (sources != expectedSources)
    {
    std::cerr << "Problem with sourceVertices(sources)" << std::endl;
    printIntegerList("sources:", sources);
    printIntegerList("expectedSources:", expectedSources);
    return EXIT_FAILURE;
    }

  std::list<int> globalSort;
  graph.topologicalSort(globalSort);

  std::list<int> expectedGlobalSort;
  expectedGlobalSort.push_back(1);
  expectedGlobalSort.push_back(5);
  expectedGlobalSort.push_back(10);
  expectedGlobalSort.push_back(12);
  expectedGlobalSort.push_back(13);
  expectedGlobalSort.push_back(2);
  expectedGlobalSort.push_back(6);
  expectedGlobalSort.push_back(11);
  expectedGlobalSort.push_back(3);
  expectedGlobalSort.push_back(4);
  expectedGlobalSort.push_back(7);
  expectedGlobalSort.push_back(8);
  expectedGlobalSort.push_back(9);

  if (globalSort != expectedGlobalSort)
  {
    std::cerr << "Problem with topologicalSort(globalSort)" << std::endl;
    printIntegerList("globalSort:", globalSort);
    printIntegerList("expectedGlobalSort:", expectedGlobalSort);
    return EXIT_FAILURE;
  }

  std::list<int> subSort10;
  graph.topologicalSort(subSort10, 10);

  std::list<int> expectedSubSort10;
  expectedSubSort10.push_back(10);
  expectedSubSort10.push_back(8);
  expectedSubSort10.push_back(11);
  expectedSubSort10.push_back(9);

  if (subSort10 != expectedSubSort10)
  {
    std::cerr << "Problem with topologicalSort(subSort10, 10)" << std::endl;
    printIntegerList("subSort10:", subSort10);
    printIntegerList("expectedSubSort10:", expectedSubSort10);
    return EXIT_FAILURE;
  }

  std::list<int> subSort12;
  graph.topologicalSort(subSort12, 12);

  std::list<int> expectedSubSort12;
  expectedSubSort12.push_back(12);

  if (subSort12 != expectedSubSort12)
  {
    std::cerr << "Problem with topologicalSort(subSort12, 12)" << std::endl;
    printIntegerList("subSort12:", subSort12);
    printIntegerList("expectedSubSort12:", expectedSubSort12);
    return EXIT_FAILURE;
  }

  }

  // check that topological ordering and paths
  // work on a null graph
  {
  const int numberOfVertices = 3;

  ctkDependencyGraph graph(numberOfVertices);

  /*
   * 1  2  3
   */
  // a null graph has no edges
  int expectedNumberOfEdge = 0;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    std::cerr << "Number of vertices does not match (expected" << numberOfVertices << " got " << nov << ")" << std::endl;
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    std::cerr << "Number of edges does not match (expected" << expectedNumberOfEdge << " got " << noe << ")" << std::endl;
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == true )
    {
    std::cerr << "Cycle detection failed" << std::endl;
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == true )
    {
    std::cerr << "Cycle detected flag wrong" << std::endl;
    return EXIT_FAILURE;
    }

  std::list<int> sources;
  graph.sourceVertices(sources);

  std::list<int> expectedSources;
  expectedSources.push_back(1);
  expectedSources.push_back(2);
  expectedSources.push_back(3);

  if (sources != expectedSources)
    {
    std::cerr << "Problem with sourceVertices(sources)" << std::endl;
    printIntegerList("sources:", sources);
    printIntegerList("expectedSources:", expectedSources);
    return EXIT_FAILURE;
    }

  std::list<int> globalSort;
  graph.topologicalSort(globalSort);

  std::list<int> expectedGlobalSort;
  expectedGlobalSort.push_back(1);
  expectedGlobalSort.push_back(2);
  expectedGlobalSort.push_back(3);

  if (globalSort != expectedGlobalSort)
  {
    std::cerr << "Problem with topologicalSort(globalSort)" << std::endl;
    printIntegerList("globalSort:", globalSort);
    printIntegerList("expectedGlobalSort:", expectedGlobalSort);
    return EXIT_FAILURE;
  }

  std::list<int> subSort2;
  graph.topologicalSort(subSort2, 2);

  std::list<int> expectedSubSort2;
  expectedSubSort2.push_back(2);
  if (subSort2 != expectedSubSort2)
  {
    std::cerr << "Problem with topologicalSort(subSort2, 2)" << std::endl;
    printIntegerList("subSort2:", subSort2);
    printIntegerList("expectedSubSort2:", expectedSubSort2);
    return EXIT_FAILURE;
  }

  }

  return EXIT_SUCCESS;
}
