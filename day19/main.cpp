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

std::string to_trimmed(std::string const& s) {
  std::string result{};
  auto begin=s.begin();
  while  (std::isspace(*begin)) ++begin;
  auto end = s.end();--end;
  while (std::isspace(*end)) end--;
  return std::string{begin,end+1};
}

std::pair<std::string,std::string> to_splitted(std::string const& s,std::string const& delim) {
  auto pos = s.find(delim);
  if (pos != std::string::npos) {
    return {to_trimmed(s.substr(0,pos)),to_trimmed(s.substr(pos+delim.size()))};
  }
  else {
    return {to_trimmed(s),""};
  }
}

std::vector<std::string> to_tokens(std::string s,std::string const& delim) {
  std::vector<std::string> result{};
  while (true) {
    auto [head,tail] = to_splitted(s,delim);
    if (head.size()>0) {
      result.push_back(head);
      s = tail;
    }
    else break;
  }
  return result;
}

struct Resource {
  std::map<std::string,Result> m_amount;
  Resource& operator+=(Resource const& other) {
    for (auto const& [name,amount] : other.m_amount) {
      m_amount[name] += amount;
    }
    return *this;
  }
};

std::ostream& operator<<(std::ostream& os,Resource const& r) {
  for (auto iter=r.m_amount.begin();iter!=r.m_amount.end();++iter) {
    if (iter != r.m_amount.begin()) os << " and ";
    os << " name:" << iter->first << " amount:" << iter->second;
  }
  return os;
}

Resource to_resource(std::string const& name,int amount) {
  Resource result{};
  result.m_amount[name] = amount;
  return result;
}

struct BluePrint {
  std::map<std::string,Resource> cost{};
};

std::ostream& operator<<(std::ostream& os,BluePrint const& blueprint) {
  int count{};
  for (auto [name,resource] : blueprint.cost) {
    if (count++>0) os << "\n\t";
    std::cout << " robot:" << name << " cost:" << resource;
  }
  return os;
}

using BluePrints = std::vector<BluePrint>;

std::ostream& operator<<(std::ostream& os,BluePrints const& blueprints) {
  int index{};
  for (auto const& blueprint : blueprints) {
    if (index>0) std::cout << "\n";
    std::cout << "blueprint:" << index+1;
    std::cout << "\n\t" << blueprint;
    ++index;
  }
  return os;
}

struct GeodeCrackingOperation {
  BluePrint blueprint{};
};

using Model = BluePrints;

BluePrint to_blueprint(std::string const& s) {
  // Blueprint 1: Each ore robot costs 2 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 15 clay. Each geode robot costs 2 ore and 15 obsidian.
  std::cout << "\nto_blueprint(" << s << ")";
  BluePrint result{};
  auto [head,tail] = to_splitted(s,":");
  std::cout << "\n\thead:" << std::quoted(head) << " tail:" << std::quoted(tail);
  auto [left,right] = to_splitted(head," ");
  std::cout << "\n\thead.left:" << std::quoted(left) << " head.right:" << std::quoted(right);
  auto blueprint_id = std::stoi(right);
  auto tokens = to_tokens(tail,".");
  std::cout << "\n\t";
  for (auto const& token : tokens) {
    std::cout << "\n\ttail.token:" << std::quoted(token);
    auto words = to_tokens(token," ");
    for (auto const& word : words) {
      std::cout << "\n\t\t" << std::quoted(word);
    }
    int robot_id{};
    if (words[1]=="ore") {
      std::cout << "\n\tORE";
      robot_id=0;
    }
    else if (words[1]=="clay") {
      std::cout << "\n\tCLAY";
      robot_id=1;
    }
    else if (words[1]=="obsidian") {
      std::cout << "\n\tOBSIDIAN";
      robot_id=2;
    }
    else if (words[1]=="geode") {
      std::cout << "\n\tGEODE";      
      robot_id=3;
    }
    for (int i = 4;i<words.size();++i) {
      std::cout << " " << words[i];
    }
    auto left{4};
    auto right{left+1};
    while (right < words.size()) {
      std::cout << " left:" << words[left] << " right:" << words[right];
      result.cost[words[1]] += to_resource(words[right],std::stoi(words[left]));
      left += 2;
      if (words[left] == "and") ++left;
      right = left+1;
    }
  }
  return result;
}

Model parse(auto& in) {
    Model result{};
    std::vector<std::string> lines{};
    bool has_empty_lines{false};
    {
      std::string line{};
      while (std::getline(in,line)) {
          lines.push_back(line);
          has_empty_lines = has_empty_lines or (line.size()==0); 
      }    
    }
    std::string row{};
    for (auto const& line : lines) {
      if (has_empty_lines == false) {
        result.push_back(to_blueprint(line));
      }
      else {
        if (line.size()==0) {
          result.push_back(to_blueprint(row));
          row.clear();
        }
        else {
          row += line;
        }
      }
    }
    result.push_back(to_blueprint(row));
    row.clear();
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

/*
blueprint:1
	 robot:clay cost: name:ore amount:2
	 robot:geode cost: name:obsidian amount:7 and  name:ore amount:2
	 robot:obsidian cost: name:clay amount:14 and  name:ore amount:3
	 robot:ore cost: name:ore amount:4
blueprint:2
	 robot:clay cost: name:ore amount:3
	 robot:geode cost: name:obsidian amount:12 and  name:ore amount:3
	 robot:obsidian cost: name:clay amount:8 and  name:ore amount:3
	 robot:ore cost: name:ore amount:2

*/

char const* pTest = R"(Blueprint 1:
  Each ore robot costs 4 ore.
  Each clay robot costs 2 ore.
  Each obsidian robot costs 3 ore and 14 clay.
  Each geode robot costs 2 ore and 7 obsidian.

Blueprint 2:
  Each ore robot costs 2 ore.
  Each clay robot costs 3 ore.
  Each obsidian robot costs 3 ore and 8 clay.
  Each geode robot costs 3 ore and 12 obsidian.)";
char const* pData = R"(Blueprint 1: Each ore robot costs 2 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 15 clay. Each geode robot costs 2 ore and 15 obsidian.
Blueprint 2: Each ore robot costs 4 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 12 clay. Each geode robot costs 3 ore and 8 obsidian.
Blueprint 3: Each ore robot costs 4 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 17 clay. Each geode robot costs 4 ore and 16 obsidian.
Blueprint 4: Each ore robot costs 4 ore. Each clay robot costs 4 ore. Each obsidian robot costs 2 ore and 16 clay. Each geode robot costs 4 ore and 16 obsidian.
Blueprint 5: Each ore robot costs 4 ore. Each clay robot costs 3 ore. Each obsidian robot costs 4 ore and 15 clay. Each geode robot costs 3 ore and 12 obsidian.
Blueprint 6: Each ore robot costs 2 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 17 clay. Each geode robot costs 3 ore and 11 obsidian.
Blueprint 7: Each ore robot costs 3 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 19 clay. Each geode robot costs 2 ore and 9 obsidian.
Blueprint 8: Each ore robot costs 3 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 18 clay. Each geode robot costs 2 ore and 11 obsidian.
Blueprint 9: Each ore robot costs 3 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 9 clay. Each geode robot costs 3 ore and 7 obsidian.
Blueprint 10: Each ore robot costs 4 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 15 clay. Each geode robot costs 4 ore and 17 obsidian.
Blueprint 11: Each ore robot costs 3 ore. Each clay robot costs 3 ore. Each obsidian robot costs 2 ore and 9 clay. Each geode robot costs 2 ore and 9 obsidian.
Blueprint 12: Each ore robot costs 2 ore. Each clay robot costs 4 ore. Each obsidian robot costs 3 ore and 20 clay. Each geode robot costs 2 ore and 17 obsidian.
Blueprint 13: Each ore robot costs 4 ore. Each clay robot costs 3 ore. Each obsidian robot costs 4 ore and 8 clay. Each geode robot costs 3 ore and 7 obsidian.
Blueprint 14: Each ore robot costs 3 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 20 clay. Each geode robot costs 4 ore and 16 obsidian.
Blueprint 15: Each ore robot costs 4 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 20 clay. Each geode robot costs 2 ore and 12 obsidian.
Blueprint 16: Each ore robot costs 4 ore. Each clay robot costs 3 ore. Each obsidian robot costs 2 ore and 13 clay. Each geode robot costs 2 ore and 10 obsidian.
Blueprint 17: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 18 clay. Each geode robot costs 2 ore and 19 obsidian.
Blueprint 18: Each ore robot costs 3 ore. Each clay robot costs 4 ore. Each obsidian robot costs 2 ore and 15 clay. Each geode robot costs 3 ore and 7 obsidian.
Blueprint 19: Each ore robot costs 3 ore. Each clay robot costs 4 ore. Each obsidian robot costs 3 ore and 10 clay. Each geode robot costs 4 ore and 8 obsidian.
Blueprint 20: Each ore robot costs 4 ore. Each clay robot costs 4 ore. Each obsidian robot costs 3 ore and 6 clay. Each geode robot costs 2 ore and 14 obsidian.
Blueprint 21: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 16 clay. Each geode robot costs 2 ore and 11 obsidian.
Blueprint 22: Each ore robot costs 3 ore. Each clay robot costs 3 ore. Each obsidian robot costs 2 ore and 19 clay. Each geode robot costs 2 ore and 20 obsidian.
Blueprint 23: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 2 ore and 17 clay. Each geode robot costs 3 ore and 19 obsidian.
Blueprint 24: Each ore robot costs 3 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 16 clay. Each geode robot costs 3 ore and 15 obsidian.
Blueprint 25: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 17 clay. Each geode robot costs 3 ore and 10 obsidian.
Blueprint 26: Each ore robot costs 4 ore. Each clay robot costs 4 ore. Each obsidian robot costs 3 ore and 20 clay. Each geode robot costs 2 ore and 10 obsidian.
Blueprint 27: Each ore robot costs 4 ore. Each clay robot costs 4 ore. Each obsidian robot costs 2 ore and 11 clay. Each geode robot costs 2 ore and 7 obsidian.
Blueprint 28: Each ore robot costs 4 ore. Each clay robot costs 3 ore. Each obsidian robot costs 2 ore and 10 clay. Each geode robot costs 4 ore and 10 obsidian.
Blueprint 29: Each ore robot costs 2 ore. Each clay robot costs 2 ore. Each obsidian robot costs 2 ore and 8 clay. Each geode robot costs 2 ore and 14 obsidian.
Blueprint 30: Each ore robot costs 3 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 18 clay. Each geode robot costs 3 ore and 8 obsidian.)";
