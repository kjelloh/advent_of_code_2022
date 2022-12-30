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

using Result = long long int;
using Answers = std::vector<std::pair<std::string,Result>>;

struct Vector {
  int row;
  int col;
  Vector operator+(Vector const& other) const {
    return Vector {
      .row = row + other.row
      ,.col = col + other.col
    };
  }
  bool operator==(Vector const& other) const {
    if (row == other.row) return col == other.col;
    else return row == other.row;
  }
};

std::ostream& operator<<(std::ostream& os,Vector const& v) {
  os << "[row:" << v.row << ",col:" << v.col << "]";
  return os;
}

using Vectors = std::vector<Vector>;

const Vectors dirs{
  {-1,0}    // N
  ,{0,1}    // E
  ,{1,0}    // S
  ,{0,-1}   // W
};

using Map = std::vector<std::vector<char>>;

struct Model {
  Map map;
  Vector start;
  Vector end;
};

bool on_map(Vector const& pos,Map const& map) {
  return (pos.row>=0 and pos.row<map.size() and pos.col>=0 and pos.col<map[0].size());
}

Vectors to_neighbours(Vector const& pos,Map const& map) {
  Vectors result{};
  for (auto const& delta : dirs) {
    auto neighbour = pos+delta;
    if (on_map(neighbour,map)) {
      std::cout << "\n\ton_map:" << neighbour;
      result.push_back(neighbour);
    }
  }
  return result;
}

Model parse(auto& in) {
    Model result{};
    std::string line{};
    int row{0};
    Vector start{},end{};
    while (std::getline(in,line)) {
      result.map.push_back({});
      int col{0};
      for (char ch : line) {
        if (ch=='S') {
          start.row = row;
          start.col = col;
          result.map.back().push_back('a');
          result.start = start;
        }
        else if (ch=='E') {
          end.row=row;
          end.col=col;
          result.map.back().push_back('z');
          result.end = end;
        }
        else result.map.back().push_back(ch);
        ++col;
      }
      ++row;
    }
    return result;
}

std::ostream& operator<<(std::ostream& os,Map const& map) {
  for (int row=0;row<map.size();++row) {
    if (row>0) os << "\n";
    for (int col=0;col<map[0].size();++col) {
      os << map[row][col];
    }
  }
  return os;
}

std::pair<Result,std::string> dfs(Vector const& start,Vector const& end,Map const& map,Map& marked) {
  std::pair<Result,std::string> result{};
  std::cout << "\ndfs(" << start << ") ch:" << map[start.row][start.col];
  marked[start.row][start.col] = 'X';
  auto neighbours = to_neighbours(start,map);
  std::vector<std::pair<Result,std::string>> candidates{};
  for (auto const& next : neighbours) {
    std::cout << "\n\tnext:" << next;
    if (marked[next.row][next.col] == 'X') continue;
    if (map[next.row][next.col] >= map[start.row][start.col] + 2) continue;
    if (next==end) {
      std::cout << " END!";
      candidates.push_back({0,"E"}); // dfs(next) is 0
    }
    candidates.push_back(dfs(next,end,map,marked));
  }
  if (candidates.size()>0) {
    auto iter = std::min_element(candidates.begin(),candidates.end(),[](auto const& e1,auto const& e2){
      return e1.first < e2.first;
    });
    result.first = iter->first + 1;
    result.second = iter->second + map[start.row][start.col];

  }
  else {
    result.first = 240000000; // Eternity
  }
  std::cout << " dfs_result start:" << start;
  std::cout << " path:";
  for (auto const& ch : result.second) std::cout << ch;
  return result;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\n" << data_model.map;
      Map marked(data_model.map.size(),std::vector(data_model.map[0].size(),' '));
      auto [steps,path] = dfs(data_model.start,data_model.end,data_model.map,marked);
      result = steps;
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

char const* pTest = R"(Sabqponm
abcryxxl
accszExk
acctuvwj
abdefghi)";
char const* pData = R"(abccccccccccccccccccaaaaaaaaacccccccccccccccccccccccccccccccccccccaaaa
abcccccccccccccccaaaaaaaaaaacccccccccccccccccccccccccccccccccccccaaaaa
abcaaccaacccccccccaaaaaaaaaacccccccccccccccccccccaaacccccccccccccaaaaa
abcaaaaaaccccccccaaaaaaaaaaaaacccccccccccccccccccaacccccccccccccaaaaaa
abcaaaaaacccaaacccccaaaaaaaaaaaccccccccccccccccccaaaccccccccccccccccaa
abaaaaaaacccaaaaccccaaaaaacaaaacccccccccccaaaacjjjacccccccccccccccccca
abaaaaaaaaccaaaaccccaaaaaaccccccaccccccccccaajjjjjkkcccccccccccccccccc
abaaaaaaaaccaaacccccccaaaccccccaaccccccccccajjjjjjkkkaaacccaaaccaccccc
abccaaacccccccccccccccaaccccaaaaaaaacccccccjjjjoookkkkaacccaaaaaaccccc
abcccaacccccccccccccccccccccaaaaaaaaccccccjjjjoooookkkkcccccaaaaaccccc
abcccccccaacccccccccccccccccccaaaacccccccijjjoooooookkkkccaaaaaaaccccc
abccaaccaaaccccccccccccccccccaaaaacccccciijjooouuuoppkkkkkaaaaaaaacccc
abccaaaaaaaccccccccccaaaaacccaacaaaccciiiiiooouuuuupppkkklllaaaaaacccc
abccaaaaaacccccccccccaaaaacccacccaaciiiiiiqooouuuuuupppkllllllacaccccc
abcccaaaaaaaacccccccaaaaaaccccaacaiiiiiqqqqoouuuxuuupppppplllllccccccc
abccaaaaaaaaaccaaaccaaaaaaccccaaaaiiiiqqqqqqttuxxxuuuppppppplllccccccc
abccaaaaaaaacccaaaaaaaaaaacccaaaahiiiqqqttttttuxxxxuuuvvpppplllccccccc
abcaaaaaaacccaaaaaaaaaaacccccaaaahhhqqqqtttttttxxxxuuvvvvvqqlllccccccc
abcccccaaaccaaaaaaaaaccccccccacaahhhqqqttttxxxxxxxyyyyyvvvqqlllccccccc
abcccccaaaccaaaaaaaacccccccccccaahhhqqqtttxxxxxxxyyyyyyvvqqqlllccccccc
SbcccccccccccaaaaaaaaaccccccccccchhhqqqtttxxxxEzzzyyyyvvvqqqmmlccccccc
abcccccccccccaaaaaaaacccaacccccccchhhppptttxxxxyyyyyvvvvqqqmmmcccccccc
abccccccccccaaaaaaaaaaccaacccccccchhhpppptttsxxyyyyyvvvqqqmmmccccccccc
abcaacccccccaaaaaaacaaaaaaccccccccchhhppppsswwyyyyyyyvvqqmmmmccccccccc
abaaaacccccccaccaaaccaaaaaaacccccccchhhpppsswwyywwyyyvvqqmmmddcccccccc
abaaaaccccccccccaaaccaaaaaaacccccccchhhpppsswwwwwwwwwvvqqqmmdddccccccc
abaaaacccccccccaaaccaaaaaaccccccccccgggpppsswwwwrrwwwwvrqqmmdddccccccc
abccccccaaaaaccaaaacaaaaaaccccccaacccggpppssswwsrrrwwwvrrqmmdddacccccc
abccccccaaaaaccaaaacccccaaccccaaaaaacggpppssssssrrrrrrrrrnmmdddaaccccc
abcccccaaaaaaccaaaccccccccccccaaaaaacggppossssssoorrrrrrrnnmdddacccccc
abcccccaaaaaaccccccccaaaaccccccaaaaacgggoooossoooonnnrrnnnnmddaaaacccc
abccccccaaaaaccccccccaaaacccccaaaaaccgggoooooooooonnnnnnnnndddaaaacccc
abccccccaaaccccccccccaaaacccccaaaaacccgggoooooooffennnnnnnedddaaaacccc
abcccccccccccccccccccaaacccccccaacccccggggffffffffeeeeeeeeeedaaacccccc
abccccccccccccccccccaaacccccaccaaccccccggfffffffffeeeeeeeeeecaaacccccc
abccccccccccccccccccaaaacccaaaaaaaaaccccfffffffaaaaaeeeeeecccccccccccc
abccccccccaacaaccccaaaaaacaaaaaaaaaaccccccccccaaaccaaaaccccccccccccccc
abccccccccaaaaacccaaaaaaaaaaacaaaaccccccccccccaaaccccaaccccccccccaaaca
abcccccccaaaaaccccaaaaaaaaaaacaaaaacccccccccccaaaccccccccccccccccaaaaa
abcccccccaaaaaacccaaaaaaaaaacaaaaaacccccccccccaaccccccccccccccccccaaaa
abcccccccccaaaaccaaaaaaaaaaaaaaccaaccccccccccccccccccccccccccccccaaaaa)";
