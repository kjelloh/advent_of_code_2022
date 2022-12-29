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

using Result = long int;
using Answers = std::vector<std::pair<std::string,Result>>;

using Instruction = std::pair<std::string,std::optional<int>>;
using Program = std::vector<Instruction>;
using Model = Program;

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

struct CPU {
  CPU(Program const& program) : m_program{program} {
    count_down=to_count_down(program[0].first);
  }
  int to_count_down(std::string const& op) {
    int result{};
    if (op=="noop") result=1;
    else if (op=="addx") result=2;
    return result;
  }
  Program m_program;
  int count_down{0};
  int ix{0};
  int call_count{0};
  Result x{1};
  CPU& operator++() {
    std::cout << "\n\toperator++() call_count:" << call_count << " ix:" << ix << " count_down:" << count_down;
    if (count_down==0) {
      auto [op,arg] = m_program[ix];
      if (op=="addx") {
        x += *arg;
        std::cout << " x:" << x;
      }
      ++ix;
      count_down = to_count_down(m_program[ix].first);
    }
    --count_down;
    ++call_count;
    return *this;
  }
};

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
      CPU cpu{data_model};
      /*
      Find the signal strength during the 20th, 60th, 100th, 140th, 180th, and 220th cycles. 
      What is the sum of these six signal strengths?
      */
      const std::vector<int> STOPS{20,60,100,140,180,220};
      for (int cycle=1;cycle<=STOPS.back();++cycle) {
        ++cpu;
        Result delta{0};
        if (std::any_of(STOPS.begin(),STOPS.end(),[&cycle](int i){ return i == cycle;})) {
            delta = cycle*cpu.x;
            std::cout << "\ncycle:" << cycle << " delta:" << delta;
        }
        result += delta;
        if (delta>0) std::cout << " result:" << result;
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
  answers.push_back({"Part 1     ",part1::solve_for(pData)});
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

// char const* pTest = R"(noop
// addx 3
// addx -5)";
char const* pTest = R"(addx 15
addx -11
addx 6
addx -3
addx 5
addx -1
addx -8
addx 13
addx 4
noop
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx -35
addx 1
addx 24
addx -19
addx 1
addx 16
addx -11
noop
noop
addx 21
addx -15
noop
noop
addx -3
addx 9
addx 1
addx -3
addx 8
addx 1
addx 5
noop
noop
noop
noop
noop
addx -36
noop
addx 1
addx 7
noop
noop
noop
addx 2
addx 6
noop
noop
noop
noop
noop
addx 1
noop
noop
addx 7
addx 1
noop
addx -13
addx 13
addx 7
noop
addx 1
addx -33
noop
noop
noop
addx 2
noop
noop
noop
addx 8
noop
addx -1
addx 2
addx 1
noop
addx 17
addx -9
addx 1
addx 1
addx -3
addx 11
noop
noop
addx 1
noop
addx 1
noop
noop
addx -13
addx -19
addx 1
addx 3
addx 26
addx -30
addx 12
addx -1
addx 3
addx 1
noop
noop
noop
addx -9
addx 18
addx 1
addx 2
noop
noop
addx 9
noop
noop
noop
addx -1
addx 2
addx -37
addx 1
addx 3
noop
addx 15
addx -21
addx 22
addx -6
addx 1
noop
addx 2
addx 1
noop
addx -10
noop
noop
addx 20
addx 1
addx 2
addx 2
addx -6
addx -11
noop
noop
noop)";

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
