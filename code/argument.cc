///////////////////////////////////////////////////////////////////////////
//
//      SneezyMUD++ - All rights reserved, SneezyMUD Coding Team
//
///////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"

// split up "argument" on whitespace and place resulting sstrings in "args"
// returns number of arguments parsed
// you should use one of the other argument_parser definitions if possible
int argument_parser(const sstring &argument, vector <sstring> &args)
{
  unsigned int copy_begin=0, copy_end=0, count=0;
  sstring whitespace=" \f\n\r\t\v"; // taken from isspace() man page

  while(1){
    // find first non-whitespace past our last working point
    copy_begin=argument.find_first_not_of(whitespace, copy_end);
    
    // if nothing found, no more words, return
    if(copy_begin == sstring::npos)
      return count;
    
    // find our first whitespace past last non-whitespace
    copy_end=argument.find_first_of(whitespace, copy_begin);
    
    // if nothing found, we're on the last word, no trailing whitespace
    // push back the rest of the argument and return
    if(copy_end == sstring::npos){
      args.push_back(argument.substr(copy_begin));
      return count+1;
    }
    
    // otherwise, push back word and continue
    args.push_back(argument.substr(copy_begin, copy_end-copy_begin));
    count++;
  }

  return args.size();
}


int argument_parser(const sstring &argument, sstring &arg1)
{
  vector <sstring> args;

  int ret=argument_parser(argument, args);
  
  if(ret>=1)
    arg1=args[0];

  return ret;
}

int argument_parser(const sstring &argument, sstring &arg1, sstring &arg2)
{
  vector <sstring> args;

  int ret=argument_parser(argument, args);
  
  if(ret>=1)
    arg1=args[0];

  if(ret>=2)
    arg2=args[1];

  return ret;
}


int argument_parser(const sstring &argument, sstring &arg1, sstring &arg2, sstring &arg3)
{
  vector <sstring> args;

  int ret=argument_parser(argument, args);
  
  if(ret>=1)
    arg1=args[0];

  if(ret>=2)
    arg2=args[1];
  
  if(ret>=3)
    arg3=args[2];

  return max(3,ret);
}



