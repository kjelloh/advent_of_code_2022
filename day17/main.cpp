#include <iostream>
#include <iomanip> // E.g., std::quoted
#include <string>
#include <sstream> // E.g., std::istringstream, std::ostringstream
#include <vector>
#include <set>
#include <unordered_set>
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
#include <bit> // E.g. std::rotl
#include <cassert>

extern char const* pTest;
extern char const* pData;

using Result = long long int;
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
  bool operator==(Vector const& other) const {
    return row==other.row and col == other.col;
  }
  Vector& operator+=(Vector const& delta) {
    row += delta.row;
    col += delta.col;
    return *this;
  }
  Vector operator+(Vector const& other) const {
    Vector result{*this};
    result += other;
    return result;
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
    // std::cout << "\nSprite(pixel_top_left:" << pixel_top_left << " row[0]:" << m_rows[0] << ") frame_top_left:" << m_frame_top_left << " m_frame_bottom_right:" << m_frame_bottom_right;
  }
  bool in_frame(Vector const& pos) const {
    return (pos >= m_frame_top_left + Vector{.row=m_virtual_rows_count,.col=0} and pos <= m_frame_bottom_right);
  }
  char& at(Vector const& pos) {
    // std::cout << "\nat(" << pos << ")";
    static char NULL_CHAR{'?'};
    auto row = m_frame_top_left.row - pos.row;
    if (in_frame(pos)) {
      return m_rows[pos.row - m_virtual_rows_count - m_frame_bottom_right.row][pos.col - m_frame_top_left.col]; // rows max..0, cols 0..max
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
    // std::cout << "\npush_row(" << std::quoted(entry) << ")";
    m_rows.push_back(entry);
    ++m_frame_top_left.row;
    // std::cout << " m_frame_top_left:" << m_frame_top_left << " m_frame_bottom_right:" << m_frame_bottom_right;
    return *this;
  }
  Sprite& operator+=(Sprite const& other) {
    // std::cout << "\nSprite::operator+=(Sprite const& other)";
    // merge other with us
    // NOTE: Assumes other does NOT have virtual rows
    assert(other.m_virtual_rows_count==0);
    const std::string empty_row(m_frame_bottom_right.col - m_frame_top_left.col+1,'.');
    while (other.m_frame_top_left.row > m_frame_top_left.row) push_row(empty_row);
    for (auto row=other.m_frame_top_left.row;row>=other.m_frame_bottom_right.row;--row) {
      for (auto col=other.m_frame_top_left.col;col<=other.m_frame_bottom_right.col;++col) {
        Vector pos{.row=row,.col=col};
        if (auto other_ch = other.at(pos);other_ch!='.') this->at(pos) = other_ch;
      }
    }

    return *this;
  }
  bool does_collide_with(Sprite const& other) {
    static int call_count{};
    bool result{false};
    // std::cout << "\ndoes_collide_with m_frame_top_left:" << m_frame_top_left << " m_frame_bottom_right:" << m_frame_bottom_right; 
    for (auto row=m_frame_top_left.row;row>=m_frame_bottom_right.row and !result;--row) {
      for (auto col=m_frame_top_left.col;col<=m_frame_bottom_right.col and !result;++col) {
        Vector pos{.row=row,.col=col};
        if (this->at(pos)!='.') {
          if (other.in_frame(pos)) {
            // std::cout << "\nother.in_frame TRUE";
            result = result or other.at(pos)!='.';
            // std::cout << "\npos:" << pos << " this:" << this->at(pos) << " other:" << other.at(pos) << " does_collide:" << result << std::flush;
          }
        } 
      }
    }
    return result;
  }
  Sprite& add_virtual_rows(Result delta) {
    // move top_left without moving bottom_right
    m_virtual_rows_count += delta;
    m_frame_top_left.row += delta;
    return *this;
  }
  Vector m_frame_top_left;
  Vector m_frame_bottom_right;
  Rows m_rows;
  Result m_virtual_rows_count{0};
};

std::ostream& operator<<(std::ostream& os,Sprite const& sprite) {
  if (sprite.m_virtual_rows_count==0) {
    int row = sprite.m_rows.size()-1;
    for (auto iter=sprite.m_rows.rbegin();iter != sprite.m_rows.rend();++iter) {
      if (iter!=sprite.m_rows.rbegin()) std::cout << "\n";
      std::string indent(sprite.m_frame_top_left.col,'.');
      os << indent << *iter << " : " << row--;
    }
  }
  else {
    os << "\noperator<< can't print sprite with virtual rows";
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
  static const int PATTERN_BUFFER_LENGTH{30};
  auto const& top_left() const {return m_sprite.m_frame_top_left;}
  auto const& bottom_right() const {return m_sprite.m_frame_bottom_right;}
  Chamber(Jets const& jets) : m_jets{jets} {}
  bool can_move_left(Rock rock) {
    rock.m_sprite += Sprite::LEFT;
    return (top_left().col < rock.top_left().col) and !rock.m_sprite.does_collide_with(this->m_sprite);
  }
  bool can_move_right(Rock rock) {
    // std::cout << "\ncan_move_right(rock::bottom_right:" << rock.bottom_right() << ") this->bottom_right:" << bottom_right(); 
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
    // std::cout << "\nplace_rock(" << rock.top_left() << ")";
    m_sprite += rock.m_sprite;
    // std::cout << "\nadd chamber sides";
    for (auto row=rock.top_left().row;row>=rock.bottom_right().row;--row) {
      m_sprite.at(Vector{.row=row,.col=0}) = '|';
      m_sprite.at(Vector{.row=row,.col=m_sprite.m_frame_bottom_right.col}) = '|';
    }
    return *this;
  }
  Chamber& drop() {
    rock_index = (m_rocks_count % ROCKS.size());
    auto rock_rows = ROCKS[rock_index];
    Vector rock_top_left{.row=static_cast<Result>(top_left().row + rock_rows.size() -1 + 4),.col=3}; // col:0 is left boundary
    Rock rock(rock_top_left,rock_rows);
    // std::cout << "\n" << rock.m_sprite << std::flush;
    bool is_falling{true};
    while (is_falling) {
      rock = to_moved_rock(rock,m_jets[jet_index]);
      // std::cout << "\ngusted to" << rock.m_sprite.m_frame_top_left << "\n" << rock.m_sprite << std::flush;
      if (can_move_down(rock)) {
        rock.m_sprite += Sprite::DOWN;
        // std::cout << "\nfallen to" << rock.m_sprite.m_frame_top_left << "\n" << rock.m_sprite << std::flush;
      }
      else {
        is_falling=false;
        place_rock(rock);
      }
      jet_index = (jet_index+1) % m_jets.size(); // next wind gust to apply on next rock to drop
    }
    ++m_rocks_count; // this many dropped
    if (top_left().row>PATTERN_BUFFER_LENGTH) {
      // We have enough on the rock pile to calculate a pile_key
      // State reflects number of dropped rocks, a key for the current rock pile, the index of current wind gust and the top_left of the whole pile
      m_state = State{.jet_index=jet_index,.rock_index=rock_index,.pile_key=pile_key(),.rocks_count=m_rocks_count,.top_left=top_left()};
    }
    return *this;
  }
  Result rocks_count() const {return m_rocks_count;}
  using Key = std::size_t;
  Key pile_key() const {
    Key result{};
    Result start_row = (m_sprite.m_rows.size()>PATTERN_BUFFER_LENGTH)?m_sprite.m_rows.size()-PATTERN_BUFFER_LENGTH:0;
    for (Result row=start_row;row<m_sprite.m_rows.size();++row) {
      result = std::rotl(result,1) ^ std::hash<std::string>{}(m_sprite.m_rows[row]);
      // result ^= std::hash<std::string>{}(m_sprite.m_rows[row]);
    }
    return result;
  }

  struct State {
    int jet_index;
    int rock_index;
    Key pile_key;
    Result rocks_count;
    Vector top_left;
    bool operator<(State const& other) const {
      if (jet_index==other.jet_index) {
        if (rock_index==other.rock_index) return pile_key<other.pile_key;
        else return rock_index<other.rock_index;
      }
      else return jet_index<other.jet_index;
    }
  };

  friend std::ostream& operator<<(std::ostream& os,Chamber::State const& state);

  struct Cycle {
    Result rocks_count;
    Result pile_gain;
    bool operator<(Cycle const& other) const {
      if (rocks_count<other.rocks_count) return pile_gain<other.pile_gain;
      else return rocks_count<other.rocks_count;
    }
  };

  State m_state{};
  std::set<State> m_seen{};

  std::optional<Cycle> cycle() {
    std::optional<Cycle> result{};
    if (m_state.rocks_count>PATTERN_BUFFER_LENGTH*2) {
      // having dropped PATTERN_BUFFER_LENGTH*2 rocks we have at least PATTERN_BUFFER_LENGTH rows in the pile sprite?
      if (auto iter = m_seen.find(m_state);iter!=m_seen.end()) {
        // std::cout << "\n SEEN " << m_rocks_count << " " << m_state.rock_index << " " << m_state.top_left << " " << m_state.pile_key << std::flush;
        // std::cout << "\n PREV " << iter->m_rocks_count << " " << iter->rock_index << " " << iter->top_left << " " << iter->pile_key << std::flush;
        std::cout << "\n SEEN " << m_state;
        std::cout << "\n PREV " << *iter;
        result=Cycle{.rocks_count=m_state.rocks_count - iter->rocks_count,.pile_gain=m_state.top_left.row - iter->top_left.row};
        m_seen.erase(m_state); // do not use again
      }
      else m_seen.insert(m_state);
    }
    return result;
  }

  void advance(Result rocks_count,Result pile_gain) {
    m_rocks_count += rocks_count;
    m_sprite.add_virtual_rows(pile_gain);
    m_state = State{.jet_index=jet_index,.rock_index=rock_index,.pile_key=pile_key(),.rocks_count=m_rocks_count,.top_left=top_left()};
  }

private:
  friend std::ostream& operator<<(std::ostream& os,Chamber const& chamber);

  Sprite m_sprite{Vector{.row=0,.col=0},Sprite::Rows{1,"|-------|"}};
  Result m_rocks_count{};
  Jets m_jets{};
  int jet_index{0};
  int rock_index{};
};

std::ostream& operator<<(std::ostream& os,Chamber::State const& state) {
  os << " jet_index:" << state.jet_index;
  os << " rock_index:" << state.rock_index;
  os << " Key pile_key:" << state.pile_key;
  os << " rocks_count:" << state.rocks_count;
  os << " top_left:" << state.top_left;
  return os;
}


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
      for (int i=1;i<=2022;++i) {
        chamber.drop();
      }
      std::cout << "\n\n" << chamber;
      std::cout << "\nchamber.top_left:" << chamber.top_left();
      result = chamber.top_left().row;
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      Chamber chamber{data_model};
      std::cout << "\n" << chamber << std::flush;
      // const Result TARGET_ROCKS_COUNT{1000000000000};
      // const Result TARGET_ROCKS_COUNT{2022};
      const Result TARGET_ROCKS_COUNT{96 + 35*2};
      bool cycled{false};
      // for (Result i=1;i<=TARGET_ROCKS_COUNT;++i) {
      while (chamber.rocks_count()<TARGET_ROCKS_COUNT) {
        chamber.drop();
        auto i = chamber.rocks_count(); // drop count is the count of rocks on the pile
        std::cout << "\n" << chamber.m_state;
        if (!cycled) {
        // if (true) {
          if (auto cycle = chamber.cycle()) {
            std::cout << "\ni:" << i << "  ROCKS COUNT CYCLE:" << cycle->rocks_count;
            std::cout << "\ni:" << i << "    PILE ROWS CYCLE:" << cycle->pile_gain;
/*
 jet_index:19 rock_index:0 Key pile_key:12385374603614952172 rocks_count:96 top_left:[row:150,col:0]
 SEEN  jet_index:19 rock_index:0 Key pile_key:12385374603614952172 rocks_count:96 top_left:[row:150,col:0]
 PREV  jet_index:19 rock_index:0 Key pile_key:12385374603614952172 rocks_count:61 top_left:[row:97,col:0]
i:96  ROCKS COUNT CYCLE:35
i:96    PILE ROWS CYCLE:53
i:96        CYCLES LEFT:1

...
 expected: jet_index:19 rock_index:0 Key pile_key:12385374603614952172 rocks_count:131 top_left:[row:203,col:0]
 end state  jet_index:19 rock_index:0 Key pile_key:12385374603614952172 rocks_count:131 top_left:[row:203,col:0] OK

*/            
            auto cycle_counts = ((TARGET_ROCKS_COUNT-i) / cycle->rocks_count);
            // auto cycle_counts = 1;
            std::cout << "\ni:" << i << "        CYCLES LEFT:" << cycle_counts;
            auto delta = cycle_counts*cycle->rocks_count;
            if (i+delta<=TARGET_ROCKS_COUNT) {
              // It is ok to jump to 
              chamber.advance(delta,cycle_counts*cycle->pile_gain);
              std::cout << "\n   new pile top:" << chamber.top_left().row;
              std::cout << "\nnew rocks count:" << chamber.rocks_count();
            }
            // // exit(1);
            // assert(i<=TARGET_ROCKS_COUNT);
            // cycled=true;
          }
        }
      }
      // std::cout << "\n" << chamber;
      std::cout << "\nend state " << chamber.m_state;
      result = chamber.top_left().row;
      return result;
  }
}

void test() {
  Rock r1{Vector{.row=8,.col=2},ROCKS[1]};
  Rock r2{Vector{.row=6,.col=6},ROCKS[4]};
  while (!r1.m_sprite.does_collide_with(r2.m_sprite)) {
    r2.m_sprite += Sprite::LEFT;
    Rock merged{Vector{.row=6,.col=0},Sprite::Rows(10,"                  ")};
    merged.m_sprite += r1.m_sprite;
    merged.m_sprite += r2.m_sprite;
    std::cout << "\n\n" << merged.m_sprite; 
  }
  exit(0);
}

int main(int argc, char *argv[])
{
  // test();
  Answers answers{};
  // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
  // answers.push_back({"Part 1     ",part1::solve_for(pData)});
  answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
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
