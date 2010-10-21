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
class ctkDependencyGraphPrivate
{
  Q_DECLARE_PUBLIC(ctkDependencyGraph);

public:

  ctkDependencyGraph* const q_ptr;

  ctkDependencyGraphPrivate(ctkDependencyGraph& p);
  
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
  int edge(int vertice, int degree)const;

  void verticesWithIndegree(int indegree, QList<int>& list);

  int subgraphSize(int rootId);
  void subgraphSizeRec(int rootId, QSet<int>& uniqueVertices);

  void subgraphInsert(ctkDependencyGraph& subgraph, int rootId,
                      QHash<int,int>& subgraphIdToGlobal, QHash<int,int>& globalIdToSubgraph);

  int getOrGenerateSubgraphId(QHash<int, int>& subgraphIdToGlobal,
                      QHash<int, int>& globalIdToSubgraph,
                      int globalId);

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
  
};

//----------------------------------------------------------------------------
// ctkInternal methods

//----------------------------------------------------------------------------
ctkDependencyGraphPrivate::ctkDependencyGraphPrivate(ctkDependencyGraph& object)
  :q_ptr(&object)
{
  this->NVertices = 0; 
  this->NEdges = 0; 
  this->Abort = false;
  this->Verbose = false;
  this->CycleDetected = false;
  this->CycleOrigin = 0;
  this->CycleEnd = 0;
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::computeOutdegrees(QVarLengthArray<int, MAXV>& computedOutdegrees)
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
void ctkDependencyGraphPrivate::traverseUsingDFS(int v)
{
  Q_Q(ctkDependencyGraph);
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
		if (q->shouldExcludeEdge(this->edge(v, i)) == false)
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
void ctkDependencyGraphPrivate::processEdge(int from, int to)
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
void ctkDependencyGraphPrivate::processVertex(int v)
{
	if (this->Verbose)
	  {
	  qDebug() << "processed vertex " << v;
	  }
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::setEdge(int vertice, int degree, int value)
{
  Q_ASSERT(vertice <= this->NVertices);
  Q_ASSERT(degree < MAXDEGREE);
  (*this->Edges[vertice])[degree] = value; 
}

//----------------------------------------------------------------------------
int ctkDependencyGraphPrivate::edge(int vertice, int degree)const
{
  Q_ASSERT(vertice <= this->NVertices);
  Q_ASSERT(degree < MAXDEGREE);
  return (*this->Edges[vertice])[degree];
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::findPathDFS(int from, int to, QList<int>& path)
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
void ctkDependencyGraphPrivate::findPathsRec(
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

void ctkDependencyGraphPrivate::verticesWithIndegree(int indegree, QList<int>& list)
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

void ctkDependencyGraphPrivate::subgraphSizeRec(int rootId, QSet<int>& uniqueVertices)
{
  Q_ASSERT(rootId > 0);

  for (int i = 0; i < this->OutDegree[rootId]; ++i)
    {
    int child = this->edge(rootId, i);
    uniqueVertices << child;
    subgraphSizeRec(child, uniqueVertices);
    }
}

int ctkDependencyGraphPrivate::subgraphSize(int rootId)
{
  Q_ASSERT(rootId > 0);

  QSet<int> vertices;
  vertices << rootId;
  this->subgraphSizeRec(rootId, vertices);
  return vertices.size();
}

void ctkDependencyGraphPrivate::subgraphInsert(
    ctkDependencyGraph& subgraph, int rootId,
    QHash<int,int>& subgraphIdToGlobal, QHash<int,int>& globalIdToSubgraph)
{
  int from = this->getOrGenerateSubgraphId(subgraphIdToGlobal, globalIdToSubgraph, rootId);
  for (int i = 0; i < this->OutDegree[rootId]; ++i)
  {
    int childId = this->edge(rootId, i);
    int to = this->getOrGenerateSubgraphId(subgraphIdToGlobal, globalIdToSubgraph,
                                           childId);

    subgraph.insertEdge(from, to);
    this->subgraphInsert(subgraph, childId, subgraphIdToGlobal, globalIdToSubgraph);
  }
}

int ctkDependencyGraphPrivate::getOrGenerateSubgraphId(
    QHash<int, int>& subgraphIdToGlobal,
    QHash<int, int>& globalIdToSubgraph,
    int globalId)
{
  // If needed, generate vertex id
  int subgraphId = -1;
  if (!globalIdToSubgraph.keys().contains(globalId))
    {
    subgraphId = globalIdToSubgraph.keys().size() + 1;
    globalIdToSubgraph[globalId] = subgraphId;
    subgraphIdToGlobal[subgraphId] = globalId;
    }
  else
    {
    subgraphId = globalIdToSubgraph[globalId];
    }
  return subgraphId;
}
    
//----------------------------------------------------------------------------
// ctkDependencyGraph methods

//----------------------------------------------------------------------------
ctkDependencyGraph::ctkDependencyGraph(int nvertices)
  :d_ptr(new ctkDependencyGraphPrivate(*this))
{
  Q_D(ctkDependencyGraph);
  d->NVertices = nvertices; 
  
  // Resize internal array
  d->Processed.resize(nvertices + 1);
  d->Discovered.resize(nvertices + 1);
  d->Parent.resize(nvertices + 1);
  d->Edges.resize(nvertices + 1);
  d->OutDegree.resize(nvertices + 1);
  d->InDegree.resize(nvertices + 1);

  for (int i=1; i <= nvertices; i++)
    {
    d->OutDegree[i] = 0;
    d->InDegree[i] = 0;
    }
    
  // initialize Edge adjacency list
  for (int i=0; i <= nvertices; i++)
    {
    d->Edges[i] = new QVarLengthArray<int, MAXDEGREE>();
    d->Edges[i]->resize(MAXDEGREE);
    }
    
  // initialize search
  for (int i=1; i <= nvertices; i++)
    {
    d->Processed[i] = false;
    d->Discovered[i] = false;
    d->Parent[i] = -1;
    }
}

//----------------------------------------------------------------------------
ctkDependencyGraph::~ctkDependencyGraph()
{
  Q_D(ctkDependencyGraph);
  // Clean memory
  for (int i=0; i <= d->NVertices; i++)
    {
    delete d->Edges[i];
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::printAdditionalInfo()const
{
  Q_D(const ctkDependencyGraph);
  qDebug() << "ctkDependencyGraph (" << this << ")" << endl
           << " NVertices:" << this->numberOfVertices() << endl
           << " NEdges:" << this->numberOfEdges() << endl
           << " Abort:" << d->Abort;
           
  qDebug() << " [Processed]";
  for(int i=1; i < d->Processed.size(); i++)
    {
    qDebug() << i << "->" << d->Processed[i];
    }
  qDebug() << " [Discovered]";
  for(int i=1; i < d->Discovered.size(); i++)
    {
    qDebug() << i << "->" << d->Discovered[i];
    }
  qDebug() << " [Parent]";
  for(int i=1; i < d->Parent.size(); i++)
    {
    qDebug() << i << "->" << d->Parent[i];
    }
  qDebug() << " [Graph]"; 
  this->printGraph();
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::printGraph()const
{
  Q_D(const ctkDependencyGraph);
  for(int i=1; i <= d->NVertices; i++)
    {
    std::cout << i << ":";
    for (int j=0; j < d->OutDegree[i]; j++)
      {
      std::cout << " " << d->edge(i, j);
      }
    std::cout << std::endl;
    }
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::numberOfVertices()const
{
  Q_D(const ctkDependencyGraph);
  return d->NVertices;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::numberOfEdges()const
{
  Q_D(const ctkDependencyGraph);
  return d->NEdges;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::setVerbose(bool verbose)
{
  Q_D(ctkDependencyGraph);
  d->Verbose = verbose;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::setEdgeListToExclude(const QList<int>& list)
{
  Q_D(ctkDependencyGraph);
  d->ListOfEdgeToExclude = list;
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::shouldExcludeEdge(int edge)const
{
  Q_D(const ctkDependencyGraph);
  return d->ListOfEdgeToExclude.contains(edge);
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::checkForCycle()
{
  Q_D(ctkDependencyGraph);
  if (d->NEdges > 0)
    {
    // Store unprocessed vertex ids
    QList<int> uncheckedVertices;
    for (int i = 1; i <= d->NVertices; ++i)
      {
        uncheckedVertices << i;
      }

    // Start the cycle detection on the source vertices
    QList<int> sources;
    this->sourceVertices(sources);
    foreach(int sourceId, sources)
      {
      d->traverseUsingDFS(sourceId);
      if (this->cycleDetected()) return true;

      for (int i=0; i < d->Processed.size(); i++)
        {
          if (d->Processed[i] == true)
            {
            uncheckedVertices.removeOne(i);
            }

          d->Discovered[i] = false;
          d->Processed[i] = false;
        }
      }

    // If a component does not have a source vertex,
    // i.e. it is a cycle a -> b -> a, check all non
    // processed vertices.
    while (!uncheckedVertices.empty())
      {
      d->traverseUsingDFS(uncheckedVertices.last());
      if (this->cycleDetected()) return true;

      for (int i=0; i < d->Processed.size(); i++)
        {
          if (d->Processed[i] == true)
            {
            uncheckedVertices.removeOne(i);
            }

          d->Discovered[i] = false;
          d->Processed[i] = false;
        }
      }
    }
  return this->cycleDetected();
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::cycleDetected()const
{
  Q_D(const ctkDependencyGraph);
  return d->CycleDetected;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::cycleOrigin()const
{
  Q_D(const ctkDependencyGraph);
  return d->CycleOrigin;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::cycleEnd()const
{
  Q_D(const ctkDependencyGraph);
  return d->CycleEnd;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::insertEdge(int from, int to)
{
  Q_D(ctkDependencyGraph);
  Q_ASSERT(from > 0 && from <= d->NVertices);
  Q_ASSERT(to > 0 && to <= d->NVertices);
  
  // resize if needed
  int capacity = d->Edges[from]->capacity(); 
  if (d->OutDegree[from] > capacity)
    {
    d->Edges[from]->resize(capacity + capacity * 0.3);
    }

  d->setEdge(from, d->OutDegree[from], to);
  d->OutDegree[from]++;
  d->InDegree[to]++;

  d->NEdges++;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::findPaths(int from, int to, QList<QList<int>* >& paths)
{
  Q_D(ctkDependencyGraph);
  QList<int>* path = new QList<int>;
  *path << from; 
  paths << path;
  d->findPathsRec(from, to, path, paths);

  QList<int> pathToRemove;
  // Remove list no ending with the requested element
  int i = 0; 
  while (!paths.isEmpty() && i < paths.size())
    {
    QList<int>* path = paths[i];
    Q_ASSERT(path);
    if (path->last() != to)
      {
      paths.removeAt(i);
      delete path; 
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
  QList<QList<int>* > paths;
  this->findPaths(from, to, paths);
  if (!paths.empty())
    {
    path << *(paths.first());
    }

  qDeleteAll(paths);
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::topologicalSort(QList<int>& sorted, int rootId)
{
  Q_D(ctkDependencyGraph);
  if (rootId > 0)
    {
    ctkDependencyGraph subgraph(d->subgraphSize(rootId));
    QHash<int,int> subgraphIdToGlobal;
    QHash<int,int> globalIdToSubgraph;
    d->subgraphInsert(subgraph, rootId, subgraphIdToGlobal, globalIdToSubgraph);

    QList<int> subgraphSorted;
    bool result = subgraph.topologicalSort(subgraphSorted);
    foreach(int subgraphId, subgraphSorted)
      {
      sorted << subgraphIdToGlobal[subgraphId];
      }
    return result;
    }

  QVarLengthArray<int, MAXV> outdegree; // outdegree of each vertex
  QQueue<int> zeroout;	  // vertices of outdegree 0
	int x, y;			        // current and next vertex
  
  outdegree.resize(d->NVertices + 1);
	
	// resize if needed
	if (d->NVertices > MAXV)
	  {
    outdegree.resize(d->NVertices);
	  }

  d->computeOutdegrees(outdegree);
	
	for (int i=1; i <= d->NVertices; i++)
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
    for (int i=0; i < d->OutDegree[x]; i++)
		  {
			y = d->edge(x, i);
      outdegree[y] --;
      if (outdegree[y] == 0)
			  {
        zeroout.enqueue(y);
			  }
		  }
	  }

	if (j != d->NVertices)
	  {
		return false;
		}
		
  return true;
}

void ctkDependencyGraph::sourceVertices(QList<int>& sources)
{
  Q_D(ctkDependencyGraph);
  d->verticesWithIndegree(0, sources);
}
