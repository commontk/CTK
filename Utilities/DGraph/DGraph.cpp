
// QT includes
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>
#include <QDebug>

// CTK includes
#include <ctkDependencyGraph.h>

// STD includes
#include <cstdlib>
#include <iostream>

//----------------------------------------------------------------------------
QString help(const QString& progName)
{
  QString msg = "Usage: %1 <graphfile>";
  return msg.arg(progName);
}

//----------------------------------------------------------------------------
void displayError(const QString& progName, const QString& msg)
{
  std::cerr << QString("%1\n%2\n%3\n").arg(progName).
                                       arg(msg).
                                       arg(help(progName)).toStdString();
}

//----------------------------------------------------------------------------
int getOrGenerateId(QHash<int, QString>& vertexIdToLabel,
                    QHash<QString, int>& vertexLabelToId,
                    const QString& label)
{
  // If needed, generate vertex id
  int vertexId = -1;
  if (!vertexLabelToId.keys().contains(label))
    {
    vertexId = vertexLabelToId.keys().size() + 1;
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
  
  // a graph file is expected
  if (argc < 2)
    {
    displayError(argv[0], QLatin1String("Missing one argument"));
    return EXIT_FAILURE;
    }
    
  QString filepath = QString::fromLatin1(argv[1]);
  if (!QFile::exists(filepath))
    {
    displayError(argv[0], QString("File '%1' doesn't exists !").arg(filepath));
    return EXIT_FAILURE;
    }

  QFile data(filepath);
  if (!data.open(QFile::ReadOnly))
    {
    displayError(argv[0], QString("Failed to open file '%1' !").arg(filepath));
    return EXIT_FAILURE; 
    }
    
  QTextStream in(&data);
  QString header = in.readLine();
  if (header.isNull())
    {
    displayError(argv[0], QString("Failed to read Header line in file '%1' !").arg(filepath));
    return EXIT_FAILURE;
    }

  // Regular expression to extract two integers
  QRegExp twoint_re("^([0-9]+)\\s+([0-9]+)");
  
  // Extract numberOfVertices and numberOfEdges
  int pos = twoint_re.indexIn(header.trimmed());
  if (pos != 0)
    {
    displayError(argv[0], QString("Error in file '%1' - First line should look like: <#Vertices> <#Edges>")
      .arg(filepath));
    return EXIT_FAILURE;
    }
  QStringList list = twoint_re.capturedTexts();
  Q_ASSERT(list.size() == 3);

  int numberOfVertices = list[1].toInt();
  int numberOfEdges = list[2].toInt();

  if (verbose)
    {
    qDebug() << "#Vertices:" << numberOfVertices << "#Edges:" << numberOfEdges;
    }

  // Init
  ctkDependencyGraph mygraph(numberOfVertices, numberOfEdges);
  mygraph.setVerbose(verbose);

  // Map between vertex label and vertex id
  QHash<int, QString> vertexIdToLabel;
  QHash<QString, int> vertexLabelToId;
  
  // Regular expression to extract two label
  QRegExp twolabel_re("^(.+)\\s+(.+)");
  
  // Read vertex connection
  int lineNumber = 2;
  QString line = in.readLine();
  do
    {
    // Skip empty line or commented line
    if (line.isEmpty() || line.startsWith("#"))
      {
      continue;
      }

    // Extract vertex points
    
    int pos = twolabel_re.indexIn(line.trimmed());
    if (pos != 0)
      {
      displayError(argv[0], QString("Error in file '%1' - line:%2 - Expected format is: <label> <label>")
        .arg(filepath).arg(lineNumber));
      return EXIT_FAILURE;
      }
    lineNumber++;

    QStringList list = twolabel_re.capturedTexts();
    Q_ASSERT(list.size() == 3);

    int from = getOrGenerateId(vertexIdToLabel, vertexLabelToId, list[1]);
    int to = getOrGenerateId(vertexIdToLabel, vertexLabelToId, list[2]);

    // Insert edge
    mygraph.insertEdge(from, to);

    line = in.readLine();
    }
  while (!line.isNull());

  if (verbose)
    {
    mygraph.printGraph();
    qDebug() << "> Check for cycle ...";
    }
   
  mygraph.checkForCycle();
  
  if (mygraph.cycleDetected())
    {
    std::cerr << "Cycle detected !" << std::endl;
    QList<int> path; 
    mygraph.findPath(mygraph.cycleOrigin(), mygraph.cycleEnd(), path);
    
    for(int i = 0; i < path.size(); ++i)
      {
      std::cerr << vertexIdToLabel[path[i]].toStdString();
      if (i != path.size() - 1)
        {
        std::cerr << " -> ";
        }
      }
    std::cerr << std::endl;
    
    path.clear();
    mygraph.findPath(mygraph.cycleEnd(), mygraph.cycleOrigin(), path);

    for(int i = 0; i < path.size(); ++i)
      {
      std::cerr << vertexIdToLabel[path[i]].toStdString();
      if (i != path.size() - 1)
        {
        std::cerr << " -> ";
        }
      }
    std::cerr << std::endl;
    
    return EXIT_FAILURE;
    }

  if (verbose)
    {
    qDebug() << "> Topological order ...";
    }
  QList<int> out;
  if (mygraph.topologicalSort(out))
    {
    for(int i=out.size() - 1; i >= 0; --i)
      {
      std::cout << vertexIdToLabel[out[i]].toStdString();
      if (i != 0)
        {
        std::cout << " ";
        }
      }
    std::cout << std::endl;
    }
    
  return EXIT_SUCCESS;
}