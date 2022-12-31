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

using Result = long long int;
using Answers = std::vector<std::pair<std::string,Result>>;

struct Vector {
  Result row;
  Result col;
  bool operator!=(Vector const& other) {
    return row!=other.row or col != other.col;
  }
  bool operator<(Vector const& other) const {
    if (row == other.row) return col < other.col;
    else return row < other.row;
  }
  bool operator<=(Vector const& other) const {
    if (row == other.row) return col <= other.col;
    else return row <= other.row;
  }
};

std::ostream& operator<<(std::ostream& os,Vector const& v) {
  os << "[row:" << v.row << ",col:" << v.col << "]";
  return os;
}

using SensorBeaconPair = std::pair<Vector,Vector>;
using SensorBeaconPairs = std::vector<SensorBeaconPair>;

using Map = std::map<Vector,char>;

Vector to_top_left(Vector const& v1,Vector const& v2) {
  Vector result{.row = std::min(v1.row,v2.row),.col=std::min(v1.col,v2.col)};
  return result;
}

Vector to_bottom_right(Vector const& v1,Vector const& v2) {
  std::cout << "\nto_bottom_right(" << v1 << "," << v2 << ")";
  Vector result{.row = std::max(v1.row,v2.row),.col=std::max(v1.col,v2.col)};
  return result;
}

struct Model {
  Model(SensorBeaconPairs const& sb_pairs) : sb_pairs{sb_pairs}, map{to_map(sb_pairs)} {
    top_left=map.begin()->first;
    bottom_right=map.begin()->first;
    for (auto const& entry : map) {
      top_left = to_top_left(top_left,entry.first);
      bottom_right = to_bottom_right(bottom_right,entry.first);
    }
  }
  void insert(Vector const& pos,char ch) {
    map[pos] = ch;
    top_left = to_top_left(top_left,pos);
    bottom_right = to_bottom_right(bottom_right,pos);
  }
  SensorBeaconPairs sb_pairs;
  Map map;
  Vector top_left;
  Vector bottom_right;
  Map to_map(SensorBeaconPairs const& sb_pairs) {
    std::cout << "\nto_map(sb_pairs.size():" << sb_pairs.size() << ")";
    Map result{};
    for (auto const& sb_pair : sb_pairs) {
      insert(sb_pair.first,'S');
      insert(sb_pair.second,'B');
    }
    return result;
  }
};

std::ostream& operator<<(std::ostream& os,Model const& model) {
  const Result frame=3;
  os << "\n<map> top_left:" << model.top_left << " bottom_right:" << model.bottom_right;
  for (auto row=model.top_left.row-frame;row<=model.bottom_right.row+frame;++row) {
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
  std::cout << "\nto_vertex(" << s << ")";
  auto [l_expr,r_expr] = to_splitted(s,", ");
  auto [s_x,s_col] = to_splitted(l_expr,"="); 
  auto [s_y,s_row] = to_splitted(r_expr,"="); 
  Vector result{.row=std::stoi(s_row),.col=std::stoi(s_col)};
  std::cout << " pos:" << result;
  return result;  
}

SensorBeaconPair to_sb_pair(std::string const& s) {
  std::cout << "\nto_sb_pair(" << s << ")";
  // Sensor at x=2, y=18: closest beacon is at x=-2, y=15
  auto [head,tail] = to_splitted(s,":");
  std::cout << " head:" << std::quoted(head) << " tail:" << std::quoted(tail);
  const std::string sensor_at{"Sensor at"};
  const std::string closest_beacon_is_at{"closest beacon is at"};
  head = to_trimmed(head.substr(sensor_at.size()));
  tail = to_trimmed(tail.substr(closest_beacon_is_at.size()));
  auto sensor = to_vertex(head);
  auto beacon = to_vertex(tail); 
  std::cout << " head:" << std::quoted(head) << " tail:" << std::quoted(tail);
  return SensorBeaconPair{sensor,beacon};
}


Model parse(auto& in) {
    SensorBeaconPairs sb_pairs{};
    std::string line{};
    while (std::getline(in,line)) {
      auto sb_pair = to_sb_pair(line);
      sb_pairs.push_back(sb_pair);
    }
    return Model{sb_pairs};
}

void mark_covarage(Model& model,SensorBeaconPair const& sb_pair) {
  std::cout << "\nmark_covarage(S:" << sb_pair.first << " B:" << sb_pair.second << ")"; 
  Result manhattan_distance = std::abs(sb_pair.first.row - sb_pair.second.row) + std::abs(sb_pair.first.col - sb_pair.second.col);
  std::cout << "\n\tmanhattan_distance:" << manhattan_distance;
  Vector upper_left{.row=sb_pair.first.row-manhattan_distance,.col=sb_pair.first.col-manhattan_distance};
  Vector lower_right{.row=sb_pair.first.row+manhattan_distance,.col=sb_pair.first.col+manhattan_distance};
  std::cout << "\n\tupper_left:" << upper_left << " lower_right:" << lower_right;
  for (auto row=upper_left.row;row<=lower_right.row;++row) {
    for (auto col=upper_left.col;col<=lower_right.col;++col) {
      Vector pos{.row=row,.col=col};
      std::cout << "\n\tpos:" << pos;
      auto pos_manhattan_distance = (std::abs(sb_pair.first.row - pos.row) + std::abs(sb_pair.first.col - pos.col));
      std::cout << " pos_manhattan_distance:" << pos_manhattan_distance;
      if ((pos!=sb_pair.first and (pos_manhattan_distance <= manhattan_distance) and pos!=sb_pair.second)) {
        model.insert(pos,'!'); // covered by sensor
      }
    }
  }
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\n" << data_model;
      for (auto const& sb_pair : data_model.sb_pairs) {
        mark_covarage(data_model,sb_pair);
      }
      std::cout << "\n" << data_model;
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

char const* pTest = R"(Sensor at x=2, y=18: closest beacon is at x=-2, y=15
Sensor at x=9, y=16: closest beacon is at x=10, y=16
Sensor at x=13, y=2: closest beacon is at x=15, y=3
Sensor at x=12, y=14: closest beacon is at x=10, y=16
Sensor at x=10, y=20: closest beacon is at x=10, y=16
Sensor at x=14, y=17: closest beacon is at x=10, y=16
Sensor at x=8, y=7: closest beacon is at x=2, y=10
Sensor at x=2, y=0: closest beacon is at x=2, y=10
Sensor at x=0, y=11: closest beacon is at x=2, y=10
Sensor at x=20, y=14: closest beacon is at x=25, y=17
Sensor at x=17, y=20: closest beacon is at x=21, y=22
Sensor at x=16, y=7: closest beacon is at x=15, y=3
Sensor at x=14, y=3: closest beacon is at x=15, y=3
Sensor at x=20, y=1: closest beacon is at x=15, y=3)";
char const* pData = R"(Sensor at x=346753, y=3974683: closest beacon is at x=527500, y=3570474
Sensor at x=3718952, y=2421864: closest beacon is at x=3871651, y=2935623
Sensor at x=919295, y=2535231: closest beacon is at x=527500, y=3570474
Sensor at x=3982082, y=2935124: closest beacon is at x=3871651, y=2935623
Sensor at x=3693260, y=3095908: closest beacon is at x=3669901, y=3086819
Sensor at x=3273954, y=2072378: closest beacon is at x=3275433, y=2000000
Sensor at x=2936799, y=2402146: closest beacon is at x=2472215, y=2601723
Sensor at x=3153125, y=3571786: closest beacon is at x=3669901, y=3086819
Sensor at x=3892050, y=3718400: closest beacon is at x=3871651, y=2935623
Sensor at x=1670487, y=618098: closest beacon is at x=-171619, y=497931
Sensor at x=277848, y=3523156: closest beacon is at x=527500, y=3570474
Sensor at x=3412305, y=198361: closest beacon is at x=3275433, y=2000000
Sensor at x=2324630, y=2084344: closest beacon is at x=2472215, y=2601723
Sensor at x=3718763, y=3088136: closest beacon is at x=3669901, y=3086819
Sensor at x=303245, y=10539: closest beacon is at x=-171619, y=497931
Sensor at x=3230426, y=1961497: closest beacon is at x=3275433, y=2000000
Sensor at x=3616662, y=3171194: closest beacon is at x=3669901, y=3086819
Sensor at x=3673367, y=3084002: closest beacon is at x=3669901, y=3086819
Sensor at x=3350734, y=2116841: closest beacon is at x=3275433, y=2000000
Sensor at x=1348476, y=3646233: closest beacon is at x=527500, y=3570474
Sensor at x=2817552, y=1654754: closest beacon is at x=3275433, y=2000000
Sensor at x=22462, y=1187401: closest beacon is at x=-171619, y=497931
Sensor at x=3982255, y=1687776: closest beacon is at x=3275433, y=2000000
Sensor at x=3995256, y=2821993: closest beacon is at x=3871651, y=2935623
Sensor at x=501107, y=3777047: closest beacon is at x=527500, y=3570474
Sensor at x=2507803, y=2761101: closest beacon is at x=2472215, y=2601723
Sensor at x=2252560, y=2800057: closest beacon is at x=2370240, y=2923480
Sensor at x=3536077, y=2843837: closest beacon is at x=3669901, y=3086819
Sensor at x=533494, y=3224642: closest beacon is at x=527500, y=3570474
Sensor at x=2962174, y=3016018: closest beacon is at x=2370240, y=2923480
Sensor at x=177378, y=3319282: closest beacon is at x=527500, y=3570474
Sensor at x=2074019, y=3934937: closest beacon is at x=2370240, y=2923480
Sensor at x=2295953, y=3295493: closest beacon is at x=2370240, y=2923480)";
