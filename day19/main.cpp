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

using State = std::array<int,9>; // time left,4 resource count 4 robot count
std::ostream& operator<<(std::ostream& os,State const& state) {
  for (auto x : state) os << " " << x; 
  return os;
}

struct BFS {
public:
  BFS(BluePrint const& blueprint) : m_blueprint{blueprint} {}
  Result best(int start_time) {
    return bfs(start_time,1,0,0,0);
  } 
private:
  BluePrint m_blueprint;
  using Q = std::deque<State>;
  // based on https://github.com/jonathanpaulson/AdventOfCode/blob/master/2022/19.py
  // many thanks and credits to Jonathan Paulson (https://youtu.be/yT3yHDp6hss)
  Result bfs(int t,int r1, int r2, int r3, int r4) {
    static int call_count{0};
    Result best{};
    Q q{};
    q.push_back({t,0,0,0,0,r1,r2,r3,r4});
    std::set<State> seen{};
    while (!q.empty()) {
      auto state = q.front(); // Breadth first search
      auto [t,a1,a2,a3,a4,r1,r2,r3,r4] = state;
      q.pop_front();
      best = std::max(best,state[4]); // current geodes count best?
      if (t==0) continue; // times up = exhausted state

      if (call_count++ % 50000 == 0) std::cout << "\n" << state << " " << best;

      auto max1 = m_blueprint.max_required(0);
      auto max2 = m_blueprint.max_required(1);
      auto max3 = m_blueprint.max_required(2);

      // Reduce our search state
      if (r1 > max1) r1 = max1; // Reduce ore robot count so they still produce ore to buy eny robot in one turn
      if (r2 > max2) r2 = max2; // Reduce clay robot count so they still produce clay to buy eny robot in one turn
      if (r3 > max3) r3 = max3; // Reduce obsidian robot count so they still produce obsidian to buy any robot in one turn

      auto net1 = max1*t - r1*(t-1); // Amount of ore we need to buy any robot in time left
      if (a1 > net1) a1 = net1; // Reduce ore so we have enough for our needs the next t minutes
      auto net2 = max2*t - r2*(t-1); // Amount of clay we need to buy any robot in time left
      if (a2 > net2) a2 = net2; // Reduce clay so we have enough for our needs the next t minutes
      auto net3 = max3*t - r3*(t-1); // Amount of obsidian we need to buy any robot in time left
      if (a2 > net2) a2 = net2; // Reduce obsidian so we have enough for our needs the next t minutes

      State reduced_state{t,a1,a2,a3,a4,r1,r2,r3,r4};
      if (seen.contains(reduced_state)) continue; // seen = exhausted state
      seen.insert(reduced_state);

      q.push_back({t-1,a1+r1,a2+r2,a3+r3,a4+r4,r1,r2,r3,r4}); // harvest but no new robots

      auto c1 = m_blueprint.cost[0].m_amounts[0]; // cost of ore robot in ore
      if (a1 >= c1) // enough ore?
        q.push_back({t-1,a1+r1-c1,a2+r2,a3+r3,a4+r4,r1+1,r2,r3,r4}); // harvest, pay ore and create ore robot

      auto c2 = m_blueprint.cost[1].m_amounts[0]; // cost clay robot in ore
      if (a1 >= c2) // enough ore?
        q.push_back({t-1,a1+r1-c2,a2+r2,a3+r3,a4+r4,r1,r2+1,r3,r4}); // harvest, pay ore and create clay robot

      auto c31 = m_blueprint.cost[2].m_amounts[0]; // cost obsidian robot in ore
      auto c32 = m_blueprint.cost[2].m_amounts[1]; // cost obsidian robot in clay
      if ((a1 >= c31) and (a2 >= c32)) // enough ore and clay?
        q.push_back({t-1,a1+r1-c31,a2+r2-c32,a3+r3,a4+r4,r1,r2,r3+1,r4}); // buy obsidian robot with ore and clay
      
      auto c41 = m_blueprint.cost[3].m_amounts[0]; // cost geode robot in ore
      auto c42 = m_blueprint.cost[3].m_amounts[2]; // cost geode robot in obsidian
      if ((a1 >= c41) and (a3 >= c42)) // Enough ore and obsidian? 
        q.push_back({t-1,a1+r1-c41,a2+r2,a3+r3-c42,a4+r4,r1,r2+1,r3,r4+1}); // buy geode robot with ore and obsidian
    }
    return best;
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
        BFS bfs{blueprint};
        auto best = bfs.best(24);
        result += best * (index+1);
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
  return result;
}

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
    // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
    // exec_times.push_back(std::chrono::system_clock::now());
    answers.push_back({"Part 1     ",part1::solve_for(pData)});
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