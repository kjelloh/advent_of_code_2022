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
#include <unordered_set>
#include <ranges> // E.g., std::subrange, std::view 
#include <utility> // E.g., std::pair,..
#include <algorithm> // E.g., std::find, std::all_of,...
#include <numeric> // E.g., std::accumulate
#include <limits> // E.g., std::numeric_limits
#include <cassert>

extern char const* pTest;
extern char const* pData;

using Result = long int;
using Answers = std::vector<std::pair<std::string,Result>>;

struct Vector {
  int row;
  int col;
  bool operator==(Vector const& other) const {return row==other.row and col==other.col;}
  Vector operator+(Vector const& other) const {return {.row=row+other.row,.col=col+other.col};}
  bool operator<(Vector const& other) const {return (row==other.row)?col<other.col:row<other.row;}
};

extern const std::set<std::pair<Vector,int>> EXPECTED;

const std::vector<Vector> DIRS{
   {-1,0}
  ,{0,-1}
  ,{0,1}
  ,{1,0}
};

struct Blizzard {
  char dir;
  Vector pos;
};
using Blizzards = std::vector<Blizzard>;
struct BlizzardsHash {
  std::size_t operator()(Blizzards const& blizzards) const noexcept {
    std::size_t result{};
    for (auto const& blizzard : blizzards) {
      result ^= (std::hash<char>{}(blizzard.dir)) << 1;
      result ^= (std::hash<int>{}(blizzard.pos.row)) << 1;
      result ^= (std::hash<int>{}(blizzard.pos.col)) << 1;
    }
    return result;
  }
};

using Row = std::string;
using Map = std::vector<Row>;

Map to_map(std::string const& s) {
  std::istringstream in{s};
  Map result{};
  std::string line{};
  while (std::getline(in,line)) {
    result.push_back({});
    for (int col=0;col<line.size();++col) {
      auto ch = line[col];
      result.back().push_back(ch);
    }
  }
  return result;  
}

std::ostream& operator<<(std::ostream& os,Map const& map) {
  for (int row=0;row<map.size();++row) {
    if (row>0) os << "\n";
    os << map[row];
  }
  return os;
}

std::ostream& operator<<(std::ostream& os,Vector const& v) {
  os << "[row:" << v.row << ",col:" << v.col << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os,Blizzard const& blizzard) {
  os << blizzard.dir << " at " << blizzard.pos;
  return os;
}

std::ostream& operator<<(std::ostream& os,Blizzards const& blizzards) {
  for (int index=0;index<blizzards.size();++index) {
    if (index>0) os << "\n";
    os << blizzards[index];
  }
  return os;
}

// forward
class Valley;
Valley parse(auto& in);
Valley valley_at_t(Valley const& valley_at_0,int t);
Map map_at_t(Valley const& valley_at_0,int t);


class Valley {
public:
  friend std::ostream& operator<<(std::ostream& os,Valley const& valley);
  Valley& push_back(Row const& line) {
    ++m_bottom_right.row;
    for (int col=0;col<line.size();++col) {
      auto ch = line[col];
      for (auto bch : {'^','>','v','<'}) {
        if (bch==ch) {
          m_blizzards.push_back(Blizzard{ch,Vector{.row=m_bottom_right.row,.col=col}});
          break;
        }
      }
    }
    m_bottom_right.col = line.size()-1; // col 0..size-1
    return *this;
  }
  Vector const& top_left() const {
    return m_top_left;
  }
  Vector const&  bottom_right() const {
    return m_bottom_right;
  }
  Blizzards& blizzards() {
    return m_blizzards;
  }
  Blizzards const& blizzards() const {
    return m_blizzards;
  }
  bool has_blizzard_at_pos(Vector const& pos) {
    bool Result{};
    auto iter = std::find_if(m_blizzards.begin(),m_blizzards.end(),[&pos](Blizzard const& blizzard){
      return blizzard.pos==pos;
    });
    return (iter!=m_blizzards.end());
  }
  bool is_free(Vector const& pos) {
    bool result{};
    if (pos.row<m_top_left.row) result = false;
    else if (pos.row>m_bottom_right.row) result = false;
    else if (pos.row==m_top_left.row and pos.col!=1) result = false;
    else if (pos.row==m_bottom_right.row and pos.col!=m_bottom_right.col-1) result = false;
    else if (pos.col<=m_top_left.col or pos.col>=m_bottom_right.col) result = false;
    else result = (has_blizzard_at_pos(pos) == false);
    // std::cout << "\nis_free(" << pos << ") = " << result;
    return result;
  }

  Map to_map() const {
    int min_col{top_left().col};
    int max_col{bottom_right().col};
    int min_row{top_left().row};
    int max_row{bottom_right().row};
    auto result = Map(max_row+1,Row(max_col+1,'.'));
    for (int row=min_row;row<=max_row;++row) {
      for (int col=min_col;col<=max_col;++col) {
        if (row==0 and col!=1) result[row][col] = '#';
        else if (col==0 or col==max_col) result[row][col] = '#';
        else if (row==max_row and col!=max_col-1) result[row][col] = '#';
      }
    }
    for (auto const& blizzard : blizzards()) {
      auto& map_ch_ref = result[blizzard.pos.row][blizzard.pos.col];
      if (map_ch_ref == '.') map_ch_ref = blizzard.dir;
      else if (map_ch_ref == '^' or map_ch_ref == '>' or map_ch_ref == 'v' or map_ch_ref == '<') {
        map_ch_ref = '2';
      }
      else if (map_ch_ref >= '2' and map_ch_ref <= '8') {
        map_ch_ref = static_cast<char>(map_ch_ref + 1);
      }
      else map_ch_ref = 'X';
    }
    return result;
  }

  static void test(const char* pData) {
    std::istringstream in{pData};
    auto valley_at_0 = parse(in);
    auto map_at_0 = valley_at_0.to_map();
    std::cout << "\n" << map_at_0;
    if (map_at_0 == ::to_map(pData)) {
      std::cout << "\nSAME";
    }
    std::cout << "\n" << valley_at_0.blizzards();
    for (int t=0;t<10;++t) {
      auto map_t = map_at_t(valley_at_0,t);
      std::cout << "\nt:" << t << "\n" << map_t;
    }
    int t = 18;
    auto map_t = map_at_t(valley_at_0,t);
    std::cout << "\nt:" << t;
    std::cout << "\n" << map_t;
    if (map_t == ::to_map(R"(#.######
#>2.<.<#
#.2v^2<#
#>..>2>#
#<....>#
######.#)"))  {
      std::cout << "\nSAME";
    }
    else {
      std::cout << "\nDEVIATES";
    }

  }
private:
  Blizzards m_blizzards{};
  Vector m_top_left{};
  Vector m_bottom_right{.row=-1,.col=-1}; // empty
};

std::ostream& operator<<(std::ostream& os,Valley const& valley) {
  os << valley.to_map();
  return os;
}

using Model = Valley;

Model parse(auto& in) {
    Model result{};
    std::string line{};
    while (std::getline(in,line)) {
      result.push_back(line);
    }
    return result;
}

Blizzard blizzard_at_t(Blizzard const& blizzard_at_0,int t,Vector const& top_left,Vector const& bottom_right) {
  Blizzard result{blizzard_at_0};
  // std::cout << "\nt:" << t << " " << blizzard_at_0;
  int row{blizzard_at_0.pos.row};
  int col{blizzard_at_0.pos.col};
  auto height = (bottom_right.row-top_left.row)+1;
  auto width = (bottom_right.col - top_left.col)+1;
  // std::cout << "\n\theight:" << height << " width:" << width;
  switch (blizzard_at_0.dir) {
    case '^': {
      row = ((row-1 - t) % (height-2)) + 1;
      if (row <= 0) row += (height-2);
      // std::cout << "\n\t^ row:" << row;
     } break; 
    case '>': {
      col = ((col-1 + t) % (width-2)) + 1;
      // std::cout << "\n\t> col:" << row;
     } break;
    case 'v': { 
      row = ((row-1 + t) % (height-2)) + 1;
      // std::cout << "\n\tv row:" << row;
    } break;
    case '<': {
      col = ((col-1 - t) % (width-2)) + 1;
      if (col <= 0) col += (width-2);
      // std::cout << "\n\t< col:" << row;
     } break;
  }
  result.pos = Vector{.row=row,.col=col};
  // std::cout << "\nt:" << t << " " << blizzard_at_0.dir << " :" << blizzard_at_0 << " to " << result;
  return result;
}

Valley valley_at_t(Valley const& valley_at_0,int t) {
  Valley result{valley_at_0};
  for (auto& blizzard : result.blizzards()) {
    blizzard = blizzard_at_t(blizzard,t,valley_at_0.top_left(),valley_at_0.bottom_right());
  }
  return result;
}

Map map_at_t(Valley const& valley_at_0,int t) {
  auto valley = valley_at_t(valley_at_0,t);
  return valley.to_map();
}

const int REVISIT_LIMIT{10};
const int TIME_LIMIT{468};

class DFS {
public:
  class State {
  public:
    State(int t,Valley const& valley,Vector const& pos,int run_count) 
      : m_t{t}
       ,m_valley{valley}
       ,m_pos{pos}
       ,m_run_count{run_count} {}
    Result t() const {return m_t;}
    Valley const& valley() const {return m_valley;}
    Vector const& pos() const {return m_pos;}
    int run_count() const {return m_run_count;}
    bool operator<(State const& other) const {
      auto height = (m_valley.bottom_right().row - m_valley.top_left().row)+1;
      auto width = (m_valley.bottom_right().col - m_valley.top_left().col)+1;
      auto cycle = (height-1)*(width-2);
      if (m_run_count==other.m_run_count) {
        if ((m_t % cycle) == (other.m_t % cycle)) return (m_pos < other.m_pos);
        else return ((m_t % cycle) < (other.m_t % cycle));
      }
      else return (m_run_count < other.m_run_count);
    }
  private:
    Result m_t;
    Valley m_valley;
    Vector m_pos;
    int m_run_count;
  };
  struct StateHash
  {
      std::size_t operator()(State const& state) const noexcept
      {
          std::size_t result = std::hash<int>{}(state.t());
          result ^= (BlizzardsHash{}(state.valley().blizzards()) << 1);
          result ^= (std::hash<int>{}(state.pos().row) << 1);
          result ^= (std::hash<int>{}(state.pos().col) << 1);
          result ^= (std::hash<int>{}(state.run_count()) << 1);
          return result;
      }
  };  
  DFS(Valley const& valley_at_0,bool rerun = false) 
    : m_initial_state(0,valley_at_0,Vector{.row=0,.col=1},rerun?3:1)
     ,m_end{.row=valley_at_0.bottom_right().row,.col = valley_at_0.bottom_right().col-1} {
    m_best = dfs(m_initial_state);
  }
  Result best() {return m_best;}
private:
  State m_initial_state;
  Vector m_end;
  std::deque<State> m_Q{};
  // std::map<Vector,std::vector<State>> m_revisited{};
  std::set<State> m_seen{};
  Result m_best{std::numeric_limits<Result>::max()};
  std::vector<State> adj(State const& state_t) {
    if (state_t.pos().row==0 or state_t.pos().row==1) std::cout << "\nadj(" << state_t.t() << "," << state_t.pos() << ")";
    std::vector<State> result{};
    auto adj_valley = valley_at_t(m_initial_state.valley(),state_t.t()+1);
    for (auto const& delta : DIRS) {
      auto adj_pos = state_t.pos() + delta;
      if (adj_pos.row==0 or adj_pos.row==1) std::cout << " adj_pos:" << adj_pos;
      if (adj_valley.is_free(adj_pos)) {
        if (adj_pos.row==0 or adj_pos.row==1) std::cout << " free adj_pos:" << adj_pos;
        result.push_back(State(state_t.t()+1,adj_valley,adj_pos,state_t.run_count()));
      }
    }
    if (adj_valley.is_free(state_t.pos())) {
      // wait at t+1 without moving
      if (state_t.pos().row==0 or state_t.pos().row==1) std::cout << " wait in pos:" << state_t.pos();
      result.push_back(State(state_t.t()+1,adj_valley,state_t.pos(),state_t.run_count()));
    }
    // std::cout << "\nadj:" << result.size();
    return result;
  }
  Result dfs(State const& initial_state) {
    Result result{std::numeric_limits<Result>::max()-1};
    m_Q.push_back(initial_state);
    int hit_count{};
    Result call_count{};
    State state=initial_state;
    State best_end_state=initial_state;
    Result acc_result{};
    do {
      result = std::numeric_limits<Result>::max()-1;
      hit_count=0;
      while (m_Q.size()>0) {
        if (call_count++ % 1000 == 0) std::cout << "\n" << call_count << " " << state.run_count() << " " << state.pos() << " " << m_Q.size() << " hit_count:" << hit_count << std::flush;
        // std::cout << "\nm_Q:" << m_Q.size();
        state = m_Q.back();
        m_Q.pop_back();
        // std::cout << "\nt:" << state.t() << " pos:" << state.pos();
        // std::cout << "\n" << state.valley();
        // m_revisited[state.pos()].push_back(state);
        if (state.pos()==m_end) {
          if (state.t()<result) {
            best_end_state = state;
            result = state.t();
            std::cout << "\nCANDIDATE:" << result;
            call_count=0;
          }
          ++hit_count;
          continue;
        }
        m_seen.insert(state);
        for (auto const& adj_state : adj(state)) {
          if (adj_state.t()>=result) {
            // std::cout << "\nBEST:" << result;
            break;
          }
          if (adj_state.t()>TIME_LIMIT) {
            // std::cout << "\nTIME EXHAUST at t:" << adj_state.t();
            break;
          }
          if (m_seen.contains(adj_state)) {
            // std::cout << "\nseen t:" << adj_state.t() << " " << adj_state.pos();
            continue;
          }
          // if (m_revisited[adj_state.pos()].size() > REVISIT_LIMIT) {
          //     // std::cout << "\nREVISIT EXHAUST pos:" << adj_state.pos() << " count:" << m_revisited[adj_state.pos()].size();
          //     continue;
          // }
          // std::cout << "\nnew t:" << adj_state.t() << " " << adj_state.pos();
          m_Q.push_back(adj_state);
        }
      } 
      // One run done
      assert(hit_count>0);
      std::cout << "\nBEST:" << result;
      acc_result += result;
      std::cout << "\nACC:" << acc_result;
      if (m_end==Vector{.row=state.valley().bottom_right().row,.col = state.valley().bottom_right().col-1}) {
        m_end=Vector{0,1};
        std::cout << "\nTURNING BACK TO BEGINNING";
      }
      else if (m_end==Vector{0,1}) {
        m_end=Vector{.row=state.valley().bottom_right().row,.col = state.valley().bottom_right().col-1};
        std::cout << "\nTURNING BACK TO END";
      }
      state = State(best_end_state.t(),best_end_state.valley(),best_end_state.pos(),best_end_state.run_count()-1);
      m_Q.push_back(state);
      call_count=0;
    } while (state.run_count()>0);
    return acc_result;
  }
};

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      if (false) {
        Valley::test(pData);
      }
      else {
        std::stringstream in{ pData };
        auto data_model = parse(in);
        DFS dfs{data_model};
        result = dfs.best();
        std::cout << "\nvalley bottom_right:" << data_model.bottom_right();
      }
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      DFS dfs{data_model,true};
      result = dfs.best();
      std::cout << "\nvalley bottom_right:" << data_model.bottom_right();
      return result;
  }
}

int main(int argc, char *argv[])
{
  if (false) {
    // Test modulo calculus
    for (int row=-10;row<10; ++row) {
      std::cout << "\nrow:" << row << " row%4:" << row%4;
    }
    return 0;
  }
  std::chrono::time_point<std::chrono::system_clock> start_time{};
  Answers answers{};
  std::vector<std::chrono::time_point<std::chrono::system_clock>> exec_times{};
  exec_times.push_back(std::chrono::system_clock::now());
  // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
  // exec_times.push_back(std::chrono::system_clock::now());
  // answers.push_back({"Part 1     ",part1::solve_for(pData)});
  // exec_times.push_back(std::chrono::system_clock::now());
  answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
  // exec_times.push_back(std::chrono::system_clock::now());
  // answers.push_back({"Part 2     ",part2::solve_for(pData)});
  exec_times.push_back(std::chrono::system_clock::now());
  for (int i=0;i<answers.size();++i) {
    std::cout << "\nduration:" << std::chrono::duration_cast<std::chrono::milliseconds>(exec_times[i+1] - exec_times[i]).count() << "ms"; 
    std::cout << " answer[" << answers[i].first << "] " << answers[i].second;
  }
}

const std::set<std::pair<Vector,int>> EXPECTED{
   {{1,1},1}
   ,{{5,6},18}
};

/*
Minute 1, move down:
#.######
#E>3.<.#
#<..<<.#
#>2.22.#
#>v..^<#
######.#

Minute 2, move down:
#.######
#.2>2..#
#E^22^<#
#.>2.^>#
#.>..<.#
######.#

Minute 3, wait:
#.######
#<^<22.#
#E2<.2.#
#><2>..#
#..><..#
######.#

Minute 4, move up:
#.######
#E<..22#
#<<.<..#
#<2.>>.#
#.^22^.#
######.#

Minute 5, move right:
#.######
#2Ev.<>#
#<.<..<#
#.^>^22#
#.2..2.#
######.#

Minute 6, move right:
#.######
#>2E<.<#
#.2v^2<#
#>..>2>#
#<....>#
######.#

Minute 7, move down:
#.######
#.22^2.#
#<vE<2.#
#>>v<>.#
#>....<#
######.#

Minute 8, move left:
#.######
#.<>2^.#
#.E<<.<#
#.22..>#
#.2v^2.#
######.#

Minute 9, move up:
#.######
#<E2>>.#
#.<<.<.#
#>2>2^.#
#.v><^.#
######.#

Minute 10, move right:
#.######
#.2E.>2#
#<2v2^.#
#<>.>2.#
#..<>..#
######.#

Minute 11, wait:
#.######
#2^E^2>#
#<v<.^<#
#..2.>2#
#.<..>.#
######.#

Minute 12, move down:
#.######
#>>.<^<#
#.<E.<<#
#>v.><>#
#<^v^^>#
######.#

Minute 13, move down:
#.######
#.>3.<.#
#<..<<.#
#>2E22.#
#>v..^<#
######.#

Minute 14, move right:
#.######
#.2>2..#
#.^22^<#
#.>2E^>#
#.>..<.#
######.#

Minute 15, move right:
#.######
#<^<22.#
#.2<.2.#
#><2>E.#
#..><..#
######.#

Minute 16, move right:
#.######
#.<..22#
#<<.<..#
#<2.>>E#
#.^22^.#
######.#

Minute 17, move down:
#.######
#2.v.<>#
#<.<..<#
#.^>^22#
#.2..2E#
######.#

Minute 18, move down:
#.######
#>2.<.<#
#.2v^2<#
#>..>2>#
#<....>#
######E# 5,6
*/

// char const* pTest = R"(#.#####
// #.....#
// #>....#
// #.....#
// #...v.#
// #.....#
// #####.#)";
// char const* pTest = R"(#.#####
// #.^..<#
// #>....#
// #.....#
// #...v.#
// #.....#
// #####.#)";
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
