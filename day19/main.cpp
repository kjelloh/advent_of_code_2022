#include <iostream>
#include <iomanip> // E.g., std::quoted
#include <string>
#include <sstream> // E.g., std::istringstream, std::ostringstream
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
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
    bool result{true};
    for (int index=0;index<SYMBOL_TABLE.size() and result;++index) {
      result = result and m_amounts[index] >= other.m_amounts[index];
    }
    return result;
  }
  bool operator>(Resources const& other) const {
    bool all_are_at_least_equal = *this >= other;
    bool at_least_one_is_greater{false};
    for (int n=0;n<m_amounts.size() and !at_least_one_is_greater;++n) {
      at_least_one_is_greater = at_least_one_is_greater or (m_amounts[n]>other.m_amounts[n]);
    }
    return all_are_at_least_equal and at_least_one_is_greater;
  }
  Resources& operator+=(Resources const& other) {
    for (int index=0;index<SYMBOL_TABLE.size();++index) {
      m_amounts[index] += other.m_amounts[index];
    }
    return *this;
  }
  Resources& operator-=(Resources const& other) {
    for (int index=0;index<SYMBOL_TABLE.size();++index) {
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

struct ResourcesHash {
  std::size_t operator()(Resources const& resources) const {
    std::size_t result{};
    for (auto amount : resources.m_amounts) {
      result = (result << 1) ^ std::hash<Result>{}(amount);
    }
    return result;
  }
};

std::ostream& operator<<(std::ostream& os,Resources::Amounts const& amounts) {
  os << "[";
  for (int index=0;index<amounts.size();++index) {
    if (index>0) os << ",";
    os << amounts[index];
  }
  os << "]";
  return os;
}

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
  int max_required(int index) {
    int result{0};
    for (int n=0;n<cost.size();++n) {
      result = std::max(result,cost[n].m_amounts[index]);
    }
    return result;
  }
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
  int time{};
  Resources robots{};
  Resources resources{};
  State& harvest_resources();
  bool operator==(State const& other) const {
    return false; // All states with different hash are unique
  }
};

struct StateHash {
  std::size_t operator()(State const& state) const {
    std::size_t result = std::hash<int>{}(state.time);
    result = (result << 1) ^ ResourcesHash{}(state.robots);
    result = (result << 1) ^ ResourcesHash{}(state.resources);
    return result;
  }
};

using StateCache = std::unordered_map<State,Result,StateHash>;

State& State::harvest_resources() {
  for (int index=0;index<this->robots.m_amounts.size();++index) {
    // std::cout << "\n" << this->robots.m_amounts[index] << " of robot " << to_name(index) << " collects " << this->robots.m_amounts[index] << " of " << to_name(index); 
    this->resources.m_amounts[index] += this->robots.m_amounts[index]; // each robot creates one of its resource type
    if (index==3 and this->robots.m_amounts[index]>0) {
      std::cout << "\nGEODE++ " << this->robots.m_amounts << " -> " << this->resources.m_amounts;
    }
    // std::cout << ". You have " << this->resources.m_amounts[index] << " " << to_name(index);
  }
  return *this;
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

class GeodeCrackingEnterprise {
public:
  GeodeCrackingEnterprise(BluePrint const& blueprint) : m_blueprint{blueprint} {}
  Result most_cracked_geodes(int minutes_to_run) {
    return dfs(State{.time=1,.robots={.m_amounts={1,0,0,0}},.resources={}},minutes_to_run);
  }
private:
  BluePrint m_blueprint;
  StateCache m_known{};
  int call_count{0};
  std::vector<int> m_peak_robots = std::vector<int>(4,0);
  Resources m_peak_robot_state{{0,0,0,0}};
  Result dfs(State const& state,int end_time) {    
    Result result{state.resources.m_amounts[3]}; // result = count of created geodes so far
    if (state.time==end_time) return result; // return candidate
    if (m_known.contains(state)) return m_known[state];
    if (call_count++ % 10000==0) std::cout << "\n" << call_count << " " << m_known.size() << " " << state << " " <<  m_peak_robots << result;
    if (true) {
      for (int i=0;i<SYMBOL_TABLE.size();++i) m_peak_robots[i] = std::max(state.robots.m_amounts[i],m_peak_robots[i]);
      if (state.resources.m_amounts[1]>=m_blueprint.cost[3].m_amounts[1] and state.resources.m_amounts[2]>=m_blueprint.cost[3].m_amounts[2]) {
        std::cout << "\nCAN BUILD GEODE CRACKING ROBOT ---------------------------";
      }
    }
    if (state.robots > m_peak_robot_state) {
      m_peak_robot_state = state.robots;
      std::cout << "\n" << call_count << " " << m_known.size() << " " << m_peak_robot_state << " " << result;
    }
    if (state.robots.m_amounts[3]>0) {
      std::cout << "\nGEODES! " << call_count << " " << m_known.size() << " " << state;
    }
    bool built_a_robot{false};
    for (int index=SYMBOL_TABLE.size()-1;index>=0;--index) {
      auto cost = m_blueprint.cost[index];
      if (state.resources >= cost) {
        // can afford this robot -> harvest resources and add new robot to robot collection
        // Max clay robots required is max cost of clay for any robot.
        // The same for the other resources
        // index is the robot candidate to produce more of its resources.
        // if (/* robots(index) < max_required(index), OK, we benefit from building this robot */)
        if ((index == 3) or (state.robots.m_amounts[index] < m_blueprint.max_required(index))) {
          State adj_state{.time=state.time+1,.robots=state.robots,.resources=state.resources-cost};
          if (index>2) {
            std::cout << "\n-" << call_count << " " << m_known.size() << " " << adj_state;
          }
          adj_state.harvest_resources(); // execute existing robots in next state
          if (index>2) {
            std::cout << "\nh" << call_count << " " << m_known.size() << " " << adj_state;
          }
          ++adj_state.robots.m_amounts[index]; // create the new robot
          if (index>2) {
            std::cout << "\np" << call_count << " " << m_known.size() << " " << adj_state;
          }
          result = std::max(dfs(adj_state,end_time),result);
          built_a_robot = true;
        }
        else {
          if (index > 1) std::cout << "\nEnough of robot " << index << " " << state;
        }
      }
    }
    // if (!built_a_robot) {
      // Assume it is only worth to harvest with existing robots if no new robot could be built? 
    if (true) {
      // explore next state with same robots but harvested resources
      State adj_state{.time=state.time+1,.robots=state.robots,.resources=state.resources};
      adj_state.harvest_resources(); // execute existing robots to harvest more resources
      result = std::max(dfs(adj_state,end_time),result);
    }
    m_known[state] = result; // keep to short cut future dfs call with same state
    return result;
  }
};

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\n" << data_model;
      Result best{};
      for (int index=0;index<data_model.size();++index) {
        auto const& blueprint = data_model[index];
        std::cout << "\n\nTRY BLUEPRINT";
        std::cout << "\n\t" << blueprint;
        GeodeCrackingEnterprise gce{data_model[index]};
        best = gce.most_cracked_geodes(24);
        result += best * index;
        std::cout << "\nblueprint:" << index << " best:" << best << " result:" << result;
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

bool test(char const* pData) {
  bool result{true};
  if (result) {
    result = false;
    std::stringstream in{ pData };
    auto data_model = parse(in);
    auto blueprint = data_model[0];
    int index = 0;
    GeodeCrackingEnterprise gce{data_model[index]};
    auto best = gce.most_cracked_geodes(24);
    std::cout << "\nblueprint:" << index << " best:" << best;
  }
  return result;
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
  if (argc>1 and std::string_view{argv[1]}=="test") {
    std::cout << "\nTEST";
    test(pTest);
  }
  else {
    Answers answers{};

    std::chrono::time_point<std::chrono::system_clock> start_time{};
    std::vector<std::chrono::time_point<std::chrono::system_clock>> exec_times{};
    exec_times.push_back(std::chrono::system_clock::now());
    answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 1     ",part1::solve_for(pData)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 2     ",part2::solve_for(pData)});
    exec_times.push_back(std::chrono::system_clock::now());
    for (int i=0;i<answers.size();++i) {
      std::cout << "\nduration:" << std::chrono::duration_cast<std::chrono::milliseconds>(exec_times[i+1] - exec_times[i]).count() << "ms"; 
      std::cout << " answer[" << answers[i].first << "] " << answers[i].second;
    }
    std::cout << "\n";
  }
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