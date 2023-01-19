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
#include <fstream> // std::ofstream

extern char const* pTest;
extern char const* pData;
namespace test {bool does_comply();}

using Result = long int;
using Answers = std::vector<std::pair<std::string,Result>>;

namespace dim2 {

  enum COORD : int {
     x=0
    ,y=1
  };

  using Vector = std::array<int,2>; // x,y,z

  std::ostream& operator<<(std::ostream& os,Vector const& v) {
    os << '[';
    for (int c=0;c<v.size();++c) {
      if (c>0) os << ',';
      os << v[c];
    }
    os << ']';
    return os;
  }

  const Vector X_UNIT{1,0};
  const Vector Y_UNIT{0,1};

  Vector& operator+=(Vector& v1,Vector const& v2) {
    for (int i=0;i<v1.size();++i) v1.at(i) += v2.at(i);
    return v1;
  }

  Vector operator+(Vector const& v1,Vector const& v2) {
    auto result = v1;
    result += v2;
    return result;
  }

  Vector& operator-=(Vector& v1,Vector const& v2) {
    for (int i=0;i<v1.size();++i) v1.at(i) -= v2.at(i);
    return v1;
  }

  Vector operator-(Vector const& v1,Vector const& v2) {
    auto result = v1;
    result -= v2;
    return result;
  }

  Vector operator-(Vector const& v1) {
    auto result = v1;
    for (int i=0;i<result.size();++i) result.at(i) = -result.at(i);
    return result;
  }

  const Vector RIGHT{0,1};
  const Vector DOWN{1,0};
  const Vector LEFT{0,-1};
  const Vector UP{-1,0};

  using Matrix = std::array<Vector,3>;

  // struct Matrix {
  //   std::vector<Vector> rows;
  //   Vector operator*(Vector const& v) const {
  //     Vector result{};
  //     result[ROW] = rows[0][ROW]*v[ROW] + rows[0][COL]*v[COL];
  //     result[COL] = rows[1][ROW]*v[ROW] + rows[1][COL]*v[COL];
  //     return result;
  //   }
  // };

  Vector operator*(Matrix const& m,Vector const& v) {
    Vector result{};
    result[0] = m[0][0]*v[0] + m[0][1]*v[1];
    result[1] = m[1][0]*v[0] + m[1][1]*v[1];
    return result;
  }

  const Matrix NORM_Z_TURN_RIGHT{{
    {0,1}
    ,{-1,0}
  }};

  const Matrix NORM_Z_TURN_LEFT{{
    {0,-1}
    ,{1,0}
  }};

} // namespace dim2
using dim2::operator<<;

namespace dim3 {
  using Vector = std::array<int,3>;

  std::ostream& operator<<(std::ostream& os,Vector const& v) {
    os << '[';
    for (int c=0;c<v.size();++c) {
      if (c>0) os << ',';
      os << v[c];
    }
    os << ']';
    return os;
  }

  enum COORD : int {
     x=0
    ,y=1
    ,z=2
  };

  const Vector X_UNIT{1,0,0};
  const Vector Y_UNIT{0,1,0};
  const Vector Z_UNIT{0,0,1};

  // bool operator==(Vector const& v1,Vector const& v2) {
  //   return (v1.at(0)==v2.at(0) and v1.at(1)==v2.at(1) and v1.at(2)==v2.at(2));
  // }

  Vector& operator+=(Vector& v1,Vector const& v2) {
    for (int i=0;i<v1.size();++i) v1.at(i) += v2.at(i);
    return v1;
  }

  Vector operator+(Vector const& v1,Vector const& v2) {
    auto result = v1;
    result += v2;
    return result;
  }

  Vector& operator-=(Vector& v1,Vector const& v2) {
    for (int i=0;i<v1.size();++i) v1.at(i) -= v2.at(i);
    return v1;
  }

  Vector operator-(Vector const& v1,Vector const& v2) {
    auto result = v1;
    result -= v2;
    return result;
  }

  Vector operator-(Vector const& v1) {
    auto result = v1;
    for (int i=0;i<result.size();++i) result.at(i) = -result.at(i);
    return result;
  }

  int inner_product(Vector const& v,Vector const& w) {
    int result{};
    for (int i=0;i<v.size();++i) result += v.at(i)*w.at(i);
    return result;
  }

  using Matrix = std::array<Vector,3>;
  Vector operator*(Matrix const& m, Vector const& v) {
    Vector result{};
    for (int r=0;r<m.size();r++) {
      for (int c=0;c<m[0].size();c++) {
        result[r] += m[r][c]*v[c];
      }
    }
    return result;
  }
  Matrix operator*(Matrix const& m1, Matrix const& m2){
    Matrix result{};
    for (int r=0;r<m1.size();r++) {
      for (int c=0;c<m2[0].size();c++) {
        for (int k=0;k<m1[0].size();k++) {
          result[r][c] += m1[r][k]*m2[k][c];
        }
      }
    }
    return result;
  }

  std::ostream& operator<<(std::ostream& os,Matrix const& m) {
    for (int r=0;r<m.size();++r) {
      if (r>0) os << "\n";
      os << "[";
      for (int c=0;c<m[0].size();++c) {
        if (c>0) os << ',';
        os << std::setw(4) << m[r][c];
      }
      os << "]";
    }
    return os;
  }

  // Helper class to create rotations that turn any 3d object into any of its 24 possible orientations in 3D space.
  // Observation: Imagine a cube and each normal vector of the faces of that cube.
  // Each such normal vector defines a possible orientation of the cube
  // For each such face normal, we can rotate around the face normal to get an additional 4 orientation
  // In total 6 faces x 4 rotations = 24 orientations
  class Rotations {
  public:

    static const int X0_Y0_Z0{0};
    static const int X90_Y0_Z0{1};
    static const int X180_Y0_Z0{2};
    static const int X270_Y0_Z0{3};
    static const int Z90_Y0_X0{4};
    static const int Z90_Y90_X0{5};
    static const int Z90_Y180_X0{6};
    static const int Z90_Y270_X0{7};
    static const int Z180_X0_Y0{8};
    static const int Z180_X90_Y0{9};
    static const int Z180_X180_Y0{10};
    static const int Z180_X270_Y0{11};
    static const int Z270_Y0_X0{12};
    static const int Z270_Y90_X0{13};
    static const int Z270_Y180_X0{14};
    static const int Z270_Y270_X0{15};
    static const int Y90_Z0_X0{16};
    static const int Y90_Z90_X0{17};
    static const int Y90_Z180_X0{18};
    static const int Y90_Z270_X0{19};
    static const int Y270_Z0_X0{20};
    static const int Y270_Z90_X0{21};
    static const int Y270_Z180_X0{22};
    static const int Y270_Z270_X0{23};

    Rotations() {
      // Generate all 24 rotations
      for (auto i : {1,2,3,4,5,6}) {
        // Loop over all outgoing normal vectors of a cube (starting with the x axis)
        switch (i) {
          // create clockwise rotations around normal vectors of cube faces
          /* 0 */ case 1: rotations.push_back(RUNIT); break;                        // x
          /* 4 */ case 2: rotations.push_back(RZ90*rotations[0]); break;            // x -> y   z90
          /* 8 */ case 3: rotations.push_back(RZ90*RZ90*rotations[0]);break;         // x -> -x  z180
          /* 12 */ case 4: rotations.push_back(RZ90*RZ90*RZ90*rotations[0]); break;   // x-> -y   z270
          /* 16 */ case 5: rotations.push_back(RY90*rotations[0]);break;            // x -> -z  y90
          /* 20 */ case 6: rotations.push_back(RY90*RY90*RY90*rotations[0]); break; // x -> z   y270
        }
        for (auto j : {2,3,4}) {
          // for each cube face normal vector, create the three alternative rotations
          switch (i) {
            /* 1,2,3    */ case 1: rotations.push_back(RX90*rotations.back()); break; // rotate x   x90,x180,x270
            /* 5,6,7  */ case 2: rotations.push_back(RY90*rotations.back()); break; // rotate y     z90 -> y90,y180,y270
            /* 9,10,11 */ case 3: rotations.push_back(RX90*rotations.back()); break; // rotate -x   z180 -> x90,x180,x270
            /* 13,14,15 */ case 4: rotations.push_back(RY90*rotations.back()); break; // rotate -y  z270 -> y90,y180,y270
            /* 17,18,19 */ case 5: rotations.push_back(RZ90*rotations.back()); break; // rotate -z  y90 -> z90,z180,z270
            /* 21,22,23 */ case 6: rotations.push_back(RZ90*rotations.back()); break; // rotate z   y270 -> z90,z180,z270
          }
        }
      }
    }

    // See 3d_rotations_matrices.png (https://github.com/kjelloh/advent_of_code_2021/tree/main/day19 )
    // We can use these to generate all 24 possible rotations
    // 6 for each "facing" out through a face of a die
    // 4 rotations around the face normal
    // 24 in total 
    static constexpr Matrix const RUNIT = {{ // No rotation
      {1,0,0}
      ,{0,1,0}
      ,{0,0,1}}};
    static constexpr Matrix const RX90 = {{
      {1,0,0}
      ,{0,0,-1}
      ,{0,1,0}}};
    static constexpr Matrix const RY90 = {{
      {0,0,1}
      ,{0,1,0}
      ,{-1,0,0}}};
    static constexpr Matrix const RZ90 = {{
      {0,-1,0}
      ,{1,0,0}
      ,{0,0,1}}};

    auto begin() const {return rotations.begin();}
    auto end() const {return rotations.end();}
    auto size() {return rotations.size();}
    auto const& operator[](auto i) const {return rotations.at(i);}

    static void test() {
      Rotations rots{};
      Vector x{1,0,0},y{0,1,0},z{0,0,1};
      for (int i=0;i<rots.size();++i) {
        std::cout << "\nrot:" << i << rots[i]*x << " " << rots[i]*y << " " << rots[i]*z;
      }
      assert((rots[Rotations::X0_Y0_Z0]*Vector{1,3,7}==Vector{1,3,7}));
      assert((rots[Rotations::X90_Y0_Z0]*y==z));
      assert((rots[Rotations::X180_Y0_Z0]*y==-y));
      assert((rots[Rotations::X270_Y0_Z0]*y==-z));
      assert((rots[Rotations::Z90_Y0_X0]*x==y));
      assert((rots[Rotations::Z90_Y90_X0]*y==z));
      assert((rots[Rotations::Z90_Y180_X0]*y==x));
      assert((rots[Rotations::Z90_Y270_X0]*y==-z));
      assert((rots[Rotations::Z180_X0_Y0]*x==-x));
      assert((rots[Rotations::Z180_X90_Y0]*y==-z));
      assert((rots[Rotations::Z180_X180_Y0]*y==y));
      assert((rots[Rotations::Z180_X270_Y0]*y==z));
      assert((rots[Rotations::Z270_Y0_X0]*y==x));
      assert((rots[Rotations::Z270_Y90_X0]*y==-z));
      assert((rots[Rotations::Z270_Y180_X0]*y==-x));
      assert((rots[Rotations::Z270_Y270_X0]*y==z));
      assert((rots[Rotations::Y90_Z0_X0]*x==-z));
      assert((rots[Rotations::Y90_Z90_X0]*z==y));
      assert((rots[Rotations::Y90_Z180_X0]*z==-x));
      assert((rots[Rotations::Y90_Z270_X0]*z==-y));
      assert((rots[Rotations::Y270_Z0_X0]*z==-x));
      assert((rots[Rotations::Y270_Z90_X0]*z==-y));
      assert((rots[Rotations::Y270_Z180_X0]*z==x));
      assert((rots[Rotations::Y270_Z270_X0]*z==y));
    }

  private:
    std::vector<Matrix> rotations{};
  };

  const Rotations ROTATIONS{};

  /*
    Define a Square in 3D space as follows.
                                        
                       3                   tlf ---> col (y)
                0------------ 3            /|
               /             /            / |
            0 / Square   / 2             /  |
             /             /            /   v
            1-------------2            z    row (x)
                  1

      - each square has four corners 0...3 and four edges 0..3
      - Each face square now gets a normal "up" in the z direction (3D coordinate system right hand rule  x cross-product y = z)
      - The square Corners are numbered anti clockwise (right hand rule)
      - The square edges are numbered anti clock-wise (right hand rule)
      - Each square position in 3D space is defined by the position of corner 0.
  */

  using Point = Vector;
  using Edge = std::pair<Point,Point>;

  struct Square {
    Vector pos; // Host coordinate system position
    std::array<Point,4> corners{}; // Local coordinate system corners
    std::array<Edge,4> edges{}; // local coordinate system edges
  };

  std::ostream& operator<<(std::ostream& os,Square const& square) {
    os << "\n[" << square.corners[0] << ":" << square.pos << "\t" << square.corners[3] << "]"; 
    for (int n=0;n<4;++n) os << "\n  " << n << ":" << square.edges[n].first << " -> " << square.edges[n].second;
    os << "\n[" << square.corners[1] << "\t\t" << square.corners[2] << "]"; 
    return os;
  }

  Square to_xy_square(Vector const& pos,int side_size) {
    Square result{
       .pos = pos
      ,.corners{{
         {0,0,0}
        ,{0,side_size,0}
        ,{side_size,side_size,0}
        ,{side_size,0,0}}}
      ,.edges = {{
         {{0,0,0},{0,side_size,0}}
        ,{{0,side_size,0},{side_size,side_size,0}}
        ,{{side_size,side_size,0},{side_size,0,0}}
        ,{{side_size,0,0},{0,0,0}}
      }}
    };
    return result;
  }

  using Squares = std::vector<Square>;

  using EnhancedVector = std::array<int,4>;
  using EnhancedMatrix = std::array<EnhancedVector,4>;

  EnhancedVector operator*(EnhancedMatrix const& m,EnhancedVector const& v) {
    EnhancedVector result{};
    for (int mr=0;mr<m.size();++mr) {
      for (int mc=0;mc<v.size();++mc) {
        result[mr] += m[mr][mc]*v[mc];
      }
    }
    return result;
  }

  std::ostream& operator<<(std::ostream& os,EnhancedVector const& v) {
    os << "[";
    for (int c=0;c<v.size();++c) {
      if (c>0) os << ',';
      os << v[c];
    }
    os << "]";
    return os;
  }

  std::ostream& operator<<(std::ostream& os,EnhancedMatrix const& m) {
    for (int r=0;r<m.size();++r) {
      if (r>0) os << "\n";
      os << "[";
      for (int c=0;c<m[0].size();++c) {
        if (c>0) os << ',';
        os << std::setw(4) << m[r][c];
      }
      os << "]";
    }
    return os;
  }
  
  // See https://en.wikipedia.org/wiki/Transformation_matrix#Affine_transformations
  EnhancedMatrix to_transformation_matrix(Matrix const& rotation,Vector const& translation) {
    EnhancedMatrix result{};
    for (int mr=0;mr<result.size();++mr) {
      for (int mc=0;mc<result[0].size();++mc) {
        if (mr<rotation.size() and mc<rotation[0].size()) result[mr][mc] = rotation[mr][mc];
        else if (mc==(result[0].size()-1) and mr<translation.size()) result[mr][mc]=translation[mr];
        else if (mr==(result.size()-1) and mc<rotation[0].size()) result[mr][mc]=0;
        else result[mr][mc]=1;
      }
    }
    return result;
  }

  EnhancedMatrix to_inverse(EnhancedMatrix const& am) {
    EnhancedMatrix result{};
    std::cerr << "\nto_inverse NOT YET IMPLEMENTED";
    return result;
  }

  /*
  Define the cube with one corner at top_left_front (tlf) and a diagonally opposite corner bottom_right_back (brb).

                *-------------*
               /|            /|               
              / |    1      / |              
             /  |      5   /  |             
          tlf-------------* 3 |            tlf ---> col (y)
            | 2 *---------|-- brb          /|
            |  /   0      |  /            / |
            | /      4    | /            /  v
            |/            |/            /  row (x)
            *-------------*            z

    - Number the faces counter clockwise from tlf with face 0 in row,col plane.
    - Number opposite faces so that the sum is 5
    - Place the coordinate system at tlf with column as x, row as y and "sheet" as z
  */


  // The hull of a cube that is the result of folding cube faces arranged on a 2D surface
  // The FoldedCube hull keeps a record of where the folds are, to enable transformation
  // between the cube hull reference frame and the unfolded cube reference frame.
  // The Hull reference frame is corner 0 of the first face in the faces array.
  struct FoldedCubeHull {
    using Cubefaces = std::array<Square,6>;
    Cubefaces faces{};
  };

  // Folds a collection of 6 squares in the xy-plane into a convex hull of a cube
  class CubeHullFolder {
  public:
    void push_back(Square const& square) {m_anonymous_faces.push_back(square); std::cout << "\n" << square;}
    FoldedCubeHull hull() {
      // Folds anonymous faces into a cube
      return FoldedCubeHull{};
    }
  private:
    Squares m_anonymous_faces{};
  };

  struct FoldedCubeHullWalker {
    std::reference_wrapper<FoldedCubeHull const> hull;
    int face_index{0};
    Vector face_pos{0,0,0};
    Vector face_delta{0,1,0};
    FoldedCubeHullWalker(FoldedCubeHull const& hull) : hull{hull} {}
    FoldedCubeHullWalker& turn_left() {
      face_delta = dim3::ROTATIONS[dim3::Rotations::Z90_Y0_X0]*face_delta;
      return *this;
    }
    FoldedCubeHullWalker& turn_right() {
      face_delta = dim3::ROTATIONS[dim3::Rotations::Z270_Y0_X0]*face_delta;
      return *this;
    }
    FoldedCubeHullWalker& operator++() {
      face_pos += face_delta;
      return *this;
    }
    Vector frame_0_pos() const {
      Vector result{};
      std::cerr << "\nFoldedCubeHullWalker::frame_0_pos NOT YET IMPLEMENTED";
      return result;
    }
    Vector frame_0_delta() const {
      Vector result{};
      std::cerr << "\nFoldedCubeHullWalker::frame_0_delta NOT YET IMPLEMENTED";
      return result;
    }
  };

} // namespace dim3
using dim3::operator<<;
using dim3::operator+;
using dim3::operator*;

using namespace dim2;

auto ROW = COORD::x;
auto COL = COORD::y;

using Strings = std::vector<std::string>;
std::ostream& operator<<(std::ostream& os,Strings const& strings) {
  for (int row=0;row<strings.size();++row) {
    if (row>0) os << "\n";
    os << std::quoted(strings[row]);
  }
  return os;
}

using Step = std::pair<int,char>;
using Path = std::vector<Step>;

using Model = std::pair<Strings,Path>;

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
    // make all rows a uniform max length
    auto& strings = result.first;
    auto max_width = std::accumulate(strings.begin(),strings.end(),std::size_t{0},[](auto acc,auto const& s) { return std::max(acc,s.size());});
    std::for_each(strings.begin(),strings.end(),[&max_width](auto& s){
      auto diff =  max_width - s.size();
      s += std::string(diff,' ');
    });
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

class Face {
public:
  Vector top_left{};
  Vector bottom_right() const {return top_left + Vector{static_cast<int>(rows[0].size()),static_cast<int>(rows.size())};}
  Strings rows{};
  int side_size() const {return bottom_right()[COORD::x] - top_left[COORD::x] + 1;}
private:
  Vector m_bottom_right{};
};
using Faces = std::vector<Face>;

Faces to_faces(Strings& strings) {
  auto result = Faces{};

  // Map row,col on grid defined by strings to faces of a cube
  // We need to figure out face size and face positions on the flat (unfolded) surface.
  // The leftmost map non-space character in the input is on face 0 row 0 col 0.
  // Faces returned are in the order encountered scanning left to right, up to down of strings.
  std::set<int> widths{};
  for (int grid_row=0;grid_row<strings.size();++grid_row) {      
    auto const& line = strings[grid_row];
    auto first = line.find_first_not_of(' ',0);
    auto end = std::min(line.size(),line.find_first_of(' ',first));
    auto width = end-first;
    assert(width<line.size());
    widths.insert(width);
  }
  int cube_side{*widths.begin()};
  for (auto width : widths) {
    cube_side = std::gcd(cube_side,width);
  }
  std::cout << "\nCUBE SIDE SIZE " << cube_side;

  // Loop over grid of unfolded faces
  // int offset{0};
  auto vert_cube_side_counts = strings.size() / cube_side;
  for (int r=0;r<vert_cube_side_counts;++r) {
    auto hor_cube_side_counts = strings[0].size() / cube_side;
    for (int c=0;c<hor_cube_side_counts;++c) {
      result.push_back(Face{}); result.back().top_left = Vector{r*cube_side,c*cube_side};
      std::cout << "\n";
      for (int dr=0;dr<cube_side;++dr) {
        auto row = r*cube_side+dr;
        auto const& line = strings[row];
        auto first = c*cube_side;
        std::cout << "\n" << std::quoted(line) << " " << r << " " << c << " " << dr << " " << first  << " " <<  result.back().top_left << std::flush;
        if (first+cube_side <= line.size()) {
          auto snippet = line.substr(first,cube_side);
          std::cout << " " << std::quoted(snippet);
          if (snippet.find_first_not_of(' ')==0) {
            result.back().rows.push_back(snippet);
            std::cout << " :)";
          }
        }          
      }
      if (result.back().rows.size()==0) result.pop_back();
    }
  }
  assert(result.size()==6);

  if (true) {
    for (int face_id=0;face_id<result.size();++face_id) {
      std::cout << "\nface_id:" << face_id << " " << result.at(face_id).top_left;
      for (auto const& line : result[face_id].rows) {
        std::cout << "\n\t" << std::quoted(line);
      }
    }
  }
  return result;
}

struct StringsWalker {
  std::reference_wrapper<Strings const> strings;
  Vector top_left{0,0};
  Vector bottom_right{-1,-1}; // empty  
  Vector face_pos{0,0};
  Vector face_delta{0,1};
  StringsWalker(Strings const& rows) : strings{rows} {
    bottom_right[ROW] = strings.get().size()-1;
    if (bottom_right[COL] < 0) bottom_right[COL] = strings.get()[0].size()-1;
    auto first_col{0};
    for (;first_col<bottom_right[COL];++first_col) {
      std::cout << "\n[row:0,col:" << first_col << "]=" << strings.get()[0][first_col];
      if (strings.get()[0][first_col]!=' ') break;
    }
    face_pos = Vector{0,first_col};    
  }
  StringsWalker& turn_left() {
    face_delta = dim2::NORM_Z_TURN_LEFT*face_delta;
    return *this;
  }
  StringsWalker& turn_right() {
    face_delta = dim2::NORM_Z_TURN_RIGHT*face_delta;
    return *this;
  }
  bool in_frame(Vector const& pos) const {
    return (pos[ROW]>=top_left[ROW] and pos[ROW]<=bottom_right[ROW] and pos[COL]>=top_left[COL] and pos[COL]<=bottom_right[COL]);
  }
  char at(Vector const& pos) const {
    if (in_frame(pos)) return strings.get()[pos[ROW]][pos[COL]];
    else return ' ';
  }
  StringsWalker& operator++() {
    Vector next{face_pos+face_delta}; // optimistic
    if (at(next)==' ') {
      // wrap around
      next -=face_delta; // back off
      while (at(next)!=' ') next -=face_delta; // back track on map
      next += face_delta; // back off
      // std::cout << "\n\tat(" << result << "):" << at(result);
    }
    if (at(next)=='#') next = face_pos;
    // std::cout << "\n\tat(" << result << "):" << at(result);
    face_pos = next;
    return *this;
  }
  Vector frame_0_pos() const {
    return face_pos;
  }
  Vector frame_0_delta() const {
    return face_delta;
  }
};

class Traveler {
public:
  Traveler(Strings const& strings,Path const path) : m_strings{strings},m_path{path},rendered{strings} {}
  Vector walk_as_if_flat() {
    // Walk the flat map using the rules of wrap around on the flat grid
    // return the position on the flat strings map
    StringsWalker walker{m_strings};
    walk_the_path(walker);
    return walker.frame_0_pos();
  }
  Vector walk_as_if_cube() {
    // Fold the flat map defined by strings into a cube
    // Walk the cube
    // return the position on the flat strings map
    Faces faces = to_faces(m_strings);
    dim3::CubeHullFolder folder{};
    for (auto const& face : faces) {
      dim3::Vector pos{face.top_left[0],face.top_left[1],0};
      folder.push_back(dim3::to_xy_square(pos,face.side_size()));
    }
    auto hull = folder.hull();
    dim3::FoldedCubeHullWalker walker{hull};
    walk_the_path(walker);
    auto frame_0_pos = walker.frame_0_pos();
    auto row = frame_0_pos[ROW];
    auto col = frame_0_pos[COL];
    dim2::Vector strings_pos{row,col};
    return strings_pos;
  }
private:
  friend std::ostream& operator<<(std::ostream& os,Traveler const& traveler);
  Strings m_strings{};
  Path m_path;
  Strings rendered{};
  template <class Walker>
  char char_of_direction(Walker const& walker) {
    auto delta = walker.frame_0_delta();
    if (delta[ROW]==0) return (delta[COL]>0)?'>':'<';
    else return (delta[ROW]>0)?'v':'^';    
  }
  template <class Walker>
  void take_step(Walker& walker) {
    auto next{walker}; ++next;
    auto frame_0_pos = walker.frame_0_pos();
    auto row = frame_0_pos[ROW];
    auto col = frame_0_pos[COL];
    dim2::Vector strings_pos{row,col};
    if (m_strings[row][col]!='#') walker=next;
    rendered[row][col] = char_of_direction(walker);
  }
  template <class Walker>
  void turn(Walker& walker,char dir) {
    switch (dir) {
      case 'R': walker.turn_right();break;
      case 'L': walker.turn_left(); break;
      default: break;
    }    
    auto frame_0_pos = walker.frame_0_pos();
    auto row = frame_0_pos[ROW];
    auto col = frame_0_pos[COL];
    rendered[row][col] = char_of_direction(walker);
  }
  template <class Walker>
  void walk_the_path(Walker& walker) {
    for (auto const& step : m_path) {
      for (int i=0;i<step.first;++i) take_step(walker);
      turn(walker,step.second);
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
      Traveler traveler{data_model.first,data_model.second};
      auto pos = traveler.walk_as_if_flat();
      result = 1000*(pos[ROW]+1) + 4*(pos[COL]+1);
      std::cout << traveler;
      return result;
  }
}

namespace part2 {

  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << data_model;
      Traveler traveler{data_model.first,data_model.second};
      auto pos = traveler.walk_as_if_cube();
      result = 1000*(pos[ROW]+1) + 4*(pos[COL]+1);
      std::cout << traveler;
      return result;
  }
}

namespace test {
  using dim3::operator<<;

  /* Part 1 example
  "        >>^#    "
  "        .#..    "
  "        #...    "
  "        <<<<    "
  "...#.....^.#    "
  "........#^..    "
  "..#....#.^..    "
  ".........^#.    "
  "        >^<#.>>>"
  "        >>^..#>>"
  "        .#^....."
  "        ..^...#."
  */  

  struct CSV {
    dim3::Vector v{};
  };

  std::ostream& operator<<(std::ostream& os,CSV const& csv) {
    os << "\n";
    for (int i=0;i<csv.v.size();++i) {
      if (i>0) os << ',';
      os << csv.v[i];
    }
    return os;
  }

  bool does_comply() {
    bool result{true};
    if (result) {
      result = false;
      dim3::EnhancedVector v0{0,50,0,1};
      std::cout << "\n" << v0;
      dim3::Vector translation{0,50,0};
      std::cout << "\n" << translation;
      auto T01 = dim3::to_transformation_matrix(dim3::ROTATIONS[dim3::Rotations::X270_Y0_Z0],translation);
      std::cout << "\n" << T01;
      auto v1 = T01*v0;
      std::cout << "\n" << v1;
    }
    if (result) {
      // Generate x,y,z csv for input flat map
      std::stringstream in{ pData };
      auto data_model = parse(in);
      Faces faces = to_faces(data_model.first);
      dim3::Squares squares{};
      for (auto const& face : faces) {
        dim3::Vector pos{face.top_left[0],face.top_left[1],0};
        squares.push_back(dim3::to_xy_square(pos,face.side_size()));
      }
      std::ofstream of{"day22.csv"};
      for (auto const& square : squares) {
        auto pos = square.pos; 
        of << CSV{pos};
        for (auto v : square.corners) of << CSV{pos+v};
      }

    }
    return result;
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
    // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 1     ",part1::solve_for(pData)});
    // exec_times.push_back(std::chrono::system_clock::now());
    answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
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
