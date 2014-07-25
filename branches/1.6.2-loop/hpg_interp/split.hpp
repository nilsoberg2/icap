#ifndef __SPLIT_HPP_________________________20080425101818__
#define __SPLIT_HPP_________________________20080425101818__


#include <vector>

//-----------------------------------------------------------
// StrT:    Type of string to be constructed
//          Must have char* ctor.
// str:     String to be parsed.
// delim:   Pointer to delimiter.
// results: Vector of StrT for strings between delimiter.
// empties: Include empty strings in the results. 
//-----------------------------------------------------------
// By Paul Wiess
// http://www.codeproject.com/KB/string/stringsplit.aspx

template< typename StrT >
int wiess_split(const char* str, const char* delim, 
                std::vector<StrT>& results, bool empties = true)
{
  char* pstr = const_cast<char*>(str);
  char* r = NULL;
  r = strstr(pstr, delim);
  int dlen = (int)strlen(delim);
  while( r != NULL )
  {
    char* cp = new char[(r-pstr)+1];
    memcpy(cp, pstr, (r-pstr));
    cp[(r-pstr)] = '\0';
    if( strlen(cp) > 0 || empties )
    {
      StrT s(cp);
      results.push_back(s);
    }
    delete[] cp;
    pstr = r + dlen;
    r = strstr(pstr, delim);
  }
  if( strlen(pstr) > 0 || empties )
  {
    results.push_back(StrT(pstr));
  }
  return (int)results.size();
}


#endif//__SPLIT_HPP_________________________20080425101818__
