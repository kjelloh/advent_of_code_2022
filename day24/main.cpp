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
#include <chrono>
#include <cassert>
#include <bitset>
#include <cmath> // pow
#include <functional> // E.g., std::reference_wrapper 

extern char const* pTest0;
extern char const* pTest;
extern char const* pData;

namespace test {bool does_comply();}

using Integer = int;
using Result = Integer;
using Answers = std::vector<std::pair<std::string,Result>>;

using Vector = std::array<int,2>;
Vector operator+(const Vector& v1,const Vector& v2) {return Vector{v1[0]+v2[0],v1[1]+v2[1]};}
Vector operator-(const Vector& v) {return Vector{-v[0],-v[1]};}
std::ostream& operator<<(std::ostream& os,Vector const& v) {
  os << "[";
  for (int n=0;n<v.size();++n) {
    if (n>0) os << ",";
    os << v[n];
  }
  os << "]";
  return os;
}

using Obstacle = std::pair<Vector,char>;
using Obstacles = std::set<Obstacle>;
using Occupied = std::set<Vector>;
Vector to_top_left(Vector const& v1,Vector const& v2) {
  return {std::min(v1[0],v2[0]),std::min(v1[1],v2[1])};
}
Vector to_bottom_right(Vector const& v1,Vector const& v2) {
  return {std::max(v1[0],v2[0]),std::max(v1[1],v2[1])};
}
struct Valley {
  Valley(Obstacles obstacles_0)  {
    for (auto const& [pos,ch] : obstacles_0) {
      top_left = to_top_left(top_left,pos); 
      bottom_right = to_bottom_right(bottom_right,pos);
      std::cout << "\n" << pos << " " << ch;
    }
    width_cycle = bottom_right[1]-top_left[1]-1; // Example col 6-0-1 = 5 (for 7 wide valley)
    height_cycle = bottom_right[0]-top_left[0]-1; // Example row 6-0-1 = 5 (for 7 high valley)
    entrance = top_left + Vector{0,1};
    exit = bottom_right + Vector{0,-1};
    auto obstacles_t = obstacles_0;
    obstacles_t.insert(Obstacle{entrance + Vector{-1,0},'#'}); // Block behind entrance
    obstacles_t.insert(Obstacle{exit + Vector{1,0},'#'}); // Block beyond exit
    top_left = top_left + Vector{-1,0}; // Make frame Cover obstacle behind entrance
    bottom_right = bottom_right + Vector{1,0}; // make frame cover obstacle beyond exit
    do {
      std::cout << "\nobstacles[" << obstacles.size() << "]";
      obstacles.push_back({obstacles_t});
      bad.push_back({});
      // Where are all the obstacles at t+1?
      obstacles_t.clear();
      for (auto const& [pos,ch] : obstacles.back()) {
        Vector next = pos;
        bad.back().insert(pos);
        switch (ch) {
          case '#': break; // leave where it is
          case '>': next = Vector{pos[0],((pos[1])%width_cycle)+1};break; // col 1,2,3,4,5 -> next (1,2,3,4,5)%5+1 ->next col 2,3,4,5,1
          case '<': next = Vector{pos[0],((pos[1]+(width_cycle-2))%width_cycle)+1};break; // col 5,4,3,2,1 -> next (8,7,6,5,4)%5+1 ->col 4,3,2,1,5
          case 'v': next = Vector{((pos[0])%height_cycle)+1,pos[1]};break; // row 1,2,3,4,5 -> next (1,2,3,4,5)%5+1 -> next row 2,3,4,5,1
          case '^': next = Vector{((pos[0]+(width_cycle-2))%width_cycle)+1,pos[1]};break; // row 5,4,3,2,1 -> next (8,7,6,5,4)%5+1 -> next 4,3,2,1,5
          default: std::cerr << "\nERROR: Unknown char " << ch;
        }
        obstacles_t.insert(Obstacle{next,ch}); // update
        // std::cout << "\n" << pos << " " << pos[1]%5 << " " << next << "=" << ch;
      }
      // std::cout << "\n" << obstacles.size() << " " << (obstacles_t == obstacles[0]);
    } while ((obstacles.size()==1) or ((obstacles.size()>1) and (obstacles_t != obstacles[0])));
    std::cout << "\n" << obstacles.size() << " " << bad.size();
    assert(obstacles.size()==bad.size());
  }
  Vector top_left{};
  Vector bottom_right{};
  Vector entrance{};
  Vector exit{};
  int width_cycle{};
  int height_cycle{};
  std::vector<Obstacles> obstacles{}; // obstacles[t] is set of obstacles at time t
  std::vector<Occupied> bad{}; // bad[t] is all positions at time t that are occupied
};

using Model = Valley;

std::ostream& operator<<(std::ostream& os,Model const& model) {
  for (int n=0;n<model.obstacles.size();++n) {
    std::cout << "\n" << n << " " << model.obstacles.size();
    {
      auto map = std::vector<std::string>(model.bottom_right[0]-model.top_left[0]+1,std::string(model.bottom_right[1]-model.top_left[1]+1,' '));
      for (auto const& [pos,ch] : model.obstacles[n]) {
        map[pos[0]-model.top_left[0]][pos[1]-model.top_left[1]] = ch;
      }
      for (int row=0;row<map.size();++row) {
        os << "\n" << map[row];
      }
    }
    {
      auto map = std::vector<std::string>(model.bottom_right[0]-model.top_left[0]+1,std::string(model.bottom_right[1]-model.top_left[1]+1,' '));
      for (auto const& pos : model.bad[n]) {
        map[pos[0]-model.top_left[0]][pos[1]-model.top_left[1]] = '+';
      }
      for (int row=0;row<map.size();++row) {
        os << "\n" << map[row];
      }
    }    
  }
  return os;
}

Model parse(auto& in) {
  std::cout << "parse";
  std::string line{};
  Obstacles obstacles{};
  int row{};
  while (std::getline(in,line)) {
    std::cout << "\n";
    for (int col=0;col<line.size();++col) {
      if (line[col]!='.') {
        obstacles.insert(Obstacle{Vector{row,col},line[col]});
      }
      std::cout << line[col];
    }
    ++row;
  }
  return Model{obstacles};
}

struct DFS {
  using State = std::pair<Vector,int>; // row, col, t
  DFS(Valley const& valley) : m_valley{valley} {}
  bool not_bad(State const& state) {
    auto const& [pos,t] = state;
    bool bad = m_seen.contains({pos,t%m_valley.bad.size()}) or m_valley.bad[t%m_valley.bad.size()].contains(pos);
    if (!bad) std::cout << "\nnot bad: " << pos << " " << t << " " << m_seen.contains({pos,t%m_valley.bad.size()}) << " " << m_valley.bad[t%m_valley.bad.size()].contains(pos);
    return !bad;
  }
  // Earliest arrival at "to" when starting at "start" from "from"
  Result earliest_arrival(Vector const& from,Vector const& to,int start) {
    Result result{std::numeric_limits<Result>{}.max()};
    std::vector<State> q{};
    q.push_back({from,start});
    int call_count{-1};
    int hit_count{};
    while (q.size()>0) {
      auto state = q.back();
      q.pop_back();
      auto const& [pos,t] = state;
      if (pos==to) {
        result = std::min(result,t);
        ++hit_count;
        continue;
      }
      if (t>result) continue;
      if (++call_count%1==0) std::cout << "\n" << call_count << " " << q.size() << " " << m_seen.size() << " " << pos << " " << hit_count << " " << result;
      if (auto adj = State{pos+Vector{0,-1},t+1}; not_bad(adj)) q.push_back(adj);
      if (auto adj = State{pos+Vector{-1,0},t+1}; not_bad(adj)) q.push_back(adj);
      if (auto adj = State{pos+Vector{0,0},t+1}; not_bad(adj)) q.push_back(adj);
      if (auto adj = State{pos+Vector{1,0},t+1}; not_bad(adj)) q.push_back(adj);
      if (auto adj = State{pos+Vector{0,1},t+1}; not_bad(adj)) q.push_back(adj);
      m_seen.insert({pos,t%m_valley.bad.size()});
    }
    return 0;
  }
  Valley const& m_valley;
  std::set<State> m_seen{};
};

namespace part1 {
  Result solve_for(char const* pData) {
      std::cout << "\n\nPART 1";
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\n" << data_model;
      DFS dfs{data_model};
      auto t0=0;
      result = dfs.earliest_arrival(data_model.entrance,data_model.exit,t0);
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      std::cout << "\n\nPART 2";
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      DFS dfs{data_model};
      auto t0=0;
      auto t1=dfs.earliest_arrival(data_model.entrance,data_model.exit,t0);
      auto t2=dfs.earliest_arrival(data_model.exit,data_model.entrance,t1);
      auto t3=dfs.earliest_arrival(data_model.entrance,data_model.exit,t2);
      result = t3-t0;
      return result;
  }
}

namespace test {
  bool does_comply() {
    return false;
  }
}

int main(int argc, char *argv[])
{
  if (argc>1 and std::string{argv[1]}=="test") {
    if (test::does_comply()) std::cout << "\nTest: You have arrived at your destination :)";
    else std::cout << "\nTest: Not there yet...";
  }
  else {
    Answers answers{};
    std::chrono::time_point<std::chrono::system_clock> start_time{};
    std::vector<std::chrono::time_point<std::chrono::system_clock>> exec_times{};
    exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 1 Test",part1::solve_for(pTest0)});
    // exec_times.push_back(std::chrono::system_clock::now());
    answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 1     ",part1::solve_for(pData)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 2     ",part2::solve_for(pData)});
    exec_times.push_back(std::chrono::system_clock::now());
    std::cout << "\n\nANSWERS";
    for (int i=0;i<answers.size();++i) {
      std::cout << "\nduration:" << std::chrono::duration_cast<std::chrono::milliseconds>(exec_times[i+1] - exec_times[i]).count() << "ms"; 
      std::cout << " answer[" << answers[i].first << "] " << answers[i].second;
    }
    std::cout << "\n";
  }
  return 0;
}

char const* pTest0 = R"(#.#####
#.....#
#>....#
#.....#
#...v.#
#.....#
#####.#)";

char const* pTest = R"(#.######
#>>.<^<#
#.<..<<#
#>v.><>#
#<^v^^>#
######.#)";
char const* pData = R"(#.####################################################################################################
#>^v^v>><.>>><>^v<>^v>v<>^<<>>v>>.>><v>^<>^.<>v.>v^>vvvv<<<<>>^v^^^>><><.^^^.<<<>><^v^vv^v<.v.v>>.vv<#
#>^><>^v^<>.<v><.<.v<v>vv<^<vvvv.^^...<<<^^vv<<v<>>>>v^<>>^^^><.>>>>v<<>^.>.^^<v<.><<.<><.>>>^^^>>.<>#
#<.vvv^^v>^<<^^^>.^<..v.v<<^>vv<>>>v^>vv<><v<<vv<v^>v<^^^^v>.<.vv><v>v<v<^v^>>^<^^<>^>v^<^.^^v><>.><>#
#>..<^^<>>v>><>vv^<v.^v.>^<.v^<v><<^<><v^v<.vv^>>vvv.vvv<>v>><<.>^><v^<<<v<<>><v><^v.v.<>^>v><v<^<<.>#
#>^^<v>v><^<^>^<vvv^.>^^><^>><^^.vv>v<>v<>^>^v><v>>.>>^<v><^v^v^>v^<.<v^^^<>vvv^^<.<v<>>>.^>><<.vvv.>#
#><v>v^>^<>v^>>^^<>.><^v<^vvv>vv<.><>v>v<..<^>>v^v<<<<v>v^.<<<^^^^.^v^<<>^vvv>^><.^v<^<>vv><<^<>^<^<<#
#.^vvv<vv>^^v<>v<<v^^>.<<<v><<vv<v>>^<vv<v>><<^>^vv<<>^<<>^v.>>.<>>^<^<v<^^^<.vv^vv><^><v^^v>v^v>^vv.#
#<v.>^<^><^..>^.>>v>>vv<^<.<vv<<v^v<.vvv>>.^v^^<^v<^<.^vv>v>^><v<v^>^v>>>>^^^v>^^>>>v<v^vv<.^><vv<>^<#
#><>v^>^v<.<.>.<^.<>>^^>v^<v^<v>vvv<<^>vv<^^>^<^>v<.^.v<<.v.v^v>v><>..^^<.v^^<.<<<<<><>v><v<^^v<^<vv<#
#><<>v><>^^^.<.<<>^v>v^>^>>>>v>.^>.v>.^>^.v<^<>v.v>v^v^^>^v>>>.<^v^^.>>>>^<<v<.^^>>>>^><v^v>vvv^>^><<#
#><vvvv.>v^.v>>><>^>><><v^<.>.^<^v.>.^<^>>^v<.<^^v<^><..<vv^v<>vv<<vv^v.>>^.<v^>^>^<^<^>><><v<<>.><<>#
#>.<^^v^><<<.v<vv<>^<>v^vv<^^.<>>.^<vv>>^.v.>^><v^^>.<<>v<>^v<v><v^^<v.v><<>vvv.v<>v>vvv^<>v^>^v<<<><#
#.>><<vv><>vv>v..<<^^vv^>^<>^^v>v^vv>.><>^<vv^^<v><^^>><>>v<<v>^v^^><>.^.vv>v.^^>^vv<><.>>^^<^<><>v>.#
#><<v>v<><^<<^><..<^>v<<^.^^<.^^v<>>^v>.^^^<<v>v>><><.<>>^v<^.v<vv<<^>>>vv^<vv.>^^^<><>v.>.^<^v<vv^><#
#<.>vvv<v>^.vv.>.<v^.<..<>^v<>>^^vv>.>^v^>>>vv^<v><^v.v^>>>>><>v>v<>v^^>.^^v>^v><<>vv^<><>>v<^<v<<<<>#
#<v<><^v<>>.<v^><<>^>v<<.v^v.<^v.^<vv<.v.^<v><><><v^^>v<>.vv^>vv<<^^<^^^><<>^>.^v<<<^<^<^^>>vv.<v.v<.#
#<>vv^^<<^^^v^^<v^vvv^>^^.vvv^^^^v^>^^^v<^v><<^^^.<><v.vv<<^v<^>^><.^><^<>.v>^vv^v<<^<.<^.^>vv>^v>^<.#
#<.vv>><.<vv<^^^<.^v><>>>><<^^<><^<vv<<>^v..>v<^^v><^><<^^^<<>^^>.>v><^<v<><v>v^<v<v^v.vv>v^<<<v>.^>>#
#<^vv<<v^><>^^<<^.<v^>^vv.><^>v.>^vv.<^v^^<v<<v<><^<>v<<v<<><>^v^>v^^^><^>>^.<v.^<.^^.v<^v<>>.<<<v>^<#
#>>..^^^<v.v<>>^^<v^..^vv<>.<>>>^><>^<v><<.v.^v^^<^<><>..>^<>.>^^<v.^v<v<<^v>^vv.v>.<.^^<v^<<>^^<^v<.#
#>>vvv^v^v^>v<<vv.^.^<<vv^>^>v<.^>^<v^>^^<^<>^.><^v<<<v>>..v><>v^>^>><^^<>.v<vv^>>>>v<^v.v>v.>^vv^.^<#
#<^<>.^<>>^<^v>.^v.><.<v.v.^v><^>.>^<>^^>>^^^.<^<>><>^><vvv^>^.v^^v>vv<<vvv>^.^>><.<>^.^<>.>>>v>v<v^>#
#<^^vv<<^<^.<^>^<vv.>>.>><^<^^^<<>.v>v>v>v.^<<v<.<>v<<<>v<>^<v.^<<<v<<>>>>.^><^^<>>>^<><^.^.v^^v^^>..#
#..v>vv^<vv>><^>^^^<v>><v^^><>.^^><^<><>^>><v>^v<><v.^<>><.>^>v^>^v<><v^v>><><^>v>.v^v>v<><.^^.>.v.^<#
#.<v>v<^v.<<<vv<>^v^vv<^v^>^^v^<>^>>vv<^^v^^^^<.^v<>^<<^<v^v<><v>v^<<>>.vvv.>>^v>>>vv<<><^v<v^v<<.>>>#
#<<vv.v^.>..v><v<><vv<<<<.>vv>v>>^v^v<v^<v<>v<<v^v^v^<<v^>v<^^^>v<^^^^^>>><<v<>v<>.>v^v<><.<<v<>^.^^.#
#>>>>>^^.><^<>^<>v>.<^vv^^<>>^v><^<^>vv^<v><.>^v>.v<v<>>>>^.^>^v^^^<vvv^.<<..^v<.>^><<>><<^<.<<v<>^v<#
#<.v.^>.^<.v<^^<.<^<<^.^<>><^<<^<><.<>.><<<><^v<v>^^v<<v>><^<>>^>^^>vv>^<.^<v><>.><v^v<vv^<^^<v>>>^v<#
#<>v^>^^v>>^<^>v>>>.>^.><>..>^v>><>v<>^>.^v<<<<^>v>v><v^<^>^^>^<<>^>vv><<<v>v^^v<vv<^<^>vv^<.v^<^v>^>#
#><<vvv.<<<vv<vv^^^v^v<^>vv^<>^^v<>v<^^^^><>^^>^<<<>v.vv^<><<><.^<<<^>>^vv<.v^^>vv><<<>v.<><v^><^^^v.#
#<><v<v><^<^^vvv^>><><<<.<<vv<^<>.^<>>^v^>>vvv>>v..><<^^.v<><^^><^>>^<<v<<^v^>v>vvvvv...^<v^<^<v<.^<.#
#>><<<v^^v^>^<<.>>vvv^v<^<>.^v^<^v<^^>>v<.>v<v..^^v.^>.v>^>>v^<<.v<^<..>.v<<..^^^v>v<.^<.>v^v^<v^v>^>#
#<^>v<>^<<v>>vv>.><^v<^.><<^v>v<^<<<v^.^v.><>v><>^v><<<v.>vv<>vv>><>>^><><v>>v^.^v<<v^vvv<.<<>.v^^v<>#
#<^<^><<>v..>v^>v>.v<<<^<<<v^v<<>^<.<.<v.v^<><v^<<>^><vv.v>>vv^.v^.^^>^^<<^^>.vv<.>.^^<>.<<v><vv<<>>>#
#>v>>vv<.v<v.<^.^^><><^^v>^>>>>^>vv><<v>vv^.>>v^^vv^v^.>^v<>>>^^^>..vv^.^><..>>v<>^>^^<<.v<>v^^^v>^v>#
####################################################################################################.#)";
