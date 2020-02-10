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
/*=========================================================================

 Portions (c) Copyright Brigham and Women's Hospital (BWH) 
 All Rights Reserved.

 See http://www.slicer.org/copyright/copyright.txt for details.

 Program:   Module Description Parser

=========================================================================*/

// CTK includes
#include "ctkBinaryFileDescriptor.h"
#include "ctkPimpl.h"

// BinUtils includes
#include <bfd.h>

// STD includes
#include <cstdlib>
#include <utility>
#include <vector>

//-----------------------------------------------------------------------------
class ctkBinaryFileDescriptorPrivate
{
public:
  // Convenient typedefs
  typedef std::pair<asection*, void* > MemorySectionType;
  typedef std::vector<MemorySectionType> MemorySectionContainer;
  
  ctkBinaryFileDescriptorPrivate();

  /// Resolves a symbol
  void* resolve(const char * symbol);

  MemorySectionContainer Sections;
  bfd *                  BFD;
  
  QString FileName;
};

// --------------------------------------------------------------------------
// ctkBinaryFileDescriptorPrivate methods

// --------------------------------------------------------------------------
ctkBinaryFileDescriptorPrivate::ctkBinaryFileDescriptorPrivate()
{
  this->BFD = 0;
}

// --------------------------------------------------------------------------
void* ctkBinaryFileDescriptorPrivate::resolve(const char * symbol)
{
  if (!this->BFD)
    {
    return 0;
    }

  void *addr = 0;
  
  // Get the symbol table
  long storageNeeded = bfd_get_symtab_upper_bound(this->BFD);
  asymbol ** symbolTable = reinterpret_cast<asymbol **>(malloc(storageNeeded));
  
  long numberOfSymbols = bfd_canonicalize_symtab(this->BFD, symbolTable);
    
  // Run through the symbol table, looking for the requested symbol
  for (int i = 0; i < numberOfSymbols; i++) 
    {
    if (strcmp(symbol, symbolTable[i]->name) == 0)
      { 
      // Found the symbol, get the section pointer
      asection *p = bfd_get_section(symbolTable[i]);
          
      // Do we have this section already?
      MemorySectionContainer::iterator sit;
      for (sit = this->Sections.begin(); sit != this->Sections.end(); ++sit)
        {
        if ((*sit).first == p)
          {
          break;
          }  
        }
            
      PTR mem;
      if (sit == this->Sections.end())
        {
        // Get the contents of the section
        bfd_size_type sz = bfd_get_section_size (p);
        mem = malloc (sz);
        if (bfd_get_section_contents(this->BFD, p, mem, static_cast<file_ptr>(0), sz))
          {
          this->Sections.push_back( MemorySectionType(p, mem) );
          }
        else
          {
          // Error reading section
          free(mem);
          break;
          }
        }
      else
        {
        // pull the start of the section block from the cache
        mem = const_cast<void*>((*sit).second);
        }
            
      // determine the address of this section
      addr = reinterpret_cast<char *>(mem)
          + (bfd_asymbol_value(symbolTable[i]) - bfd_asymbol_base(symbolTable[i]));
      break;
      }
    }

  // cleanup. just delete the outer vector for the symbol table
  free(symbolTable);
  
  return addr;
}

// --------------------------------------------------------------------------
// ctkBinaryFileDescriptor methods

// --------------------------------------------------------------------------
ctkBinaryFileDescriptor::ctkBinaryFileDescriptor(): d_ptr(new ctkBinaryFileDescriptorPrivate)
{
}

// --------------------------------------------------------------------------
ctkBinaryFileDescriptor::ctkBinaryFileDescriptor(const QString& _fileName): 
  d_ptr(new ctkBinaryFileDescriptorPrivate)
{
  Q_D(ctkBinaryFileDescriptor);
  d->FileName = _fileName;
}

// --------------------------------------------------------------------------
ctkBinaryFileDescriptor::~ctkBinaryFileDescriptor()
{
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkBinaryFileDescriptor, QString, fileName, FileName);
CTK_SET_CPP(ctkBinaryFileDescriptor, const QString&, setFileName, FileName);

// --------------------------------------------------------------------------
bool ctkBinaryFileDescriptor::isLoaded() const
{
  Q_D(const ctkBinaryFileDescriptor);
  return (d->BFD != 0);
}

// --------------------------------------------------------------------------
bool ctkBinaryFileDescriptor::load()
{
  Q_D(ctkBinaryFileDescriptor);
  
  bfd_init();
  bfd * abfd = bfd_openr(d->FileName.toUtf8(), NULL);
  if (!abfd)
    {
    return false;
    }
  
  /* make sure it's an object file */
  if (!bfd_check_format (abfd, bfd_object)) 
    {
    bfd_close(abfd);
    return false;
    }
  
  d->BFD = abfd;
  return true;
}

// --------------------------------------------------------------------------
bool ctkBinaryFileDescriptor::unload()
{
  Q_D(ctkBinaryFileDescriptor);
  
  if (d->BFD)
    {
    bfd_close(d->BFD);
    d->BFD = 0; 
    }
  return true;
}

// --------------------------------------------------------------------------
void* ctkBinaryFileDescriptor::resolve(const char * symbol)
{
  Q_D(ctkBinaryFileDescriptor);
  return d->resolve(symbol);
}
