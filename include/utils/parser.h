#ifndef _PARSER_H
#define _PARSER_H

// #include "all.h"

// #include <assert.h>
#include <iostream>


#include <algorithm>
// #include <string>
// #include <ctime>
#include <cmath>
// #include <cstdlib>

// #include <sstream>
// #include <fstream>

// #include <iomanip>
// #include <vector>
#include <map>
#include <set>
#include <random>
// #include <numeric>

// #include <stdio.h>
// #include <stdlib.h>
#include "utils.h"
#include "solver.h"


namespace ns 
{
  class Parser;
}

// Parser class
class ns::Parser {
public:
  // Supported formats for parsing
  enum parseFormatT { FORMAT_CSV, FORMAT_UNDEF };

  Parser(std::string fileName, parseFormatT format);
  ~Parser();
  std::vector<std::string> yieldLine();
  std::vector<std::vector<std::string>> yieldLines();

private:
  // Handle to file
  std::ifstream *fileP;
};

ns::Parser::Parser( std::string fileName, ns::Parser::parseFormatT format )
/*!endpublic*/
{
   // Assertion on supported parsing formats
   /*
   ASSERT( format == FORMAT_CSV, "Unsupported parse format" );
   fileP    = new ifstream;
   fileP->open( fileName, ifstream::in );
   ASSERT( fileP->is_open(), "Unable to open file: %s", fileName.c_str() );
   */
}

/*
 * Destructor for parser class
 */
/*!public*/
ns::Parser::~Parser( )
/*!endpublic*/
{
}

/*
 * Yields all records, populates the standard structure and returns
 * This function acts as an abstract layer to hide different formats
 * that might be supported in future
 *
 * Returns: Array of strings
 */
/*!public*/

std::vector<std::vector<std::string>> ns::Parser::yieldLines()
{
/*!endpublic*/
  /*
   vector <vector<string>> lines;
   vector <string> line;
   while( true ){
      line = yieldLine();

      // Stop if no new line to process
      if( line.size() == 0 ) break;
      lines.push_back( line );
   }
   return lines;
   */
   return {};
}

/*
 * Yields a single record, populates the standard structure and returns
 * This function acts as an abstract layer to hide different formats
 * that might be supported in future
 *
 * Returns: Array of strings
 */
/*!public*/
std::vector <std::string> ns::Parser::yieldLine()
/*!endpublic*/
{
  /*
   ASSERT( fileP->is_open(), "Illegal call to yieldLine. File not open yet!" );

   string buffer;
   vector <string> line;

   // Get a line that has something except \n
   if( getline(*fileP, buffer) ){
      string token;
      istringstream iss(buffer);
      while( getline(iss, token, ',') ){
         line.push_back( trim(token) );
      }
   }

   return line;
   */
  return {};
}


#endif // _PARSER_H
