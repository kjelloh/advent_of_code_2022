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

using Result = size_t;
using Answers = std::vector<std::pair<std::string,Result>>;

using Model = std::vector<std::string>;

Model parse(auto& in) {
    Model result{};
    std::string line{};
    while (std::getline(in,line)) {
        result.push_back(line);
    }
    return result;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
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

bool test() {
  return false;
}

int main(int argc, char *argv[])
{
  if (argc>1 and std::string{argv[1]}=="test") {
    if (test()) std::cout << "\nTest: You have arrived at your destination :)";
    else std::cout << "\nTest: Not there yet...";
  }
  else {
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
  }
  return 0;
}

char const* pTest = R"()";
char const* pData = R"()";
