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

std::string to_trimmed(std::string const& s) {
  std::string result{};
  auto begin=s.begin();
  while  (std::isspace(*begin)) ++begin;
  auto end = s.end();--end;
  while (std::isspace(*end)) end--;
  return std::string{begin,end+1};
}

std::pair<std::string,std::string> to_splitted(std::string const& s,std::string const& delim) {
  auto pos = s.find(delim);
  if (pos != std::string::npos) {
    return {to_trimmed(s.substr(0,pos)),to_trimmed(s.substr(pos+delim.size()))};
  }
  else {
    return {to_trimmed(s),""};
  }
}

std::vector<std::string> to_tokens(std::string s,std::string const& delim) {
  std::vector<std::string> result{};
  while (true) {
    auto [head,tail] = to_splitted(s,delim);
    if (head.size()>0) {
      result.push_back(head);
      s = tail;
    }
    else break;
  }
  return result;
}

struct Vector {
  int row;
  int col;
  bool operator!=(Vector const& other) {
    return row != other.row or col != other.col;
  }
  bool operator<(Vector const& other) const {
    if (row == other.row) return col < other.col;
    else return row < other.row;
  }
  bool operator<=(Vector const& other) const {
    if (row == other.row) return col <= other.col;
    else return row <= other.row;
  }
  Vector operator+(Vector const& other) const {
    return {.row = row+other.row,.col=col+other.col};
  }
};

using Vectors = std::vector<Vector>;

std::ostream& operator<<(std::ostream& os,Vector const& v) {
  os << "[row:" << v.row << ",col:" << v.col << "]";
  return os;
}

using Map = std::map<Vector,char>;

Vector to_top_left(Vector const& v1,Vector const& v2) {
  // std::cout << "\nto_top_left(" << v1 << "," << v2 << ")";
  Vector result{.row = std::min(v1.row,v2.row),.col=std::min(v1.col,v2.col)};
  return result;
}

Vector to_bottom_right(Vector const& v1,Vector const& v2) {
  // std::cout << "\nto_bottom_right(" << v1 << "," << v2 << ")";
  Vector result{.row = std::max(v1.row,v2.row),.col=std::max(v1.col,v2.col)};
  return result;
}

struct Model {
  Model(Map const& map = Map{}) : map{map} {
    top_left=map.begin()->first;
    bottom_right=map.begin()->first;
    for (auto const& entry : map) {
      top_left = to_top_left(top_left,entry.first);
      bottom_right = to_bottom_right(bottom_right,entry.first);
    }
  }
  void insert(Vector const& pos,char ch) {
    std::cout << "\ninsert(pos:" << pos << ",ch:" << ch << ")";
    map[pos] = ch;
    top_left = to_top_left(top_left,pos);
    bottom_right = to_bottom_right(bottom_right,pos);
  }
  void erase(Vector const& pos) {
    if (map.contains(pos)) map.erase(pos);
  }
  Map map;
  Vector top_left;
  Vector bottom_right;
};

std::ostream& operator<<(std::ostream& os,Model const& model) {
  const int frame=3;
  os << "\n<map> top_left:" << model.top_left << " bottom_right:" << model.bottom_right;
  for (auto row=model.top_left.row;row<=model.bottom_right.row+frame;++row) {
    os << "\n";
    for (auto col=model.top_left.col-frame;col<=model.bottom_right.col+frame;++col) {
      Vector pos{.row=row,.col=col};
      if (model.map.contains(pos)) {
        os << model.map.at(pos);
      }
      else os << '.';
    }
  }
  return os;
}

Vector to_vertex(std::string const& s) {
  auto [s_col,s_row] = to_splitted(s,",");
  std::cout << "\nto_vertex(" << s << ")";
  Vector result{.row = std::stoi(s_row),.col=std::stoi(s_col)};
  std::cout << " " << result;
  return result;
}

using Vertices = std::vector<Vector>;
Vertices to_vertices(std::string const& s) {
  std::cout << "\nto_vertices(" << s << ")";
  Vertices result{};
  auto tokens = to_tokens(s,"->");
  for (auto const& token : tokens) {
    std::cout << "\n\ttoken:" << std::quoted(token);
    result.push_back(to_vertex(token));
  }
  return result;
}

std::vector<Vector> to_brick_line(std::pair<Vector,Vector> edge) {
  std::cout << "\nto_brick_line(" << edge.first << "," << edge.second << ")";
  std::vector<Vector> result{};
  if (edge.second<edge.first) {
    auto temp = edge.first;
    edge.first = edge.second;
    edge.second = temp;
    std::cout << "\ni.e., to_brick_line(" << edge.first << "," << edge.second << ")";
  }
  if (edge.first.row == edge.second.row) {
    for (int col=edge.first.col;col<=edge.second.col;++col) {
      Vector pos{.row=edge.first.row,.col=col};
      std::cout << "\n\tbrick:" << pos;
      result.push_back(pos);
    }
  }
  else if (edge.first.col == edge.second.col) {
    for (int row=edge.first.row;row<=edge.second.row;++row) {
      Vector pos{.row=row,.col=edge.first.col}; 
      std::cout << "\n\tbrick:" << pos;
      result.push_back(pos);
    }
  }
  else {
    std::cerr << "\nERROR, not a valid line " << edge.first << " to " << edge.second;
  }
  return result;
}

Model parse(auto& in) {
    Map map{};
    std::string line{};
    while (std::getline(in,line)) {
      auto vertices = to_vertices(line);
      std::pair<Vector,Vector> edge{}; edge.second = vertices[0];
      for (int index=1;index<vertices.size();++index) {
        edge.first = edge.second;
        edge.second = vertices[index];
        auto bricks = to_brick_line(edge);
        for (auto const& brick : bricks) {
          map[brick] = '#';
        }
      }
    }
    return Model{map};
}

struct Grain {
  Vector pos;
};


struct GrainEngine {
  std::vector<Grain> m_grains{};
  Model m_model;
  bool m_eternal_floor;

  GrainEngine(Model const& model,bool eternal_floor=false) 
    :  m_grains(1,Grain{Vector{.row=0,.col=500}})
      ,m_model{model}
      ,m_eternal_floor{eternal_floor} {
    m_model.insert(m_grains.back().pos,'O');
    if (m_eternal_floor) {
      m_model.bottom_right.row += 2;
    }
  }

  bool is_free(Map const& map,Vector const& pos) {
    if (map.contains(pos)) return (map.at(pos)=='.');
    else return (m_eternal_floor)?pos.row<m_model.bottom_right.row:true;
  }

  const Vectors DELTAS {
    {1,0}
    ,{1,-1}
    ,{1,1}
  };

  Vector to_moved_grain(Map const& map,Grain const& grain) {
    Vector result{grain.pos};
    for (auto const& delta : DELTAS) {
      auto pos = grain.pos + delta;
      if (is_free(map,pos)) {
        result = pos;
        break;
      }
    }
    return result;
  }

  bool operator++() {
    bool result{true};
    m_model.erase(m_grains.back().pos);
    auto pos = to_moved_grain(m_model.map,m_grains.back());
    if (pos != m_grains.back().pos) {
      // moved :)
      if (pos.row == m_model.bottom_right.row) {
        // overflow :)
        std::cout << "\nOVERFLOWED grain:" << pos;
        m_grains.pop_back();
        result = false;
      }
      else {
        m_grains.back().pos = pos;
        m_model.insert(m_grains.back().pos,'O');
      }
    }
    else {
      // at rest
      m_model.insert(m_grains.back().pos,'O');
      if (m_grains.back().pos.row!=0) {
        // drop new grain
        m_grains.push_back(Grain{Vector{.row=0,.col=500}});
        m_model.insert(m_grains.back().pos,'O');
      } 
      else {
        // Full
        result = false;
      }

    }
    return result;
  }
};

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\n" << data_model;
      GrainEngine ge{data_model};
      std::cout << "\n" << ge.m_model.map;
      while (++ge) {
        // std::cout << "\n" << ge.m_model.map;
      }
      std::cout << "\n" << ge.m_model.map;
      result = ge.m_grains.size();
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      GrainEngine ge{data_model,true};
      std::cout << "\n" << ge.m_model.map;
      while (++ge) {
        // std::cout << "\n" << ge.m_model.map;
      }
      std::cout << "\n" << ge.m_model.map;
      result = ge.m_grains.size();
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

char const* pTest = R"(498,4 -> 498,6 -> 496,6
503,4 -> 502,4 -> 502,9 -> 494,9)";
char const* pData = R"(504,28 -> 509,28
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
500,26 -> 505,26
494,30 -> 499,30
525,159 -> 529,159
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
484,40 -> 484,42 -> 479,42 -> 479,49 -> 496,49 -> 496,42 -> 489,42 -> 489,40
484,40 -> 484,42 -> 479,42 -> 479,49 -> 496,49 -> 496,42 -> 489,42 -> 489,40
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
484,40 -> 484,42 -> 479,42 -> 479,49 -> 496,49 -> 496,42 -> 489,42 -> 489,40
513,36 -> 513,37 -> 525,37
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
489,86 -> 493,86
516,162 -> 520,162
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
479,89 -> 479,91 -> 478,91 -> 478,95 -> 486,95 -> 486,91 -> 483,91 -> 483,89
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
522,156 -> 526,156
482,65 -> 482,67 -> 475,67 -> 475,70 -> 495,70 -> 495,67 -> 488,67 -> 488,65
485,98 -> 485,101 -> 478,101 -> 478,108 -> 493,108 -> 493,101 -> 489,101 -> 489,98
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
484,40 -> 484,42 -> 479,42 -> 479,49 -> 496,49 -> 496,42 -> 489,42 -> 489,40
482,65 -> 482,67 -> 475,67 -> 475,70 -> 495,70 -> 495,67 -> 488,67 -> 488,65
502,132 -> 506,132
516,34 -> 521,34
482,65 -> 482,67 -> 475,67 -> 475,70 -> 495,70 -> 495,67 -> 488,67 -> 488,65
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
508,128 -> 512,128
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
495,80 -> 499,80
484,40 -> 484,42 -> 479,42 -> 479,49 -> 496,49 -> 496,42 -> 489,42 -> 489,40
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
497,28 -> 502,28
534,156 -> 538,156
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
528,156 -> 532,156
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
498,83 -> 502,83
537,159 -> 541,159
485,98 -> 485,101 -> 478,101 -> 478,108 -> 493,108 -> 493,101 -> 489,101 -> 489,98
482,65 -> 482,67 -> 475,67 -> 475,70 -> 495,70 -> 495,67 -> 488,67 -> 488,65
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
485,98 -> 485,101 -> 478,101 -> 478,108 -> 493,108 -> 493,101 -> 489,101 -> 489,98
528,150 -> 532,150
522,162 -> 526,162
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
505,32 -> 510,32
498,32 -> 503,32
495,34 -> 500,34
484,40 -> 484,42 -> 479,42 -> 479,49 -> 496,49 -> 496,42 -> 489,42 -> 489,40
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
495,73 -> 495,74 -> 497,74 -> 497,73
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
479,89 -> 479,91 -> 478,91 -> 478,95 -> 486,95 -> 486,91 -> 483,91 -> 483,89
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
479,89 -> 479,91 -> 478,91 -> 478,95 -> 486,95 -> 486,91 -> 483,91 -> 483,89
479,89 -> 479,91 -> 478,91 -> 478,95 -> 486,95 -> 486,91 -> 483,91 -> 483,89
514,132 -> 518,132
479,89 -> 479,91 -> 478,91 -> 478,95 -> 486,95 -> 486,91 -> 483,91 -> 483,89
479,89 -> 479,91 -> 478,91 -> 478,95 -> 486,95 -> 486,91 -> 483,91 -> 483,89
513,36 -> 513,37 -> 525,37
505,134 -> 509,134
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
489,80 -> 493,80
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
501,30 -> 506,30
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
509,34 -> 514,34
511,134 -> 515,134
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
501,86 -> 505,86
488,34 -> 493,34
491,32 -> 496,32
510,166 -> 510,167 -> 516,167 -> 516,166
479,89 -> 479,91 -> 478,91 -> 478,95 -> 486,95 -> 486,91 -> 483,91 -> 483,89
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
488,112 -> 501,112
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
502,34 -> 507,34
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
485,98 -> 485,101 -> 478,101 -> 478,108 -> 493,108 -> 493,101 -> 489,101 -> 489,98
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
499,134 -> 503,134
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
531,159 -> 535,159
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
534,162 -> 538,162
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
485,98 -> 485,101 -> 478,101 -> 478,108 -> 493,108 -> 493,101 -> 489,101 -> 489,98
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
525,153 -> 529,153
531,153 -> 535,153
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
492,77 -> 496,77
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
486,83 -> 490,83
492,83 -> 496,83
495,73 -> 495,74 -> 497,74 -> 497,73
528,162 -> 532,162
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
510,166 -> 510,167 -> 516,167 -> 516,166
511,130 -> 515,130
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
512,32 -> 517,32
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
519,159 -> 523,159
485,98 -> 485,101 -> 478,101 -> 478,108 -> 493,108 -> 493,101 -> 489,101 -> 489,98
540,162 -> 544,162
482,65 -> 482,67 -> 475,67 -> 475,70 -> 495,70 -> 495,67 -> 488,67 -> 488,65
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
508,132 -> 512,132
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
495,86 -> 499,86
495,73 -> 495,74 -> 497,74 -> 497,73
485,98 -> 485,101 -> 478,101 -> 478,108 -> 493,108 -> 493,101 -> 489,101 -> 489,98
484,40 -> 484,42 -> 479,42 -> 479,49 -> 496,49 -> 496,42 -> 489,42 -> 489,40
510,166 -> 510,167 -> 516,167 -> 516,166
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
483,86 -> 487,86
482,65 -> 482,67 -> 475,67 -> 475,70 -> 495,70 -> 495,67 -> 488,67 -> 488,65
482,65 -> 482,67 -> 475,67 -> 475,70 -> 495,70 -> 495,67 -> 488,67 -> 488,65
496,23 -> 496,18 -> 496,23 -> 498,23 -> 498,14 -> 498,23 -> 500,23 -> 500,22 -> 500,23 -> 502,23 -> 502,15 -> 502,23
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62
513,147 -> 513,137 -> 513,147 -> 515,147 -> 515,140 -> 515,147 -> 517,147 -> 517,140 -> 517,147 -> 519,147 -> 519,140 -> 519,147 -> 521,147 -> 521,140 -> 521,147 -> 523,147 -> 523,141 -> 523,147 -> 525,147 -> 525,143 -> 525,147 -> 527,147 -> 527,146 -> 527,147 -> 529,147 -> 529,142 -> 529,147
517,134 -> 521,134
508,30 -> 513,30
505,130 -> 509,130
493,125 -> 493,119 -> 493,125 -> 495,125 -> 495,123 -> 495,125 -> 497,125 -> 497,124 -> 497,125 -> 499,125 -> 499,120 -> 499,125 -> 501,125 -> 501,122 -> 501,125 -> 503,125 -> 503,118 -> 503,125 -> 505,125 -> 505,121 -> 505,125 -> 507,125 -> 507,116 -> 507,125 -> 509,125 -> 509,119 -> 509,125
470,62 -> 470,60 -> 470,62 -> 472,62 -> 472,54 -> 472,62 -> 474,62 -> 474,54 -> 474,62 -> 476,62 -> 476,56 -> 476,62 -> 478,62 -> 478,60 -> 478,62 -> 480,62 -> 480,61 -> 480,62 -> 482,62 -> 482,60 -> 482,62 -> 484,62 -> 484,60 -> 484,62)";
