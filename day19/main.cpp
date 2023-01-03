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

using Result = int;
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

const std::vector<std::string> SYMBOL_TABLE{"ore","clay","obsidian","geode"};

std::string to_name(int index) {
  return SYMBOL_TABLE[index];
}

int to_index(std::string const& name) {
  auto iter = std::find_if(SYMBOL_TABLE.begin(),SYMBOL_TABLE.end(),[&name](auto const& s){
    return s==name;
  });
  return std::distance(SYMBOL_TABLE.begin(),iter);
}

struct Resources {
  using Amounts = std::vector<Result>;
  Amounts m_amounts{Amounts(SYMBOL_TABLE.size(),0)};
  bool operator>=(Resources const& other) const {
    return m_amounts >= other.m_amounts;
  }
  Resources& operator+=(Resources const& other) {
    for (int index=0;index<other.m_amounts.size();++index) {
      m_amounts[index] += other.m_amounts[index];
    }
    return *this;
  }
  Resources& operator-=(Resources const& other) {
    for (int index=0;index<other.m_amounts.size();++index) {
      m_amounts[index] -= other.m_amounts[index];
    }
    return *this;
  }
  Resources operator-(Resources const& other) const {
    Resources result{.m_amounts=other.m_amounts};
    result -= other;
    return result;
  }
};

std::ostream& operator<<(std::ostream& os,Resources const& r) {
  for (int index=0;index<r.m_amounts.size();++index) {
    if (index>0) os << " and ";
    os << " name:" << to_name(index) << " amount:" << r.m_amounts[index];
  }
  return os;
}

Resources to_resource(std::string const& name,int amount) {
  // std::cout << "\nto_resource(name:" << name << " amount:" << amount << ")" << std::flush; 
  Resources result{.m_amounts=Resources::Amounts(SYMBOL_TABLE.size(),{})};
  // std::cout << " to_index(" << name << ")=" << to_index(name) << std::flush;
  result.m_amounts[to_index(name)] = amount;
  // std::cout << " result" << std::flush;
  return result;
}

struct BluePrint {
  std::vector<Resources> cost{std::vector<Resources>(SYMBOL_TABLE.size(),Resources{})};
};

std::ostream& operator<<(std::ostream& os,BluePrint const& blueprint) {
  for (int index=0;index<blueprint.cost.size();++index) {
    if (index>0) os << "\n\t";
    os << " robot:" << to_name(index) << " cost:" << blueprint.cost[index];
  }
  return os;
}

using BluePrints = std::vector<BluePrint>;

std::ostream& operator<<(std::ostream& os,BluePrints const& blueprints) {
  int index{};
  for (auto const& blueprint : blueprints) {
    if (index>0) os << "\n";
    os << "blueprint:" << index+1;
    os << "\n\t" << blueprint;
    ++index;
  }
  return os;
}

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
    int left{4};
    int right{left+1};
    while (right < words.size()) {
      std::cout << " left:" << words[left] << " right:" << words[right] << std::flush;
      result.cost[to_index(words[1])] += to_resource(words[right],std::stoi(words[left]));
      left += 2;
      if (left < words.size() and words[left] == "and") ++left;
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
    if (has_empty_lines == true) {
      result.push_back(to_blueprint(row));
      row.clear();
    }
    return result;
}

struct State {
  BluePrint* blueprint;
  int time{};
  Resources robots{};
  Resources resources{};
  State& update_resources();
};

State& State::update_resources() {
  for (int index=0;index<this->robots.m_amounts.size();++index) {
    std::cout << "\n" << this->robots.m_amounts[index] << " of robot " << to_name(index) << " collects " << this->robots.m_amounts[index] << " of " << to_name(index); 
    this->resources.m_amounts[index] += this->robots.m_amounts[index]; // each robot creates one of its resource type
    std::cout << ". You have " << this->resources.m_amounts[index] << " " << to_name(index);
  }
  return *this;
}

std::vector<State> adj(State const& state) {
  // Create possible (adjacent) sub-states of state
  std::vector<State> result;
  if (state.time>0) {
    auto adj_t = state.time-1;
    auto resources = state.resources;
    // Try adding a robot for next state (going from max index to 0, i.e., assuming it is better to aim at building a high value robot before a low value?)
    for (int index=0;index<SYMBOL_TABLE.size();++index) {
      auto cost = state.blueprint->cost.at(index);
      if (resources >= cost) {
        State new_state{state};
        new_state.time = adj_t;
        new_state.resources = resources-cost;
        new_state.update_resources(); // exercise robots
        ++new_state.robots.m_amounts[index]; // build new robot
        result.push_back(new_state);
        resources = new_state.resources;
      }
    }
    // Try next time t without building any robot (?)
    State new_state{state};
    new_state.time = adj_t;
    new_state.update_resources(); // exercise robots
    result.push_back(new_state);
  }
  return result;
}

std::ostream& operator<<(std::ostream& os,State const& state) {
  os << " time:" << state.time;
  os << " robots[";
  for (int index = 0;index<state.robots.m_amounts.size();++index) {
    if (index>0) os << ",";
    os << state.robots.m_amounts[index];
  }
  os << "]";
  os << " resources[";
  for (int index = 0;index<state.resources.m_amounts.size();++index) {
    if (index>0) os << ",";
    os << state.resources.m_amounts[index];
  }
  os << "]";
  return os;
}

Result dfs(State const& state,int end_time) {
  std::cout << "\ndfs(state.time:" << state.time  << ")";
  std::cout << state;
  Result result{state.resources.m_amounts[4]}; // count of geodes
  if (state.time>end_time) {
    for (auto const& adj_state : adj(state)) {
      result = std::max(dfs(adj_state,end_time),result);
    }
  }
  return result;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\n" << data_model;
      for (auto& blueprint : data_model) {
        std::cout << "\n\nTRY BLUEPRINT";
        std::cout << "\n\t" << blueprint;
        result = std::max(dfs(State{.blueprint=&blueprint,.time=24,.robots={.m_amounts={1,0,0,0}},.resources={}},0),result);
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

void test(char const* pData) {
  std::stringstream in{ pData };
  auto data_model = parse(in);
  auto blueprint = data_model[0];
  std::cout << "\n" << data_model;
  std::cout << "\n\nTRY BLUEPRINT";
  std::cout << "\n\t" << blueprint;
  State state{.blueprint=&blueprint,.time=1,.robots={.m_amounts={1,0,0,0}},.resources={}};
  std::vector<std::pair<int,std::string>> ops{
     {3,"clay"}
    ,{5,"clay"}
    ,{7,"clay"}
    ,{11,"obsidian"}
    ,{12,"clay"}
    ,{15,"obsidian"}
    ,{18,"geode"}
    ,{21,"geode"}
  };
  int op_index{0};
  while (op_index<ops.size()) {
    auto op = ops[op_index];
    while (state.time < op.first) {
      std::cout << "\n== minute " << state.time << " ==";
      state.update_resources();
      ++state.time;
    }
    std::cout << "\n== minute " << state.time << " ==";
    auto index = to_index(op.second);
    auto cost = state.blueprint->cost.at(to_index(op.second));
    if (state.resources >= cost) {
      std::cout << "\nSpend " << cost << " to start building a " << to_name(index) << " collectin/cracking robot.";
      state.resources -= cost;
      state.update_resources();
      ++state.robots.m_amounts[index];
      ++state.time;
    }
    ++op_index;
  }
  while (state.time <= 24) {
    std::cout << "\n== minute " << state.time << " ==";
    state.update_resources();
    ++state.time;
  }
}

/*

== minute 24 ==
1 of robot ore collects 1 of ore. You have 6 ore
4 of robot clay collects 4 of clay. You have 41 clay
2 of robot obsidian collects 2 of obsidian. You have 8 obsidian
2 of robot geode collects 2 of geode. You have 9 geode%         

== Minute 24 ==
1 ore-collecting robot collects 1 ore; you now have 6 ore.
4 clay-collecting robots collect 4 clay; you now have 41 clay.
2 obsidian-collecting robots collect 2 obsidian; you now have 8 obsidian.
2 geode-cracking robots crack 2 geodes; you now have 9 open geodes.
*/

int main(int argc, char *argv[])
{
  if (true) {
    std::cout << "\nTEST";
    test(pTest);
    exit(0);
  }
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