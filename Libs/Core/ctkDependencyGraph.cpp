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

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <cassert>

#define MAXV 100
#define MAXDEGREE 2000

//----------------------------------------------------------------------------
class ctkDependencyGraphPrivate
{

public:

  ctkDependencyGraph* const q_ptr;

  ctkDependencyGraphPrivate(ctkDependencyGraph& p);
  ~ctkDependencyGraphPrivate();
  
  /// Compute outdegree
  void computeOutdegrees(std::vector<int>& computedOutdegrees);
  
  /// Traverse tree using Depth-first_search
  void traverseUsingDFS(int v);
  
  /// Called each time an edge is visited
  void processEdge(int from, int to); 
  
  /// Called each time a vertex is processed
  void processVertex(int v);

  /// Retrieve the path between two vertices
  void findPathDFS(int from, int to, std::list<int>& path);

  /// Recursive function used by findPaths to retrieve the path between two vertices
  void findPathsRec(int from, int to, std::list<int>* path, std::list<std::list<int>* >& paths);
  
  void setEdge(int vertice, int degree, int value);
  int edge(int vertice, int degree)const;

  void verticesWithIndegree(int indegree, std::list<int>& list);

  int subgraphSize(int rootId);
  void subgraphSizeRec(int rootId, std::set<int>& uniqueVertices);

  void subgraphInsert(ctkDependencyGraph& subgraph, int rootId,
                      std::map<int,int>& subgraphIdToGlobal, std::map<int,int>& globalIdToSubgraph);

  int getOrGenerateSubgraphId(std::map<int, int>& subgraphIdToGlobal,
                      std::map<int, int>& globalIdToSubgraph,
                      int globalId);

  /// See http://en.wikipedia.org/wiki/Adjacency_list
  std::vector<std::vector<int>* > Edges;
  std::vector<int> OutDegree;
  std::vector<int> InDegree;
  int NVertices;
  int NEdges;
  
  /// Structure used by DFS
  /// See http://en.wikipedia.org/wiki/Depth-first_search
  std::vector<bool> Processed;	// processed vertices
  std::vector<bool> Discovered; // discovered vertices
  std::vector<int>  Parent;	    // relation discovered
  
  bool    Abort;	// Flag indicating if traverse should be aborted
  bool    Verbose; 
  bool    CycleDetected; 
  int     CycleOrigin; 
  int     CycleEnd;
  
  std::list<int> ListOfEdgeToExclude;

};

//----------------------------------------------------------------------------
// Returns a space separated string of T.
template<class T>
std::string listToString(const std::list<T>& list)
{
  std::stringstream outputString;

  typename std::list<T>::const_iterator iter;
  for (iter = list.begin(); iter != list.end(); iter++)
    {
    outputString << *iter << " ";
    }

  return outputString.str();
}

//----------------------------------------------------------------------------
// Returns true if the map contains the key and false otherwise.
template<class T1, class T2>
bool mapContainsKey(const std::map<T1, T2>& map, const T1& key)
{
  bool result = false;

  if (map.find(key) != map.end())
    {
    result = true;
    }

  return result;
}

//----------------------------------------------------------------------------
// Returns true if the list contains the value and false otherwise.
template<class T>
bool listContainsValue(const std::list<T>& list, const T& value)
{
  bool result = false;

  typename std::list<T>::const_iterator iter;
  for (iter = list.begin(); iter != list.end(); iter++)
    {
    if ((*iter) == value)
      {
      result = true;
      break;
      }
    }
  return result;
}

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

ctkDependencyGraphPrivate::~ctkDependencyGraphPrivate()
{
  std::vector<std::vector<int>* >::iterator edgesIterator;
  for (edgesIterator = this->Edges.begin(); edgesIterator != this->Edges.end(); edgesIterator++)
    {
    if (*edgesIterator != NULL)
      {
      delete *edgesIterator;
      }
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::computeOutdegrees(std::vector<int>& computedOutdegrees)
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
		if (q_ptr->shouldExcludeEdge(this->edge(v, i)) == false)
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
      std::list<int> path;
      this->findPathDFS(from, to, path);
      std::cerr << "ERROR: Cycle detected from " << to << " to " << from << std::endl;
      std::cerr << " " << listToString<int>(path) << std::endl;
      path.clear();
      this->findPathDFS(to, from, path);
      std::cerr << " " << listToString<int>(path) << std::endl;
      }
    this->Abort = true;
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::processVertex(int v)
{
	if (this->Verbose)
	  {
	  std::cout << "processed vertex " << v << std::endl;
	  }
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::setEdge(int vertice, int degree, int value)
{
  assert(vertice <= this->NVertices);
  assert(degree < MAXDEGREE);
  (*this->Edges[vertice])[degree] = value; 
}

//----------------------------------------------------------------------------
int ctkDependencyGraphPrivate::edge(int vertice, int degree)const
{
  assert(vertice <= this->NVertices);
  assert(degree < MAXDEGREE);
  return (*this->Edges[vertice])[degree];
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::findPathDFS(int from, int to, std::list<int>& path)
{
  if ((from == to) || (to == -1))
    {
    path.push_back(from);
    }
  else 
    {
    this->findPathDFS(from, this->Parent[to], path);
    path.push_back(to);
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::findPathsRec(
  int from, int to, std::list<int>* path, std::list<std::list<int>* >& paths)
{
  if (from == to)
    {
    return;
    }
  
  std::list<int> branch;
  branch = *path;

  int child = from;
  for (int j=0; j < this->OutDegree[child]; j++)
    {
    if (j == 0)
      {
      int parent = this->edge(child, j);
      (*path).push_back(parent);
      this->findPathsRec(parent, to, path, paths);
      }
    else
      {
      int parent = this->edge(child, j);
      // Copy path and add it to the list
      std::list<int>* pathCopy = new std::list<int>();
      *pathCopy = branch;
      paths.push_back(pathCopy);
      (*pathCopy).push_back(parent);
      this->findPathsRec(parent, to, pathCopy, paths);
      }
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::verticesWithIndegree(int indegree, std::list<int>& list)
{
  assert(indegree >= 0);

  for (int i=1; i <= this->NVertices; i++)
    {
    if (this->InDegree[i] == indegree)
      {
      list.push_back(i);
      }
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraphPrivate::subgraphSizeRec(int rootId, std::set<int>& uniqueVertices)
{
  assert(rootId > 0);

  for (int i = 0; i < this->OutDegree[rootId]; ++i)
    {
    int child = this->edge(rootId, i);
    uniqueVertices.insert(child);
    subgraphSizeRec(child, uniqueVertices);
    }
}

//----------------------------------------------------------------------------
int ctkDependencyGraphPrivate::subgraphSize(int rootId)
{
  assert(rootId > 0);

  std::set<int> vertices;
  vertices.insert(rootId);
  this->subgraphSizeRec(rootId, vertices);
  return static_cast<int>(vertices.size());
}

void ctkDependencyGraphPrivate::subgraphInsert(
    ctkDependencyGraph& subgraph, int rootId,
    std::map<int,int>& subgraphIdToGlobal, std::map<int,int>& globalIdToSubgraph)
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

//----------------------------------------------------------------------------
int ctkDependencyGraphPrivate::getOrGenerateSubgraphId(
    std::map<int, int>& subgraphIdToGlobal,
    std::map<int, int>& globalIdToSubgraph,
    int globalId)
{
  // If needed, generate vertex id
  int subgraphId = -1;
  if (!mapContainsKey<int, int>(globalIdToSubgraph,globalId))
    {
    subgraphId = static_cast<int>(globalIdToSubgraph.size()) + 1;
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
  d_ptr->NVertices = nvertices;
  
  // Resize internal array
  d_ptr->Processed.resize(nvertices + 1);
  d_ptr->Discovered.resize(nvertices + 1);
  d_ptr->Parent.resize(nvertices + 1);
  d_ptr->Edges.resize(nvertices + 1);
  d_ptr->OutDegree.resize(nvertices + 1);
  d_ptr->InDegree.resize(nvertices + 1);

  for (int i=1; i <= nvertices; i++)
    {
    d_ptr->OutDegree[i] = 0;
    d_ptr->InDegree[i] = 0;
    }
    
  // initialize Edge adjacency list
  for (int i=0; i <= nvertices; i++)
    {
    d_ptr->Edges[i] = new std::vector<int>();
    d_ptr->Edges[i]->resize(MAXDEGREE);
    }
    
  // initialize search
  for (int i=1; i <= nvertices; i++)
    {
    d_ptr->Processed[i] = false;
    d_ptr->Discovered[i] = false;
    d_ptr->Parent[i] = -1;
    }
}

//----------------------------------------------------------------------------
ctkDependencyGraph::~ctkDependencyGraph()
{
  if (d_ptr != NULL)
    {
    delete d_ptr;
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::printAdditionalInfo()const
{
  std::cout << "ctkDependencyGraph (" << this << ")" << std::endl
            << " NVertices:" << this->numberOfVertices() << std::endl
            << " NEdges:" << this->numberOfEdges() << std::endl
            << " Abort:" << d_ptr->Abort << std::endl;

  std::cout << " [Processed]" << std::endl;
  for(unsigned int i=1; i < d_ptr->Processed.size(); i++)
    {
    std::cout << i << "->" << d_ptr->Processed[i] << std::endl;
    }
  std::cout << " [Discovered]" << std::endl;
  for(unsigned int i=1; i < d_ptr->Discovered.size(); i++)
    {
    std::cout << i << "->" << d_ptr->Discovered[i] << std::endl;
    }
  std::cout << " [Parent]" << std::endl;
  for(unsigned int i=1; i < d_ptr->Parent.size(); i++)
    {
    std::cout << i << "->" << d_ptr->Parent[i] << std::endl;
    }
  std::cout << " [Graph]" << std::endl;
  this->printGraph();
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::printGraph()const
{
  for(int i=1; i <= d_ptr->NVertices; i++)
    {
    std::cout << i << ":";
    for (int j=0; j < d_ptr->OutDegree[i]; j++)
      {
      std::cout << " " << d_ptr->edge(i, j);
      }
    std::cout << std::endl;
    }
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::numberOfVertices()const
{
  return d_ptr->NVertices;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::numberOfEdges()const
{
  return d_ptr->NEdges;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::setVerbose(bool verbose)
{
  d_ptr->Verbose = verbose;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::setEdgeListToExclude(const std::list<int>& list)
{
  d_ptr->ListOfEdgeToExclude = list;
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::shouldExcludeEdge(int edge)const
{
  return listContainsValue(d_ptr->ListOfEdgeToExclude, edge);
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::checkForCycle()
{
  if (d_ptr->NEdges > 0)
    {
    // Store unprocessed vertex ids
    std::list<int> uncheckedVertices;
    for (int i = 1; i <= d_ptr->NVertices; ++i)
      {
        uncheckedVertices.push_back(i);
      }

    // Start the cycle detection on the source vertices
    std::list<int> sources;
    this->sourceVertices(sources);
    std::list<int>::const_iterator sourcesIterator;
    for (sourcesIterator = sources.begin(); sourcesIterator != sources.end(); sourcesIterator++)
      {
      d_ptr->traverseUsingDFS(*sourcesIterator);
      if (this->cycleDetected()) return true;

      for (unsigned int i=0; i < d_ptr->Processed.size(); i++)
        {
          if (d_ptr->Processed[i] == true)
            {
            uncheckedVertices.remove(i);
            }

          d_ptr->Discovered[i] = false;
          d_ptr->Processed[i] = false;
        }
      }

    // If a component does not have a source vertex,
    // i.e. it is a cycle a -> b -> a, check all non
    // processed vertices.
    while (!uncheckedVertices.empty())
      {
      d_ptr->traverseUsingDFS((*uncheckedVertices.rbegin()));
      if (this->cycleDetected()) return true;

      for (unsigned int i=0; i < d_ptr->Processed.size(); i++)
        {
          if (d_ptr->Processed[i] == true)
            {
            uncheckedVertices.remove(i);
            }

          d_ptr->Discovered[i] = false;
          d_ptr->Processed[i] = false;
        }
      }
    }
  return this->cycleDetected();
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::cycleDetected()const
{
  return d_ptr->CycleDetected;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::cycleOrigin()const
{
  return d_ptr->CycleOrigin;
}

//----------------------------------------------------------------------------
int ctkDependencyGraph::cycleEnd()const
{
  return d_ptr->CycleEnd;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::insertEdge(int from, int to)
{
  assert(from > 0 && from <= d_ptr->NVertices);
  assert(to > 0 && to <= d_ptr->NVertices);
  
  // resize if needed
  size_t capacity = d_ptr->Edges[from]->capacity();
  if (d_ptr->OutDegree[from] > static_cast<int>(capacity))
    {
    d_ptr->Edges[from]->resize(capacity + capacity * 0.3);
    }

  d_ptr->setEdge(from, d_ptr->OutDegree[from], to);
  d_ptr->OutDegree[from]++;
  d_ptr->InDegree[to]++;

  d_ptr->NEdges++;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::findPaths(int from, int to, std::list<std::list<int>* >& paths)
{
  std::list<int>* path = new std::list<int>;
  (*path).push_back(from);
  (paths).push_back(path);
  d_ptr->findPathsRec(from, to, path, paths);

  // Remove lists not ending with the requested element
  std::list<std::list<int>* >::iterator pathsIterator;
  pathsIterator = paths.begin();

  while (paths.size() > 0 && pathsIterator != paths.end())
    {
    std::list<int>* pathToCheck = (*pathsIterator);
    assert(pathToCheck);

    if (*(pathToCheck->rbegin()) != to)
      {
      pathsIterator = paths.erase(pathsIterator);
      }
    else
      {
      pathsIterator++;
      }
    }
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::findPath(int from, int to, std::list<int>& path)
{
  std::list<std::list<int>* > paths;
  this->findPaths(from, to, paths);

  if (!paths.empty())
    {
    std::list<int>::iterator pathIterator;
    for (pathIterator = (*(paths.begin()))->begin(); pathIterator != (*(paths.begin()))->end(); pathIterator++)
      {
      path.push_back(*pathIterator);
      }
    }

  std::list<std::list<int>* >::iterator pathsIterator;
  for(pathsIterator = paths.begin(); pathsIterator != paths.end(); pathsIterator++)
  {
    if (*pathsIterator != NULL)
    {
      delete *pathsIterator;
    }
  }
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::topologicalSort(std::list<int>& sorted, int rootId)
{
  if (rootId > 0)
    {
    ctkDependencyGraph subgraph(d_ptr->subgraphSize(rootId));
    std::map<int,int> subgraphIdToGlobal;
    std::map<int,int> globalIdToSubgraph;
    d_ptr->subgraphInsert(subgraph, rootId, subgraphIdToGlobal, globalIdToSubgraph);

    std::list<int> subgraphSorted;
    bool result = subgraph.topologicalSort(subgraphSorted);
    std::list<int>::const_iterator subgraphSortedIterator;
    for (subgraphSortedIterator = subgraphSorted.begin(); subgraphSortedIterator != subgraphSorted.end(); subgraphSortedIterator++)
      {
      sorted.push_back(subgraphIdToGlobal[*subgraphSortedIterator]);
      }
    return result;
    }

  std::vector<int> outdegree; // outdegree of each vertex
  outdegree.resize(MAXV);
  std::queue<int> zeroout;	  // vertices of outdegree 0
	int x, y;			        // current and next vertex
  
  outdegree.resize(d_ptr->NVertices + 1);
	
	// resize if needed
	if (d_ptr->NVertices > MAXV)
	  {
    outdegree.resize(d_ptr->NVertices);
	  }

	d_ptr->computeOutdegrees(outdegree);
	
	for (int i=1; i <= d_ptr->NVertices; i++)
	  {
    if (outdegree[i] == 0)
		  {
      zeroout.push(i);
		  }
		}

	int j=0;
  while (zeroout.empty() == false)
	  {
		j = j+1;
    x = zeroout.front();
    zeroout.pop();
		sorted.push_back(x);
    for (int i=0; i < d_ptr->OutDegree[x]; i++)
		  {
			y = d_ptr->edge(x, i);
      outdegree[y] --;
      if (outdegree[y] == 0)
			  {
        zeroout.push(y);
			  }
		  }
	  }

	if (j != d_ptr->NVertices)
	  {
		return false;
		}
		
  return true;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::sourceVertices(std::list<int>& sources)
{
  d_ptr->verticesWithIndegree(0, sources);
}
