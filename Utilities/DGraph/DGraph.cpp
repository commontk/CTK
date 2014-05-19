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
#include <ctkDependencyGraph.h>

// STD includes
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <vector>
#include <cassert>

using namespace std;

//----------------------------------------------------------------------------
std::string help(const std::string& progName)
{
  std::string msg = std::string("Usage: ") + progName + std::string(" <graphfile> [-paths Label | -sort Label]");
  return msg;
}

//----------------------------------------------------------------------------
void displayError(const std::string& progName, const std::string& msg)
{
  std::cerr << progName << std::endl << msg << std::endl << help(progName) << std::endl;
}

std::vector< std::string > splitString(const std::string& string)
{
  std::vector<std::string> results;
  std::stringstream stringStream;
  stringStream << string;
  do
    {
    std::string nextString;
    stringStream >> nextString;
    size_t found = nextString.find_first_not_of(" ");
    if (found != string::npos)
      {
      results.push_back(nextString.substr(found));
      }
    } while (!stringStream.eof());

  return results;
}

std::string listToString(const std::list<int>& list)
{
  std::stringstream stream;

  if (list.size() == 0)
    {
    stream << "empty";
    }
  else
  {
    unsigned int counter = 0;
    std::list<int>::const_iterator iterator;
    for (iterator = list.begin(); iterator != list.end(); iterator++)
      {
      stream << *iterator;
      counter++;

      if (counter != list.size())
        {
        stream << " ";
        }
      }
  }
  return stream.str();
}

int getLastElement(const std::list<int>& list)
{
  int result = -1;

  if (list.size() > 0)
    {
    std::list<int>::const_reverse_iterator iterator;
    iterator = list.rend();
    result = *iterator;
    }

  return result;
}

//----------------------------------------------------------------------------
int getOrGenerateId(std::map<int, std::string>& vertexIdToLabel,
                    std::map<std::string, int>& vertexLabelToId,
                    const std::string& label)
{
  // If needed, generate vertex id
  int vertexId = -1;
  if (vertexLabelToId.find(label) == vertexLabelToId.end())
    {
    vertexId = vertexLabelToId.size() + 1;
    vertexLabelToId[label] = vertexId;
    vertexIdToLabel[vertexId] = label;
    }
  else
    {
    vertexId = vertexLabelToId[label];
    }
  return vertexId; 
}

//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  bool verbose = false;
  bool outputTopologicalOrder = true;

  // a graph file is expected
  if (argc < 2)
    {
    displayError(argv[0], std::string("Missing one argument"));
    return EXIT_FAILURE;
    }

  bool outputPath = false;
  bool outputSort = false;
  std::string label;
  if (argc == 3)
    {
    displayError(argv[0], std::string("Wrong argument"));
    return EXIT_FAILURE;
    }
  if (argc == 4)
    {
    std::string arg2 = std::string(argv[2]);
    if (arg2.compare("-paths")!=0 && arg2.compare("-sort")!=0)
      {
      displayError(argv[0], std::string("Wrong argument: ") + arg2);
      return EXIT_FAILURE;
      }
    label = std::string(argv[3]);
    outputTopologicalOrder = false;
    if (arg2.compare("-paths") == 0)
      {
      outputPath = true;
      }
    else
      {
      outputSort = true;
      }

    if (verbose)
      {
      std::cout << "label:" << label << std::endl;
      }
    }

  // Open File.

  std::string filepath = std::string(argv[1]);
  if (verbose)
    {
    std::cout << "filename:" << filepath << std::endl;
    }

  std::ifstream data;
  data.open(filepath.c_str(), ifstream::in);

  if (!data.is_open())
    {
    displayError(argv[0], std::string("Failed to open file '") + filepath + "' !");
    return EXIT_FAILURE;
    }

  // Read first line, called the header.

  std::string header;
  std::getline (data, header);
  if (verbose)
    {
    std::cout << "header:" << header << std::endl;
    }
  if (header.length() == 0)
    {
    displayError(argv[0], std::string("Failed to read Header line in file '") + filepath + "' !");
    return EXIT_FAILURE;
    }

  // Extract two integers

  int numberOfVertices = -1;
  int numberOfEdges = -1;

  std::stringstream stringStream;
  stringStream << header;
  stringStream >> numberOfVertices;
  stringStream >> numberOfEdges;
  
  if (numberOfVertices == -1 || numberOfEdges == -1)
    {
    displayError(argv[0], std::string("Error in file '") + filepath + "' - First line should look like: <#Vertices> <#Edges>");
    return EXIT_FAILURE;
    }

  if (verbose)
    {
    std::cout << "#Vertices:" << numberOfVertices << " #Edges:" << numberOfEdges << std::endl;
    }

  // Init dependency graph, and maps.

  ctkDependencyGraph mygraph(numberOfVertices);
  mygraph.setVerbose(verbose);
  std::map<int, std::string> vertexIdToLabel;
  std::map<std::string, int> vertexLabelToId;

  // Repeatedly read lines containing labels.
  
  std::string line;
  int lineNumber = 2;
  std::getline(data, line);

  do
    {
    // Skip empty line or commented line
    if (line.length() == 0 || line[0] == '#')
      {
      std::getline(data, line);
      continue;
      }

    // Extract two strings
    stringStream.clear();
    stringStream << line;

    std::vector<std::string> strings = splitString(line);

    if (strings.size() < 1 || strings.size() > 2)
      {
      stringStream << "Error in file '" << filepath << "' - line:" << lineNumber << " - Expected format is: <label> [<label>]" << std::endl;
      std::string message;
      stringStream >> message;
      displayError(argv[0], message);
      }
    
    lineNumber++;

    int from = -1;
    int to = -1;

    if (strings.size() == 2)
      {
      from = getOrGenerateId(vertexIdToLabel, vertexLabelToId, strings[0]);
      to = getOrGenerateId(vertexIdToLabel, vertexLabelToId, strings[1]);
      if (verbose)
        {
        std::cout << "Line='" << line << "', line number " << lineNumber << ", from (" << strings[0] << ", " << from << ") to (" << strings[1] << ", " << to << ")"  << std::endl;
        }
      }
    else
      {
      if (verbose)
        {
        std::cout << "Line='" << line << "', line number " << lineNumber << ", from (" << strings[0] << ", " << from << ") to (<null>, " << to << ")"  << std::endl;
        }
      }

    if (to > -1)
      {
      // Insert edge if we got two vertices
      mygraph.insertEdge(from, to);
      }
    else
      {
      // Just generate an entry in the vertexIdToLabel map
      getOrGenerateId(vertexIdToLabel, vertexLabelToId, "");
      }

    std::getline(data, line);
    }
  while (!data.eof());

  assert(numberOfEdges == mygraph.numberOfEdges());

  if (verbose)
    {
    mygraph.printGraph();
    std::cout << "> Check for cycle ..." << std::endl;
    }
   
  mygraph.checkForCycle();
  
  if (mygraph.cycleDetected())
    {
    std::cerr << "Cycle detected !" << std::endl;

    std::list<int> path;
    std::list<int>::iterator pathIterator;
    unsigned int pathIteratorCounter = 0;

    mygraph.findPath(mygraph.cycleOrigin(), mygraph.cycleEnd(), path);
    
    for (pathIterator = path.begin(); pathIterator != path.end(); pathIterator++)
      {
      std::cerr << vertexIdToLabel[*pathIterator];
      if (pathIteratorCounter != path.size() - 1)
        {
        std::cerr << " -> ";
        }
      pathIteratorCounter++;
      }
    std::cerr << std::endl;
    
    path.clear();
    mygraph.findPath(mygraph.cycleEnd(), mygraph.cycleOrigin(), path);

    pathIteratorCounter = 0;
    for (pathIterator = path.begin(); pathIterator != path.end(); pathIterator++)
      {
      std::cerr << vertexIdToLabel[*pathIterator];
      if (pathIteratorCounter != path.size() - 1)
        {
        std::cerr << " -> ";
        }
      }
    std::cerr << std::endl;
    
    return EXIT_FAILURE;
    }

  if (outputTopologicalOrder)
    {
    if (verbose)
      {
      std::cerr << "> Topological order ..." << std::endl;
      }
    std::list<int> out;
    std::list<int>::reverse_iterator outIterator;
    unsigned int outIteratorCounter = 0;

    if (mygraph.topologicalSort(out))
      {
      outIteratorCounter = 0;
      for (outIterator = out.rbegin(); outIterator != out.rend(); outIterator++)
        {
        std::cout << vertexIdToLabel[*outIterator];
        if (outIteratorCounter != out.size() - 1)
          {
          std::cout << " ";
          }
        }
      std::cout << std::endl;
      }
    }

  if (verbose)
    {
    std::list<int> sources;
    mygraph.sourceVertices(sources);
    std::cout << "Source vertices: " << listToString(sources) << std::endl;
    }
    
  if (outputPath)
    {
    // TODO Make sure label is valid
    std::list<int> out;
    if (mygraph.topologicalSort(out))
      {
      std::list<int>::iterator outIterator;
      for (outIterator = out.begin(); outIterator != out.end(); outIterator++)
        {
        // Assume all targets depend on the first lib
        // We could get all sinks and find all paths
        // from the rootId to the sink vertices.
        int rootId = getLastElement(out);
        int labelId = vertexLabelToId[label];

        std::list<std::list<int>*> paths;
        mygraph.findPaths(labelId, rootId, paths);

        std::list<std::list<int>*>::iterator pathsIterator;
        std::list<std::list<int>*>::iterator pathsIteratorPlus1;

        for (pathsIterator = paths.begin(); pathsIterator != paths.end(); pathsIterator++)
          {
          std::list<int>* p = *pathsIterator;
          assert(p);

          std::list<int>::iterator pIterator;
          std::list<int>::iterator pIteratorPlus1;
          for (pIterator = p->begin(); pIterator != p->end(); pIterator++)
            {
            int id = *pIterator;
            std::cout << vertexIdToLabel[id];

            pIteratorPlus1 = pIterator;
            pIteratorPlus1++;

            if (pIteratorPlus1 != p->end())
              {
              std::cout << " ";
              }
            }

          pathsIteratorPlus1 = pathsIterator;
          pathsIteratorPlus1++;

          if (pathsIteratorPlus1 != paths.end())
            {
            std::cout << ";";
            }
          }

        for (pathsIterator = paths.begin(); pathsIterator != paths.end(); pathsIterator++)
          {
            if (*pathsIterator != NULL)
              {
              delete *pathsIterator;
              }
          }
        }
      }
    }

  if (outputSort)
    {
    // TODO Make sure label is valid
    std::list<int> out;
    int labelId = vertexLabelToId[label];
    if (labelId < 1)
      {
      std::cout << label;
      return EXIT_SUCCESS;
      }
    if (mygraph.topologicalSort(out, labelId))
      {
      std::list<int>::iterator outIterator;
      std::list<int>::iterator outIteratorPlus1;

      for (outIterator = out.begin(); outIterator != out.end(); outIterator++)
        {
        int id = *outIterator;
        std::cout << vertexIdToLabel[id];

        outIteratorPlus1 = outIterator;
        outIteratorPlus1++;

        if (outIteratorPlus1 != out.end())
          {
          std::cout << " ";
          }
        }
      }
    }
  return EXIT_SUCCESS;
}
