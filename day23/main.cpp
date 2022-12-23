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

class Vector {
public:
  Vector(int col,int row) : m_col{col},m_row{row} {}
  int col() const {return m_col;} 
  int row() const {return m_row;} 
private:
  int m_col;
  int m_row;
};

class Elf {
public:
  Elf(Vector const& pos) : m_pos{pos} {}
  Vector const& pos() const {return m_pos;}
private:
  Vector m_pos;
};

struct MapBounds {
  int m_west_bound{1000};
  int m_east_bound{0};
  int m_north_bound{1000};
  int m_south_bound{0};
};

class Map {
public:
  void push_back(Elf const& elf) {
    m_elves.push_back(elf);
    m_bounds.m_west_bound = std::min(elf.pos().col(),m_bounds.m_west_bound);
    m_bounds.m_east_bound = std::max(elf.pos().col(),m_bounds.m_east_bound);
    m_bounds.m_north_bound = std::min(elf.pos().row(),m_bounds.m_north_bound);
    m_bounds.m_south_bound = std::max(elf.pos().row(),m_bounds.m_south_bound);
  }
MapBounds const& bounds() const {return m_bounds;}
private:
  std::vector<Elf> m_elves;
  MapBounds m_bounds{};
};

using Model = Map;

std::ostream& operator<<(std::ostream& os,Vector const& pos) {
  std::cout << "[col:" << pos.col() << ",row:" << pos.row() << "]"; 
  return os;
}

std::ostream& operator<<(std::ostream& os,MapBounds const& bounds) {
  std::cout << "west:" << bounds.m_west_bound << " .. east:" << bounds.m_east_bound;
  std::cout << " north:" << bounds.m_north_bound << " .. south:" << bounds.m_south_bound;
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
          std::cout << "\nelf at " << elf.pos();
        }
        ++col;
      }
      ++row;      
    }
    return result;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\nbounds:" << data_model.bounds();
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
