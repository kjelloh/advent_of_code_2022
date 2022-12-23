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

struct Vector {
  int col;
  int row;
  Vector operator+(Vector const& other) const {
    return {.col=col+other.col,.row = row + other.row};
  }
  bool operator==(Vector const& other) const {
    return (col == other.col and row == other.row);
  }
};

struct Elf {
  Vector pos;
};

struct MapBounds {
  int west{1000};
  int east{0};
  int north{1000};
  int south{0};
};

class Map {
public:
  using Elves = std::vector<Elf>;
  void push_back(Elf const& elf) {
    m_elves.push_back(elf);
    m_bounds.west = std::min(elf.pos.col,m_bounds.west);
    m_bounds.east = std::max(elf.pos.col,m_bounds.east);
    m_bounds.north = std::min(elf.pos.row,m_bounds.north);
    m_bounds.south = std::max(elf.pos.row,m_bounds.south);
  }
  Elves const& elves() const {return m_elves;}
MapBounds const& bounds() const {return m_bounds;}
private:
  Elves m_elves;
  MapBounds m_bounds{};
};

using Model = Map;

std::ostream& operator<<(std::ostream& os,Vector const& pos) {
  std::cout << "[col:" << pos.col << ",row:" << pos.row << "]"; 
  return os;
}

std::ostream& operator<<(std::ostream& os,MapBounds const& bounds) {
  std::cout << "west:" << bounds.west << " .. east:" << bounds.east;
  std::cout << " north:" << bounds.north << " .. south:" << bounds.south;
  return os;
}

Model parse(auto& in) {
    Model result{};
    std::string line{};
    int row{0};
    while (std::getline(in,line)) {
      int col{0};
      for (char ch : line) {
        if (ch == '#') {
          Vector pos{col,row};
          Elf elf{pos};
          result.push_back(elf);
          std::cout << "\nelf at " << elf.pos;
        }
        ++col;
      }
      ++row;      
    }
    return result;
}

bool has_neighbour_elf(Elf const& elf,Map const& map) {
  bool result{false};
  Vector delta{};
  for (int i=0;i<8 and result==false;++i) {
    switch (i) {
      case 0: delta = Vector{.col=0,.row=-1}; break; // N
      case 1: delta = Vector{.col=1,.row=-1}; break; // NE
      case 2: delta = Vector{.col=1,.row=0};  break; // E
      case 3: delta = Vector{.col=1,.row=1};  break; // SE
      case 4: delta = Vector{.col=0,.row=1};  break; // S
      case 5: delta = Vector{.col=-1,.row=1}; break; // SW
      case 6: delta = Vector{.col=-1,.row=0}; break; // W
      case 7: delta = Vector{.col=-1,.row=-1}; break; // NW
    }
    auto pos_to_check = elf.pos + delta;
    result = std::any_of(map.elves().begin(),map.elves().end(),[pos_to_check](Elf const& elf){
      return (pos_to_check == elf.pos);
    });
  }
  return result;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\nbounds:" << data_model.bounds();
      auto map = data_model;
      std::vector<char> directions_to_consider{'N','S','W','E'};
      for (int i=0;i<10;++i) {
        std::cout << "\nround:" << i+1;
        for (Elf const& elf : map.elves()) {
          if (has_neighbour_elf(elf,map)) {
            std::cout << "\n\thas neighbour elf:" << elf.pos;
            for (char dir : directions_to_consider) {
              std::cout << "\n\tconsider:" << dir;
            }
          }
        }
        directions_to_consider.push_back(directions_to_consider[0]);
        directions_to_consider.erase(directions_to_consider.begin());
      }
      // The number of unoccupied positions on the bounded map are the size of the map minus the elf count
      result = (map.bounds().east - map.bounds().west + 1) * (map.bounds().south - map.bounds().north + 1) - map.elves().size();
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

char const* pTest = R"(.....
..##.
..#..
.....
..##.
.....)";
// char const* pTest = R"(..............
// ..............
// .......#......
// .....###.#....
// ...#...#.#....
// ....#...##....
// ...#.###......
// ...##.#.##....
// ....#..#......
// ..............
// ..............
// ..............)";
char const* pData = R"(##...####.#.####.#...##...#####.##.##.#..#.#..#....#....#..#.##....#.#.
##########.##.#.###.####..######..#.###.#.....##.##..####....###..#..##
.......#..##....##..##.#...###.#.#..###.#.###.######....##..##...##...#
..##.#..#.#.###.###..####.#.####..#...##....#..#.#..##.#####.......###.
######..#..#...##.##.#..#####..#..#.#.#.#.#####.#.##..#.#.....##....###
..##.#.##.#####.###...#...#.##..#..####......#####.#.##..#.#...##.#.#..
#.##....####.#..##..##....###.######...#.#####..#.####.##.#....#.#####.
.####....####.........#.##.#..#.#.#.#.#.#####..###.####..###..#.#...#.#
#####.#.##..#..#.....###..#####.####.#.#######.#.#...###.##.....##.##.#
.#..#..##..###..#.####..####....#...##.#...#.#....#..##.###..#.#.######
#.#.####..#..#....#..#.#.#####...#######.####.#.##..#####.####....###..
.######...###..#####.#####.#.#..#....####.####..##.#.#..###...#.#..#.#.
##.#.#######.#.........####.##.##.#.###.#..#.####.##....##.#.###.#.#..#
.###.##.####.#.#..#...###.##.##....#.###..#...##.......####..###.##..##
.##..##..##.#.#..#..##.##.#.##.###.#####.....#####.##.####..#.##..##...
..#......###..#....##..####...#....#.######.....#..##.##...#..#####..#.
.###..##..#.#..#.###..##..##......###.#....#.##...#...##.#.###.#.##.##.
#......##..##...##.###.##.........###..#..##..##..#.###....#.##.#....#.
..#...#.#..#.##..#...##....##....#.##....###..##.#.....###.###..#...##.
..#....####.##.#..###...#..#.#######...###..##.###.##..#.....#..##..##.
..##.####...###.#.###.#..#####...#..#..#...#....###...#.#..#.#.###.....
.#.#####....##.#........#.##..#.#.#####..#.#.##.#....##.#..#....####...
####....#..#.#.#.....#.###.#.##.##.##..#..........#....#.###.#..#####.#
##..#.#....####.###..###.####..#..###......#...#.#.#.##..#..#...#..####
##..##..#######..##.......#....###..##.###..#...###..#.#####.##..#.##.#
..#.##..#..#.###.###.####.#......#..##.##......#.##.#..#..#.#...#....##
.....#.#.##.#####..##.###.###..#..#.##..#.##..##..##..###.#.###.##..###
.##.###..#..###.###.#.##...####..###...#.#.#.##..##.###....#.#...#.##.#
...#..#.###..#.#.#########.#..#.###.#...#..#.#.#..##.#.##..#.##........
##.######..#.#.##.#..##......#.###..##.....#..###.##...#..#.#####.##.#.
#####..#..##.##.#....##.###...####.#..###.##....#.#..#...#.#....#.##.##
.##...##..#.#....#...##...#...##..#.#.....#.####.#..#########...#####.#
#..##.#...##.....#..#.#.#...#...##.######.#.###...##.#.......#.#...#...
.##.#####..####.####..#.....#.######.#.#.##.####...##.....###..#.###...
#.#..##.###..#....#.....##.#..####..#..#.#.#.....####...##..#.###..#.##
#..##...##.#...#.########....#.........###..#..######.##..#..####.###.#
##.#.#....#.##.#.##..##.##.#....#.##.#.##.#..####.#.#..##..####.#.#..##
#..##..####.......##.###.#..##...#.##..#.#..#..#..####.#.##...###.#..##
.#..##.##.#######..########.#.#....#.#...###..#...##.##.#.##.###.#.....
#.#.###..#....#..##.###.#####.#.#..##.#####..#....##.##.#..##.##.####..
###.#..#....#.......##.#.#.###.##.#.#.#.#.#.#.####....#...##..#..###...
..##......###.#...###.....##...#.###.##.##..###.##.#..#..###......##...
..#...#.....##....###.#.##.#.#..##.#....#...#.####.....###..##.#.#.#...
###.........##...##.#...##..##.##....##.#.##........#.##########...#...
..##.##.####.###..........#.##.#.##..##.####....##.#...####.#..##..#.#.
...#.###.###..#.##...##..#.#..#.##.##.###.#....#.#.#..###.##.##...#...#
..#..#.####.####...#.##.####.#####.....##..#.###...##.....####.#..##.#.
#..##.###..###....###...##.#.#.#.##.#.##.######..#...#.#..#.#..##.#....
##..#####.#..#...##.###.##.##.##.#.#.#..##...#.#####.###.#.#....###.#..
..#.##.#..##.#..#..#.#.##..###.#.###.#..##.#.#.##.#.#..##.####...###.#.
..####.#.#####.#.##.#######.###.##.#..#..##..#.##..#####.####...#...###
...#...#.#.....#.....##..###.##..#..#.#....#.#.#.#....#......#.#...#..#
......#.##..#...###.#.#..##..#........#.#..#.#...#.####.#.#..##.#.##.##
#..##.#.##..#.##....#....##.##.#.#.##..####...#....#.##.##.##.##...#...
.###...##....####.#...#.#....#####.#.####.....#..##.#....##.#.#..#####.
.##.##..#...#..#.###.#...#..#######....##.###.#...####....##.#.#.##.#.#
##.#....#######.###...#.###...#.##...#.#.#.....#.#....#..#..###.#.####.
#..##.##.##...##.......###..#...###..#.......##.#.#####..##..#...#..##.
##.##...#.#..#...########.#.#..##.#.#.#..#####.##...#.####.##..###.###.
#.##..#.##..###.#..###.....######.##...##.##..###..####.#.####....##...
#...#......##..##...#.#.#...##..#..####..###.#.#..###..#.##...####.#.#.
#..###..##..####.##...#.###..####...#..#...##.#....###.....##..#.#.###.
..##.#..#.##.##..#..#.#.....#..#...#...##.##.#.#..###..#...#####.###...
###.##...##..####...##..##...#####..##.###..####....#.#.#.#.#..#..###.#
#...###..#..#.#........##.#..#..#####..##...##...#..#.##.#.#.#######...
.#..#.##.....#.##....##.##..#.##.#.#.#.##.#..##..#...#...#####...###.##
##..#.##..#.##.#.#....####.###..##.#.....##..###.#.###.....#..#.##.....
##.#.#.#.#.#.##...#.#.####..#...#..###..#..####...#.##......#.#.###....
###.##..###.....#.###.#......##.#.###.######....##.#.#####.#####...##.#
.##.#.#.###.######...####..##.#...#..#..#..###.########...#..#..#..###.
.#.####.#.#..##.####.##......###.......##.#.#....####.#......#..##.###.)";
