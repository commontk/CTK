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

#ifndef __ctkDependencyGraph_h
#define __ctkDependencyGraph_h

// CTK includes
#if !defined(NO_SYMBOL_EXPORT)
#include "ctkCoreExport.h"
#else
#define CTK_CORE_EXPORT
#endif

#include <list>

class ctkDependencyGraphPrivate;

/// \ingroup Core
/// \class ctkDependencyGraph
/// \brief Class to implement a dependency graph, converted to STL instead of Qt.
class CTK_CORE_EXPORT ctkDependencyGraph
{
public:
  ctkDependencyGraph(int nvertices);
  virtual ~ctkDependencyGraph();
  
  void printAdditionalInfo()const;
  void printGraph()const;
  
  /// Get the number of vertices associated with current graph
  int numberOfVertices()const;
  
  /// Get the number of edges associated with current graph
  int numberOfEdges()const;
  
  /// Traverse graph and check for cycle
  bool checkForCycle();
  
  /// Return true if there is at least one cycle
  bool cycleDetected()const;
  
  /// If a cycle has been detected, return the origin of the cycle otherwise 0.
  int cycleOrigin()const;
  
  /// If a cycle has been detected, return the end of the cycle otherwise 0.
  int cycleEnd()const;
  
  // The traverse of the tree will print information on standard output
  void setVerbose(bool verbose);
  
  /// Insert edge
  /// (from, to) indicate a relation between two vertices
  /// Note also that vertex id should be >= 1
  void insertEdge(int from, int to);

  /// Retrieve the paths between two vertices
  /// Caller is responsible to clear paths list
  void findPaths(int from, int to, std::list<std::list<int>* >& paths);
  
  /// Retrieve the path between two vertices
  void findPath(int from, int to, std::list<int>& path);
  
  /// List of edge to exclude
  /// An edge is specified using its extremity
  void setEdgeListToExclude(const std::list<int>& list);
  
  /// The default implementation check if 'edge' is in the list of edge to exclude
  /// See setEdgeListToExclude
  virtual bool shouldExcludeEdge(int edge)const;
  
  /// Called each time an edge is visited
  virtual void processEdge(int /*from*/, int /*to*/){}
  
  /// Perform a topological sort
  /// Return false if the graph contains cycles
  /// If a rootId is given, the subgraph starting at the root id is sorted
  /// See cycleDetected, cycleOrigin, cycleEnd
  bool topologicalSort(std::list<int>& sorted, int rootId = -1);

  /// Retrieve all vertices with indegree 0
  void sourceVertices(std::list<int>& sources);

protected:
  ctkDependencyGraphPrivate* d_ptr;

private:

  // Intentionally disable copy semantics
  ctkDependencyGraph(const ctkDependencyGraph &);
  ctkDependencyGraph &operator=(const ctkDependencyGraph &);
};

#endif

