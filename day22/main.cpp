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
#include <cassert>

extern char const* pTest;
extern char const* pData;

using Result = long int;
using Answers = std::vector<std::pair<std::string,Result>>;

namespace dim2 {
  struct Vector {
    int row;
    int col;
    Vector& operator-=(Vector const& other) {
      row -= other.row;
      col -= other.col;
      return *this;
    }
    Vector operator-() const {
      Vector result{.row=-row,.col=-col};
      return result;
    }
    Vector& operator+=(Vector const& other) {
      row += other.row;
      col += other.col;
      return *this;
    }
    Vector operator+(Vector const& other) const {
      Vector result{*this};
      result += other;
      return result;
    }
    bool operator==(Vector const& other) const {
      if (row == other.row) return col == other.col;
      else return row == other.row;
    }

    using Vectors = std::vector<Vector>;

  };

  const Vector RIGHT{.row=0,.col=1};
  const Vector DOWN{.row=1,.col=0};
  const Vector LEFT{.row=0,.col=-1};
  const Vector UP{.row=-1,.col=0};

  struct Matrix {
    std::vector<Vector> rows;
    Vector operator*(Vector const& v) const {
      Vector result{};
      result.row = rows[0].row*v.row + rows[0].col*v.col;
      result.col = rows[1].row*v.row + rows[1].col*v.col;
      return result;
    }
  };

  const Matrix TURN_RIGHT{{
    {0,1}
    ,{-1,0}
  }};

  const Matrix TURN_LEFT{{
    {0,-1}
    ,{1,0}
  }};

}

namespace dim3 {
  using Vector3D = std::array<int,3>;
  using Matrix3D = std::array<Vector3D,3>;
  Vector3D operator*(Matrix3D const& m, Vector3D const& v){
    Vector3D result{};
    for (int r=0;r<m.size();r++) {
      for (int c=0;c<m[0].size();c++) {
        result[r] += m[r][c]*v[c];
      }
    }
    return result;
  }
  Matrix3D operator*(Matrix3D const& m1, Matrix3D const& m2){
    Matrix3D result{};
    for (int r=0;r<m1.size();r++) {
      for (int c=0;c<m2[0].size();c++) {
        for (int k=0;k<m1[0].size();k++) {
          result[r][c] += m1[r][k]*m2[k][c];
        }
      }
    }
    return result;
  }

  Vector3D operator+(Vector3D const& v1,Vector3D const& v2) {
    return {v1.at(0)+v2.at(0),v1.at(1)+v2.at(1),v1.at(2)+v2.at(2)};
  }

  Vector3D operator-(Vector3D const& v1,Vector3D const& v2) {
    return {v1.at(0)-v2.at(0),v1.at(1)-v2.at(1),v1.at(2)-v2.at(2)};
  }
}

namespace dim = dim2;

/*
Facing is 0 for right (>), 1 for down (v), 2 for left (<), and 3 for up (^).
*/
const std::vector<dim::Vector> DELTA{dim::RIGHT,dim::DOWN,dim::LEFT,dim::UP};
char char_of_delta(dim::Vector const& delta) {
  if (delta.row==0) return (delta.col>0)?'>':'<';
  else return (delta.row>0)?'v':'^';
}

std::ostream& operator<<(std::ostream& os,dim::Vector const& v) {
  os << "[row:" << v.row << ",col:" << v.col << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os,std::vector<std::string> rows) {
  for (int row=0;row<rows.size();++row) {
    if (row>0) os << "\n";
    os << std::quoted(rows[row]);
  }
  return os;
}

class RowsGrid {
public:
  using Row = std::string;
  using Map=std::vector<Row>;
  RowsGrid& push_back(Row const& row) {
    m_map.push_back(row);
    m_bottom_right.row = m_map.size()-1;
    if (m_bottom_right.col < 0) m_bottom_right.col = row.size()-1;
    // std::cout << "\nPUSH_BACK m_bottom_right.col:" << m_bottom_right.col << " row.size()-1:" << row.size()-1;
    if (static_cast<Result>(row.size()-1) < m_bottom_right.col) {
      auto& ref = m_map.back();
      auto diff =  m_bottom_right.col - static_cast<Result>(ref.size() - 1);
      if (diff>0) {
        // std::cout << "\ndiff:" << diff << std::flush;
        auto expansion = std::string(diff,' ');
        ref += expansion;
        // std::cout << " expansion:" << std::quoted(expansion) << " ref:" << std::quoted(ref) << std::flush;
      }
    }
    else if (m_bottom_right.col < static_cast<Result>(row.size()-1)) {
      m_bottom_right.col = row.size()-1;
      // std::cout << "\nEXPAND!" << std::flush;
      // expand map to the right
      for (auto& ref : m_map) {
        // std::cout << "\nexpand ref:" << std::quoted(ref);
        auto diff =  m_bottom_right.col - static_cast<Result>(ref.size() - 1);
        if (diff>0) {
          // std::cout << "\ndiff:" << diff << std::flush;
          auto expansion = std::string(diff,' ');
          ref += expansion;
          // std::cout << " expansion:" << std::quoted(expansion) << " ref:" << std::quoted(ref) << std::flush;
        }
      }
    }
    return *this;
  }
  Map const& map() const {
    return m_map;
  }
  dim::Vector const& top_left() const {
    return m_top_left;
  }
  dim::Vector const& bottom_right() const {
    return m_bottom_right;
  }
  bool in_frame(dim::Vector const& pos) const {
    return (pos.row>=top_left().row and pos.row<=bottom_right().row and pos.col>=top_left().col and pos.col<=bottom_right().col);
  }
  dim::Vector next(dim::Vector const& pos,dim::Vector const& delta) const {
    // std::cout << "\nnext(pos:" << pos << ",delta:" << delta << ")"; 
    dim::Vector result{pos+delta}; // optimistic
    // std::cout << "\n\tat(" << result << "):" << at(result);
    // we are now at '.', '#' or at ' ' (the last meaning out of map or out of frame)
    if (at(result)==' ') {
      // wrap around
      result -=delta; // back off
      while (at(result)!=' ') result -=delta; // back track on map
      result += delta; // back off
      // std::cout << "\n\tat(" << result << "):" << at(result);
    }
    if (at(result)=='#') result = next(result,-delta);
    // std::cout << "\n\tat(" << result << "):" << at(result);
    return result;
  }
  char at(dim::Vector const& pos) const {
    if (in_frame(pos)) return m_map.at(pos.row).at(pos.col);
    else return ' ';
  }
  char& at(dim::Vector const& pos) {
    static char NULL_POS{' '};
    if (in_frame(pos)) return m_map[pos.row][pos.col];
    else return NULL_POS;
  }
private:
  dim::Vector m_top_left{.row=0,.col=0};
  dim::Vector m_bottom_right{.row=-1,.col=-1}; // empty
  Map m_map{};
};

std::ostream& operator<<(std::ostream& os,RowsGrid const& grid) {
  os << "\n" << grid.map();
  return os;
}

using Step = std::pair<int,char>;
using Path = std::vector<Step>;

using Model = std::pair<RowsGrid,Path>;

Model parse(auto& in) {
    Model result{};
    std::string line{};
    int state{0};
    while (std::getline(in,line)) {
      if (line.size()==0) {
        ++state;
      }
      switch (state) {
        case 0: {
          result.first.push_back(line);
        } break;
        case 1: {
          int steps{};
          char turn;
          while (in) {
            if (in >> steps) {
              result.second.push_back(Step{steps,' '});
            }
            if (in >> turn) {
              result.second.push_back(Step{0,turn});
            } 
          }
        } break;
      }
    }
    return result;
}

std::ostream& operator<<(std::ostream& os,Step const& step) {
  if (step.second >' ') os << step.second;
  else os << step.first;
  return os;
}

std::ostream& operator<<(std::ostream& os,Path const& path) {
  for (auto const& step : path) os << "\n\tstep:" << step.first << " turn:" << step.second;
  return os;
}

std::ostream& operator<<(std::ostream& os,Model const& model) {
  os << "\n" << model.first;
  os << "\n\npath:" << model.second;
  return os;
}

class Traveler {
public:
  using LocationAndDir = std::pair<dim::Vector,int>;
  Traveler(RowsGrid const& grid,Path const path) : m_grid{grid},m_path{path},rendered{grid} {
    std::cout << "\nTraveler::Traveler";
    auto first_col{m_grid.top_left().col};
    for (;first_col<m_grid.bottom_right().col;++first_col) {
      std::cout << "\n[row:0,col:" << first_col << "]=" << m_grid.map()[0][first_col];
      if (m_grid.map()[0][first_col]!=' ') break;
    }
    m_location_and_dir.first = dim::Vector{.row=0,.col=first_col};
    walk_the_path();
  }
  LocationAndDir location_and_dir() {
    return m_location_and_dir;
  }
  dim::Vector const& pos() const {
    return m_location_and_dir.first;
  }
  static void test(RowsGrid const& grid,Path const path) {
    std::cout << "\ntest(...)";
    if (false) {
      RowsGrid temp{};
      dim::Vector pos{};
      dim::Vector delta{dim::RIGHT};
      std::cout << "\npos:" << pos;
      for (auto i : {0,0,0,0}) pos += delta;
      std::cout << "\npos:" << pos;
      delta = dim::TURN_RIGHT*delta;
      for (auto i : {0,0,0,0}) pos += delta;
      std::cout << "\npos:" << pos;
      delta = dim::TURN_LEFT*delta;
      for (auto i : {0,0,0,0}) pos += delta;
      std::cout << "\npos:" << pos;      
    }
    if (false) {
      std::cout << "\n2 path:" << path;
      RowsGrid temp{};
      dim::Vector pos{};
      dim::Vector delta{dim::RIGHT};
      for (auto const& step : path) {
        std::cout << "\nfrom:" << pos << " step:" << step; 
        switch (step.second) {
          case 'R': delta = dim::TURN_RIGHT*delta; ;break;
          case 'L': delta = dim::TURN_LEFT*delta; break;
          default: break;
        }
        temp.at(pos) = char_of_delta(delta);
        for (int i=0;i<step.first;++i) {
          pos += delta;
        }
        temp.at(pos) = char_of_delta(delta);
        std::cout << "\n" << temp;
      }
    }
/*

        >>v#    
        .#v.    
        #.v.    
        ..v.    
...#...v..v#    
>>>v...>#.>>    
..#v...#....    
...>>>>v..#.    
        ...#....
        .....#..
        .#......
        ......#.

"        >>v#    "
"        .#v.    "
"        #.v.    "
"        ..v.    "
"...#...v..v#    "
">>>v...>#.>>    "
"..#v...#....    "
"...>>>>v..#.    "
"        ...#...."
"        .....#.."
"        .#......"
"        ......#."
*/    
    if (true) {
      std::cout << "\n3 path:" << path << std::flush;
      Traveler traveler{grid,path};
      RowsGrid rendered{grid};
      auto pos = traveler.pos();
      std::cout << "\nstart_pos" << pos;
      auto delta = dim::RIGHT;
      std::cout << "\n" << rendered;
      for (auto const& step : path) {
        std::cout << "\nfrom:" << pos << " step:" << step; 
        switch (step.second) {
          case 'R': delta = dim::TURN_RIGHT*delta; ;break;
          case 'L': delta = dim::TURN_LEFT*delta; break;
          default: break;
        }
        rendered.at(pos) = char_of_delta(delta);
        for (int i=0;i<step.first;++i) {
          pos = traveler.m_grid.next(pos,delta);
          rendered.at(pos) = char_of_delta(delta);
        }
        std::cout << "\n" << rendered;
      }
    }
  }
private:
  friend std::ostream& operator<<(std::ostream& os,Traveler const& traveler);
  RowsGrid m_grid;
  Path m_path;
  dim::Vector delta{dim::RIGHT};
  LocationAndDir m_location_and_dir{};
  RowsGrid rendered{};
  void take_step() {
    auto next = m_grid.next(m_location_and_dir.first,delta);
    if (m_grid.at(next)!='#') m_location_and_dir.first = next;
    rendered.at(m_location_and_dir.first) = char_of_delta(delta);
  }
  void turn(char dir) {
    switch (dir) {
      case 'R': delta = dim::TURN_RIGHT*delta; ;break;
      case 'L': delta = dim::TURN_LEFT*delta; break;
      default: break;
    }
    rendered.at(m_location_and_dir.first) = char_of_delta(delta);
  }
  void walk_the_path() {
    for (auto const& step : m_path) {
      for (int i=0;i<step.first;++i) take_step();
      turn(step.second);
    }
  }
};

std::ostream& operator<<(std::ostream& os,Traveler const& traveler) {
  os << "traveled:" << traveler.m_path;
  os << "\n" << traveler.rendered;
  return os;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << data_model;
      if (false) {
        Traveler::test(data_model.first,data_model.second);        
      }
      else {
        Traveler traveler{data_model.first,data_model.second};
        auto location_and_dir = traveler.location_and_dir();
        result = 1000*(location_and_dir.first.row+1) + 4*(location_and_dir.first.col+1) + location_and_dir.second;
        std::cout << traveler;
      }
      return result;
  }
}

namespace part2 {
  /*
  Define the cube with one corner at top_left_front (tlf) and a diagonally opposite corner bottom_right_back (brb).

                /-------------
               /|            /|               sheet (z)
              / |    1      / |              /
             /  |      5   /  |             /
          tlf-------------- 3 |            tlf ---> col (x)
            | 2 |---------|-- brb           |
            |  /   0      |  /              |
            | /      4    | /               v
            |/            |/               row (y)
             --------------

    - Number the faces counter clockwise from tlf with face 0 in row,col plane.
    - Number opposite faces so that the sum is 5
    - Place the coordinate system at tlf with column as x, row as y and sheet as z

    NOTE: In the puzzle example the numbering used is the order of faces found, NOT as above.

    - going right will travel faces 0,3,5,2,0...
    - going left will travel faces in the opposite order of going right
    - going up will travel faces 0,1,5,4,0...
    - going down will faces in the opposite order of going up 1,5,6,2,1,...

    - Define Horizontal Orbits = faces 0,3,5,2,0...
    - Define Vertical Orbits  = 0,1,5,4,0...
    - define rotation_direction = clockwise or counter clockwise.

  */

  const std::map<int,int> H_ORBIT{{0,3},{3,5},{5,2},{2,0}}; // Horizontal Orbit
  const std::map<int,int> V_ORBIT{{0,1},{1,5},{5,4},{4,0}}; // Vertical Orbit
  
  using Faces = std::vector<std::vector<std::string>>;

  Faces to_faces(RowsGrid const& grid) {
    auto result = Faces(6);
    // map row,col on grid to the faces of the cube
    // We need to figure out what faces we encounter and in what order.
    // The leftmost map character in the input is on face 0
    auto face_rows = std::vector<std::tuple<int,int>>{}; // col, width
    for (int grid_row=0;grid_row<grid.map().size();++grid_row) {      
      auto line = grid.map()[grid_row]; line += " ";
      auto first = line.find_first_not_of(' ',0);
      auto end = line.find_first_of(' ',first);
      auto width = end-first;
      face_rows.push_back({first,width});
      std::cout << "\ngrid_row:" << grid_row << " first:" << first << " end:" << end;
    }
    if (true) {
      // LOG
      std::cout << "\nGRID PARTITION";
      for (int i=0;i<face_rows.size();++i) {
        std::cout << "\n" << std::get<0>(face_rows[i]) << " " << std::get<1>(face_rows[i]) << " " << grid.map()[i];
      }
    }
    int cube_side{std::get<0>(face_rows.front())};
    for (int i=1;i<face_rows.size();++i) {
      cube_side = std::gcd(cube_side,std::get<1>(face_rows[i]));
    }
    std::cout << "\nCUBE SIDE SIZE " << cube_side;

    int face_id{};
    std::array<std::array<int,5>,4> face_id_grid{{
       {{5 ,2,0,3, 5}}
      ,{{1 ,2,4,3, 1}}
      ,{{-1,2,5,3,-1}}
      ,{{-1,1,-1,-1, -1}}}}; // face id relative face 0 in possible unfolds
    int lookup_col_offset{0};
    for (int face_id_grid_row=0;face_id_grid_row<face_id_grid.size();++face_id_grid_row) {
      std::cout << "\nface_id_grid_row:" << face_id_grid_row;
      auto face_count = (std::get<1>(face_rows[face_id_grid_row*cube_side]) / cube_side);
      std::cout << "\n\tface_count:" << face_count;
      for (int face_id_grid_col=0;face_id_grid_col<5;++face_id_grid_col) {
        // Loop col 0..4 assuming faces are in the input as in face_id_grid
        std::cout << "\n\tj:" << face_id_grid_col;
        for ( int grid_row=face_id_grid_row*cube_side;grid_row<std::min(static_cast<int>(grid.map().size()),(face_id_grid_row+1)*cube_side);++grid_row) {
          auto [first,width] = face_rows[grid_row];
          auto line = grid.map()[grid_row];
          std::cout << "\n\t" << first << " " << width << " " << std::quoted(line);
          if (grid_row==0) {
            // face_id = 0;
            lookup_col_offset = 2-first/cube_side; // if first/cube_side 0, face 0 is first so we need to offset lookup off the id with + 2
            std::cout << "\nlookup_col_offset:" << lookup_col_offset;
            assert(-1 <= lookup_col_offset and lookup_col_offset<=2);
          }
          auto lookup_col = face_id_grid_col+lookup_col_offset;
          std::cout << " id_grid " << face_id_grid_row << " " << face_id_grid_col;
          std::cout << " face_id " << face_id_grid[face_id_grid_row][lookup_col];
          face_id = face_id_grid[face_id_grid_row][lookup_col];
          int begin = face_id_grid_col*cube_side;
          std::cout << " " << begin << " " << begin-first;
          if (begin >= first and 0 <= begin-first and  begin-first < width) {
            auto face_row = line.substr(face_id_grid_col*cube_side,cube_side);
            std::cout << " " << std::quoted(face_row);
            result[face_id].push_back(face_row);
          }
        }
      }
    }
    if (true) {
      for (int face_id=0;face_id<result.size();++face_id) {
        std::cout << "\nface_id:" << face_id;
        for (auto const& line : result[face_id]) {
          std::cout << "\n\t" << std::quoted(line);
        }
      }
    }
    return result;
  }

/*

        ...#  0
        .#..
        #...
        ....
...#.......#      1,2,4
........#..A
..#....#....
.D........#.
        ...#..B. 5,3
        .....#..
        .#......
        ..C...#.

face_id:0
	"...#"
	".#.."
	"#..."
	"...."
face_id:1
	"...#"
	"...."
	"..#."
	"...."
face_id:2
	"...."
	"...."
	"...#"
	"...."
face_id:3
	"...."
	".#.."
	"...."
	"..#."
face_id:4
	"...#"
	"#..."
	"...."
	"..#."
face_id:5
	"...#"
	"...."
	".#.."
	"...."

*/  

  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto [grid,path] = parse(in);
      auto faces = to_faces(grid);
      return result;
  }
}

int main(int argc, char *argv[])
{
  Answers answers{};
  // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
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

// char const* pTest = R"(        ...#
//         .#..
//         #...
//         ....
// ...#.......#
// ........#...
// ..#....#....
// ..........#.
//         ...#....
//         .....#..
//         .#......
//         ......#.

// 10R5L5R10L4R5L5)";
char const* pTest = R"(        ...#
        .#..
        #...
        ....
...#.......#
........#...
..#....#....
..........#.
        ...#....
        .....#..
        .#......
        ......#.

10R3R4R0R4R2L5R10L4R5L5)";

char const* pData = R"(                                                  ..........#..................#...........#.................#......#..................#...........#..
                                                  .........................#.......#.....#..............................#...........#.......#.........
                                                  .................#...............#.#....#.........#................#...##.....#...........#.........
                                                  .#........##........#..................#...........................#.##...............#.............
                                                  .....#..#.....#.....................#.................#...#...#.#........##...........#.............
                                                  .##.#...............#......#..#.....#.........#..............#..............#.......#............#..
                                                  .#..#.....#..........................................#......#......#......#..#........#.#...........
                                                  ......#.#.#.......#...........#...#....#...#......#......#............................#..#..........
                                                  .......#...........#.....#...........#....#..#..#.........#.........................................
                                                  ........##.....#................#........#..........#..........##....#......#...#.#.....#...........
                                                  .................#.............#............#.............##.....#..............#...#........#......
                                                  #............................................#................#......................#..............
                                                  .................##....................#...#.....#................#......#.#..#....#.............#.#
                                                  ...........#...#............#.........##........#...##..........................#.#................#
                                                  ....#.............#...............#...#......#........##....#........#...#..........#...##..........
                                                  ..#..#..#.............................#........................#...................................#
                                                  .#............................................#......#....#........#....................#.......#...
                                                  ................#.#.....................#..........#..............#..#....#.#.#.....................
                                                  .#....................#.#..............................#..................#...#....#................
                                                  ............#.........................................##.#..............#..........................#
                                                  ...#..#.....#.#...#......#.....#..........#............#............................................
                                                  ............#...#.......#...........................#....#........................#.......#.........
                                                  ..#..........#....#.....#...................#.............................#.#.#...............##..#.
                                                  ...............................#.#...##............#.....##..#.#....................................
                                                  .........#......#......#.....#..#.......................#.#........#..#....#......##................
                                                  ......#...........#............#.......#.#....#...............##.......#..........#.................
                                                  ................#......#..#...#...#............#...##...#...#...........##.....#........#.....#.....
                                                  .......................#.#....#..................#.................#.#.#...................#........
                                                  ................#....#..............#..#..#....#.......#................#.......................#...
                                                  ............#..#.....##...#..........#.......#.........#.................#.....#........#.........#.
                                                  ..#.......................#.........#..........##............#...#.........................#........
                                                  ...#......#.........#...............................#..................#.....................#......
                                                  .........#...............#.#.................#....#.....#...............#.#.........................
                                                  .#...........#................#..#...............#............#.....#...................#.....##..#.
                                                  ..#.#.................#.#..........#.....#......#...........##..#...................................
                                                  .#....##.#...............#...........#............................#..........#.............#......#.
                                                  #................#.................#......###...........................#...........#....#.......#..
                                                  ................................................#.....................#..#.........#.......##.......
                                                  ......#.........................#..#......#......#........................#.....#.#............#....
                                                  ....#..........................#...........................#.#...........#..................#.......
                                                  ....................#..........#....#..........##...............##.....#.........##..........#......
                                                  .#..................#.............#.....#.#..#.#.........#..........................................
                                                  ..#.........#...##.........#...............#................#..............#................##......
                                                  .#...#.........#...............#....#..#......#...##............#..#...#...........#................
                                                  #......#..#...........#..................#...#.....#.#.......#......................#.#.#..#........
                                                  #..#..................#....#....................................#.............#......##..........#..
                                                  ............................................#.............#..#......#.........#........#............
                                                  #........#................#......#..###.#................#.........................#................
                                                  ...#...#..........#........#....#.#...........##.............##........#......#.............##......
                                                  ..................#....#.............................#..#............................#......#......#
                                                  ...#...............#......#....#...#..#...........
                                                  .......##.......#...###.......#.........#.......#.
                                                  ...........#...................#.#....#......#....
                                                  .........#.................#....#...#.#..........#
                                                  ..........#.....#............#.............#......
                                                  ..#............................#.......#.........#
                                                  .............#.....##..............#..#...........
                                                  ............#.#....#...#.#.##.#...............#...
                                                  ........................#................##......#
                                                  ......#......................................#....
                                                  ....................#......##....#.#..............
                                                  #........#........#.#................#..#.........
                                                  ............................#.#.........#..#......
                                                  #..........#..................#.......#.#..#......
                                                  #...#...#..#.#.....#.#....#....................#..
                                                  ................#....................#............
                                                  ..........#.................................#.....
                                                  ...#....###........................#..............
                                                  ..................#....#.........#................
                                                  .......###...................#...#....#...........
                                                  ..............#.................#...#........#....
                                                  ..........................#.......................
                                                  ...#..........................#...................
                                                  ..#...##.................................#.....#..
                                                  ...........#..............#....................#..
                                                  ....#..#............................#.............
                                                  ..#....#...........#.#........................#...
                                                  .....#......#..###..........#...#..........#......
                                                  .#.......#...............#..#..............#......
                                                  ...#....#......#........#..#.#..#.#...............
                                                  ..#..#..........#..#...#....##............#..#....
                                                  ........#......#...............#........#.#.......
                                                  .....#...........##...#..........#..#.......#.....
                                                  ...........#......#.......#...........#....#......
                                                  .#........#.......................#...#...........
                                                  ..........#............##.....#......#....#..##...
                                                  ........#............#.#.......................#..
                                                  ..#..................#............................
                                                  ...............................#.#.#....#.#.#.....
                                                  ................#.#......#.#...................#..
                                                  ...#...#.#..#.........#.................#.........
                                                  .................#....#........#..##..............
                                                  #...................#............#....#....#......
                                                  ..................................................
                                                  .........#......#......#...............#..........
                                                  ......#.........#.#....#....................#.#...
                                                  ....#..........#..##..................#......#....
                                                  ..............................#......#............
                                                  #.......#.............#............#..............
                                                  ...................##.....###.................#...
...................#....#.......#....#.##.#..#.#..#............................#....##..............
........#.#........#..#.....#.....#..........#.#....#.............#..#..............................
...#...................#.#......#....................#..........................#...................
.....................#...........#.......................#..........................##..#..#...#....
........................#.......................#......#.................#..........#...........#...
.......#...................##..........#..........#.........#.....#...............#....##.#.........
............#..#.......#..#...##..........#....#...#..#............#............#..#.#....#...#....#
....#.#....#...........#....#...................#.............................#..................#..
.....................................#.#.........#........#.....#.##....#................#....#...##
....#.#.......#......#..#..............#.#...............##......#.............#................#...
.#......#........................#........#...#...............#......#...................#..#.......
......#........##.........#....##........#.....#.......#......#...........#.......#.................
......#.#........#.....#.......##...##.#..........#.....#..........#..............##...#.......#....
...#...#..#........#......##................#...#.......#........##.....#.......#...................
.......##.#..#............#.............##..................#........#..........#........#..........
..#.......#.......#.........#.....#.........#..#.....#...........#....#.......#.#...................
.......#...........#........#.........................#......#...#..#...............................
#...#.............##.........................#..#..................#...#..............#...##........
.................##...............................................#.................................
......##..............#...##..#.....#......#..........##............#..#......#.....#...#...#.......
...#............#..............#................#..#.......................#.#..##.#..#.............
.........##........##...........#...........................#......#...#.......#.....#......#.......
......#...............#.#...........#....#..#.#.#....................#........#...#........#..##....
...#.............#.......................................................#..............#...##..##..
....#.............#..........................#...........#..#.#...............................#.....
..........#.#.......#......................#.............#.....#.............#...............#......
...............##........#....#.#......................................................#............
...........................#.....#..........#..........#.#.................#.#...#.......#....#.....
.....#...............#..........#.#....................#..................#.............#...........
......#.#.....#....#...#............#..........#.............#........#..............#..............
.........#.......#...........................#...#...............#......#.#......#....#.#..##..#....
....#.#....#...#...#.............##..#....#.......#.....#.#.........#......#..........#...##........
.....#.............#.#.............................#.........................#...........#..........
...#.....#............................#....#.................#......#..#.........................#..
#....#....#..........#......................#...#..........................##.............#......#..
.......................#..............#............#................#...#.......#.............#.....
...#.................#........#.................#.....#....................#........................
............#.#...##...............#..#........#..............#.....#..#...##.........#.............
....#...#..............##....#.................#..#..#.......#.#..#.................................
.............#.#........................#..........................#......#...#.....................
..................#.........................#................#...#..........#....................#..
.........#...........#............#..........#...............#..................#.........#.....#.#.
.............#.....................#....................#....#.................................#....
...............#......#...#..#....#..#.........#............#........................#.#....#.....#.
.........##................#........................#.........#............#..#..#..#...........#...
.#...##.#..........#....#..#.....#..........##......#.##...............#.................#.#........
..#...#......#.....#.#.......##...#.......#..............................#..#.#..#..................
...................##..##.....#.#....#..........##...#..#.#.......###.#.....#.......#...#...........
....#....#................##.#...###....#....#....................................................#.
#.#........#.......##...................#.#...........#.................#.....#.................##..
..............#..........#........................
...#..#.......#..............................#..#.
..............#....##.....................#...#.#.
......#.....#..........##.#.##..#...#.......#.....
.........................#...............#.#......
..#..............#.##.......#...#....#...#......#.
.......................#..............#...........
...#...........#.....##.#..##..#...#......#..#....
.#..............#..........#...............#......
.......#..........#..#.........#....#.............
.....#..#......#..........###............#........
................#......#....#...............#.....
................#.........#..............#.....#..
...................#..............#.......#.......
...........#....................#.....#..#........
..............#....................#...........#..
..#......#..............................#.........
........................................#........#
.....#....#..........#.......#....................
....#......##..............#..........#...........
...............#....##....#....#.....#.#......#...
..................................................
...................##..........................#..
..#.......#......#...#..#......................#..
......#...##.......#.#...........#................
#.......................#....................#....
...#...#..................#...#......#............
#..#..............#....#......#.........#..#......
.#.......................#...........#..#.........
.......#.....#...#.....#..........#.#...#.........
#..#....................................#...##....
.............................#....#.#........#....
.................................................#
....#....#..............#.....#..................#
..#...........#.#........................#.#......
...........................#.......#..#....##....#
.............#....................##..#...........
...#...................#.......................#..
........#............##.................#...#.....
........#.....................................#...
.....#.................................#..#.......
.#.................##..........#.......#..#.......
...................#........#.............#.#.....
.........#.....#..................................
..................#.....#.........................
........#.#.......................................
....#...##........#....#......##.....#...#..#..#..
............#.#.#...........................#.....
............#......#..#.......#.....##........#..#
......#.........#...#....#........................

16L3L21R23R30R5L5R46R14R33R20L39R5R27L14L21L44L35R39R9L11L19R50R34R7R43R29R21R7R13L6R18L19L45R50L27R29L4L35L39R14L19L31R39R50L14L7R30R43L12R18R2L19L28R9L28L26R29R9L21L11R22R3R41L27R6R47L31R26L48R34L11L25R1R20R30R26L20R10R14L40L33R41L10R5R24R19L36R2R22L41L16R12R36R3L50R3L1L38R15L40L3L47L2R41R15L7L43R30R20R1L13L17R14L16L35R41L6L16R19R40R16L23L25L33L32L22L34L42R36R16R13L9L21R22L44L10L25L21L36R42R10L44L31R10R39R36R36R32L2R1L10R11R11L39R26L25L46R38L5L43L16L2L29R44L28R16L29R49R36R40L12L44L43L11R7L46L28L45R5L21R11R50R40R30R35R9R28L2L24R16L23R13L18R17L15R35L20L49L34R15L17R39R47R15L32R24R43L2R8L12L1R50R2L24L21R31R19R41R10L31L24L12L33R24R16R17L9L24R21L36R17R42L35L2L34R27R10R9R1R11L2R27L43L46R35L8L41R11R42R5R20L44L35L21L15R32L34R38R37R42L4R14L16R40L6L2L41L11R4R3R10L48R35R1R17R7R32L44L31R37R33R8L34R3L6R7R50R13R50R7R29R6L29L47R17R12R2L32L22L33R22L37L32L14L43L32L3R8L23R44L28L12L40L42R33L36R42L28R42R7R14R13L44R30R33R33L34L20L28L9L5R30L7L23R48L41L49L34R1R18R44R50L18L5L7L39R37L28L12R35R8L30L34R43R12L44L45R29R45R27R4R35R26R14R42R4L50L7R7R27R32L28L9R18R26L47R17L40R3L31R11R32R27L39R20R25R33L34R3R39R3L46R15R22L43L31L31L21R44R12L46L41L42R43L29L11R34L36R48L28L40R32R46R41R4R25L39L44R43L39L43L9L47R49L22L33L47L42R33L39R28R18L40L30R26R35R11L48L45R2R37R17R30R36R27L39L46L48L40L7R43R6R40R40R30R45R35L26R45L6R11L33R15L3R32R2L12R10L2R30L24R40R11L14L16R10L25R3R49L7R29L33L34R19L22L11L14R14R45R22R1L38L47L44R32L24L19R9R2R38R14L11L42R3R29L34R50L39R39R15R46L1L23L46L24L33R37L14L20R8L28L26R46L13L15R40L26R48L9L50R36R45L43R46R37R5R34L33L3R49R27L36L27L21L1L35R37R18L41L5R46L10R2L17R36L49R18R44R31R35L34L37L7R11R14L45R37L44L25L50R41L36R48L36R5R38L24R28R46L22L42L13L44L12L49L47L44L44L37L22R2L49L20R23R50L46R33R36L41L27L29R17L13R9L22L40L13R19L21L27L7L46R11L13R14R49L6L6R42L24L44L38R10R13L33L42L33R11R3R1L24R17R49R23R44R3L43R39R47L46R34R1L17L23R50R14R4L50L6L43R14R10L29R12R45L48R28R13R18R13R50L14R38L37L17R49R3R44L16L6L36L45R20L46R21R22R31L7L27L49R44L5R17L40L34L2R40L3L12L7R32R8R12L46R40R37L9L10L22R30R45L30L9R40L28L13R46R4R38R24R43L8R47R39R18R12R1R43L46L11L17R21L48L19R48R33R26L27R24L12R34R31L41R14R40R4L25R34R36L47L29R13L45L5L12L2R32L29R39L42R34R7L47L23L31R13R47R8L25L17R29R20L25R42R42R28L48R37L48R25R5R30R7R25L16R37R50L39R26L19R48L2L47L48L21R3L49L43L6L40R14R25L13R42L39L1R40L35R5L43R5R42R5L31L2R10R42R23R4R17R41L5R50R41L19R12L42R41R42R2L27L39R36R44L48L19R7R26L29R19L16L32L47L5L47R39L22L32L45L36L14L27R21L47R47L34R9R43R19R30R4R41L11L24L37R24L28L31L33R6L7L34R4L26L4L31L44R22R4R36L20L36R7R48R2L39R28L15R44R18L10L23L31R45L30L3L14R11L1R22L38L11R48R10R13L27L24R34R15R18R47L40L41R23R45L17R21L29L45L39L25L28L21L31R28L26R49L17R21L7R31R24L29R6R29R23R20R9L35L28L44R28L34R32R26L42L34R22R34R38L50R11R47L11R18L43R29R30R22L41L46L36R10R5R16L41L27R28L8L6L13L5L11R30R38L29R15L15R48L2R41L2R45L25L32R13L8L1L33R45R42R1R42L24L41L24R3R38R22R7L10L22R15L18L31L48L26R3R11R23L15R3L36L20R44L13L19R43R23R41L47L41L5R10L28R44R26R50L35L3L18L22L1R8R25R27R7L41L31R13R39R5R14R12L4R22R48R11L26R10L24L48R45L2R41R4R46L19R24R44L8R44R22R40R19L39L9R7R39L46L38L10R1R22R37L36L47R40R31L38L16L6R13R20L39L35R1R46L34R42L14R50L46L4L38R14L36L13L36L23R10L45L28L43R15L33L10L10L8R49L40R29R29L12R10R47L14L36R12R1L10L6L31L6L35R2L23L28L46R9R13L14R16R29R41L31L3R25L31R3R16R18R10R42L27L10R42L2R19L18L42L7L22L45L47R9R24R46L6L4L16L8R26R36R26L42R43R6L39L19L16R47R40L21L33R21L49L14R45R42L31L1L27R22L6L45L37R46R32R48R17L25L2R29L49R27R41L22L11L5R25L30L13R23R8R15L2L21R33L8L25R27R22L20L10R27L7R4R39R42R28L6L32L16R20L21L15R26R25R15R32L32R10R39L19R11R37R29L3L48L45R48R37L45L42R38L49L46R16R20R33R11L12L2L29R40R33L25R49R6R40R46R10L36L38R31R23L4L25R10L35L10R25R27L22R6R19L26L26L22R18R30R38R29L19R7R7R4L10R44R9R24L2R5R39L40L48R46R48R31L6R44R28L33L43R2R9R41R10R4L28L10R47L16L10L14L24R49L14L34R26R25L47L30R46R44L35L46R42L38R13R45R17R25L36R36L17R37L30L21L30L47L37R34R14R23L15R19L22R49L44R2L1R10R30L36L47R38R34L42R9L2L11R26R31L46R30R20L48R1R43L36L29L47R33R3L41L41L3R37L11R11R40L26L28R37R18R3R16R8L1L27L24L9L37R37L46R2L18L21L10L35R22L30R42R49R9R39R3R47L2R24R41R35R13R25L26L20L14R39R50L3L49R45L45R3R41L7R43L49R12R22L10R30R11R11L42R35L46R22L44R9L25L26L27L28R26R41L6R25L17L3R45L3R30L38L31L21R38R14R2R33L20L32R4L17R34L13L18R33L10L44R47R8L31R27L22L46R33R22R43L20L44R35L25L25R21L1L22L6R30R33L31L27R45L23L35R16L22R24L1R30R49L24L17R13L11L1L2R30R38L15L15L35R14L34R30R48R42L12R27R3L41R5R13L32L46R32L17R7L5R28L15L5L8L42L33L29L24L33R18L19R10L26L6R28L42R7R18R46L26R11R31R3R17L5R5L40L13L28R2L50R7L8L33L15L12L27L3L1L5L50R3R16R20R41R12R24L42R29L31R39L47R4L10R7L37L36L13R12R4R34R43L31L16R12L46R6R50L13R37R30L30R17L1L22R26R7L25L8R5R13R21L50R23R22R6L50L33L24L15L13L46R18R27L12L8L28R1L23R34R7R36R5L48L37L50R30R38R28R16L34L26R48R39L29R25R44R25L38R27R33L6L15R45L48L20R46L33L29R43L42L9R21R33L47R18L27L32R32L29L22R35L49R50R42R39R38R43R35L33R45R32L40R7L11L12R50L24R5L19R26L24L10L33R8R9L14R19L47R31R39L19R45R26L21L29L34L39R49L25R40L29R28L8L20L28R36L1L39R45R17L37R29R25L2L19L23R12L18R11R40R15L13R19R19L3R41L45R15R2R4R22R1L18R50R38R1L48L14L6L7L24L23R24R29R37L34R32R36R40L6L22L44R14R9L37L29R34L28R39R12L23R40R47R25L6L16R13L23R11L50L27L24R34R40R22R39R8R44R13R15L42R3R30R6R22R37R43R34L46R1R5L36R30L18R48L25L50R23R29R11L28L48R40L2L34R35L34L46L2L32L50L16R28R18R27L18R11R27L5L33L36R15R5L41L4R27R37L14R27L21R49R20R48R37L37L23R39L21R31R26L20L15R19L37R38R12L29L6L50R16L28L50L47L1R28R44R47R42R44L18R13R39L27R26L34R7R27L27R2R5L32L23R31R43R39R38R28L29R8R39L45R37L13L28L3R33L33L15L31R46R20R1R19L20R39L45R9R13L45L14L10R4L43L40R19R29L38L20R18L35L35L48L36R28L23R39L15L41L34R16L44L17R11R47R3R21R38L7R8R33L28R7R37R24R5R41R42L38L28R14R4R31R33L8R11L6L17R4R38R3L17L16R5R22R42L14R27R25L7L39L50L50R2L32R14R45L13L19R50R45R45L42L26R13L42R26R24L41R39R23R1L31R4L13R44L32R14R38R42R14L40R21R14R38L34L2L14R31)";
