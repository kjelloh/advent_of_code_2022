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

bool operator<(const Vector& v1,const Vector& v2) {return (v1[0]==v2[0])?(v1[1]<v2[1]):v1[0]<v2[0];}
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
      // Where are all the obstacles at t+1?
      obstacles_t.clear();
      for (auto const& [pos,ch] : obstacles.back()) {
        Vector next = pos;
        switch (ch) {
          case '#': break; // leave where it is
          case '>': next = Vector{pos[0],((pos[1])%width_cycle)+1};break; // col 1,2,3,4,5 -> next (1,2,3,4,5)%5+1 ->next col 2,3,4,5,1
          case '<': next = Vector{pos[0],((pos[1]+(width_cycle-2))%width_cycle)+1};break; // col 5,4,3,2,1 -> next (8,7,6,5,4)%5+1 ->col 4,3,2,1,5
          case 'v': next = Vector{((pos[0])%height_cycle)+1,pos[1]};break; // row 1,2,3,4,5 -> next (1,2,3,4,5)%5+1 -> next row 2,3,4,5,1
          case '^': next = Vector{((pos[0]+(width_cycle-2))%width_cycle)+1,pos[1]};break; // row 5,4,3,2,1 -> next (8,7,6,5,4)%5+1 -> next 4,3,2,1,5
          default: std::cerr << "\nERROR: Unknown char " << ch;
        }
        obstacles_t.insert(Obstacle{next,ch}); // update
        std::cout << "\n" << pos << " " << pos[1]%5 << " " << next << "=" << ch;
      }
      std::cout << "\n" << obstacles.size() << " " << (obstacles_t == obstacles[0]);
    } while ((obstacles.size()==1) or ((obstacles.size()>1) and (obstacles_t != obstacles[0])));
  }
  Vector top_left{};
  Vector bottom_right{};
  Vector entrance{};
  Vector exit{};
  int width_cycle{};
  int height_cycle{};
  std::vector<Obstacles> obstacles{}; // obstacles[t] is set of obstacles at time t
};

using Model = Valley;

std::ostream& operator<<(std::ostream& os,Model) {

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

namespace part1 {
  Result solve_for(char const* pData) {
      std::cout << "\n\nPART 1";
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      std::cout << "\n\nPART 2";
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
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

char const* pTest = R"(#.#####
#.....#
#>....#
#.....#
#...v.#
#.....#
#####.#)";
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
