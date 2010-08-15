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

// Qt includes
#include <QQueue>
#include <QVarLengthArray>
#include <QDebug>

// CTK includes
#include "ctkDependencyGraph.h"

// STD includes
#include <iostream>

#define MAXV 100
#define MAXDEGREE 50

//----------------------------------------------------------------------------
class ctkDependencyGraph::ctkInternal
{
public:
  ctkInternal(ctkDependencyGraph* p);
  
  /// Compute outdegree
  void computeOutdegrees(QVarLengthArray<int, MAXV>& computedOutdegrees);
  
  /// Traverse tree using Depth-first_search
  void traverseUsingDFS(int v);
  
  /// Called each time an edge is visited
  void processEdge(int from, int to); 
  
  /// Called each time a vertex is processed
  void processVertex(int v);

  /// Retrieve the path between two vertices
  void findPathDFS(int from, int to, QList<int>& path);

  /// Recursive function used by findPaths to retrieve the path between two vertices
  void findPathsRec(int from, int to, QList<int>* path, QList<QList<int>* >& paths);
  
  void setEdge(int vertice, int degree, int value);
  int edge(int vertice, int degree);

  void verticesWithIndegree(int indegree, QList<int>& list);
  
  /// See http://en.wikipedia.org/wiki/Adjacency_list
  QVarLengthArray<QVarLengthArray<int,MAXDEGREE>*, MAXV+1> Edges;
  QVarLengthArray<int, MAXV+1> OutDegree;
  QVarLengthArray<int, MAXV+1> InDegree;
  int NVertices;
  int NEdges;
  
  /// Structure used by DFS
  /// See http://en.wikipedia.org/wiki/Depth-first_search
  QVarLengthArray<bool, MAXV> Processed;	// processed vertices
  QVarLengthArray<bool, MAXV> Discovered; // discovered vertices
  QVarLengthArray<int, MAXV>  Parent;	    // relation discovered
  
  bool    Abort;	// Flag indicating if traverse should be aborted
  bool    Verbose; 
  bool    CycleDetected; 
  int     CycleOrigin; 
  int     CycleEnd;
  
  QList<int> ListOfEdgeToExclude;
  
  /// Pointer to the public API
  ctkDependencyGraph* P;
};

//----------------------------------------------------------------------------
// ctkInternal methods

//----------------------------------------------------------------------------
ctkDependencyGraph::ctkInternal::ctkInternal(ctkDependencyGraph* p)
{
  Q_ASSERT(p);
  this->P = p;
  this->NVertices = 0; 
  this->NEdges = 0; 
  this->Abort = false;
  this->Verbose = false;
  this->CycleDetected = false;
  this->CycleOrigin = 0;
  this->CycleEnd = 0;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::ctkInternal::computeOutdegrees(QVarLengthArray<int, MAXV>& computedOutdegrees)
{
	for (int i=1; i <= this->NVertices; i++)
	  {
    computedOutdegrees[i] = 0;
	  }

	for (int i=1; i <= this->NVertices; i++) 
	  {
    for (int j=0; j < this->OutDegree[i]; j++)
		  {
      computedOutdegrees[ this->edge(i,j) ] ++;
		  }
		}
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::ctkInternal::traverseUsingDFS(int v)
{
  // allow for search termination
	if (this->Abort)
	  {
	  return;
	  }

	this->Discovered[v] = true;
	this->processVertex(v);

  int y; // successor vertex
  for (int i=0; i<this->OutDegree[v]; i++)
	  {
		y = this->edge(v, i);
		if (this->P->shouldExcludeEdge(this->edge(v, i)) == false)
		  {
      this->Parent[y] = v;
			if (this->Discovered[y] == false)
			  {
				this->traverseUsingDFS(y);
			  } 
			else 
			  {
				if (this->Processed[y] == false)
				  {
					this->processEdge(v,y);
					}
			  }
		  }
		if (this->Abort)
		  {
		  return;
		  }
	}

	this->Processed[v] = true;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::ctkInternal::processEdge(int from, int to)
{
  if (this->Discovered[to] == true)
    {
    this->CycleDetected = true;
    this->CycleOrigin = to; 
    this->CycleEnd = from;
    if (this->Verbose)
      {
      QList<int> path;
      this->findPathDFS(from, to, path);
      qWarning() << "Cycle detected from " << to << " to " << from;
      qWarning() << " " << path;
      path.clear();
      this->findPathDFS(to, from, path);
      qWarning() << " " << path;
      }
    this->Abort = true;
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::ctkInternal::processVertex(int v)
{
	if (this->Verbose)
	  {
	  qDebug() << "processed vertex " << v;
	  }
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::ctkInternal::setEdge(int vertice, int degree, int value)
{
  Q_ASSERT(vertice <= this->NVertices);
  Q_ASSERT(degree < MAXDEGREE);
  (*this->Edges[vertice])[degree] = value; 
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::ctkInternal::edge(int vertice, int degree)
{
  Q_ASSERT(vertice <= this->NVertices);
  Q_ASSERT(degree < MAXDEGREE);
  return (*this->Edges[vertice])[degree];
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::ctkInternal::findPathDFS(int from, int to, QList<int>& path)
{
  if ((from == to) || (to == -1))
    {
    path << from;
    }
  else 
    {
    this->findPathDFS(from, this->Parent[to], path);
    path << to;
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::ctkInternal::findPathsRec(
  int from, int to, QList<int>* path, QList<QList<int>* >& paths)
{
  if (from == to)
    {
    return;
    }
  
  QList<int> branch(*path);
  int child = from;
  for (int j=0; j < this->OutDegree[child]; j++)
    {
    if (j == 0)
      {
      int parent = this->edge(child, j);
      *path << parent;
      this->findPathsRec(parent, to, path, paths);
      }
    else
      {
      int parent = this->edge(child, j);
      // Copy path and add it to the list
      QList<int>* pathCopy = new QList<int>(branch);
      paths << pathCopy;
      *pathCopy << parent;
      this->findPathsRec(parent, to, pathCopy, paths);
      }
    }
}

void ctkDependencyGraph::ctkInternal::verticesWithIndegree(int indegree, QList<int>& list)
{
  Q_ASSERT(indegree >= 0);

  for (int i=1; i <= this->NVertices; i++)
    {
    if (this->InDegree[i] == indegree)
      {
      list << i;
      }
    }
}
    
//----------------------------------------------------------------------------
// ctkDependencyGraph methods

//----------------------------------------------------------------------------
ctkDependencyGraph::ctkDependencyGraph(int nvertices)
{
  this->Internal = new ctkInternal(this);
  
  this->Internal->NVertices = nvertices; 
  
  // Resize internal array
  this->Internal->Processed.resize(nvertices + 1);
  this->Internal->Discovered.resize(nvertices + 1);
  this->Internal->Parent.resize(nvertices + 1);
  this->Internal->Edges.resize(nvertices + 1);
  this->Internal->OutDegree.resize(nvertices + 1);
  this->Internal->InDegree.resize(nvertices + 1);

  for (int i=1; i <= nvertices; i++)
    {
    this->Internal->OutDegree[i] = 0;
    this->Internal->InDegree[i] = 0;
    }
    
  // initialize Edge adjacency list
  for (int i=0; i <= nvertices; i++)
    {
    this->Internal->Edges[i] = new QVarLengthArray<int, MAXDEGREE>();
    this->Internal->Edges[i]->resize(MAXDEGREE);
    }
    
  // initialize search
  for (int i=1; i <= nvertices; i++)
    {
    this->Internal->Processed[i] = false;
    this->Internal->Discovered[i] = false;
    this->Internal->Parent[i] = -1;
    }
}

//----------------------------------------------------------------------------
ctkDependencyGraph::~ctkDependencyGraph()
{
  // Clean memory
  for (int i=0; i <= this->Internal->NVertices; i++)
    {
    delete this->Internal->Edges[i];
    }
    
  delete this->Internal; 
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::printAdditionalInfo()
{
  qDebug() << "ctkDependencyGraph (" << this << ")" << endl
           << " NVertices:" << this->numberOfVertices() << endl
           << " NEdges:" << this->numberOfEdges() << endl
           << " Abort:" << this->Internal->Abort;
           
  qDebug() << " [Processed]";
  for(int i=1; i < this->Internal->Processed.size(); i++)
    {
    qDebug() << i << "->" << this->Internal->Processed[i];
    }
  qDebug() << " [Discovered]";
  for(int i=1; i < this->Internal->Discovered.size(); i++)
    {
    qDebug() << i << "->" << this->Internal->Discovered[i];
    }
  qDebug() << " [Parent]";
  for(int i=1; i < this->Internal->Parent.size(); i++)
    {
    qDebug() << i << "->" << this->Internal->Parent[i];
    }
  qDebug() << " [Graph]"; 
  this->printGraph();
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::printGraph()
{
  for(int i=1; i <= this->Internal->NVertices; i++)
    {
    std::cout << i << ":";
    for (int j=0; j < this->Internal->OutDegree[i]; j++)
      {
      std::cout << " " << this->Internal->edge(i, j);
      }
    std::cout << std::endl;
    }
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::numberOfVertices()
{
  return this->Internal->NVertices;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::numberOfEdges()
{
  return this->Internal->NEdges;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::setVerbose(bool verbose)
{
  this->Internal->Verbose = verbose;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::setEdgeListToExclude(const QList<int>& list)
{
  this->Internal->ListOfEdgeToExclude = list;
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::shouldExcludeEdge(int edge)
{
  return this->Internal->ListOfEdgeToExclude.contains(edge);
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::checkForCycle()
{
  if (this->Internal->NEdges > 0)
    {
    // get a valid vertice Id
    int verticeId = 1;
    this->Internal->traverseUsingDFS(verticeId);
    }
  return this->cycleDetected();
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::cycleDetected()
{
  return this->Internal->CycleDetected;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::cycleOrigin()
{
  return this->Internal->CycleOrigin;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::cycleEnd()
{
  return this->Internal->CycleEnd;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::insertEdge(int from, int to)
{
  Q_ASSERT(from > 0 && from <= this->Internal->NVertices);
  Q_ASSERT(to > 0 && to <= this->Internal->NVertices);
  
  // resize if needed
  int capacity = this->Internal->Edges[from]->capacity(); 
  if (this->Internal->OutDegree[from] > capacity)
    {
    this->Internal->Edges[from]->resize(capacity + capacity * 0.3);
    }

  this->Internal->setEdge(from, this->Internal->OutDegree[from], to);
  this->Internal->OutDegree[from]++;
  this->Internal->InDegree[to]++;

  this->Internal->NEdges++;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::findPaths(int from, int to, QList<QList<int>* >& paths)
{
  QList<int>* path = new QList<int>;
  *path << from; 
  paths << path;
  this->Internal->findPathsRec(from, to, path, paths);

  QList<int> pathToRemove;
  // Remove list no ending with the requested element
  int i = 0; 
  while (!paths.isEmpty() && i < paths.size())
    {
    QList<int>* p = paths[i];
    Q_ASSERT(p);
    if (p->last() != to)
      {
      paths.removeAt(i);
      delete p; 
      }
    else
      {
      i++;
      }
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::findPath(int from, int to, QList<int>& path)
{
  int child = from;
  int parent = this->Internal->edge(child, 0);
  path << child; 
  while (parent > 0)
    {
    path << parent;
    if (parent == to)
      {
      break;
      }
    child = parent;
    parent = this->Internal->edge(child, 0);
    }
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::topologicalSort(QList<int>& sorted)
{
  QVarLengthArray<int, MAXV> outdegree; // outdegree of each vertex
  QQueue<int> zeroout;	  // vertices of outdegree 0
	int x, y;			        // current and next vertex
  
  outdegree.resize(this->Internal->NVertices + 1);
	
	// resize if needed
	if (this->Internal->NVertices > MAXV)
	  {
    outdegree.resize(this->Internal->NVertices);
	  }

  this->Internal->computeOutdegrees(outdegree);
	
	for (int i=1; i <= this->Internal->NVertices; i++)
	  {
    if (outdegree[i] == 0)
		  {
      zeroout.enqueue(i);
		  }
		}

	int j=0;
  while (zeroout.empty() == false)
	  {
		j = j+1;
    x = zeroout.dequeue();
		sorted << x;
    for (int i=0; i < this->Internal->OutDegree[x]; i++)
		  {
			y = this->Internal->edge(x, i);
      outdegree[y] --;
      if (outdegree[y] == 0)
			  {
        zeroout.enqueue(y);
			  }
		  }
	  }

	if (j != this->Internal->NVertices)
	  {
		return false;
		}
		
  return true;
}

void ctkDependencyGraph::sourceVertices(QList<int>& sources)
{
  this->Internal->verticesWithIndegree(0, sources);
}
