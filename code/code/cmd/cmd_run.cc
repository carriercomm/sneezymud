#include <deque>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include "sstring.h"
#include "being.h"
#include "extern.h"


using namespace std;

namespace {
  deque<pair<int, char> > collection;
  int n = 1;

  void add_int(int const& k) {
    n = k;
  }

  void add_char(char const& c) {
    collection.push_back(make_pair(n, c));
    n = 1;
  }
};

bool parse(string path, deque<pair<int, char> >& res )
{
  string::const_iterator begin = path.begin();
  string::const_iterator end = path.end();
  using boost::spirit::qi::int_;
  using boost::spirit::qi::char_;

  boost::spirit::qi::parse(
    begin, end,
    *(-int_[&add_int] >> char_("neswud")[&add_char]));

  if (begin == end) {
    res.resize(collection.size());
    copy(collection.begin(), collection.end(), res.begin());
  }

  collection.clear();

  return begin == end;
}

/*
int main() {
  deque<pair<int, char> > res;

  if (parse("neswud1n2e10s11w20u21d1000n", res)) {
    cout << "Good parse:" << endl;
    while (!res.empty()) {
      cout << res.front().first << res.front().second << endl;
      res.pop_front();
    }
  } else {
    cout << "Bad parse" << endl;
  }
*/

void TBeing::doRun(sstring const& path) {
  deque<pair<int, char> > res;
  if (parse(path, res)) {
    while (!res.empty()) {
      int steps = res.front().first;
      char dir = res.front().second;
      for (int i = 0; i < steps; i++) {
	doMove(getDirFromChar(sstring() + dir));
      }
      res.pop_front();
    }
  }
}
