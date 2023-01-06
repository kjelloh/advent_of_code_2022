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

struct Vector {
  int row;
  int col;
};

struct Blizzard {
  char dir;
  Vector pos;
};
using Blizzards = std::vector<Blizzard>;
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
  std::cout << "[row:" << v.row << ",col:" << v.col << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os,Blizzard const& blizzard) {
  std::cout << blizzard.dir << " at " << blizzard.pos;
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
  std::cout << "\nt:" << t << " " << blizzard_at_0;
  int row{blizzard_at_0.pos.row};
  int col{blizzard_at_0.pos.col};
  auto height = (bottom_right.row-top_left.row)+1;
  auto width = (bottom_right.col - top_left.col)+1;
  std::cout << "\n\theight:" << height << " width:" << width;
  switch (blizzard_at_0.dir) {
    case '^': {
      row = ((row-1 - t) % (height-2)) + 1;
      if (row <= 0) row += (height-2);
      std::cout << "\n\t^ row:" << row;
     } break; 
    case '>': {
      col = ((col-1 + t) % (width-2)) + 1;
      std::cout << "\n\t> col:" << row;
     } break;
    case 'v': { 
      row = ((row-1 + t) % (height-2)) + 1;
      std::cout << "\n\tv row:" << row;
    } break;
    case '<': {
      col = ((col-1 - t) % (width-2)) + 1;
      if (col <= 0) col += (width-2);
      std::cout << "\n\t< col:" << row;
     } break;
  }
  result.pos = Vector{.row=row,.col=col};
  std::cout << "\nt:" << t << " " << blizzard_at_0.dir << " :" << blizzard_at_0 << " to " << result;
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

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      if (true) {
        Valley::test(pData);
      }
      else {
        std::stringstream in{ pData };
        auto data_model = parse(in);        
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
  if (false) {
    // Test modulo calculus
    for (int row=-10;row<10; ++row) {
      std::cout << "\nrow:" << row << " row%4:" << row%4;
    }
    return 0;
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
