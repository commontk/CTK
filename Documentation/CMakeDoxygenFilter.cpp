/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>

#include <assert.h>

//--------------------------------------
// Utilitiy classes and functions
//--------------------------------------

struct ci_char_traits : public std::char_traits<char>
    // just inherit all the other functions
    //  that we don't need to override
{
  static bool eq(char c1, char c2)
  { return toupper(c1) == toupper(c2); }

  static bool ne(char c1, char c2)
  { return toupper(c1) != toupper(c2); }

  static bool lt(char c1, char c2)
  { return toupper(c1) <  toupper(c2); }

  static bool gt(char c1, char c2)
  { return toupper(c1) >  toupper(c2); }

  static int compare(const char* s1, const char* s2, std::size_t n)
  {
    while (n-- > 0)
    {
      if (lt(*s1, *s2)) return -1;
      if (gt(*s1, *s2)) return 1;
      ++s1; ++s2;
    }
    return 0;
  }

  static const char* find(const char* s, int n, char a)
  {
    while (n-- > 0 && toupper(*s) != toupper(a))
    {
      ++s;
    }
    return s;
  }
};

typedef std::basic_string<char, ci_char_traits> ci_string;

//--------------------------------------
// Lexer
//--------------------------------------

class CMakeLexer
{
public:

  enum Token {
    TOK_EOF = -1,

    // commands
    TOK_MACRO = -2, TOK_ENDMACRO = -3,
    TOK_FUNCTION = -4, TOK_ENDFUNCTION = -5,
    TOK_DOXYGEN_COMMENT = -6,

    TOK_STRING_LITERAL = -100,

    // primary
    TOK_IDENTIFIER = -200
  };

  CMakeLexer(std::istream& is)
    : _lastChar(' '), _is(is), _line(1), _col(1)
  {}

  int getToken()
  {
    // skip whitespace
    while (isspace(_lastChar))
    {
      _lastChar = getChar();
    }

    if (isalpha(_lastChar) || _lastChar == '_')
    {
      _identifier = _lastChar;
      while (isalnum(_lastChar = getChar()) || _lastChar == '-' || _lastChar == '_')
      {
        _identifier += _lastChar;
      }

      if (_identifier == "function")
        return TOK_FUNCTION;
      if (_identifier == "macro")
        return TOK_MACRO;
      if (_identifier == "endfunction")
        return TOK_ENDFUNCTION;
      if (_identifier == "endmacro")
        return TOK_ENDMACRO;
      return TOK_IDENTIFIER;
    }

    if (_lastChar == '#')
    {
      _lastChar = getChar();
      if (_lastChar == '!')
      {
        // found a doxygen comment marker
        _identifier.clear();

        _lastChar = getChar();
        while (_lastChar != EOF && _lastChar != '\n' && _lastChar != '\r')
        {
          _identifier += _lastChar;
          _lastChar = getChar();
        }
        return TOK_DOXYGEN_COMMENT;
      }

      // skip the comment
      while (_lastChar != EOF && _lastChar != '\n' && _lastChar != '\r')
      {
        _lastChar = getChar();
      }
    }

    if (_lastChar == '"')
    {
      _lastChar = getChar();
      _identifier.clear();
      while (_lastChar != EOF && _lastChar != '"')
      {
        _identifier += _lastChar;
        _lastChar = getChar();
      }

      // eat the closing "
      _lastChar = getChar();
      return TOK_STRING_LITERAL;
    }

    // don't eat the EOF
    if (_lastChar == EOF) return TOK_EOF;

    // return the character as its ascii value
    int thisChar = _lastChar;
    _lastChar = getChar();
    return thisChar;
  }

  std::string getIdentifier() const
  {
    return std::string(_identifier.c_str());
  }

  int curLine() const
  { return _line; }

  int curCol() const
  { return _col; }

  int getChar()
  {
    int c = _is.get();
    updateLoc(c);
    return c;
  }

private:

  void updateLoc(int c)
  {
    if (c == '\n' || c == '\r')
    {
      ++_line;
      _col = 1;
    }
    else
    {
      ++_col;
    }
  }

  ci_string _identifier;
  int _lastChar;
  std::istream& _is;

  int _line;
  int _col;
};

//--------------------------------------
// Parser
//--------------------------------------

class CMakeParser
{

public:

  CMakeParser(std::istream& is, std::ostream& os)
    : _is(is), _os(os), _lexer(is), _curToken(CMakeLexer::TOK_EOF)
  { }

  int curToken()
  {
    return _curToken;
  }

  int nextToken()
  {
    return _curToken = _lexer.getToken();
  }

  void handleMacro()
  {
    if(!parseMacro())
    {
      // skip token for error recovery
      nextToken();
    }
  }

  void handleFunction()
  {
    if(!parseFunction())
    {
      // skip token for error recovery
      nextToken();
    }
  }

  void handleDoxygenComment()
  {
    _os << "///" << _lexer.getIdentifier() << std::endl;
    nextToken();
  }

  void handleTopLevelExpression()
  {
    // skip token
    nextToken();
  }

private:

  void printError(const char* str)
  {
    std::cerr << "Error: " << str << " (at line " << _lexer.curLine() << ", col " << _lexer.curCol() << ")\n";
  }

  bool parseMacro()
  {
    if (nextToken() != '(')
    {
      printError("Expected '(' after MACRO");
      return false;
    }

    nextToken();
    std::string macroName = _lexer.getIdentifier();
    if (curToken() != CMakeLexer::TOK_IDENTIFIER || macroName.empty())
    {
      printError("Expected macro name");
      return false;
    }

    _os << macroName << '(';
    if (nextToken() == CMakeLexer::TOK_IDENTIFIER)
    {
      _os << _lexer.getIdentifier();
      while (nextToken() == CMakeLexer::TOK_IDENTIFIER)
      {
        _os << ", " << _lexer.getIdentifier();
      }
    }

    if (curToken() != ')')
    {
      printError("Missing expected ')'");
    }
    else
    {
      _os << ");" << std::endl;
    }

    // eat the ')'
    nextToken();
    return true;
  }

  bool parseFunction()
  {
    if (nextToken() != '(')
    {
      printError("Expected '(' after FUNCTION");
      return false;
    }

    nextToken();
    std::string funcName = _lexer.getIdentifier();
    if (curToken() != CMakeLexer::TOK_IDENTIFIER || funcName.empty())
    {
      printError("Expected function name");
      return false;
    }

    _os << funcName << '(';
    if (nextToken() == CMakeLexer::TOK_IDENTIFIER)
    {
      _os << _lexer.getIdentifier();
      while (nextToken() == CMakeLexer::TOK_IDENTIFIER)
      {
        _os << ", " << _lexer.getIdentifier();
      }
    }

    if (curToken() != ')')
    {
      printError("Missing expected ')'");
    }
    else
    {
      _os << ");" << std::endl;
    }

    // eat the ')'
    nextToken();

    return true;
  }

  std::istream& _is;
  std::ostream& _os;
  CMakeLexer _lexer;
  int _curToken;
};


#define STRINGIFY(a) #a
#define DOUBLESTRINGIFY(a) STRINGIFY(a)

int main(int argc, char** argv)
{
  assert(argc > 1);

  for (int i = 1; i < argc; ++i)
  {
    std::ifstream ifs(argv[i]);
    std::ostream& os = std::cout;

    #ifdef USE_NAMESPACE
    os << "namespace " << DOUBLESTRINGIFY(USE_NAMESPACE) << " {\n";
    #endif

    CMakeParser parser(ifs, os);
    parser.nextToken();
    while (ifs.good())
    {
      switch (parser.curToken())
      {
      case CMakeLexer::TOK_EOF:
        return ifs.get(); // eat EOF
      case CMakeLexer::TOK_MACRO:
        parser.handleMacro();
        break;
      case CMakeLexer::TOK_FUNCTION:
        parser.handleFunction();
        break;
      case CMakeLexer::TOK_DOXYGEN_COMMENT:
        parser.handleDoxygenComment();
        break;
      default:
        parser.handleTopLevelExpression();
        break;
      }
    }

    #ifdef USE_NAMESPACE
    os << "}\n";
    #endif
  }

  return EXIT_SUCCESS;
}
