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

using Instruction = std::pair<std::string,std::optional<int>>;
using Model = std::vector<Instruction>;

std::ostream& operator<<(std::ostream& os,Instruction const& instruction) {
  os << "op:" << instruction.first << " arg:";
  if (instruction.second) os << *instruction.second;
  else os << "_";
  return os;
}

std::ostream& operator<<(std::ostream& os,Model const& model) {
  for (auto const& instruction : model) std::cout << "\n" << instruction;
  return os;
}

Instruction to_instruction(std::string const& entry) {
  Instruction result{};
  std::cout << "\nto_instruction(" << entry << ")";
  if (auto pos = entry.find(' '); pos != std::string::npos) {
    auto head = entry.substr(0,pos);
    auto tail = entry.substr(pos);
    if (tail.size()>0) {
      result = {head,std::stoi(tail)};
    }
    else {
      result = {head,std::nullopt};
    }
  }
  else result = {entry,std::nullopt};
  std::cout << " = " << result;
  return result;
}

Model parse(auto& in) {
    Model result{};
    std::string line{};
    while (std::getline(in,line)) {
      result.push_back(to_instruction(line));
    }
    return result;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\n" << data_model;
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

char const* pTest = R"(noop
addx 3
addx -5)";
// char const* pTest = R"(addx 15
// addx -11
// addx 6
// addx -3
// addx 5
// addx -1
// addx -8
// addx 13
// addx 4
// noop
// addx -1
// addx 5
// addx -1
// addx 5
// addx -1
// addx 5
// addx -1
// addx 5
// addx -1
// addx -35
// addx 1
// addx 24
// addx -19
// addx 1
// addx 16
// addx -11
// noop
// noop
// addx 21
// addx -15
// noop
// noop
// addx -3
// addx 9
// addx 1
// addx -3
// addx 8
// addx 1
// addx 5
// noop
// noop
// noop
// noop
// noop
// addx -36
// noop
// addx 1
// addx 7
// noop
// noop
// noop
// addx 2
// addx 6
// noop
// noop
// noop
// noop
// noop
// addx 1
// noop
// noop
// addx 7
// addx 1
// noop
// addx -13
// addx 13
// addx 7
// noop
// addx 1
// addx -33
// noop
// noop
// noop
// addx 2
// noop
// noop
// noop
// addx 8
// noop
// addx -1
// addx 2
// addx 1
// noop
// addx 17
// addx -9
// addx 1
// addx 1
// addx -3
// addx 11
// noop
// noop
// addx 1
// noop
// addx 1
// noop
// noop
// addx -13
// addx -19
// addx 1
// addx 3
// addx 26
// addx -30
// addx 12
// addx -1
// addx 3
// addx 1
// noop
// noop
// noop
// addx -9
// addx 18
// addx 1
// addx 2
// noop
// noop
// addx 9
// noop
// noop
// noop
// addx -1
// addx 2
// addx -37
// addx 1
// addx 3
// noop
// addx 15
// addx -21
// addx 22
// addx -6
// addx 1
// noop
// addx 2
// addx 1
// noop
// addx -10
// noop
// noop
// addx 20
// addx 1
// addx 2
// addx 2
// addx -6
// addx -11
// noop
// noop
// noop)";

char const* pData = R"(addx 1
addx 4
noop
noop
noop
addx 5
addx 3
noop
addx 2
noop
noop
noop
noop
addx 3
addx 5
addx 2
addx 1
noop
addx 5
addx -1
addx 5
noop
addx 3
noop
addx -40
noop
addx 38
addx -31
addx 3
noop
addx 2
addx -7
addx 8
addx 2
addx 5
addx 2
addx 3
addx -2
noop
noop
noop
addx 5
addx 2
noop
addx 3
addx 2
noop
addx 3
addx -36
noop
noop
addx 5
noop
noop
addx 8
addx -5
addx 5
addx 2
addx -15
addx 16
addx 4
noop
addx 1
noop
noop
addx 4
addx 5
addx -30
addx 35
addx -1
addx 2
addx -36
addx 5
noop
noop
addx -2
addx 5
addx 2
addx 3
noop
addx 2
noop
noop
addx 5
noop
addx 14
addx -13
addx 5
addx -14
addx 18
addx 3
addx 2
addx -2
addx 5
addx -40
noop
addx 32
addx -25
addx 3
noop
addx 2
addx 3
addx -2
addx 2
addx 2
noop
addx 3
addx 5
addx 2
addx 9
addx -36
addx 30
addx 5
addx 2
addx -25
addx 26
addx -38
addx 10
addx -3
noop
noop
addx 22
addx -16
addx -1
addx 5
addx 3
noop
addx 2
addx -20
addx 21
addx 3
addx 2
addx -24
addx 28
noop
addx 5
addx 3
noop
addx -24
noop)";
