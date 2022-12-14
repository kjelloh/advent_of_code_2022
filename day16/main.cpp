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
#include <chrono>
#include <cassert>
#include <bitset>
#include <cmath> // pow

extern char const* pTest;
extern char const* pData;

using Integer = long int;
using Result = Integer;
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

using Valves = std::vector<Valve>;

using Connection = std::pair<Valve,std::vector<std::string>>;
using Connections = std::vector<Connection>;

class Graph {
public:
  using Index = int;
  auto size() const {return m_adj.size();}
  auto const& adj(Index index) const {return m_adj.at(index);}
  Graph& insert(Index index1,Index index2) {
    m_adj[index1].insert(index2);
    m_adj[index2].insert(index1);
    return *this;
  }
private:
  std::map<Index,std::set<Index>> m_adj{};
};

class CaveSystem {
public:
  using Index = Graph::Index;
  CaveSystem(Connections connections) {
    // make the valves with a flow rate have the index range 0..
    for (auto [valve,names] : connections) {
      if (valve.flow_rate>0) {
        m_name2index[valve.name] = index(valve.name);
        m_valves.push_back(valve);
      }
    }
    for (auto [valve,names] : connections) {
      if (valve.flow_rate==0) {
        m_name2index[valve.name] = index(valve.name);
        m_valves.push_back(valve);
      }
    }
    for (auto [valve,names] : connections) {
      for (auto const& name : names) {
        m_name2index[name] = index(name);
        m_graph.insert(index(valve.name),index(name));
      }
    }
  }
  auto size() const {return m_graph.size();}
  std::string name(Index index) const {
    auto iter = std::find_if(m_name2index.begin(),m_name2index.end(),[index](auto const& entry){
      return (entry.second == index);
    });
    if (iter!=m_name2index.end()) return iter->first;
    else return "??";
  }
  Index index(std::string const& name) const {
    if (m_name2index.contains(name)) return m_name2index.at(name);
    else return m_name2index.size();
  }
  Graph const& graph() const {return m_graph;}
  Valves const& valves() const {return m_valves;}
private:
  Graph m_graph{};
  std::map<std::string,Index> m_name2index{};
  Valves m_valves{};
};

std::ostream& operator<<(std::ostream& os,CaveSystem const& cave_system) {  
  for (int i=0;i<cave_system.graph().size();++i) {
    if (i>0) os << "\n";
    os << i << ":" << cave_system.name(i) << " ";
    for (auto adj : cave_system.graph().adj(i)) {
      os << " -> " << adj << ":" << cave_system.name(adj);
    }
  }
  return os;
}

class MaxFlow {
public:
  using Index = CaveSystem::Index;
  using BitMap = std::bitset<15>;
  MaxFlow(CaveSystem const& cave_system) 
    : m_cave_system{cave_system}
      ,m_cache(KEY_RANGE,-1) {
    m_flowrate.resize(cave_system.size());
    for (auto const& valve : m_cave_system.valves()) {
      m_flowrate[m_cave_system.index(valve.name)] = valve.flow_rate;
    }
    for (int index=0;index<m_flowrate.size();++index) {
      std::cout << "\n" << index << ":" << m_cave_system.name(index) << " flowrate:" << m_flowrate[index];
    }
  }
  Result operator()(int start_time) {
    for (int t=0;t<=31;++t) {
      auto is_open = BitMap(std::string(15,'0'));
      std::bitset<26> bitmap{to_key(0,is_open,t)};
      std::cout << "\nkey:" << bitmap.to_string();
    }
    for (int i=0;i<15;++i) {
      auto is_open = BitMap(); is_open[i] = true;
      std::bitset<26> bitmap{to_key(63,is_open,31)};
      std::cout << "\nkey:" << bitmap.to_string();
    }
    for (int i=0;i<=63;++i) {
      auto is_open = BitMap(); is_open[14] = true;
      std::bitset<26> bitmap{to_key(i,is_open,31)};
      std::cout << "\nkey:" << bitmap.to_string();
    }
    return max_to_gain(m_cave_system.index("AA"),BitMap{},start_time);
  }
private:
  using Key = u_int32_t;
  const int KEY_BITS{26};
  const Key KEY_RANGE{Key{1} << (KEY_BITS+1)};
  Key to_key(Index valve_ix,BitMap is_open,int time_left) {
    // 54 valves = 6 bits
    // 15 possible valves to open (has flowrate > 0)= 15 bits
    // 31 time_left = 5 bits
    // Total  26 bits        2         1
    //                  54321098765432109876543210
    //                  vvvvvvfffffffffffffffttttt
    // v = valve index
    // f = flags for open valves (0..14)
    // t = time left
    Key result = (valve_ix<<20) + (is_open.to_ulong()<<5) + time_left;
    assert(result<=KEY_RANGE);
    return result;
  }
  std::vector<Result> m_cache{};

  // find the maximal possible flow to gain
  // from cave with start index, provided open valves and the time left to 0
  Result max_to_gain(Index start_index,BitMap const& is_open,int time_left) {
    static int call_count{};
    Result result{};
    if (time_left==0) {
      return 0; // nothing to gain
    }
    if (auto cached = m_cache[to_key(start_index,is_open,time_left)]>=0) return cached;
    if (m_flowrate[start_index]>0 and !is_open[start_index]) {
      // try open the valve here
      assert(start_index<is_open.size());
      auto new_is_open = is_open;
      new_is_open[start_index] = true;
      auto new_candidate = m_flowrate[start_index]*(time_left-1) + max_to_gain(start_index,new_is_open,time_left-1);
      if (new_candidate>result) {
        result = new_candidate;        
      }
    }
    for (auto adj : m_cave_system.graph().adj(start_index)) {
      // try going to adjacent valve
      auto new_candidate = max_to_gain(adj,is_open,time_left-1);
      if (new_candidate>result) {
        result = new_candidate;
      }
    }
    m_cache[to_key(start_index,is_open,time_left)] = result;
    if (call_count++ % 1000 == 0) std::cout << "\n" << is_open.to_string() << " best:" << result << std::flush;
    return result;
  }
  CaveSystem const& m_cave_system;
  std::vector<int> m_flowrate{};
};

using Model = CaveSystem;

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
    std::string line{};
    Connections connections{};
    while (std::getline(in,line)) {
      connections.push_back(to_connection(line));
    }
    return Model{connections};
}

namespace jonathanpaulsson {

  // Based on https://github.com/jonathanpaulson/AdventOfCode/blob/master/2022/16.cc 
  // Thank you Jonathan! I needed guidance to get all the quirks right for this problem!
  using namespace std;
  using ll = int64_t;
  using pll = pair<ll,ll>;

  vector<ll> R;
  vector<vector<ll>> E;

  ll best = 0;
  vector<ll> DP;
  ll f(ll from, ll p1, ll U, ll time, ll other_players) {
    if(time == 0) {
      return other_players>0 ? f(-1,0,U,26,other_players-1) : 0LL;
    }

    auto key = U*R.size()*31*2 + p1*31*2 + time*2 + other_players;
    if(DP[key]>=0) {
      return DP[key];
    }

    ll ans = 0;
    bool no_p1 = ((U & (1LL<<p1)) == 0);
    if(no_p1 && R[p1]>0) {
      ll newU = U | (1LL<<p1);
      assert(newU > U);
      ans = max(ans, (time-1)*R[p1] + f(p1,p1, newU, time-1, other_players));
    }
    for(auto& y : E[p1]) {
      ans = max(ans, f(p1,y, U, time-1, other_players));
    }
    DP[key] = ans;
    /*if(DP.size() % 100000 == 0) {
      //cerr << DP.size() << " best=" << best << endl;
    }*/
    return ans;
  }

  void main(std::istream& cin) {
    map<string, pair<ll, vector<string>>> INPUT;
    while(!cin.eof()) {
      string S;
      getline(cin, S);
      std::istringstream iss(S);
      std::string word;

      ll idx = 0;
      string id;
      ll rate = 0;
      vector<string> NBR;
      while (std::getline(iss, word, ' ')) {
        if(idx == 1) {
          id = word;
        } else if(idx == 4) {
          rate = stoll(word.substr(5, word.size()-6));
        } else if(idx >= 9) {
          if(word[word.size()-1]==',') {
            word = word.substr(0, word.size()-1);
          }
          NBR.push_back(word);
        }
        idx++;
      }
      INPUT[id] = make_pair(rate, NBR);
    }

    ll n = INPUT.size();
    map<string, int> INDEX_OF;
    vector<string> ORDER;
    ll nonzero = 0;
    // Convenient to have the start position have index 0
    for(auto& p : INPUT) {
      if(p.first == "AA") {
        INDEX_OF[p.first] = ORDER.size();
        ORDER.push_back(p.first);
        nonzero++;
      }
    }
    // put valves with non-zero flow rate first
    for(auto& p : INPUT) {
      if(p.second.first > 0) {
        INDEX_OF[p.first] = ORDER.size();
        ORDER.push_back(p.first);
        nonzero++;
      }
    }
    for(auto& p : INPUT) {
      if(INDEX_OF.count(p.first)==0) {
        INDEX_OF[p.first] = ORDER.size();
        ORDER.push_back(p.first);
      }
    }

    R = vector<ll>(n, 0);
    for(ll i=0; i<n; i++) {
      R[i] = INPUT[ORDER[i]].first;
    }
    E = vector<vector<ll>>(n, vector<ll>{});
    for(ll i=0; i<n; i++) {
      for(auto& y : INPUT[ORDER[i]].second) {
        E[i].push_back(INDEX_OF[y]);
      }
    }

    DP = vector<ll>((1L<<nonzero) * n * 31 * 2, -1);
    //cerr << "DP size=" << DP.size() << endl;
    ll p1 = f(-1,0,0,30,false);
    ll p2 = f(-1,0,0,26,true);
    cout << p1 << endl;
    cout << p2 << endl;
  }  
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      if (true) {
        auto data_model = parse(in);
        std::cout << "\n" << data_model;
        MaxFlow max_flow{data_model};
        int time_left=30;
        result = max_flow(time_left);
      }
      else {
        jonathanpaulsson::main(in);
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
