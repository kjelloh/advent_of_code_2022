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

using WorryLevels = std::vector<Result>;

struct NewWlFunc {
  std::string l_term;
  char op;
  std::string r_term;
  Result operator()(Result old) {
    Result result{};
    // std::cout << "\n\tNewWlFunc::operator(" << old << ") l_term:" << std::quoted(l_term) << " op:'" << op << "' r_term:" << std::quoted(r_term);
    Result term2{};
    if (r_term == "old") {
      term2 = old;
    }
    else {
      term2=std::stoi(r_term);
    }
    switch (op) {
      case '+': result = old + term2;break;
      case '*': result = old * term2; break;
      default: std::cerr << "\nERROR, unknown op:" << op;
    }
    // std::cout << " = " << result;
    return result;
  };
};

using ToThrow = std::vector<std::pair<Result,Result>>;

struct IfTrueOrFalseOp {
  Result target_monkey_id;
  void operator()(Result wl,ToThrow& to_throw) {
    to_throw.push_back({target_monkey_id,wl});
  }
};

struct Monkey {
  int id{};
  Result reduction{3};
  Result inspect_count{};
  WorryLevels worry_levels{};
  NewWlFunc new_wl;
  Result divisor;
  IfTrueOrFalseOp if_true_op;
  IfTrueOrFalseOp if_false_op;
  void perform_turn(ToThrow& to_throw) {
    while (worry_levels.size()>0) {
      auto wl = worry_levels.front();
      worry_levels.erase(worry_levels.begin());
      wl = new_wl(wl);
      if (reduction==3) wl /= 3; // part 1
      else wl = (wl>reduction)?wl % reduction:wl; // part 2
      // std::cout << "\n\twl:" << wl << std::flush;
      if (wl % divisor == 0) {
        if_true_op(wl,to_throw);
      }
      else {
        if_false_op(wl,to_throw);
      }
      ++inspect_count;
    }
  }
};

using Monkeys = std::map<int,Monkey>;

using Model = Monkeys;

std::ostream& operator<<(std::ostream& os,Monkey const& monkey) {
  os << "monkey:" << monkey.id;
  os << "\n\tworry_levels:";
  for (auto wl : monkey.worry_levels) {
    os << " wl:" << wl;
  }
  return os;
}

std::string to_trimmed(std::string const& s) {
  std::string result{};
  auto begin=s.begin();
  while  (std::isspace(*begin)) ++begin;
  auto end = s.end();--end;
  while (std::isspace(*end)) end--;
  return std::string{begin,end+1};
}

std::pair<std::string,std::string> to_splitted(std::string const& s,char delim) {
  auto pos = s.find(delim);
  if (pos != std::string::npos) {
    return {to_trimmed(s.substr(0,pos)),to_trimmed(s.substr(pos+1))};
  }
  else {
    return {to_trimmed(s),""};
  }
}

std::vector<std::string> to_tokens(std::string s,char delim) {
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

Monkey to_monkey(std::vector<std::string> const& lines) {
  Monkey result{};
  std::cout << "\nto_monkey";
  for (int i=0;i<lines.size();++i) {
    /*
    0:Monkey 0:
    1:  Starting items: 75, 63
    2:  Operation: new = old * 3
    3:  Test: divisible by 11
    4:    If true: throw to monkey 7
    5:    If false: throw to monkey 2
    */
    auto& line = lines[i];
    auto [head,tail] = to_splitted(line,':');
    std::cout << "\n line:" << std::quoted(line) << " head:" << head << " tail:" << tail;
    switch (i) {
      case 0: {
        auto [m,id] = to_splitted(head,' ');
        result.id = std::stoi(id);
      } break;
      case 1: {
        auto tokens = to_tokens(tail,',');
        for (auto const& token : tokens) std::cout << " token:" << std::quoted(token);
        for (auto const& token : tokens) result.worry_levels.push_back(std::stoi(token));
      } break;
      case 2: {
        auto [l_term,expression] = to_splitted(tail,'=');
        std::cout << " l_term:" << std::quoted(l_term) << " expression:" << std::quoted(expression);  
        auto tokens = to_tokens(expression,' ');
        for (auto const& token : tokens) std::cout << " token:" << std::quoted(token);
        NewWlFunc new_wl{
          .l_term = tokens[0]
          ,.op = tokens[1][0]
          ,.r_term = tokens[2]
        };
        result.new_wl = new_wl;
      } break;
      case 3: {
        auto tokens = to_tokens(tail,' ');
        for (auto const& token : tokens) std::cout << " token:" << std::quoted(token);
        result.divisor = std::stoi(tokens[2]);  
      } break;
      case 4: {
        auto tokens = to_tokens(tail,' ');
        for (auto const& token : tokens) std::cout << " token:" << std::quoted(token);
        if (head.find("true") == std::string::npos) std::cerr << "\nERROR, NOT an 'if true':" << std::quoted(head);
        IfTrueOrFalseOp if_true_op{
          .target_monkey_id = std::stoi(tokens[3])
        };
        result.if_true_op = if_true_op;
      } break;
      case 5: {
        auto tokens = to_tokens(tail,' ');
        for (auto const& token : tokens) std::cout << " token:" << std::quoted(token);      
        if (head.find("false") == std::string::npos) std::cerr << "\nERROR, NOT an 'if false':" << std::quoted(head);
        IfTrueOrFalseOp if_false_op{
          .target_monkey_id = std::stoi(tokens[3])
        };
        result.if_false_op = if_false_op;
      } break;
    }
  }
  std::cout << " result:" << result;
  return result;
}

Model parse(auto& in) {
    Model result{};
    std::string line{};
    std::vector<std::string> lines{};
    while (std::getline(in,line)) {
      if (line.size()==0) {
        auto monkey = to_monkey(lines);
        result[monkey.id] = monkey;
        lines.clear();
      }
      else lines.push_back(line);
    }
    auto monkey = to_monkey(lines);
    result[monkey.id] = monkey;
    lines.clear();    
    return result;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      ToThrow to_trow{};
      for (int round = 1;round<=20;++round) {
        std::cout << "\nBEGIN round:" << round;
        for (auto& [id,monkey] : data_model) {
          ToThrow to_throw{};
          std::cout << "\n\teval monkey:" << monkey << std::flush;
          monkey.perform_turn(to_throw);
          for (auto const& entry : to_throw) {
            std::cout << "\n\twl:" << entry.second << " to monkey:" << entry.first;
            data_model[entry.first].worry_levels.push_back(entry.second);
          }
        }
      }
      Result m1{data_model[0].inspect_count};
      Result m2{data_model[1].inspect_count};
      for (auto const& entry : data_model) {
        std::cout << "\nmonkey:" << entry.second.id << " inspect_count:" << entry.second.inspect_count;
        if (entry.second.inspect_count>m1) {
          m2 = m1;
          m1 = entry.second.inspect_count;
        }
        else if (entry.second.inspect_count>m2) m2 = entry.second.inspect_count;
      }      
      std::cout << "\nm1:" << m1 << " m2:" << m2;
      result = m1*m2;
      return result;
  }
}

namespace part2 {

  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      auto reduction = std::accumulate(data_model.begin(),data_model.end(),Result{1},[](Result acc,auto const& entry){
        std::cout << "\nmonkey.divisor:" << entry.second.divisor; 
        acc *= entry.second.divisor;
        return acc;
      });
      std::cout << "\nreduction:" << reduction;
      for (auto& [id,monkey] : data_model) monkey.reduction = reduction;
      ToThrow to_trow{};
      for (int round = 1;round<=10000;++round) {
        std::cout << "\nBEGIN round:" << round;
        for (auto& [id,monkey] : data_model) {
          ToThrow to_throw{};
          std::cout << "\n\teval monkey:" << monkey << std::flush;
          monkey.perform_turn(to_throw);
          for (auto const& entry : to_throw) {
            std::cout << "\n\twl:" << entry.second << " to monkey:" << entry.first;
            data_model[entry.first].worry_levels.push_back(entry.second);
          }
        }
      }
      Result m1{};
      Result m2{};
      for (auto const& entry : data_model) {
        std::cout << "\nmonkey:" << entry.second.id << " inspect_count:" << entry.second.inspect_count;
        if (entry.second.inspect_count>m1) {
          m2 = m1;
          m1 = entry.second.inspect_count;
        }
        else if (entry.second.inspect_count>m2) m2 = entry.second.inspect_count;
      }      
      std::cout << "\nm1:" << m1 << " m2:" << m2;
      result = m1*m2;
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

char const* pTest = R"(Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1)";
char const* pData = R"(Monkey 0:
  Starting items: 75, 63
  Operation: new = old * 3
  Test: divisible by 11
    If true: throw to monkey 7
    If false: throw to monkey 2

Monkey 1:
  Starting items: 65, 79, 98, 77, 56, 54, 83, 94
  Operation: new = old + 3
  Test: divisible by 2
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 66
  Operation: new = old + 5
  Test: divisible by 5
    If true: throw to monkey 7
    If false: throw to monkey 5

Monkey 3:
  Starting items: 51, 89, 90
  Operation: new = old * 19
  Test: divisible by 7
    If true: throw to monkey 6
    If false: throw to monkey 4

Monkey 4:
  Starting items: 75, 94, 66, 90, 77, 82, 61
  Operation: new = old + 1
  Test: divisible by 17
    If true: throw to monkey 6
    If false: throw to monkey 1

Monkey 5:
  Starting items: 53, 76, 59, 92, 95
  Operation: new = old + 2
  Test: divisible by 19
    If true: throw to monkey 4
    If false: throw to monkey 3

Monkey 6:
  Starting items: 81, 61, 75, 89, 70, 92
  Operation: new = old * old
  Test: divisible by 3
    If true: throw to monkey 0
    If false: throw to monkey 1

Monkey 7:
  Starting items: 81, 86, 62, 87
  Operation: new = old + 8
  Test: divisible by 13
    If true: throw to monkey 3
    If false: throw to monkey 5)";
