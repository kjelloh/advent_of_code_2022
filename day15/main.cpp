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
  bool operator==(Vector const& other) const {
    return (row==other.row and col==other.col);
  }
  bool operator<(Vector const& other) const {
    if (row == other.row) return col < other.col;
    else return row < other.row;
  }
};

std::ostream& operator<<(std::ostream& os,Vector const& v) {
  os << "[row:" << v.row << ",col:" << v.col << "]";
  return os;
}

using SensorBeaconPair = std::pair<Vector,Vector>;
using SensorBeaconPairs = std::vector<SensorBeaconPair>;

Vector to_top_left(Vector const& v1,Vector const& v2) {
  Vector result{.row = std::min(v1.row,v2.row),.col=std::min(v1.col,v2.col)};
  return result;
}

Vector to_bottom_right(Vector const& v1,Vector const& v2) {
  std::cout << "\nto_bottom_right(" << v1 << "," << v2 << ")";
  Vector result{.row = std::max(v1.row,v2.row),.col=std::max(v1.col,v2.col)};
  return result;
}

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
  void insert(Vector const& pos,char ch) {
    // std::cout << "\nGrid::insert(" << pos << ")";
    auto iter = std::find_if(m_dict.begin(),m_dict.end(),[&pos](auto const& entry){
      return entry.first == pos;
    });
    if (iter == m_dict.end()) {
      iter = m_dict.insert(iter,{pos,m_dict.size()});
      m_value[iter->second] = ch;
      m_top_left.row = std::min(m_top_left.row,pos.row);
      m_top_left.col = std::min(m_top_left.col,pos.col);
      m_bottom_right.row = std::max(m_bottom_right.row,pos.row);
      m_bottom_right.col = std::max(m_bottom_right.col,pos.col);
    }
    else {
      m_value[iter->second] = ch;      
    }
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
    if (m_value.contains(v)) result = m_value.at(v);
    return result;
  }

  auto row_range() const {
    std::vector<Result> result{};
    for (auto row=m_top_left.row;row<=m_bottom_right.row;++row) result.push_back(row);
    return result;
  }

  auto col_range() const {
    std::vector<Result> result{};
    for (auto col=m_top_left.col;col<=m_bottom_right.col;++col) result.push_back(col);
    return result;
  }

  Vector top_left() const {return m_top_left;}
  Vector bottom_right() const {return m_bottom_right;}

private:
  Vector m_top_left{};
  Vector m_bottom_right{};
  using Dict = std::vector<std::pair<Vector,Vertex>>;
  Dict m_dict{};
  using Value = std::map<Vertex,char>;
  Value m_value{};
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


struct Model {
  Model(SensorBeaconPairs const& sb_pairs) : sb_pairs{sb_pairs}, grid{to_grid(sb_pairs)} {
  }
  SensorBeaconPairs sb_pairs;
  Grid grid;
  Grid to_grid(SensorBeaconPairs const& sb_pairs) {
    std::cout << "\nto_grid(sb_pairs.size():" << sb_pairs.size() << ")";
    Grid result{};
    for (auto const& sb_pair : sb_pairs) {
      result.insert(sb_pair.first,'S');
      result.insert(sb_pair.second,'B');
    }
    return result;
  }
};

std::ostream& operator<<(std::ostream& os,Model const& model) {
  const Result frame=3;
  auto top_left = model.grid.top_left();
  auto bottom_right = model.grid.bottom_right();
  os << "\n<map> top_left:" << top_left << " bottom_right:" << bottom_right;
  os << model.grid;
  return os;
}

Vector to_vertex(std::string const& s) {
  // std::cout << "\nto_vertex(" << s << ")";
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

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      int target_row{10}; // test
      if (data_model.grid.bottom_right().row > 2000000) target_row = 2000000; // part 1
      std::set<Vector> covered_pos_on_target_row{};
      for (auto const& sb_pair : data_model.sb_pairs) {
        auto manhattan_distance = std::abs(sb_pair.first.row - sb_pair.second.row) + std::abs(sb_pair.first.col - sb_pair.second.col);
        std::cout << "\nS:" << sb_pair.first << " B:" << sb_pair.second << " manhattan_distance:" << manhattan_distance;
        auto over_shoot =  manhattan_distance - std::abs(sb_pair.first.row - target_row);
        if (over_shoot >= 0) {
          std::cout << " over_shoot:" << over_shoot;
          // Will affect coverage on target_row
          for (int col=sb_pair.first.col-over_shoot;col<=sb_pair.first.col+over_shoot;++col) {
            Vector pos{.row=target_row,.col=col};
            // std::cout << "\n\tpos:" << pos;
            auto manhattan_distance_to_target = std::abs(sb_pair.first.row - pos.row) + std::abs(sb_pair.first.col - pos.col);
            // std::cout << " manhattan_distance_to_target:" << manhattan_distance_to_target;
            if (auto v = data_model.grid.to_vertex(pos)) {
              // do not count S or B
            }
            else {
              if (manhattan_distance_to_target <= manhattan_distance) {
                covered_pos_on_target_row.insert(pos);
                // std::cout << " Interval";
              }
            }
          }
        }
      }
      result = covered_pos_on_target_row.size();
      return result;
  }
}

Result manhattan_distance(Vector const& pos1,Vector const& pos2) {
  return std::abs(pos1.row-pos2.row) + std::abs(pos1.col-pos2.col);
}

struct BoundingBox {
  BoundingBox(Vector const& sensor,Vector const& beacon) {
    auto reach = manhattan_distance(sensor,beacon);
    top_left.row = sensor.row - reach;
    top_left.col = sensor.col - reach;
    bottom_right.row = sensor.row + reach;
    bottom_right.col = sensor.col + reach;
  }
  Vector top_left;
  Vector bottom_right;
  bool operator<(BoundingBox const& other) const {
    if (top_left==other.top_left) return bottom_right<other.bottom_right;
    else return top_left<other.top_left;
  }
};

struct SensorBeaconFrame {
  SensorBeaconFrame(Vector const& sensor_pos,Vector const& beacon_pos) 
    :  S{sensor_pos}
      ,B{beacon_pos}    
      ,reach{sensor_pos,beacon_pos} {
  }
  bool operator<(SensorBeaconFrame const& other) const {
    return reach < other.reach;
  }
  Vector S;
  Vector B;
  BoundingBox reach;
};

using SensorBeaconFrames = std::vector<SensorBeaconFrame>;

using Interval = std::pair<Result,Result>;

namespace part2 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      bool is_test_data = data_model.grid.bottom_right().row < 2000000;
      int search_boundary{20}; // part 2 test
      if (!is_test_data) search_boundary = 4000000; // part 2

      SensorBeaconFrames sb_frames{};
      for (auto const& sb : data_model.sb_pairs) {
        sb_frames.push_back(SensorBeaconFrame(sb.first,sb.second));
      }
      std::sort(sb_frames.begin(),sb_frames.end(),[](auto const& sbf1, auto const& sbf2){
        return (sbf1.reach < sbf2.reach); // sort low to high on row, for same row on col
      });

      if (true) {
        // Log
        for (auto const& sb_frame : sb_frames) {
          std::cout << "\nframe:" << sb_frame.reach.top_left << " to " << sb_frame.reach.bottom_right;
        }
      }

      if (false) {
        // Test
        auto intersect_count = std::vector<Result>(search_boundary+1,0);
        Result min_count{std::numeric_limits<Result>::max()},max_count{};
        for (int row=0;row<=search_boundary;++row) {
          for (auto const& sb_frame : sb_frames) {
            if (sb_frame.reach.top_left.row <= row and row <= sb_frame.reach.bottom_right.row) ++intersect_count[row];
          }
          min_count = std::min(min_count,intersect_count[row]);
          max_count = std::max(max_count,intersect_count[row]);
        }
        if (true) {
          int intersected_rows{};
          for (int row=0;row<=search_boundary;++row) {
            if (intersect_count[row]>0) ++intersected_rows;
          }
          std::cout << "\nintersected rows:" << intersected_rows;
          std::cout << "\nmin_count:" << min_count << " max_count:" << max_count;
          // For my input data i got
          // intersected rows:4000001
          // min_count:3 max_count:17
          // So we may be able to reduce the search space?
        }
      }

      std::map<Result,Result> uncovered{};
      for (int row=0;row<=search_boundary;++row) {
        std::vector<Interval> covered_intervals{};
        std::vector<Result> x_boundaries{};
        for (auto const& sb_frame : sb_frames) {
          if (sb_frame.reach.top_left.row <= row and row <= sb_frame.reach.bottom_right.row) {
            // intersects
            auto overreach = (row < sb_frame.S.row)?std::abs(sb_frame.reach.top_left.row-row):std::abs(sb_frame.reach.bottom_right.row - row);
            if (overreach<0) {std::cout << "\noverreach:" << overreach ;exit(1);}
            Interval covered{sb_frame.S.col-overreach,sb_frame.S.col+overreach};
            covered_intervals.push_back(covered);
            // mark the boundaries on the x-axis (column) where the covered state changes 
            x_boundaries.push_back(covered.first); // changes at start
            x_boundaries.push_back(covered.second+1); // changes at one beyond end
            if (is_test_data) std::cout << "\nrow:" << row << " covered.first:" << covered.first << " covered.second:" << covered.second;
          }
        }
        // Ensure we have the boundaries in ascending order
        std::sort(x_boundaries.begin(),x_boundaries.end());
        // Compress x_boundaries to indexed intervals mapped to bool for mark "is covered"
        // For inspiration, see solution to advent of code 2021 day 22 (but in a single dimension here)
        // The idea is to map each interval in x_boundaries to a flag indicating if that interval is covered or not
        auto boundaries_count = x_boundaries.size();
        auto on_off_vector = std::vector(boundaries_count,false);
        // Let's define a helper lambda to return the index in the boundaries vector of coordinate (in out case x, i.e., the column) in the boundaries vector
        auto on_off_vector_index = [](auto const& boundaries,Result c)  {
          // Return the index in the boundaries vector of the provided boundary c.
          // This is the distance from the beginning of the sorted boundaries vector
          // to the position of c in the vector
          auto iter = std::lower_bound(boundaries.begin(),boundaries.end(),c);
          // Note: We need to use lower_bound (and not find) as we added end+1 for the end-boundary so when
          // called with a cuboid end boundary it will not be exactly found. the lower_bound will :)
          return std::distance(boundaries.begin(),iter);
        };

        // Now map each covered interval to an index in the on_off_vector marking if this interval is covered or not
        for (auto const& interval : covered_intervals) {
          /*
          The on_off vector has a location for each interval of coordinates with the same state.
          Thus, a boundary vector {c0,c1,c2...cn} defines ranges [c0,c1[, [c1,c2[, [c2,*[ .. [*,cn[
          with index 0..n                     on_off grid index    0        1        2        n-1     
          */

          auto on_off_x_first = on_off_vector_index(x_boundaries,interval.first);
          auto on_off_x_second = on_off_vector_index(x_boundaries,interval.second+1);
          // Now mark all covered ranges with true (is covered)
          for (int on_off_x=on_off_x_first;on_off_x<on_off_x_second;on_off_x++) {
            on_off_vector[on_off_x] = true; // the interval with this index is covered
            // Note that overlapping cuboids will overwrite previous state
            // in the overlapping region (just as they should)
          }
        }

        if (true and is_test_data) {
          // Log
          if (row==11) {
            std::cout << "\non_off_vector:";
            for (auto is_covered : on_off_vector) {
              std::cout << " " << is_covered;
            }
          }
        }

        // Now Magic has happened! The on_off_vector now represents the on/off state of this row
        // We "just" needs to map the on-regions back to actual row (x) coordinates to get the
        // "count" of all positions that is "on".
        Result off_count{0};
        Result uncovered_col{};
        for (int on_off_x=0;on_off_x < boundaries_count-1;on_off_x++) {
          if (on_off_vector[on_off_x]==false) {
            // Note that the interval is (..( (the last coordinate is NOT part of the range)
            // Thus end-start IS the count of position in the range [..[
            Result dx = (x_boundaries[on_off_x+1] - x_boundaries[on_off_x]);
            if (dx>0) {
              uncovered_col=x_boundaries[on_off_x]; // assume only one for this puzzle
            }
            off_count += dx;
          }
        }
        if (true and is_test_data) {
          // Log
          std::cout << "\nrow:" << row << " off_count:" << off_count;
        }

        if (off_count==1) {
          auto y = row;
          auto x = uncovered_col;
          Vector uncovered_pos{.row=y,.col=x};
          std::cout << "\nUNCOVERED x:" << x << " y:" << y;
          result = 4000000*x + y;
          break; // assume only one uncovered on any row
        }
      }
      return result;
  }
}

int main(int argc, char *argv[])
{
  Answers answers{};
  answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
  answers.push_back({"Part 1     ",part1::solve_for(pData)});
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
