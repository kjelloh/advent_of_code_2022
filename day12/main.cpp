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

using Result = int;
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

const Vectors DELTAS {
  {-1,0} 
  ,{0,1} 
  ,{1,0} 
  ,{0,-1}
};

std::ostream& operator<<(std::ostream& os,std::vector<std::string> rows) {
  for (int row=0;row<rows.size();++row) {
    if (row>0) os << "\n";
    os << rows[row];
  }
  return os;
}

class Grid {
public:
  using Vertex = int;
  using Vertices = std::vector<Vertex>;
  using Value = char;
  auto V() {return m_dict.size();}
  void insert(Vector const& pos,Value ch) {
    std::cout << "\nGrid::insert(" << pos << ",value:" << ch << ")";
    auto iter = std::find_if(m_dict.begin(),m_dict.end(),[&pos](auto const& entry){
      return entry.first == pos;
    });
    if (iter == m_dict.end()) {
      iter = m_dict.insert(iter,{pos,m_dict.size()});
      m_values[iter->second] = ch;
      top_left.row = std::min(top_left.row,pos.row);
      top_left.col = std::min(top_left.col,pos.col);
      bottom_right.row = std::max(bottom_right.row,pos.row);
      bottom_right.col = std::max(bottom_right.col,pos.col);
    }
    std::cout << " V:" << V(); 
  }
  Vertices adj(Vertex v) const {
    Vertices result{};
    if (auto pos = to_pos(v)) {
      for (auto const& delta : DELTAS) {
        if (auto w = to_vertex(*pos+delta)) {
          if (m_values.at(*w)>m_values.at(v)+1) continue;
          result.push_back(*w);
        }
      }
    }
    return result;
  }

  Vertices vertices() const {
    Vertices result{};
    for (auto const& [pos,v] : m_dict) {
      result.push_back(v);
    }
    return result;
  }

  std::optional<Vector> to_pos(Vertex v) const {
    std::optional<Vector> result{};
    auto iter = std::find_if(m_dict.begin(),m_dict.end(),[&v](auto const& entry){
      return entry.second == v;
    });
    if (iter != m_dict.end()) result = iter->first;
    return result;
  }

  std::optional<Vertex> to_vertex(Vector pos) const {
    // std::cout << "\nto_vertex(" << pos << ")";
    std::optional<Vertex> result{};
    auto iter = std::find_if(m_dict.begin(),m_dict.end(),[&pos](auto const& entry){
      return entry.first == pos;
    });
    if (iter != m_dict.end()) {
      result = iter->second;
    }
    // if (result) std::cout << " result:" << *result;
    // else std::cout << "null";
    return result;
  }

  std::optional<char> to_value(Vertex v) const {
    std::optional<char> result{};
    if (m_values.contains(v)) result = m_values.at(v);
    return result;
  }

  auto row_range() const {
    std::vector<Result> result{};
    for (auto row=top_left.row;row<=bottom_right.row;++row) result.push_back(row);
    return result;
  }

  auto col_range() const {
    std::vector<Result> result{};
    for (auto col=top_left.col;col<=bottom_right.col;++col) result.push_back(col);
    return result;
  }

private:
  Vector top_left{};
  Vector bottom_right{};
  using Dict = std::vector<std::pair<Vector,Vertex>>;
  Dict m_dict{};
  using Values = std::map<Vertex,char>;
  Values m_values{};
};

std::ostream& operator<<(std::ostream& os,Grid const& grid) {
  std::vector<std::string> map{};
  for (auto row : grid.row_range()) {
    map.push_back("");
    for (auto col : grid.col_range()) {
      Vector pos{.row=row,.col=col};
      if (auto v = grid.to_vertex(pos)) {
        map.back() += *grid.to_value(*v);
      }
      else {
        map.back() += ' ';
      }
    }
  }
  os << "\n" << map;
  return os;
}

namespace not_used {
  class DepthFirstSearch {
  public:
    DepthFirstSearch(Grid const& grid,Grid::Vertex s) {
      m_marked[s] = true;
      dfs(grid,s);
    }
  private:
    std::vector<bool> m_marked{};
    int m_count{};
    void dfs(Grid const& grid,Grid::Vertex const& v) {
      m_marked[v] = true;
      ++m_count;
      for (auto const& w : grid.adj(v)) {
        if (m_marked[w] == false) {
          dfs(grid,w);
        }
      }
    }
  };
}

template <typename T>
class IndexMinPQ {
public:
  using Index = Grid::Vertex;
  IndexMinPQ& insert(Index const& v,T dist) {
    m_map[v] = dist;
    return *this;
  }
  bool isEmpty() const {
    return (m_map.size()==0);
  }
  Index delMin() {
    auto iter = std::min_element(m_map.begin(),m_map.end(),[](auto const& e1,auto const& e2){
      return e1.second < e2.second;
    });
    auto result = iter->first;
    m_map.erase(iter);
    // std::cout << "\ndelMin() m_map.size:" << m_map.size();
    return result;
  }
  bool contains(Index const& v) const {
    return m_map.contains(v);
  }
  void change(Index const& v,T const& val) {
    m_map[v] = val;
  }
  
private:
  std::map<Index,T> m_map{};
};

class DijkstraSP {
public:
  const Result INFINITY_RESULT{std::numeric_limits<Result>::max()};
  using Vertex = Grid::Vertex;
  using Path = std::vector<Vertex>;
  using Graph = Grid;
  DijkstraSP(Graph const& G,Grid::Vertex const& s) : start{s} {
    for (auto v : G.vertices()) {
      distTo[v] = INFINITY_RESULT;
    }
    distTo[s] = Result{0};
    pq.insert(s,Result{0});
    while (pq.isEmpty()==false) {
      relax(G,pq.delMin());
    }
  }
  bool hasPathTo(Vertex v) {
    return (distTo[v] < INFINITY_RESULT);
  }
  Path pathTo(Vertex v) {
    Path result{};
    if (hasPathTo(v)==true) {
      result.push_back(v);
      for (auto w = edgeTo[v];hasPathTo(w) and w!=start;w=edgeTo[w]) {
        result.push_back(w);
      }
      result.push_back(start);
    }
    return result;
  }
private:
  Vertex start;
  std::map<Vertex,bool> marked{};
  std::map<Vertex,Vertex> edgeTo{}; // best parent to a vertex (to track shortest path)
  std::map<Vertex,Result> distTo{};
  IndexMinPQ<Result> pq{};
  void relax(Graph const& G,Vertex v) {
    if (marked[v] == false) {
      marked[v] = true; // used as basis for relaxation
      for (auto const& w : G.adj(v)) {
        if (distTo[w] > distTo[v] + 1) {
          distTo[w] = distTo[v] + 1;
          edgeTo[w] = v;
          if (pq.contains(w)) {
            pq.change(w,distTo[w]);
          }
          else {
            pq.insert(w,distTo[w]);
          }
        }
      }
    }
  }
};

struct Model {
  Grid G;
  Vector start;
  Vector end;
};

Model parse(auto& in) {
    Model result{};
    std::string line{};
    int row{0};
    Vector start{},end{};
    while (std::getline(in,line)) {
      int col{0};
      for (char ch : line) {
        if (ch=='S') {
          start.row = row;
          start.col = col;
          result.start = start;
          ch = 'a';
        }
        else if (ch=='E') {
          end.row=row;
          end.col=col;
          result.end = end;
          ch = 'z';
        }
        Vector pos{.row=row,.col=col};
        result.G.insert(pos,ch);
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
      std::cout << "\n" << data_model.G;
      auto s = data_model.G.to_vertex(data_model.start);
      auto z = data_model.G.to_vertex(data_model.end);
      if (s and z) {
        DijkstraSP sp(data_model.G,*s);
        auto path = sp.pathTo(*z);
        result = path.size()-1; // steps one less than visited vertices
        std::string s_path{};
        for (auto const& v : path) {
          s_path += *data_model.G.to_value(v);
        }
        std::cout << " result:" << result << " path:" << s_path;
      }
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      // find all 'a' vertices
      std::vector<Grid::Vertex> start_candidates{};
      for (auto const& v : data_model.G.vertices()) {
        if (*data_model.G.to_value(v) == 'a') {
          start_candidates.push_back(v);
        }
      }
      auto z = *data_model.G.to_vertex(data_model.end);
      std::vector<DijkstraSP::Path> paths{};
      for (auto const& v : start_candidates) {
        DijkstraSP sp(data_model.G,v);
        if (sp.hasPathTo(z)) {
          std::cout << "\nhas path to:" << z << std::flush;
          paths.push_back(sp.pathTo(z));
        }
      }
      auto iter = std::min_element(paths.begin(),paths.end(),[](auto const&  p1,auto const& p2){
        return p1.size() < p2.size();
      });
      result = iter->size()-1; // steps one less than visited vertices
      std::string s_path{};
      for (auto const& v : *iter) {
        s_path += *data_model.G.to_value(v);
      }
      std::cout << " result:" << result << " path:" << s_path;
      return result;
  }
}

int main(int argc, char *argv[])
{
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
