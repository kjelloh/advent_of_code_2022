#include <iostream>
#include <iomanip> // E.g., std::quoted
#include <string>
#include <sstream> // E.g., std::istringstream, std::ostringstream
#include <vector>
#include <set>
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

using Result = long int;
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
  using Vertex = std::string;
  using Vertices = std::vector<std::string>;
  using AdjList = std::map<Vertex,Vertices>;
  Vertices const& adj(std::string const& name) const {return m_adj.at(name);}
  Graph& insert(Connection const& connection) {
    m_valves[connection.first.name] = connection.first;
    for (auto const& name : connection.second) {
      m_adj[connection.first.name].push_back(name);
    }     
    return *this;
  }
  Valves const& valves() const {
    return m_valves;
  }
  Valve const& valve(Vertex const& v) const {
    return m_valves.at(v);
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

using Model = Graph;

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
    Model result{};
    std::string line{};
    while (std::getline(in,line)) {
      auto connection = to_connection(line);
      result.insert(connection);
    }
    return result;
}

struct Flow {
  int flow_t;
  Valve on_valve;
  Result to_gain() { return flow_t * on_valve.flow_rate;}
};
using Flows = std::set<Flow>;

struct State {
  using Vertex = Graph::Vertex;
  using Path = std::vector<Vertex>;
  Result gained_for_visting_v; // gained for visiting v
  Graph::Vertex v; // vertex to visit
  int flow_t_when_visit_v;
  Path const& already_open; // already open when visiting v (including v if so chosen)
  bool operator==(State const& other) const {
    return gained_for_visting_v == other.gained_for_visting_v and v==other.v and flow_t_when_visit_v==other.flow_t_when_visit_v and already_open==other.already_open;
  }
};

// custom hash can be a standalone function object:
struct StateHash
{
    std::size_t operator()(State const& state) const noexcept {
        std::size_t result = std::hash<std::string>{}(state.v);
        result ^= std::hash<int>{}(state.flow_t_when_visit_v);
        for (auto const& p : state.already_open) {
          result ^= std::hash<std::string>{}(p);
        }
        return result;
    }
};

class MaxFlow {
private:
  using Vertex = State::Vertex;
  using Path = State::Path; 
  Graph m_graph;
  Valves m_working_valves{};
  std::unordered_map<State,Result,StateHash> m_cache{};
  Result dfs(State const& state) {
    auto dfs_id = StateHash{}(state);
    Result result{};
    auto gained_for_visting_v = state.gained_for_visting_v;
    auto const& v = state.v;
    auto flow_t_when_visit_v = state.flow_t_when_visit_v;
    auto const& already_open = state.already_open;
    std::cout << "\ndfs_id:" << dfs_id << "dfs(gained_for_visting_v:" << gained_for_visting_v << " v:" << v << " flow_t_when_visit_v:" << flow_t_when_visit_v << " already_open_count:" << already_open.size()  << ")"; 
    for (auto const& p : already_open) std::cout<< "\ndfs_id:" << dfs_id << "\talready open:" << p;
    if (result = m_cache[state];result>0) {
      std::cout << "\ndfs_id:" << dfs_id << "\talready known, RETURN gained_for_visting_v:" << result;
      return result; // We already know the result coming to v with this state
    }
    else if (already_open.size() == m_working_valves.size()) {
      // no more valves to open
      result = gained_for_visting_v; 
      std::cout << "\ndfs_id:" << dfs_id << "\tall is open, RETURN gained_for_visting_v:" << result;
      return result;
    }
    else if (flow_t_when_visit_v<=0) {
      // No more time to open valves
      result = gained_for_visting_v; 
      std::cout << "\ndfs_id:" << dfs_id << "\ttimes up, RETURN gained_for_visting_v:" << result;
      return result; 
    }
    else {
      std::cout << "\ndfs_id:" << dfs_id << "\tadjacent_to:" << v;
      for (auto const& adj_v : m_graph.adj(v)) {
        std::cout  << "\ndfs_id:" << dfs_id << "\t\tadj_v:" << adj_v;
        auto valve_adj = m_graph.valve(adj_v);
        auto iter = std::find_if(already_open.begin(),already_open.end(),[&adj_v](Vertex const& p){
          return p == adj_v; // Adjacent valve is already open (in path)
        });
        if (iter == already_open.end() and valve_adj.flow_rate>0) {
          // adj_v is NOT open and has a flow rate (not broke) -> try open adjacent valve
          std::cout  << "\ndfs_id:" << dfs_id  << "\ttry open adjacent:" << adj_v;
          auto new_open = already_open; new_open.push_back(adj_v);
          // new gain for opening adjacent valve = flow_rate of adjacent valve * flow_t left to flow when we open it
          // We will open adjacent valve at flow_t_when_visit_v - 2 (one minute to step there and one minute to open it)
          auto gain_for_opening_adj_v = valve_adj.flow_rate*(std::max(flow_t_when_visit_v-2,0));
          State state_adj_v{.gained_for_visting_v=gained_for_visting_v+gain_for_opening_adj_v,.v=adj_v,.flow_t_when_visit_v=flow_t_when_visit_v-2,.already_open=new_open};
          result = std::max(result,dfs(state_adj_v)); // update result if better
        }
        {
          // try next without opening the valve at adj_v
          std::cout  << "\ndfs_id:" << dfs_id  << "\tmove to adjacent:" << adj_v;
          auto gain_for_not_opening_adj_v = gained_for_visting_v;
          auto state_adj_v = state;
          state_adj_v.v = adj_v;
          state_adj_v.flow_t_when_visit_v = flow_t_when_visit_v-1;
          result = std::max(result,dfs(state_adj_v)); // update result if better
        }
      }
      std::cout  << "\ndfs_id:" << dfs_id  << " RETURN from dfs(gained_for_visting_v:" << gained_for_visting_v << " v:" << v << " flow_t_when_visit_v:" << flow_t_when_visit_v << " already_open_count:" << already_open.size()  << ") = " << result;
      m_cache[state] = result; // cache this result
      return result;
    }
  }
public:
  MaxFlow(Graph const& graph) : m_graph{graph} {
    for (auto const& [v,valve] : graph.valves()) {
      if (valve.flow_rate>0) {
        m_working_valves[v] = valve;
      }
    }
  }
  Result operator()(int start_t) {
    Result result{};
    return dfs(State{.gained_for_visting_v=0,.v="AA",.flow_t_when_visit_v=start_t,.already_open=Path{}});
  }
};

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\n" << data_model;
      MaxFlow max_flow{data_model};
      result = max_flow(30);
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
