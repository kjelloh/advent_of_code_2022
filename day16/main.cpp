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

struct Valve {
  std::string name;
  int flow_rate;
};

std::ostream& operator<<(std::ostream& os,Valve const& valve) {
  os << "{name:" << valve.name << ",flow_rate:" << valve.flow_rate << "}";
  return os;
}

using Valves = std::map<std::string,Valve>;

using Connection = std::pair<Valve,std::vector<std::string>>;

class Graph {
public:
  using Names = std::set<std::string>;
  using AdjList = std::map<std::string,Names>;
  Names const& adj(std::string const& name) const {return m_adj.at(name);}
  Graph& insert(Connection const& connection) {
    m_valves[connection.first.name] = connection.first;
    for (auto const& name : connection.second) {
      m_adj[connection.first.name].insert(name);
    }     
    return *this;
  }
  Valves const& valves() const {
    return m_valves;
  }
private:
  AdjList m_adj;
  Valves m_valves;
};

std::ostream& operator<<(std::ostream& os,Graph const& graph) {
  for (auto const& [v_name,valve] : graph.valves()) {
    os << "\nvalve:" << valve;
    for (auto const& adj_name : graph.adj(valve.name)) {
      os << " adj:" << adj_name;
    }
  }
  return os;
}

class Model {
public:
  Model(Graph const& graph) : m_graph{graph} {
    for (auto const& [v_name,valve] : graph.valves()) {
      ++m_V;
      for (auto const& adj_name : graph.adj(valve.name)) {
        ++m_E;
      }
    }
  }
private:
  friend std::ostream& operator<<(std::ostream& os,Model const& model);
  Graph m_graph;
  int m_E{};
  int m_V{};
};

std::ostream& operator<<(std::ostream& os,Model const& model) {
  os << "model graph V:" << model.m_V << " E:" << model.m_E; 
  os << model.m_graph;
  return os;
}

Connection to_connection(std::string const& s) {
  std::cout << "\nto_connection(" << s << ")";
  Connection result{};
  // Valve AA has flow rate=0; tunnels lead to valves DD, II, BB
  const std::string tunnels_lead_to_valves{"tunnels lead to valves"};
  auto [head,tail] = to_splitted(s,"; ");
  std::cout << "\n\thead:" << head << " tail:" << tail;
  auto head_tokens = to_tokens(head," ");
  auto valve_name = head_tokens[1];
  auto [l,flow_rate] = to_splitted(head_tokens[4],"=");
  auto valve = Valve{.name=valve_name,.flow_rate=std::stoi(flow_rate)};
  result.first = valve;
  std::cout << "\n\tvalve:name:" << std::quoted(valve_name) << " flow_rate:" << flow_rate;
  tail = tail.substr(tunnels_lead_to_valves.size());
  auto adj_names = to_tokens(tail,", ");
  for (auto const& adj_name : adj_names) {
    std::cout << " adj_name:" << std::quoted(adj_name);
    result.second.push_back(adj_name);
  }
  return result;
}

Model parse(auto& in) {
    Graph graph{};
    std::string line{};
    while (std::getline(in,line)) {
      auto connection = to_connection(line);
      graph.insert(connection);
    }
    return Model{graph};
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

char const* pTest = R"(Valve AA has flow rate=0; tunnels lead to valves DD, II, BB
Valve BB has flow rate=13; tunnels lead to valves CC, AA
Valve CC has flow rate=2; tunnels lead to valves DD, BB
Valve DD has flow rate=20; tunnels lead to valves CC, AA, EE
Valve EE has flow rate=3; tunnels lead to valves FF, DD
Valve FF has flow rate=0; tunnels lead to valves EE, GG
Valve GG has flow rate=0; tunnels lead to valves FF, HH
Valve HH has flow rate=22; tunnel leads to valve GG
Valve II has flow rate=0; tunnels lead to valves AA, JJ
Valve JJ has flow rate=21; tunnel leads to valve II)";
char const* pData = R"(Valve OK has flow rate=0; tunnels lead to valves RW, FX
Valve JY has flow rate=13; tunnel leads to valve TT
Valve FX has flow rate=16; tunnels lead to valves OK, LF, GO, IV
Valve TD has flow rate=0; tunnels lead to valves XZ, ED
Valve VF has flow rate=9; tunnels lead to valves DS, LU, TR, WO
Valve TT has flow rate=0; tunnels lead to valves XZ, JY
Valve KR has flow rate=8; tunnels lead to valves VL, CI, GO, JJ, TQ
Valve HN has flow rate=0; tunnels lead to valves YG, AA
Valve MC has flow rate=24; tunnels lead to valves MI, EE, TH, YG
Valve XM has flow rate=0; tunnels lead to valves AF, JL
Valve XE has flow rate=0; tunnels lead to valves XP, AF
Valve ZF has flow rate=0; tunnels lead to valves EM, EI
Valve DS has flow rate=0; tunnels lead to valves VF, LF
Valve AF has flow rate=7; tunnels lead to valves AW, XE, CI, BJ, XM
Valve NL has flow rate=0; tunnels lead to valves KF, EM
Valve LF has flow rate=0; tunnels lead to valves FX, DS
Valve XZ has flow rate=25; tunnels lead to valves TD, TT
Valve TQ has flow rate=0; tunnels lead to valves AA, KR
Valve WO has flow rate=0; tunnels lead to valves VF, NE
Valve TH has flow rate=0; tunnels lead to valves LU, MC
Valve AA has flow rate=0; tunnels lead to valves TQ, KF, HN, XP, TY
Valve KB has flow rate=0; tunnels lead to valves WP, XL
Valve IV has flow rate=0; tunnels lead to valves PK, FX
Valve MI has flow rate=0; tunnels lead to valves JF, MC
Valve EX has flow rate=22; tunnels lead to valves JL, ZZ, SL
Valve ZZ has flow rate=0; tunnels lead to valves EX, JS
Valve KF has flow rate=0; tunnels lead to valves NL, AA
Valve PK has flow rate=11; tunnels lead to valves IV, HP
Valve TR has flow rate=0; tunnels lead to valves DI, VF
Valve YG has flow rate=0; tunnels lead to valves HN, MC
Valve JL has flow rate=0; tunnels lead to valves EX, XM
Valve VL has flow rate=0; tunnels lead to valves JS, KR
Valve XP has flow rate=0; tunnels lead to valves AA, XE
Valve TY has flow rate=0; tunnels lead to valves JS, AA
Valve EM has flow rate=4; tunnels lead to valves JJ, NL, ZF, WP, AW
Valve BJ has flow rate=0; tunnels lead to valves WK, AF
Valve JJ has flow rate=0; tunnels lead to valves EM, KR
Valve RW has flow rate=14; tunnels lead to valves NE, OK
Valve EI has flow rate=0; tunnels lead to valves ZF, JS
Valve SL has flow rate=0; tunnels lead to valves HP, EX
Valve EE has flow rate=0; tunnels lead to valves MC, XL
Valve WK has flow rate=0; tunnels lead to valves BJ, JS
Valve AW has flow rate=0; tunnels lead to valves EM, AF
Valve XL has flow rate=21; tunnels lead to valves EE, KB
Valve JF has flow rate=0; tunnels lead to valves MI, ED
Valve LU has flow rate=0; tunnels lead to valves TH, VF
Valve CI has flow rate=0; tunnels lead to valves AF, KR
Valve ED has flow rate=23; tunnels lead to valves JF, TD
Valve JS has flow rate=3; tunnels lead to valves VL, ZZ, EI, TY, WK
Valve NE has flow rate=0; tunnels lead to valves RW, WO
Valve DI has flow rate=12; tunnel leads to valve TR
Valve WP has flow rate=0; tunnels lead to valves KB, EM
Valve GO has flow rate=0; tunnels lead to valves FX, KR
Valve HP has flow rate=0; tunnels lead to valves SL, PK)";
