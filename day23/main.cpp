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
  bool operator<(Vector const& other) const {
    if (col == other.col) return row < other.row;
    else return (col < other.col);
  }
};

std::ostream& operator<<(std::ostream& os,Vector const& pos) {
  os << "[col:" << pos.col << ",row:" << pos.row << "]"; 
  return os;
}

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
    this->set_pos(m_elves.size()-1,elf.pos);
  }

  void set_pos(int index,Vector const& new_pos) {
    // std::cout << "\nmove index:" << index << " new_pos:" << new_pos;
    auto& elf = m_elves[index];
    elf.pos = new_pos;
    m_pos_count[new_pos] += 1; // Count elves at the same position
  }

  Elves const& elves() const {return m_elves;}

  void move(int index,Vector const& delta) {
    this->set_pos(index,m_elves[index].pos + delta);
  }

  int elf_count(Vector const& pos) const {
    if (m_pos_count.contains(pos)) return m_pos_count.at(pos);
    else return 0;
  }

  bool is_alone(int index) const {
    // std::cout << "\nis_alone index:" << index << " pos:" << m_elves[index].pos;
    if (m_pos_count.contains(m_elves[index].pos)) return (m_pos_count.at(m_elves[index].pos) == 1);
    else {
      std::cerr << "\nERROR, no count recorded";
      return true;
    }
  }

  Map& update_to(Map const& other) {    
    m_pos_count.clear();
    for (int i=0;i<other.elves().size();++i) {
      if (other.is_alone(i)) {
        this->set_pos(i,other.elves()[i].pos);
      }
      else {
        // Don't move
      }
      m_pos_count[m_elves[i].pos] = 1; // All elves are now alone
    }
    return *this;
  }

MapBounds bounds() const {
  MapBounds result{
    .west = m_elves[0].pos.col
    ,.east = m_elves[0].pos.col
    ,.north = m_elves[0].pos.row
    ,.south = m_elves[0].pos.row};
  for (auto const& elf : m_elves) {
    result.west = std::min(elf.pos.col,result.west);
    result.east = std::max(elf.pos.col,result.east);
    result.north = std::min(elf.pos.row,result.north);
    result.south = std::max(elf.pos.row,result.south);
  }
  return result;
}
private:
  Elves m_elves;
  std::map<Vector,int> m_pos_count{};
};

using Model = Map;

std::ostream& operator<<(std::ostream& os,MapBounds const& bounds) {
  os << "west:" << bounds.west << " .. east:" << bounds.east;
  os << " north:" << bounds.north << " .. south:" << bounds.south;
  return os;
}

std::ostream& operator<<(std::ostream& os,Map const& map) {
  auto bounds = map.bounds();
  for (int row=bounds.north;row<=bounds.south;++row) {
    os << "\n";
    for (int col=bounds.west;col <= bounds.east;++col) {
      if (map.elf_count({.col=col,.row=row})==1) {
        os << '#';
      }
      else {
        os << '.';
      }
    }
  }
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

Vector to_delta(std::string dir_name) {
  Vector result{};
  if (dir_name=="NW") result = Vector{.col=-1,.row=-1}; // NW
  else if (dir_name=="N") result = Vector{.col=0,.row=-1}; // N
  else if (dir_name=="NE") result = Vector{.col=1,.row=-1}; // NE
  else if (dir_name=="E") result = Vector{.col=1,.row=0}; // E
  else if (dir_name=="SE") result = Vector{.col=1,.row=1}; // SE
  else if (dir_name=="S") result = Vector{.col=0,.row=1}; // S
  else if (dir_name=="SW") result = Vector{.col=-1,.row=1}; // SW
  else if (dir_name=="W") result = Vector{.col=-1,.row=0}; // W
  else {
    std::cerr << "\nERROR, to_delta for " << std::quoted(dir_name) << " = ??";
  }
  return result;
}

Vector to_delta(char dir_char) {
  return to_delta(std::string{dir_char});
}

using NeighbourCounts = std::vector<int>;
NeighbourCounts to_neighbour_counts(Elf const& elf,Map const& map) {
  // result[0] is NW,N,NE
  // result[1] is NE,E,SE
  // result[2] is SE,S,SW
  // result[4] is SW,W,NW
  NeighbourCounts result(4,0);
  std::string dir_name{};
  for (int i=0;i<8;++i) {
    switch (i) {
      case 0: dir_name="NW"; break; // NW
      case 1: dir_name="N"; break; // N
      case 2: dir_name="NE"; break; // NE
      case 3: dir_name="E";  break; // E
      case 4: dir_name="SE"; break; // SE
      case 5: dir_name="S"; break; // S
      case 6: dir_name="SW"; break; // SW
      case 7: dir_name="W"; break; // W
    }
    auto pos_to_check = elf.pos + to_delta(dir_name);
    bool occupied = std::any_of(map.elves().begin(),map.elves().end(),[pos_to_check](Elf const& elf){
      return (pos_to_check == elf.pos);
    });
    if (occupied) {
      if (dir_name.find('N') != std::string::npos) result[0] += 1;
      if (dir_name.find('E') != std::string::npos) result[1] += 1;
      if (dir_name.find('S') != std::string::npos) result[2] += 1;
      if (dir_name.find('W') != std::string::npos) result[3] += 1;
    }
  }
  return result;
}

bool is_free(Elf const& elf,char dir,NeighbourCounts const& nc) {
  bool result{false};
  switch (dir) {
    case 'N': result = (nc[0] == 0); break;
    case 'E': result = (nc[1] == 0); break;
    case 'S': result = (nc[2] == 0); break;
    case 'W': result = (nc[3] == 0); break;
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
      std::cout << "\n<initial map>";
      std::cout << "\n" << map;
      for (int i=0;i<10;++i) {
        std::cout << "\nround:" << i+1;
        auto proposed_map = map;
        for (int index = 0;index < map.elves().size();++index) {
          auto const& elf = map.elves()[index];
          auto neighbour_counts = to_neighbour_counts(elf,map);
          if (std::any_of(neighbour_counts.begin(),neighbour_counts.end(),[](int count){return count>0;})) {
            std::cout << "\n\thas neighbour elf:" << elf.pos;
            for (char dir : directions_to_consider) {
              if (is_free(elf,dir,neighbour_counts)) {
                std::cout << "\n\telf at " << elf.pos << " propeses:" << dir;
                proposed_map.move(index,to_delta(dir));
                break;
              }
            }
          }
        }
        directions_to_consider.push_back(directions_to_consider[0]);
        directions_to_consider.erase(directions_to_consider.begin());
        map.update_to(proposed_map);
        std::cout << "\nMAP after round " << i+1 << map;
      }
      // The number of unoccupied positions on the bounded map are the size of the map minus the elf count
      std::cout << "\nfinal bounds " << map.bounds();
      result = (map.bounds().east - map.bounds().west + 1) * (map.bounds().south - map.bounds().north + 1) - map.elves().size();
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      // std::cout << "\nbounds:" << data_model.bounds();
      auto map = data_model;
      std::vector<char> directions_to_consider{'N','S','W','E'};
      // std::cout << "\n<initial map>";
      // std::cout << "\n" << map;
      bool loop_again{true};
      int i=0;
      while (loop_again) {
        bool none_moved{true};
        std::cout << "\nround:" << i+1 << " bounds " << map.bounds();
        auto proposed_map = map;
        for (int index = 0;index < map.elves().size();++index) {
          auto const& elf = map.elves()[index];
          auto neighbour_counts = to_neighbour_counts(elf,map);
          if (std::any_of(neighbour_counts.begin(),neighbour_counts.end(),[](int count){return count>0;})) {
            none_moved = false;
            // std::cout << "\n\thas neighbour elf:" << elf.pos;
            for (char dir : directions_to_consider) {
              if (is_free(elf,dir,neighbour_counts)) {
                // std::cout << "\n\telf at " << elf.pos << " propeses:" << dir;
                proposed_map.move(index,to_delta(dir));
                break;
              }
            }
          }
        }
        if (none_moved) {          
          std::cout << "\nNONE MOVED for round:" << i+1;
          result = i+1;
          loop_again = false;
        }
        else {
          directions_to_consider.push_back(directions_to_consider[0]);
          directions_to_consider.erase(directions_to_consider.begin());
          map.update_to(proposed_map);
          // std::cout << "\nMAP after round " << i+1 << map;
          ++i;
        }
      }
      return result;
  }
}

int main(int argc, char *argv[])
{
  Answers answers{};
  // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
  // answers.push_back({"Part 1     ",part1::solve_for(pData)});
  answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
  answers.push_back({"Part 2     ",part2::solve_for(pData)});
  for (auto const& answer : answers) {
    std::cout << "\nanswer[" << answer.first << "] " << answer.second;
  }
  // std::cout << "\nPress <enter>...";
  // std::cin.get();
  std::cout << "\n";
  return 0;
}

// char const* pTest = R"(.....
// ..##.
// ..#..
// .....
// ..##.
// .....)";
char const* pTest = R"(..............
..............
.......#......
.....###.#....
...#...#.#....
....#...##....
...#.###......
...##.#.##....
....#..#......
..............
..............
..............)";
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
