#include <iostream>
#include <iomanip> // E.g., std::quoted
#include <string>
#include <sstream> // E.g., std::istringstream, std::ostringstream
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <deque>
#include <array>
#include <ranges> // E.g., std::subrange, std::view 
#include <utility> // E.g., std::pair,..
#include <algorithm> // E.g., std::find, std::all_of,...
#include <numeric> // E.g., std::accumulate
#include <limits> // E.g., std::numeric_limits

extern char const* pTest;
extern char const* pData;

using Result = long long int;
using Answers = std::vector<std::pair<std::string,Result>>;
struct SNAFU {
  std::string s;
};

using Model = std::vector<SNAFU>;

Model parse(auto& in) {
    Model result{};
    std::string line{};
    while (std::getline(in,line)) {
        result.push_back(SNAFU{line});
    }
    return result;
}

Result to_decimal(SNAFU const& snafu) {
  Result result{0};
  for (auto snafu_digit : snafu.s) {
    switch (snafu_digit) {
      case '2': result = result*5 + 2;break;
      case '1': result = result*5 + 1;break;
      case '0': result = result*5 + 0;break;
      case '-': result = result*5 - 1;break;
      case '=': result = result*5 - 2;break;
    }
  }
  return result;
}

/*

 SNAFU  Decimal
1=-0-2     1747
 12111      906
  2=0=      198
    21       11
  2=01      201
   111       31
 20012     1257
   112       32
 1=-1=      353
  1-12      107
    12        7
    1=        3
   122       37

SNAFU:1=-0-2 = decimal:1747
SNAFU:12111 = decimal:906
SNAFU:2=0= = decimal:198
SNAFU:21 = decimal:11
SNAFU:2=01 = decimal:201
SNAFU:111 = decimal:31
SNAFU:20012 = decimal:1257
SNAFU:112 = decimal:32
SNAFU:1=-1= = decimal:353
SNAFU:1-12 = decimal:107
SNAFU:12 = decimal:7
SNAFU:1= = decimal:3
SNAFU:122 = decimal:37
*/

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      for (auto const& snafu : data_model) {
        std::cout << "\nSNAFU:" << snafu.s << " = decimal:" << to_decimal(snafu);
      }
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      return result;
  }
}

int main(int argc, char *argv[])
{
  Answers answers{};
  answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
  // answers.push_back({"Part 1     ",part1::solve_for(pData)});
  // answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
  // answers.push_back({"Part 2     ",part2::solve_for(pData)});
  for (auto const& answer : answers) {
    std::cout << "\nanswer[" << answer.first << "] " << answer.second;
  }
  // std::cout << "\nPress <enter>...";
  // std::cin.get();
  std::cout << "\n";
  return 0;
}

char const* pTest = R"(1=-0-2
12111
2=0=
21
2=01
111
20012
112
1=-1=
1-12
12
1=
122)";
char const* pData = R"(1=21
12120-2211===-
1-2
2=2112-211-
1=0=-=22
112=022==0=--011=
22==-0-2-00-
2=20=2202=
1-22
220-==
2=11=0012
1-0=0
1=
1-1
1000-00=11-20-2
11
100-1
1-=-11
1211
1-=2
10=--00-=10-02
1=11==-20=11-1-2-
21
10=
10=212112==200=
10-=22-10
1-0211=-
10=-==221
1110022=1111-
10=2
1=1=2-0-0111=
1-=-=2=0=--
1=2=1=-0221===
1=--101=
2-000=0-21212=1-12
1===01==2=
1-==2122
100=--11-02=
1=120==1-0
201-10222-0=
11102--0=-=121-1=-
101
1=-1-00=0-12-0-120
10=--=20101==
1=0--000=
1=1==00=1-2011222
2=1-2=221-122=-==22
1--2100=1=22-=-11011
10-102-010020=
1=2=0100-22=12-2
1-=001-=220-00-02
1-2=121
2----0-
2=--022-=0-
10-22=002=-
2-=21=01
2=01-
2=1-2=--01202=2
112=20-1=-21101=
102212===201-
2-=2=21
1=-2=2--001=211==12
10=111222=-2022=12
1-12012=-10-==0==
1=1=2200=211-=
1-2-1===-00=21
2=1
12000=1
1=10=0=-1
1=2002=1-2=--0
1=2
112=0=20
1=0-2-
1-1=02
1=2==-00110
102-1-=12-==2212221
1--20--=-==021=121
11-=200==
11=010
2-00--0112102
1---=
1-=11=1-=-=0-=
2==-=22=12012-
20-=-=
12-10002-11
1=021=
1-00
1====0=2--
122
1==-02=112-0-10
1=1-11=2=--2
11211=21
1=-2-11-1100011001
2=-02101
11=1201=21=0
12100=
1-1-2==
2-1=-2=10001-=2
2==20-0=1=0=
11--112000-=11210
1-22-1-=
2111021-
102==
2002-
12=1--21-20
11=22-0-101221
101=01=----01
102--21
1---0-=-
2-==-01-202110=)";
