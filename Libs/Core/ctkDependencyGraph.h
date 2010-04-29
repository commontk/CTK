/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

#ifndef __ctkDependencyGraph_h
#define __ctkDependencyGraph_h

// Qt includes
#include <QString>
#include <QList>

// CTK includes
#if !defined(NO_SYMBOL_EXPORT)
#include "CTKCoreExport.h"
#else
#define CTK_CORE_EXPORT
#endif

class CTK_CORE_EXPORT ctkDependencyGraph
{
public:
  ctkDependencyGraph(int nvertices);
  ~ctkDependencyGraph();
  
  void printAdditionalInfo();
  void printGraph();
  
  /// Get the number of vertices associated with current graph
  int numberOfVertices();
  
  /// Get the number of edges associated with current graph
  int numberOfEdges();
  
  /// Traverse graph and check for cycle
  bool checkForCycle();
  
  /// Return true if there is at least one cycle
  bool cycleDetected();
  
  /// If a cycle has been detected, return the origin of the cycle otherwise 0.
  int cycleOrigin();
  
  /// If a cycle has been detected, return the end of the cycle otherwise 0.
  int cycleEnd();
  
  // The traverse of the tree will print information on standard output
  void setVerbose(bool verbose);
  
  /// Insert edge
  /// (from, to) indicate a relation between two vertices
  /// Note also that vertex id should be >= 1
  void insertEdge(int from, int to);

  /// Retrieve the paths between two vertices
  /// Caller is responsible to clear paths list
  void findPaths(int from, int to, QList<QList<int>* >& paths);
  
  /// Retrieve the path between two vertices
  void findPath(int from, int to, QList<int>& path);
  
  /// List of edge to exclude
  /// An edge is specified using its extremity
  void setEdgeListToExclude(const QList<int>& list);
  
  /// The default implementation check if 'edge' is in the list of edge to exclude
  /// See setEdgeListToExclude
  virtual bool shouldExcludeEdge(int edge);
  
  /// Called each time an edge is visited
  virtual void processEdge(int /*from*/, int /*to*/){}
  
  /// Perform a topological search
  /// Return false if the graph contains cycles
  /// See cycleDetected, cycleOrigin, cycleEnd
  bool topologicalSort(QList<int>& sorted);
  
private:
  class ctkInternal; 
  ctkInternal* Internal;
};

#endif

