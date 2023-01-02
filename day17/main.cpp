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

using Jets = std::string;

using Model = Jets;

// Vector on system row:max..0, col:0..max
struct Vector {
  Result row;
  Result col;
  bool operator>=(Vector const& other) const {
    if (row==other.row) return col >= other.col;
    else return row < other.row; // lower index = higher row
  }
  bool operator<=(Vector const& other) const {
    if (row==other.row) return col <= other.col;
    else return row > other.row; // higher index = lower row
  }
  Vector& operator+=(Vector const& delta) {
    row += delta.row;
    col += delta.col;
    return *this;
  }
};

std::ostream& operator<<(std::ostream& os,Vector const& v) {
  os << "[row:" << v.row << ",col:" << v.col << "]";
  return os;
}

struct Sprite {
  using Row = std::string;
  using Rows = std::vector<Row>;
  static constexpr Vector LEFT{.row=0,.col=-1};
  static constexpr Vector DOWN{.row=-1,.col=0};
  static constexpr Vector RIGHT{.row=0,.col=1}; 
  static constexpr Vector UP{.row=1,.col=0}; 
  Sprite(Vector const& pixel_top_left, Rows const& rows) 
    :  m_frame_top_left{pixel_top_left}
      ,m_frame_bottom_right{.row=static_cast<Result>(pixel_top_left.row-rows.size()+1),.col=static_cast<Result>(pixel_top_left.col + rows[0].size()-1)}
      ,m_rows{} {
    for (auto iter=rows.rbegin();iter!=rows.rend();++iter) {
      m_rows.push_back(*iter);
    }
    std::cout << "\nSprite(pixel_top_left:" << pixel_top_left << " row[0]:" << m_rows[0] << ") frame_top_left:" << m_frame_top_left << " m_frame_bottom_right:" << m_frame_bottom_right;
  }
  bool in_frame(Vector const& pos) const {
    return (pos >= m_frame_top_left and pos <= m_frame_bottom_right);
  }
  char& at(Vector const& pos) {
    std::cout << "\nat(" << pos << ")";
    static char NULL_CHAR{'?'};
    auto row = m_frame_top_left.row - pos.row;
    if (in_frame(pos)) {
      return m_rows[pos.row - m_frame_bottom_right.row][pos.col - m_frame_top_left.col]; // rows max..0, cols 0..max
    }
    else {
      std::cerr << "\nSprite::at(" << pos << ") ERROR, out-of-bounds top_left:" << m_frame_top_left << " bottom_right:" << m_frame_bottom_right;
    }
    return NULL_CHAR;
  }
  char at(Vector const& pos) const {
    auto result = const_cast<Sprite*>(this)->at(pos);
    return result;
  };

  char operator[](Vector const& pos) const {
    return this->at(pos);
  }
  Sprite& operator+=(Vector const& delta) {
    m_frame_top_left += delta;
    m_frame_bottom_right += delta;
    return *this;
  }
  Sprite& push_row(Row const& entry) {
    std::cout << "\npush_row(" << std::quoted(entry) << ")";
    m_rows.push_back(entry);
    ++m_frame_top_left.row;
    std::cout << " m_frame_top_left:" << m_frame_top_left << " m_frame_bottom_right:" << m_frame_bottom_right;
    return *this;
  }
  Sprite& operator+=(Sprite const& other) {
    std::cout << "\nSprite::operator+=(Sprite const& other)";
    // merge other with us
    const std::string empty_row(m_frame_bottom_right.col - m_frame_top_left.col+1,'.');
    while (other.m_frame_top_left.row > m_frame_top_left.row) push_row(empty_row);
    for (auto row=other.m_frame_top_left.row;row>=other.m_frame_bottom_right.row;--row) {
      for (auto col=other.m_frame_top_left.col;col<=other.m_frame_bottom_right.col;++col) {
        Vector pos{.row=row,.col=col};
        this->at(pos) = other.at(pos);
      }
    }

    return *this;
  }
  bool does_collide_with(Sprite const& other) {
    static int call_count{};
    bool result{false};
    std::cout << "\ndoes_collide_with m_frame_top_left:" << m_frame_top_left << " m_frame_bottom_right:" << m_frame_bottom_right; 
    for (auto row=m_frame_top_left.row;row>=m_frame_bottom_right.row and !result;--row) {
      for (auto col=m_frame_top_left.col;col<=m_frame_bottom_right.col and !result;++col) {
        Vector pos{.row=row,.col=col};
        if (this->at(pos)!='.') {
          if (other.in_frame(pos)) {
            std::cout << "\nother.in_frame TRUE";
            result = result or other.at(pos)!='.';
            std::cout << "\npos:" << pos << " this:" << this->at(pos) << " other:" << other.at(pos) << " does_collide:" << result << std::flush;
          }
        } 
      }
    }
    return result;
  }
  Vector m_frame_top_left;
  Vector m_frame_bottom_right;
  Rows m_rows;
};

std::ostream& operator<<(std::ostream& os,Sprite const& sprite) {
  int row = sprite.m_rows.size()-1;
  for (auto iter=sprite.m_rows.rbegin();iter != sprite.m_rows.rend();++iter) {
    if (iter!=sprite.m_rows.rbegin()) std::cout << "\n";
    std::string indent(sprite.m_frame_top_left.col,'.');
    os << indent << *iter << " : " << row--;
  }
  return os;
}

struct Rock {
  Sprite m_sprite;
  Rock(Vector const& pixel_top_left, Sprite::Rows const& rows) : m_sprite{pixel_top_left,rows} {}
  auto const& top_left() const {return m_sprite.m_frame_top_left;}
  auto const& bottom_right() const {return m_sprite.m_frame_bottom_right;}
};

const std::vector<Sprite::Rows> ROCKS {
   { "####"}
  ,{ ".#."
    ,"###"
    ,".#."}
  ,{ "..#"
    ,"..#"
    ,"###"}
  ,{ "#"
    ,"#"
    ,"#"
    ,"#"}
  ,{ "##"
    ,"##"}
};

class Chamber {
public:
  auto const& top_left() const {return m_sprite.m_frame_top_left;}
  auto const& bottom_right() const {return m_sprite.m_frame_bottom_right;}
  Chamber(Jets const& jets) : m_jets{jets} {}
  bool can_move_left(Rock rock) {
    rock.m_sprite += Sprite::LEFT;
    return (top_left().col < rock.top_left().col) and !rock.m_sprite.does_collide_with(this->m_sprite);
  }
  bool can_move_right(Rock rock) {
    std::cout << "\ncan_move_right(rock::bottom_right:" << rock.bottom_right() << ") this->bottom_right:" << bottom_right(); 
    rock.m_sprite += Sprite::RIGHT;
    return (rock.bottom_right().col < bottom_right().col) and !rock.m_sprite.does_collide_with(this->m_sprite);
  }
  bool can_move_down(Rock rock) {
    bool result{};
    rock.m_sprite += Sprite::DOWN;
    result = !rock.m_sprite.does_collide_with(this->m_sprite);
    return result;
  }

  Rock to_moved_rock(Rock const& rock,char jet) {
    Rock result{rock};
    switch (jet) {
      case '>': if (can_move_right(result)) result.m_sprite += Sprite::RIGHT; break;
      case '<': if (can_move_left(result)) result.m_sprite += Sprite::LEFT; break;
    }
    return result;
  }
  Chamber& place_rock(Rock const& rock) {
    std::cout << "\nplace_rock(" << rock.top_left() << ")";
    m_sprite += rock.m_sprite;
    std::cout << "\nadd chamber sides";
    for (auto row=rock.top_left().row;row>=rock.bottom_right().row;--row) {
      m_sprite.at(Vector{.row=row,.col=0}) = '|';
      m_sprite.at(Vector{.row=row,.col=m_sprite.m_frame_bottom_right.col}) = '|';
    }
    return *this;
  }
  Chamber& drop() {
    auto rock_index = (drop_count++ % ROCKS.size());
    auto rows = ROCKS[rock_index];
    Vector top_left{.row=static_cast<Result>(m_sprite.m_frame_top_left.row + rows.size() -1 + 4),.col=3}; // col:0 is left boundary
    Rock rock(top_left,rows);
    std::cout << "\n" << rock.m_sprite << std::flush;
    bool is_falling{true};
    while (is_falling) {
      rock = to_moved_rock(rock,m_jets[jet_index]);
      std::cout << "\ngusted to" << rock.m_sprite.m_frame_top_left << "\n" << rock.m_sprite << std::flush;
      if (can_move_down(rock)) {
        rock.m_sprite += Sprite::DOWN;
        std::cout << "\nfallen to" << rock.m_sprite.m_frame_top_left << "\n" << rock.m_sprite << std::flush;
      }
      else {
        is_falling=false;
        place_rock(rock);
      }
      jet_index = (jet_index+1) % m_jets.size();
    }
    return *this;
  }
private:
  friend std::ostream& operator<<(std::ostream& os,Chamber const& chamber);

  Sprite m_sprite{Vector{.row=0,.col=0},Sprite::Rows{1,"|-------|"}};
  Result drop_count{};
  Jets m_jets{};
  int jet_index{0};
};

Model parse(auto& in) {
    Model result{};
    std::string line{};
    while (std::getline(in,line)) {
        result = line;
    }
    return result;
}

std::ostream& operator<<(std::ostream& os,Chamber const& chamber) {
  os << chamber.m_sprite;
  return os;
}

/*

|.####..|
|....##.|
|....##.|
|....#..|
|..#.#..|
|..#.#..|
|#####..|
|..###..|
|...#...|
|..####.|
+-------+

|.####..| : 10
|....##.| : 9
|....##.| : 8
|....#..| : 7
|..#.#..| : 6
|..#.#..| : 5
|#####..| : 4
|..###..| : 3
|...#...| : 2
|..####.| : 1
|-------| : 0
*/

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      Chamber chamber{data_model};
      std::cout << "\n" << chamber;
      // for (auto const& rows : ROCKS) {
      //   Rock rock{Vector{0,0},rows};
      //   std::cout << "\n" << rock.m_sprite;
      // }
      for (int i=1;i<=2022;++i) {
        chamber.drop();
        // std::cout << "\n" << chamber;
      }
      std::cout << "\n\n" << chamber;
      std::cout << "\nchamber.top_left:" << chamber.top_left();
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

char const* pTest = R"(>>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>)";
char const* pData = R"(><<<><><<<>>><<<>>>><<<<>><<<>><<<<>>><<<><<<>><>><<><<>>><<>><<<<>>>><<<<>><<<<>><<<><>>>><>>><<>>>><>><><<<>><<<<>>><<<<>><<><<>><<><>><<<<>>><<>>><>>><<><>>>><<>><<>>>><<>>>><><<><>>><<>>><<<<><<<>><<<>>><<>>><<<>><><>><>>><<<><<<>>>><<<<><>>><<<>><<<>>>><<<<>>><<><>><<<<><><<<>>>><<<<>>>><<>><<<>>>><<<<>><><><<>><<<<>>>><<><<<<>>>><<<<>>>><<<>><<<<>><<>>>><<>>><>><<>>>><><<<>>>><<<><<<<>>><<<<>>>><<<<>>>><<>>><<<<>><><>><>>><>>>><<<<>>>><>>><<>><<<<>><<>>><<<>>><<>>>><<><<<<><<<<><<<<>><<<>><<<>>>><>><<>>><>>>><>><<><<><>>><<<>>>><<<>><<><<<>>><>><<<<>>><<>><<<>><<<<>><<<<><>><><>>>><>><<><<>><>>><><>>>><<>><<<<>>>><<<<>><<<>>>><<<<>>><<<>><<<>>><<<><<><>><<<>><<<<>>><<<<>><>><<<>>><<<>>>><>><<<>><<>>><<><>><<<<>>><>><<><<>><<>>>><>><<>><<<><>>><><>>><<<<>>>><<<>><<>><<<<><>><<>>>><<>><<<<>>><<<>>>><<<><<>><>>>><<<>>><<<<>>><<<>>>><><<<<>>>><<<>>>><<<<>>><>>><>><<<<>>>><<<><<<<>>>><<>><<><<<<>>><<<<>>><><<<>><<>><<<<>><<<><<<<>>><<<><>>><<>>><><<<<><>><<<<>>>><<<<><>>><<>>><<<<><><<<<>>><<<<><<>>><<<<>>><<<>>><<<<>>>><>>><<<<>><<<><<<>><<<<>>><<<>><<<<>>><<<>><><<<<>>>><><<<>>>><<<>><<<<>><<<>>><><>>><<><><>><<>>>><<>>><>>><<<>>>><<>>>><>>>><<<<>>><<<<>><<><<>>>><<>>>><<>>><<<><<>><>>>><>>><<<<>><>>>><>>>><>>><<>>><<<<>>>><<<<>>><<>>><<<<>>><<<<>><<>>><<<<>><<<>>>><<<<>>><>><>>><<<>>><<>><<>><<<<>>>><<<>>><<<<>>><<<>>><<<><><<<<>>><<>><<<><<<<>>><><>>>><<<>><<>><<><><<<>>>><><<<<><<>><>>><<>><>>><<<<><<<<>><<>><<<<><<<<>>><<<><>><<<>><><<>>><<<<><<<<>>>><<<>><<>>><<<><<<<><<<><>>><<<<>>>><>><>><<<><<<<>><<>>>><>><<<<>>><<>>>><<<>>>><<><<><>>>><<><<<<>>><<>>>><<<>>>><<>>>><<>>>><<<<><<<><<>>>><<<>><><<<>>><<>>>><<<>>><<>>><<>>>><<<><>>>><<<<><>>>><<>>><<<><<<><<<>>>><>>>><<><<<>>>><<<<>>><<<<><<<>><<<>>>><>>>><<>>>><<<>>>><>><<>><<>>>><<<><<><<<<>>>><<<>>>><>>><<>>><<<>><<<>>>><>><<<>><<>>><<>>>><>><>><<>>>><>><<<<>>>><><<<<><<<>>>><<<<>><>>><<<>>><<<>>><>>>><<>><<<>><>><<<>>>><<>>><><<<<><<<>>><<<<><><<<<>>>><<>><<<><<>>>><<>><<>><><><<><<<>><<<<>>>><<<<><<><<<<>><>><<<><<<<><>><<>><>>><<<><><<><<<<><<<<><<>><<>>><<>>><>><<<<><<<>>>><<<>><<>>>><<<>><>>>><<<>><<>><<><<<<>>><><<<<><<<>>><<>>>><>>><>>>><<><>><<<>>><<><<<>><<><<<><<<<><>>><><<>>>><<>><<<<>>>><<>>><<<<>><<<<>>>><<<<>>>><<><<<<>><<>>><<<<>>><><>>><>>><<><<<>>><<<<>>>><><<<<>>><<<><<<<>>><<<<><<<>>><>><>><<<<>>><><<<>>>><<<>>>><>><<>>><<>>><<>><><<<<><<<<><<<>>>><<<<><>>>><>>><>>><<<<>>><<>>>><<<<>>><>>>><>>><<<<>>><<<<>>>><<>>>><<<><<>><<<><<<<><<<<><<<>>>><<>>>><<<<><<>><>>><<<<>>><<>><<<<>>><<<<>>><<<<>><<><<<<><<<<>>><>>>><<<<>>><<<>>>><>>>><<<<>>><<<<>>><<>>>><<<>>>><>><<<>><<<<>>>><<>><<<>>><<>>>><>>>><<<<>>><<<<>>>><<>>>><<<>><<<>>><<<<>><<<>>>><<<<>><<<<>>><<<<>><<<<><>>><<<>>><<<<>>><<<>>><<<<>>>><<><>>><<<<>><<<<>>>><><>>><<<><<<><<>><<<><><<<<>><<<<>>>><<>>>><<><><<>>><<<<><<>>>><<<<>>><>>><<<<>><>>>><><<<<>><<<<>>><<<>><<><>>>><<<<>>><<><<>>>><>>>><>>><<<>><>>><<<<>>><<>><<<>>>><<<<><>><><<><<<<><>>><<>><>>><<><<<>>>><<>><>>>><<<<>><<>><<<>>><<<<>><<><<<>>><<<>><<<>><>><<<<>>>><>><<>>>><<<<>>>><>>><<>>>><<<><><<<<>><<<><><<><<><<>>><<<><<<>>><<><<><<<><<<<>><>><<<><<<>>>><<<>>><>><<<>>><<<<>>><<><<>>>><<<>><<<<><<<>><<<<>>><<>>><>><<>><><<>>>><>><>>>><>>><<<><<><<<<>>><<>>>><<<><<<>><<<>>><<>>>><><<<<>>>><<<<>>>><<>>>><<>>>><<<<>>>><<<>><<>><<<>>>><<<>>><<<<>>><<<><<<<>><<<<><<<<>>><<<<>><>>>><<><<<<>><<>>><>>><<<>><<<><<<><>>>><<<<>>>><<<>><<>>>><<>>><<><<>>>><<<<><>>>><<<<>><<<>>><<<<>><<><>><><>><>>>><<><<>>><<<<>><<<>><<<<><<<>>>><<>><>>>><<<><<>><<<<>><<<><<<><<<<>>><<>>><<<>><>>><<<<>>><<<<>>><>>><<<>>><<>>><<>>>><<<<>><><<>><>>>><><<<<>>>><<<<>><<<<>><<<<>>><<>>>><<<><<>><<<<><<<<><<>><<<<>><<><<>><<>>><<<<>>>><<><<<>>><<<>>><>><<>>>><<<<>>>><<<>><<>><>>>><<>><<>>>><>>><>><<><<<<>><<<>>><<<>><>>>><<>>><<<<>>><<<>>><<<<>>>><<<>>>><<<>>>><<<<>>>><<<>><<>>><<<<>>>><<<>>><<<>>>><<<<>><>><<<>>>><>>>><<<<><<<<><<<<>>>><<<<>><<<<><<>>>><>>>><<>>>><<<<>><>>>><<<<>>><>>>><>>>><>>><<<><>>>><<<<>>>><>><<<<>><<><<><>><<<<><>>>><<<>>><<>><<<>>>><>><<<><<><<>>>><<>>>><<><><<><>>><<>>><<>>>><<<<><<>>><>>><><><<<<>><>>><<<<>>><<<<>>>><<<>>>><<<<>>><<<>><<<>>><<>><>>>><>><<>>><<<<>>>><>><<>><>>>><>>>><<<>>><>>><<<<>>><<<>>><<>>><<<><<<<>><>><<<>>>><<<>>>><<<>><<<>><<<>><>><<<>>>><<<>>><>>>><<<<>>>><<<<>>><><<<<><<<<>>><<<>>><<<><>>>><>>><><>>><<>>>><<<<>><>>>><>>>><>><<>><<><<<>><<<>>>><<>><><>><<<><<>>><<<<>>><<<>><><<<<><<<>>>><>>><<<>><>><<>><><<>>>><<<>>>><<<><<<<>><<<<>>>><>><<<>>>><<<<>><<>><<<>>><<>>><>><<<>>><>><<<>>><<<>>><><<<<>>><<<<>>><<<<><><><<<<><<>>>><><<<<><<>>>><<>>><<<>><><<<<><<<>>>><<<>><<<<>>><<<>>><<><<><>><<>>>><<<>><<<><<<<>>>><<<>>>><<<><<>>>><><<>>><<>>>><<><<<<><<>><<<<><<><<<<>>>><<<>><>>>><>>>><<>>>><>>>><>>>><<>>><>>><<<><<<<>>>><<<<><<<>><>><<>>>><>><<<<>>>><>>>><<<<><<>>><<<>><<<<>>>><<>><<<>><>>>><<>><>>><>>><><>>><<<>>>><<<<>>>><<<<>><<>><<<<><<<>><<<>>>><<>>>><<<<>>><<<>>><<>>><<<><<<>><<>>><<<<>>>><<<<>>>><<<<><>>>><<><<>><><<<<><<<>><<>>>><><<<<>><<<>>><<>>>><>>>><<<<><<<<>>><>><<<>><<<>><>>>><<>>>><<<<><>>><>>><<<>>><>><<<<>><<<<>>><<<>>><>>><>>>><<<><<>><<>><<<<>>>><<>>><<<<><>>>><<<<><<><>>>><<<<><<<<>><<<<><<<<>>><>><<><<>><>>>><<<>>>><<<<>>><<<<>>>><<<<><<>>>><<>><>>>><<>><<<<>>>><<><>>><<<<>>>><<<>>><>><>>><<>>>><<<>>>><<<>>>><>>>><<><<<>>><<<<>>>><>>><<<>>><<<<>><<<<>>><<<>><><<<<>><><<><>>><>>><<<<>><<<<><>>>><<>>>><<<>>>><>>>><<<>>>><><<<<><>><<>>>><<<<>>><<<>>><<<<>>><<>>>><<<>><<<<>>>><><<<<>>>><<<<><><<<><<<><<>>><<<>>><<><>>>><<>>><<<>>>><<<>>><>><<<><<>><<<>><<>><>><<<>>>><<<<>>><>><<>><<<<>>><<<>>><<<<>>><<<>>><>>><<>>><<><<><<<><<<<>>><<<><<<<><<<<>>>><>>><>>>><>><>>><<<>>><<>>>><<><<>>><<<>>><<><<<<><<<<>>><<<<>><<<<>>><>><><<>>>><>><<<<><<<><><<<>>><<<<>><<<<>>>><>>>><>>><<<>>><>><<<<><<<<>>>><<<<><<<><<>>><<<<>>><<<>><<<<>>>><<<>><>>><<<<><<<>><<>>>><>><>><<<<>><<<<>>><<>>>><<<<>>><>>><<<<>>>><<<<><<<<><<><<<>><>><<<>>>><<<<>><>>>><<<>>>><>>>><<><><<<<>><<<><>>>><<><<>><<>>><<><>><<<><<>>><<<>>><<<<>>><<<><<>><<<>>><<>>><<>><<<<>>>><>>>><>><<<>>>><<<><<<><<>><<<>><<>><<<>>><<<>>><>>><<<<>>>><<>><<<><<<>>>><>>>><<<>>>><><<<<><<<<>><<<<>><<<>>>><<<<>>>><<<><>>><<<><><<>><>>><<<<>><>><<<>>>><>><<<<><<<>>>><<><<<>>><<<<>>>><<<<>>><>><>>><<<><<<<><<>>>><<>>>><<<<>>>><<<<>>><<<<>>><><><<<<>><<<<><<<<>>><<<>><<<<><<<>><>>>><<<<><<<<>>><>><<>>>><<<>>>><<<<>>><<>>><<<>><<>>><<<<>><>><<<<><<<<>><<<>>><<<>><<>>><<<<>>><<<<>>>><<>>><<<>>><<<><<<>><<><>>><<<>>>><<<<>>><>><>>>><<>>><><<<>>>><<>>><<>><<>>>><<<>>><<<<>><<<<><<<><><<<>>><<<<>><<>>>><<<>>><<<>>><<>>>><<<><<<><<><<<><>>><<<>><>>><>>>><<<>>>><<<><<<>>>><<>>>><<<>><<<<>>><<>>><<<>><<<>>>><<>><>><<<<>>>><<>>><<<>>><<<>>><<<>>><<<<>>>><>><>>><>>><<<<>>><<<<>>>><>>>><<>>>><<>><>><><<<>>><<<><<<>><<<<>><<<<>><<>>><<><<<<>>><<<<>>><<><<<<><>>><<<<>><>>>><<>>><>>>><<<><>><>>>><<>><<<<>>>><<<><<>>><<<<><<<<><<<>>>><<<>><<<<><>>>><<<<><<>>>><<<>>>><<<>><>>><<<<>>>><<<>><<>>>><<<<>><<<>>><>>><<<>>><>>>><<<<>>><<<<>>><<<><<<<>>><<><<<<>>>><><<<>>><<<<><<><<<<><<<<><<<>>><<<>><<<<>><<<<>><<<>>>><<<>>>><>>><<<<>>><<<>>>><<<<>><<<<><<>>>><<<<>><>>><>>>><<<><<<><<<<>><>>>><<<<>>>><>><<<<><<>>><<><<>>><<><<<>>>><<<>>>><>>>><<<<>>><<><<<><>><<<>>><<>>><<<><<<<>><<<><<<<>><<<<>>>><<<><<<<>>>><<>><<<<><<<><<<>><<<>><>>>><<<>>><<><<<<>><<<<>>>><<<>><<<<>>><<>>><<<<>>><<>>>><<>><<><>><>>>><<<>><<<<><<<>><<<>>>><<<<>><<><<>>><<<<><<<>>>><>>>><<<>>><>><<>>><<>>><<<>><<<>><>>>><<<>>>><<<<>>><<<<>>>><<<>>>><<>>>><<><<<>>>><<>>>><<<<>>>><<<><<><<>><<<>>><<<<>>>><<<>><>>>><><<<<>>><<>>>><<<>><<<><<<<><<>>><>>><<<>>><>><<>><<><<<>>>><>>><><<<>>>><<<><>><><>>><>><>>>><<<>><<<>>>><<>>>><<>>><>>><<<><>>>><>>>><>>><<>>><<>>>><<<<><<<>>>><<<<><<>>>><<<>>><<<<><>><<<>>><<<<>>>><<<<>>>><>>>><>><<<>>><<<>><<<><<<<><<>>><>>>><<<<>>>><<<><<<>><>><>><<<>>>><>>><><>><<><<>>>><>><>>><<><<<<>>>><><>><<>>>><<<<>>><<>>>><<<><>><<>>>><<<>><<<>><<<><<<><><><<<><>>><><<<<>><<<<>>><<>><<<<>>><<>>><<>>>><<<<>><<<<>>><<<>>><<<<>>><<<>>>><<<<>>>><<<>>><<<>>><<<<><>>>><<<><>><<<>>>><<<><<<<>>>><><<><<<<>><><<<<><<<>>>><>><<<><<><<<>>>><<<<>><<<<>>><<>>>><<<><<<<><<<>>>><<>><<<<>><<>>>><<<<>><<<>>>><>>>><<>>><<>><<<>>><>><<>><<>>>><<<<>><<>><>><<<><>><<<><<>>><<<><<>><<<<>>><<>>>><<<<>>><<<<>><>>><<<><<>>><<>><<>>>><>>><<<>>>><<<<><>>>><<<>>>><<>>>><>><<>>>><<<<>><>><<<<>>>><><<><<<<>>><<<<>>>><>>><<>><>><<>><<>>><<<>>>><<>><><<<>>><<<>>>><<>>>><<<>><<<><<<>>>><<>>><<<>>>><>><<<>><<<<>>><<>>><<<><<>>>><<<>>><<>><<<><>><<>>>><<<>>>><><><<>>><<<<>><<>>><<<<><<<>>><>>>><<<>>><><>><<>>><<<<>>><><<<>>>><<>>>><<<<>>><>>>><<>>><>><<<>>>><>>>><<<>>>><<<>>><><>><<<>>>><<<>>>><>>>><<<<><<>><<<<>>><<>>>><<>>>><<<>>><<<<>>><<>>><<<<>><<<>>>><<>><<<<>>>><<<>>><>>>><<<><<<<>>>><<>>><<<<>>>><>><>>><<<>>>><>><<<<><<<<><<<>>><>><<<>>><<<<>>><<>>><><<<<>>><<<<>><<<<><<<>>>><>><<<>>>><<<>>><<>>><<<<>>><<>>>><<<>>>><<<<>>><>>><<><<<><<<<><<<>><<<<>><<>>>><><<<<>>>><<<<>><<><<<>>>><<<>>><<><<>>>><<>>><<<<>>>><><<<>><<<<><<<>><>>><<>><<<<>>>><<<<>>><<><<<<>>>><<>>><><>><<<><<<>><<<<>>><>>><<<<>>>><>><<<>>>><<<>>>><<<>><>>><<<><>><<<<>>>><<>>><>>><>><>>><<<>>>><>><<>>>><<>>>><<<<>>><<<<>><<<<>>><><<<<>>>><<<>><<<>><<<<>>><<<>>><<<>>><<<<><<<><<<<><>>>><<<<>>><<>>><>><<>><>><>><>>><<<>>>><<><>><><<<<>><<<<>><<<<>>>><>><<<>>><<<><<<>>><<><<<<>>><<<<>><>>><<<>>>><>><<><>>><<<<>>><<>><<<>>><<>>><<<>>><<<>>><<><<<>>>><<>><<><<><<>>>><<<<>>><<<>>><<<>>>><><<<><>><>><<>>><<<<>><<>>><<<<>>>><>><<<<>><<<<>>>><<<<>>><><<>>>><<<>><<<<>>><><<>>><<>>>><<<><<<>>>><<<>>><>><<>><<>><>>><<<>>><<><<>>>><<>><><<<>>>><>><<<<><<<>><><<<<><<>>><><<<>>><<>>>><<<<><>>><><<<<>>><><>>>><<>>><<>><<<>><<<<><<><<<<>>><>>><<<<>>><<>><<<<><<<<>>><<<<><<<>><<<>>><<<<>><<>>><<>>>><<<>>>><>><>><<>>>><<<>>><<>>><<<>>>><<>>>><>>><<<>>>><<<><>>><<>>>><><><<<<><>>>><<>>>><<><<><<<<>>><<>>><<><>>>><<>>>><<>>>><><<<<><<<<>><<<<><<<>>><<<><<<><>>><<<<>>>><>><<<>>><<>>><>><>>>><<><>>><<<>>>><><>><<<<>>><<<<>>>><<>><>><>>><<<>><<><<<<>>>><<<<>>>><<><>><<<>><<>><<>>><><<<<>>>><<<>>>><>>><<<<>>>><<<<>><<><><>>><<<>>>><>>><><<<><<<<><<<<>>>><<>>><<>><<<<><<>>><>>>><<<<>><<<>>>><<><<<<>><<<>>><<>>><<<><<<><<>>><>><<<<>>>><>>><>>>><<<<>><<<>>><<<>><<<)";
