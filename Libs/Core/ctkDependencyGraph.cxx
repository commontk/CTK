
// QT includes
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
  
  /// Compute indegree
  void computeIndegrees(QVarLengthArray<int, MAXV>& computedIndegrees);
  
  /// Traverse tree using Depth-first_search
  void traverseUsingDFS(int v);
  
  /// Called each time an edge is visited
  void processEdge(int from, int to); 
  
  /// Called each time a vertex is processed
  void processVertex(int v);
  
  void setEdge(int vertice, int degree, int value);
  int edge(int vertice, int degree);
  
  /// See http://en.wikipedia.org/wiki/Adjacency_list
  QVarLengthArray<QVarLengthArray<int,MAXDEGREE>*, MAXV+1> Edges;
  QVarLengthArray<int, MAXV+1> Degree;
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
void ctkDependencyGraph::ctkInternal::computeIndegrees(QVarLengthArray<int, MAXV>& computedIndegrees)
{
	for (int i=1; i <= this->NVertices; i++)
	  {
	  computedIndegrees[i] = 0;
	  }

	for (int i=1; i <= this->NVertices; i++) 
	  {
		for (int j=0; j < this->Degree[i]; j++) 
		  {
		  computedIndegrees[ this->edge(i,j) ] ++;
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
	for (int i=0; i<this->Degree[v]; i++)
	  {
		y = this->edge(v, i);
		if (this->P->shouldExcludeEdge(this->edge(v, i)) == false)
		  {
			if (this->Discovered[y] == false)
			  {
				this->Parent[y] = v;
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
  if (this->Parent[from] != to)
    {
    this->CycleDetected = true;
    this->CycleOrigin = to; 
    this->CycleEnd = from;
    if (this->Verbose)
      {
      QList<int> path;
      this->P->findPath(from, to, path);
      qWarning() << "Cycle detected from " << to << " to " << from;
      qWarning() << " " << path;
      path.clear();
      this->P->findPath(to, from, path);
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
// ctkDependencyGraph methods

//----------------------------------------------------------------------------
ctkDependencyGraph::ctkDependencyGraph(int nvertices, int nedges)
{
  this->Internal = new ctkInternal(this);
  
  this->Internal->NVertices = nvertices; 
  this->Internal->NEdges = nedges;
  
  // Resize internal array
  this->Internal->Processed.resize(nvertices + 1);
  this->Internal->Discovered.resize(nvertices + 1);
  this->Internal->Parent.resize(nvertices + 1);
  this->Internal->Edges.resize(nvertices + 1);
  this->Internal->Degree.resize(nvertices + 1);

  for (int i=1; i <= nvertices; i++)
    {
    this->Internal->Degree[i] = 0;
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
    for (int j=0; j < this->Internal->Degree[i]; j++)
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
  if (this->Internal->Degree[from] > capacity)
    {
    this->Internal->Edges[from]->resize(capacity + capacity * 0.3);
    }

  this->Internal->setEdge(from, this->Internal->Degree[from], to);
  this->Internal->Degree[from]++;

  this->Internal->NEdges++;
}

//----------------------------------------------------------------------------
void ctkDependencyGraph::findPath(int from, int to, QList<int>& path)
{
	if ((from == to) || (to == -1))
	  {
	  path << from;
		}
	else 
	  {
		this->findPath(from, this->Internal->Parent[to], path);
		path << to;
	  }
}

//----------------------------------------------------------------------------
bool ctkDependencyGraph::topologicalSort(QList<int>& sorted)
{
	QVarLengthArray<int, MAXV> indegree; // indegree of each vertex
	QQueue<int> zeroin;	  // vertices of indegree 0
	int x, y;			        // current and next vertex
  
  indegree.resize(this->Internal->NVertices + 1);
	
	// resize if needed
	if (this->Internal->NVertices > MAXV)
	  {
	  indegree.resize(this->Internal->NVertices);
	  }

	this->Internal->computeIndegrees(indegree);
	
	for (int i=1; i <= this->Internal->NVertices; i++)
	  {
		if (indegree[i] == 0) 
		  {
		  zeroin.enqueue(i);
		  }
		}

	int j=0;
	while (zeroin.empty() == false) 
	  {
		j = j+1;
		x = zeroin.dequeue();
		sorted << x;
		for (int i=0; i < this->Internal->Degree[x]; i++)
		  {
			y = this->Internal->edge(x, i);
			indegree[y] --;
			if (indegree[y] == 0)
			  {
			  zeroin.enqueue(y);
			  }
		  }
	  }

	if (j != this->Internal->NVertices)
	  {
		return false;
		}
		
  return true;
}
