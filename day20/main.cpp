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
#include <list>
#include <ranges> // E.g., std::subrange, std::view 
#include <utility> // E.g., std::pair,..
#include <algorithm> // E.g., std::find, std::all_of,...
#include <numeric> // E.g., std::accumulate
#include <limits> // E.g., std::numeric_limits
#include <iomanip>
#include <cassert>

extern char const* pTest;
extern char const* pData;

using Integer = int64_t;
using Result = int64_t;
using Answers = std::vector<std::pair<std::string,Result>>;

using Values = std::vector<int>;

class CircularList {
public:
  friend std::ostream& operator<<(std::ostream& os,CircularList const& cl);
  struct Node {
    Integer value{};
    Node* next{nullptr};
    Node* prev{nullptr};
  };
  struct ConstIterator {
    using difference_type = int;
    using value_type = Integer;	
    using pointer	= Node*;
    using reference	= Node;
    using iterator_category = std::bidirectional_iterator_tag;
    Node const* p;
    Integer index;
    Integer cycle;
    bool is_begin{false};
    Node const* ptr() {return p;}
    int operator*() const {return p->value;}
    int pos() const {
      auto result = index%cycle;
      if (result<0) result += cycle;
      return result;
    }
    ConstIterator& operator++() {
      p=p->next;
      ++index;
      index %= cycle; // Keep within cycle
      if (index<0) index+=cycle; // use positive index 0..(cycle-1)
      // std::cout << "\n++ index:" << index << " pos:" << pos();
      is_begin=false; // no longer begin
      return *this;
    }
    ConstIterator(Node const* p,Integer index,Integer cycle,bool is_begin = false) : p{p},index{index},cycle{cycle},is_begin{is_begin} {}
    ConstIterator& operator--() {
      p=p->prev;
      --index;
      index %= cycle; // Keep within cycle
      if (index<0) index+=cycle; // use positive index 0..(cycle-1)
      // std::cout << " --index:" << index << " pos:" << pos();
      is_begin=false; // no longer begin
      return *this;
    }
    bool operator!=(ConstIterator const& other) {
      bool result{};
      if (p==other.p) result = (is_begin)?index!=other.index:false; // begin != end although they both point to same Node
      else result = p!=other.p;
      return result;
    }
  };
  Node* push_back(Integer value) {
    auto p = new Node{.value=value};
    if (m_size==0) {
      // empty
      m_last=p;
      p->next=p;
      p->prev=p;
    }
    else {
      p->next=m_last->next;
      p->prev=m_last;
      m_last->next=p;
      m_last=p;
      p->next->prev=p;
    }
    ++m_size;
    return p;
  }
  Integer size() const {return m_size;}
  ConstIterator begin() const {
    return ConstIterator{m_last->next,0,m_size,true};
  }
  ConstIterator end() const {
    return ConstIterator{m_last->next,m_size,m_size};
  }
  ConstIterator find_first(int val) {
    auto iter = begin();
    while (*iter!=val) ++iter;
    return iter;
  }
  Integer operator[](int pos) const {
    // return value at pos relative the member value 0
    if (true) {
      // Test
      auto p = m_last;
      int zero_count{0};
      for (int n=0;n<m_size;++n) {
        if (p->value==0) {
          // std::cout << "\n[" << pos << "] n:" << n <<  " NOLL NOLL NOLL NOLL NOLL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
          // std::cout << "\n\n" << std::flush;
          ++zero_count;
        }
        p=p->next;
      }
      if (zero_count!=1) {
          std::cout << "\n[" << pos << "] " << " FAILED TO FIND 0";
          std::cout << "\n\n" << std::flush;        
      }
    }
    auto p = m_last;
    while (p->value!=0) p=p->next;
    for (int n=0;n<pos;++n) p=p->next;
    return p->value;
  }

  void move(Node* p,Integer distance) {
    // std::cout << "\nmove(val:" << p->value << ",distance:" << distance;

    //    Realization: The move we simulate is an unlink, move,link-in at the new position.
    //    (or move left = swap pos with node to the left and move right = swap position with node to the right)
    //    So, moving left we link in between p->prev->prev and p->prev
    //    and moving right we link in between p->next and p->next->next.
    //    This means that when we do the move the wrap around happens each (size-1) times!
    //    Or put another way, with n items in the list, we get back to where we where after n-1 moves.
    //    So a move of 2 is the same as the move of 0. In fact a move of 0,2,4,6,8 are all the same as move 0
    
    //    This corresponds to modulo (size-1). That is x % 2 = 0,1,0,1,0,1,...
    //
    //    Ex: Say there are 3 items in the list, item a,b,c.
    //        If we move b left
    //
    //                       --- a --- b --- c ---
    //    move 0:            |                   |
    //                       ---------------------
    // 
    //                       --- b --- a --- c ---
    //    move 1:            |                   |
    //                       ---------------------
    // 
    //                       --- c --- a --- b ---      a->b->c AGAIN :) (circular list)
    //    move 2:            |                   |
    //                       ---------------------

    // But we simulate this by first removing the item to move (say b).
    // Lets see how teh insert_before pointer traverses for left move (pointed to element marked '*')
    //
    //                       --- a --- b --- c ---
    //    move 0:            |                   |
    //                       ---------------------
    //                                *
    //                            |---b-----|
    //                       --- a -------- c ---
    //    move 0:            |                   |
    //                       ---------------------
    // 
    //                            |---b-----|
    //                       --- a --------- c ---
    //    move 1:            |   *               |
    //                       ---------------------
    // 
    //                            |---b-----|
    //                       --- a --------- c ---  a->b->c again is inserted before '*'
    //    move 2:            |               *   |
    //                       ---------------------
    //
    //                       --- a --- b --- c ---
    //    stop 2:            |                   |
    //                       ---------------------

    auto full_distance = distance;
    distance = distance %  (this->size()-1);  // cut down to the sufficient move
                                              // C++ Important: ensure the divisor (in this case this->size()-1) is the SAME TYPE as the dividend.
                                              // This has bitten me several times!
                                              // C++ will treat dividend and divisor "as is" meaning a twos-complement negative number dividend
                                              // with a unsigned divisor gets thrown off by 2.
                                              // My size() function returns a signed integer so we are fine here.

    if (distance<0) assert(full_distance<0);
    if (distance>0) assert(full_distance>0);

    if (distance!=0) {

      auto insert_before = p;

      // 1. unlink p (IMPORTANT! Otherwise the wrap around will get target item WRONG)
      p->prev->next=p->next;
      p->next->prev=p->prev;

      if (distance>0) {
        for (int i=0;i<distance;++i) {
          // std::cout << " ++ ";
          insert_before = insert_before->next;
        }
        insert_before = insert_before->next;
      }
      if (distance<0) {
        for (int i=0;i<std::abs(distance);++i) {
          // std::cout << " -- ";
          insert_before = insert_before->prev;
        }
      }
      // std::cout << " ! ";
      // make p take the location of insert_before
      // 2. link in before pp
      p->next=insert_before;
      p->prev=insert_before->prev;
      insert_before->prev->next=p;
      insert_before->prev=p;
      if (p->value==0) std::cout << "\nMOVED 0 ----------------------------------------------";
      assert(p->next->prev == p);
      assert(p->prev->next==p);
    }
  }
  static void test() {
    CircularList cl{};
    cl.push_back(1);
    // std::cout << "\n" << cl;
    std::cout << "\n[0]" << cl[0];
    auto iter=cl.begin();
    auto end = cl.end();
    std::cout << "\niter!=end:" << (iter!=end);
    ++iter;
    std::cout << "\niter!=end:" << (iter!=end);
    ++iter;
    std::cout << "\niter!=end:" << (iter!=end);

    cl.push_back(2);
    std::cout << "\n" << cl;
    std::cout << "\n" << cl[1];
    cl.push_back(3);
    std::cout << "\n" << cl;
    std::cout << "\n" << cl[2];
    for (int i=-10;i<10;++i) cl.push_back(i);
    iter = cl.begin();
    std::cout << "\n" << *iter;
    std::cout << "\n" << cl;
    std::cout << "\n";
    for (int i=-30;i<30;++i) std::cout << "," << cl[i];
    {
      CircularList cl{};
      cl.push_back(0);
      cl.push_back(1);
      cl.push_back(2);
      cl.push_back(3);
      auto iter = const_cast<Node*>(cl.begin().ptr());
      cl.move(iter,0);
      std::cout << "\ntest cl:" << cl;
      cl.move(iter,1);
      cl.move(iter,-1);
      cl.move(iter,2);
      cl.move(iter,-2);
      cl.move(iter,-1);
      cl.move(iter,1);
      cl.move(iter,4); // same as move 0
      cl.move(iter,5);
      cl.move(iter,-5);
    }
    
  }
private:
  Node* m_last{nullptr};
  Integer m_size{0};
  friend bool test();
};

std::ostream& operator<<(std::ostream& os,CircularList const& cl) {
  auto offset = std::vector<std::pair<int,int>>{};
  for (auto iter = cl.begin();iter!=cl.end();++iter) {
    offset.push_back({*iter,iter.index});
  }
  std::ostringstream vals_os{};
  std::ostringstream ix_os{};
  for (auto [val,ix] : offset) {
    vals_os << std::setw(7) << val;
    ix_os << std::setw(7) << ix;
  }
  os <<   " ix:" << ix_os.str();
  os << "\nval:" << vals_os.str();
  return os;
}

std::ostream& operator<<(std::ostream& os,Values const& values) {
  os << "[";
  for (int index=0;index<values.size();++index) {
    if (index>0) os << ",";
    os << values[index];
  }
  os << "]";
  return os;
}

class Mixed {
public:
  Mixed(Values const& values,Result encryption_key=1,int mix_count=1)  {
    for (auto const& value : values) {
      m_nodes.push_back(m_mixed.push_back(value*encryption_key));
    }
    for (int n=0;n<mix_count;++n) mix();
  }
  auto size() const {return m_nodes.size();};
  auto operator[](int pos) const {std::cout << " " << pos << std::flush;return m_mixed[pos];}
  static void test() {
    Values values{1,2,3,0};
    Mixed mixed{values};
    std::cout << "\ntest:" << mixed;
  }
  friend bool test();
private:
  friend std::ostream& operator<<(std::ostream& os,Mixed const& mixed);
  std::vector<CircularList::Node*> m_nodes{};
  CircularList m_mixed{};
  void mix() {
    for (int index=0;index<m_nodes.size();++index) {
      auto delta = m_nodes[index]->value;
      m_mixed.move(m_nodes[index],delta);
      if (m_mixed[0]!=0) {
        std::cout << "\n[0]!=0 but " << m_mixed[0];
        exit(0);
      }
    }
    std::cout << " !! " << std::flush;
  }
};

std::ostream& operator<<(std::ostream& os,Mixed const& mixed) {
  os << "[";
  for (int index=0;index<mixed.size();++index) {
    if (index>0) os << ",";
    os << mixed[index];
  }
  os << "]";
  return os;
}

using Model = Values;

Model parse(auto& in) {
    Model result{};
    Result i;
    while (in >> i) {
      // std::cout << "\nparsed:" << i;
      result.push_back(i);
    }
    return result;
}

namespace korektur {
  // This code is from https://github.com/korektur/AdventOfCode22/blob/master/day20/day20.cpp
  // Many thanks to Ruslan Korektur for sharing this solution with me!

  using namespace std;

  int64_t solve(list<pair<int, int64_t>> numbers, int iterations = 1, long multiplier = 1) {
      while(iterations-- > 0) {
          for (size_t i = 0; i < numbers.size(); ++i) {
              auto it = numbers.begin();
              int64_t ind = 0;
              while (it->first != i) { it++; ind++; }
              pair<int, int64_t> p = *it;
              numbers.erase(it);
              int64_t second = p.second * multiplier;
              if (second < 0) second += (abs(second) / numbers.size() + 1) * numbers.size();
              int64_t newPosInd = (ind + second) % (int64_t) numbers.size();
              if (newPosInd == 0) newPosInd = (int) numbers.size();
              auto newPos = numbers.begin();
              advance(newPos, newPosInd);
              numbers.insert(newPos, p);
          }
      }

      int64_t ans = 0;
      auto it = find_if(numbers.begin(), numbers.end(), [=](auto &p) { return p.second == 0; });
      for(int i = 1; i <= 3000; ++i) {
          it++;
          if (it == numbers.end()) it = numbers.begin();
          if (i % 1000 == 0) ans += it->second * multiplier;
      }
      return ans;
  }

  // KoH Added to compare with my erroneous solution
  int64_t solve_in_concert(std::vector<CircularList::Node*> nodes, CircularList to_mix,list<pair<int, int64_t>> numbers, int iterations = 1, long multiplier = 1) {
      while(iterations-- > 0) {
          for (size_t i = 0; i < numbers.size(); ++i) {
              // Koh part
              // The two mixed lists should at each step of the way be "the same"
              {
                auto iter_0 = to_mix.find_first(0);
                auto it_0 = numbers.begin(); while (it_0->second!=0) ++it_0;
                std::cout << "\nto_mix:";
                static const int WINDOW_SIZE{30};
                std::deque<std::tuple<int,int,int>> window{};
                bool do_break{false};
                int break_count_down{WINDOW_SIZE/2};
                for (int n=0;n<numbers.size();++n) {
                  window.push_back({n,*iter_0,it_0->second});
                  if (window.size()>WINDOW_SIZE) window.pop_front();
                  if (*iter_0!=it_0->second) {
                    std::cout << "\nDiffers at n:" << n << " " << *iter_0 << " " << it_0->second;
                    do_break=true;
                  }
                  ++it_0; if (it_0==numbers.end()) it_0 = numbers.begin(); // wrap around
                  ++iter_0;
                  if (do_break) --break_count_down;
                  if (break_count_down<=0) break;
                }
                if (do_break) {
                  for (auto [i,x,y] : window) std::cout << "\n n:" << i << " " << x << " " << y;
                  exit(0);
                }
              }
              auto delta = nodes[i]->value*811589153;
              to_mix.move(nodes[i],delta);
              std::cout << "\ni:" << i << " " << iterations;

              // Korektur part
              auto it = numbers.begin();
              int64_t ind = 0;
              while (it->first != i) { it++; ind++; }
              pair<int, int64_t> p = *it;
              numbers.erase(it);
              int64_t second = p.second * multiplier;
              if (second < 0) second += (abs(second) / numbers.size() + 1) * numbers.size();
              int64_t newPosInd = (ind + second) % (int64_t) numbers.size();
              if (newPosInd == 0) newPosInd = (int) numbers.size();
              auto newPos = numbers.begin();
              advance(newPos, newPosInd);
              numbers.insert(newPos, p);
          }
      }

      int64_t ans = 0;
      auto it = find_if(numbers.begin(), numbers.end(), [=](auto &p) { return p.second == 0; });
      for(int i = 1; i <= 3000; ++i) {
          it++;
          if (it == numbers.end()) it = numbers.begin();
          if (i % 1000 == 0) {
            ans += it->second * multiplier;
            std::cout << "\nkorektur " << i << ":" << it->second << " " << it->second * multiplier;
          }
      }
      // auto r1000 = to_mix[1000]*811589153; std::cout << "\nr1000:" << to_mix[1000] << " " << r1000;
      // auto r2000 = to_mix[2000]*811589153; std::cout << "\nr2000:" << to_mix[2000] << " " << r2000;
      // auto r3000 = to_mix[3000]*811589153; std::cout << "\nr3000:" << to_mix[3000] << " " << r3000;
      // auto result =  r1000 + r2000 + r3000;
      Integer r1000 = to_mix[1000]*811589153; std::cout << "\nr1000:" << to_mix[1000] << " " << r1000;
      Integer r2000 = to_mix[2000]*811589153; std::cout << "\nr2000:" << to_mix[2000] << " " << r2000;
      Integer r3000 = to_mix[3000]*811589153; std::cout << "\nr3000:" << to_mix[3000] << " " << r3000;
      Integer result =  r1000 + r2000 + r3000;

      std::cout << "\n   ans:" << ans;
      std::cout << "\nresult:" << result;
/*

korektur 1000:9560 7758792302680
korektur 2000:3849 3123806649897
korektur 3000:-1257 -1020167565321

r1000:9560 2081366104
r2000:3849 1365425705
r3000:-1257 2034651127
   ans:9862431387256
result:1186475640

*/       
      return ans;
  }

}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      std::cout << "\nmodel:" << data_model;
      if (false) {
        std::list<std::pair<int, int64_t>> nums{};
        std::vector<CircularList::Node*> nodes{};
        CircularList to_mix{};

        for (int num : data_model) {
          nums.push_back({nums.size(),num});          
          nodes.push_back(to_mix.push_back(num));
        }
        result = korektur::solve_in_concert(nodes,to_mix,nums);
      }
      else {
        Mixed mixed{data_model};
        // std::cout << "\nmixed:" << mixed;
        std::cout << " !!! " << std::flush;
        if (false) {
          // test
          CircularList::test();
          // Mixed::test();
        }
        else {
          // for (int i=0;i<20;++i) std::cout << " " << i << " " << mixed.from_0(i);
          auto r1000 = mixed[1000]; std::cout << "\nr1000:" << r1000;
          auto r2000 = mixed[2000]; std::cout << "\nr2000:" << r2000;
          auto r3000 = mixed[3000]; std::cout << "\nr3000:" << r3000;
          result =  r1000 + r2000 + r3000;
        }
      }
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      // The encryption key 811 589 153 is 30 bits.
      // The values to mix are originally +/- 10000 is 14 bits
      // We need 44 bits and a sign bit = 45 bits.
      // An int64_t will suffice
      if (false) {
        std::list<std::pair<int, int64_t>> nums{};
        std::vector<CircularList::Node*> nodes{};
        CircularList to_mix{};

        for (int num : data_model) {
          nums.push_back({nums.size(),num});          
          nodes.push_back(to_mix.push_back(num));
        }
        result = korektur::solve_in_concert(nodes,to_mix,nums,10,811589153);
      }
      else {
        Mixed mixed{data_model,811589153,10};
        auto r1000 = mixed[1000]; std::cout << "\nr1000:" << r1000;
        auto r2000 = mixed[2000]; std::cout << "\nr2000:" << r2000;
        auto r3000 = mixed[3000]; std::cout << "\nr3000:" << r3000;
        result =  r1000 + r2000 + r3000;
      }
      return result;
  }
}
bool test() {
  std::cerr << "\ntest()";
  bool result{true};
  Values const const_values{-3,-2,-1,1,2,3,-17,-52,0,52,17,-6,-5,-4,4,5,6};
  if (result) {
    auto values{const_values};
    CircularList to_mix{};
    for (int j=0;j<values.size();++j) to_mix.push_back(values[j]);
    if (result) {
      result = false;
      int ok_count{0};
      for (int i=0;i<values.size();++i) {
        std::cout << "\ni:" << i;
        auto val = values[i];
        auto iter_0 = to_mix.find_first(0);
        auto iter_val = to_mix.find_first(val);
        auto iter=iter_0;
        // Test that we can iterate from iter_0 to iter_val
        for (int j=0;j<to_mix.size();++j) {
          std::cout << "\niter [" << iter.index << "]=" << *iter << " iter_val [" << iter_val.index << "]=" << *iter_val;          
          if (iter!=iter_val) std::cout << " !=";
          else {
            std::cout << " == ";
            ++ok_count;
          }
          ++iter;
        }
      }     
      std::cout << "\nok_count:" << ok_count << " required:" << values.size();
      result = ok_count==values.size();
      assert(result);
    }
    if (result) {
      result = false;
      auto begin = to_mix.begin();
      auto iter_val = to_mix.end();
      auto iter=begin;
      // Test that we can iterate from begin to end
      for (int j=0;j<to_mix.size()+1 and !result;++j) {
        std::cout << "\niter [" << iter.index << "]=" << *iter << " end [" << iter_val.index << "]=" << *iter_val;          
        if (iter!=iter_val) std::cout << " !=";
        else {
          std::cout << " == ";
          result=true;
          break;
        }
        ++iter;
      }
    }
    if (result) {
      result = false;
      // Test that we can iterate from begin to end
      int count = values.size();
      for (auto iter=to_mix.begin();iter!=to_mix.end();++iter) {
        std::cout << "\niter [" << iter.index << "]=" << *iter;
        --count;
      }
      result=count==0;
      assert(result);
    }
    if (result) {
      auto visited = std::set<int>{};
      for (int j=0;j<values.size();++j) {
        for (auto iter = to_mix.begin();iter!=to_mix.end();++iter) {
          visited.insert(*iter);
        }
        result = visited.size() == values.size();
        if (!result) {
          std::cerr << "\nvisited.size():" << visited.size() << " != values.size():" << values.size(); 
        }
      }
    }
    if (result) {
      auto offset = std::map<int,int>{};
      auto count = values.size();
      for (auto iter = to_mix.begin();iter!=to_mix.end() and result;++iter) {
        std::cout << " ! ";
        offset[*iter] = iter.index;
        --count;
        result = result and (count>=0);
      }
      std::ostringstream vals_os{};
      std::ostringstream ix_os{};
      std::cout << "\noffset.size():" << offset.size();
      for (auto val : values) {
        vals_os << std::setw(3) << val;
        ix_os << std::setw(3) << offset[val];
      }
      std::cout << "\n" << vals_os.str();
      std::cout << "\n" << ix_os.str();
    }
  }
  if (result) {
    int ok_count{};
    for (int i=0;i<const_values.size();++i) {
      auto values{const_values};
      auto val = values[i];
      CircularList to_mix{};
      for (int j=0;j<values.size();++j) to_mix.push_back(values[j]);
      std::cout << "\n\nbefore move " << val;
      std::cout << "\n" << to_mix;
      auto iter_0 = to_mix.find_first(0);
      auto iter_val = to_mix.find_first(val); // Requires unique values in input!
      auto distance_before = std::distance(iter_0,iter_val);
      std::cout << "\ndistance_before:" << distance_before;
      to_mix.move(const_cast<CircularList::Node*>(to_mix.find_first(values[i]).p),values[i]);
      std::cout << "\nafter move " << values[i];
      std::cout << "\n" << to_mix;
      iter_0 = to_mix.find_first(0);
      iter_val = to_mix.find_first(val);
      auto distance_after = std::distance(iter_0,iter_val);
      std::cout << "\ndistance_after:" << distance_after;
      auto diff = (distance_after - distance_before) % to_mix.size();
      if (val>0 and diff<0) diff += to_mix.size()-1; // there are size-1 spaces between size items
      if (val<0 and diff>0) diff -= to_mix.size()-1; // there are size-1 spaces between size items
      std::cout << "\ndiff:" << diff << " val:" << val;
      if (diff==val % to_mix.size()) {
        ++ok_count;
        std::cerr << "\nok_count:" << ok_count;
      }
      else {
        std::cerr << "\n\nFAILED\n";
      }
    }
    std::cout << "\nok_count:" << ok_count;
    result = (ok_count==const_values.size());
    assert(result);
  }
  if (result) {
    std::istringstream in{pData};
    auto values = parse(in);
    std::vector<CircularList::Node*> to_move;
    CircularList to_mix{};
    for (int i=0;i<values.size();++i) {
      auto p = to_mix.push_back(values[i]);
      to_move.push_back(p);
    }
    assert(to_move.size() == to_mix.size());
    int ok_count{};
    for (int i=0;i<to_move.size();++i) {
      std::vector<CircularList::Node const*> node_order_before{};
      {
        auto iter = to_mix.find_first(0);
        for (int n=0;n<to_mix.size();++n) {
          node_order_before.push_back(iter.p);
          ++iter;
        }
      }
      auto p = to_move[i];
      auto dist = p->value;
      if (dist<0) {
        auto before_p = p;
        for (int n=0;n<std::abs(dist);++n) before_p = before_p->prev;
        to_mix.move(p,dist);
        if (before_p->prev == p) {
          ++ok_count;
          std::cout << "\nok_count:" << ok_count;
        }
        else {
          std::cerr << "\n\nFAILED\n";
        }
      }
      else if (dist>0) {
        auto after_p = p;
        for (int n=0;n<std::abs(dist);++n) after_p = after_p->next;
        to_mix.move(p,dist);
        if (after_p->next == p) {
          ++ok_count;
          std::cout << "\nok_count:" << ok_count;
        }
        else {
          std::cerr << "\n\nFAILED\n";
        }
      }
      assert(std::distance(to_mix.begin(),to_mix.end()) == to_move.size());
      assert(to_move.size() == to_mix.size());      
      int missmatch_count{};
      {
        auto iter = to_mix.find_first(0);
        for (int n=0;n<node_order_before.size();++n) {
          auto p = node_order_before[n];
          if (p!=iter.p) {
            if (p==iter.p->prev) {
              ++missmatch_count;
              --iter;
            }
            else if (p==iter.p->next) {
              ++missmatch_count;
              ++iter;
            }
          }
          ++iter;
        }
      }
      std::cout << "\ndist:" << dist <<  " missmatch_count:" << missmatch_count << std::flush;
      if (dist % static_cast<int>(to_move.size()) != 0) {
        // one missmatch for node moved one step, otherwise two missmatches (one for location of moved-from and one for location moved-to)
        assert(missmatch_count==1 or missmatch_count==2);
      }
      else {
        std::cout << "\n" << std::flush;
        assert(missmatch_count==0);
      }
    }
    auto iter_0 = to_mix.find_first(0);
    {
      auto iter=iter_0;
      for (int i=0;i<=3000;++i) {
        if (i==1000) std::cout << "\n[1000]=" << *iter;
        if (i==2000) std::cout << "\n[2000]=" << *iter;
        if (i==3000) std::cout << "\n[3000]=" << *iter;
        ++iter;
      }
    }
  }
  assert(result);
  return result;
}

int main(int argc, char *argv[])
{
  if (argc>1 and std::string{argv[1]}=="test") {
    if (test()) std::cout << "\nTest: You have arrived at your destination :)";
    else std::cout << "\nTest: Not there yet...";
  }
  else {
    Answers answers{};
    // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
    // answers.push_back({"Part 1     ",part1::solve_for(pData)});
    // answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
    answers.push_back({"Part 2     ",part2::solve_for(pData)}); // 1186475640 is too low
    for (auto const& answer : answers) {
      std::cout << "\nanswer[" << answer.first << "] " << answer.second;
    }
    // std::cout << "\nPress <enter>...";
    // std::cin.get();
    std::cout << "\n";
  }
  return 0;
}

char const* pTest = R"(1
2
-3
3
-2
0
4)";

// List where each member will shift to itself = should NOT shift/mix
// char const* pTest = R"(-3
// 6
// 0)";

char const* pData = R"(6948
-3729
3230
7551
460
216
-4424
-4355
4026
-7119
9802
-1890
8881
-4740
-2071
6436
6911
5966
4098
2665
-2157
-8781
1088
9398
-2426
-230
-3432
1678
3807
-6707
8732
-5532
-3945
5187
-484
-8420
-3363
-1392
3787
-5412
-8417
1456
-5868
-4727
-7226
6045
-2061
3203
-6503
5313
1350
6364
-9669
-8454
5259
-7800
-7589
-3317
7758
-2801
-8787
-810
4514
-6936
3040
-5932
6236
-2616
4716
-6688
-4040
-7811
1886
8106
-2570
-2434
-7989
6103
-4395
-4985
-42
7292
3403
5296
-3419
-368
4326
-1506
7400
3161
-1636
7671
6015
-8450
-3176
-8625
2262
-5913
-9976
-5642
-6651
-2831
-2615
-5785
-6616
-1084
888
8626
-976
-9863
-7470
7223
7805
-4100
-7944
-4744
8515
7667
4139
-7111
6545
3873
-325
9876
3229
-294
-7437
-5517
-2391
7136
252
2026
-9644
-2175
-1202
-459
-6836
-1388
-3681
8487
-1288
8976
7452
-6926
-3162
1722
5740
-4649
4930
327
9420
1075
4023
3640
-403
2130
9080
1306
-4757
-77
4488
-2146
-9109
911
-3076
4085
3264
834
-1603
-4263
-256
-2527
-754
5094
-9296
-4879
-3045
221
3963
-1967
-583
-3609
-131
-4014
3723
2549
-947
-1257
-562
-3260
-3005
-6803
3064
-6786
-8034
-8170
1184
4386
2115
4227
-9448
-1768
7034
7915
4449
-1719
4294
-8710
-8781
-4465
-2876
2034
-1468
8021
8400
-4915
4098
-9485
7326
689
2983
-7890
8231
-1219
-6914
-8878
5074
3310
4976
2832
6604
-4873
-5382
9638
-2890
-5616
5624
-2817
-6616
8464
-5407
-8227
-7452
3209
-5250
-5278
2115
-642
910
8279
-8868
-3690
9485
3216
4273
7879
-9361
9437
2323
8601
3061
-2149
6351
-1368
4988
3586
-4654
4533
-7455
-2675
7059
-9762
4085
-9505
2878
7199
-2507
3336
3787
-5642
-9054
-4100
-2857
2275
1658
5434
-92
77
3148
-134
6656
-2357
952
5386
6634
-8430
1083
6133
4913
-1292
-1665
123
4842
-9174
-5636
-3848
-1148
-8360
7550
-8364
5300
-1055
-8550
6113
-3022
5120
-2913
-628
-3164
-3274
-5595
6086
9861
8382
1482
-8656
-2681
-9833
5571
1448
571
2672
-3671
-4621
-6001
7583
-8301
1995
7269
5920
2564
9843
-7589
-3439
8862
-4834
-6576
3120
4234
512
859
6190
-9755
3807
-1543
9771
-6397
4900
-7681
6712
-5707
4399
-3113
-5528
4459
881
-5714
-1136
4604
-486
-2658
-2566
8614
9337
-5210
5008
-3300
4963
8810
-8167
-9470
6205
-8446
3051
-3987
7344
-2497
397
-8730
-2299
3239
-522
-3161
9337
-8898
-5274
362
6935
7893
4633
-1692
-6226
986
-8215
-4212
4894
223
-3300
3364
9016
-6525
-3399
-2661
-2812
9095
-306
5042
-9350
6106
7722
-2390
8637
4183
-5309
8334
-4063
4890
9368
-8381
-6560
2812
9456
-7581
7150
2113
-5894
5346
-2620
3902
8621
-6332
4359
9802
-6579
-5310
-4804
5545
-817
5159
7568
4161
-306
-597
-588
-5957
-1185
-2
2734
-8137
1439
9259
1127
-6672
1365
-5552
-155
9044
-8712
-326
-8309
3072
-2277
8163
-3647
6863
-1376
3652
-60
1947
6658
8364
-5951
-4375
3527
-6709
-8598
-968
3383
-3796
-9065
-8383
-1096
7149
-6750
-793
5894
-3720
-1558
-9361
-7552
9275
1641
-7987
5054
-2452
3576
-3758
-326
-3164
-8433
8389
6567
-1652
-1698
4374
-9006
4899
8081
4147
2445
-7113
-8806
-4312
7326
7132
4129
5414
-3810
-3179
-9006
-6090
3104
1725
-9586
-3359
1140
4292
5618
4621
1357
-4743
-6446
-9383
4208
1877
3856
-9414
-814
9700
3305
7355
-3291
-6296
2226
7779
4723
7330
9621
-6478
6441
3450
-7658
7011
-7756
-2778
-4114
4030
4384
-5777
1216
-8732
-6187
-2546
9520
-3741
-9861
2360
3275
-8420
-6503
8010
-2769
679
-1820
8222
-810
8950
6683
6523
-7387
5727
-7711
-305
-2419
7101
-967
6545
1350
4049
-9586
-2349
5570
-4454
2384
2571
-8612
-4212
5862
7978
373
2029
1424
-6610
6948
9316
-4125
-194
-397
8999
9202
1231
1467
-7292
1886
-6777
-9492
-1135
8260
3326
9550
-4891
4408
-5932
-7283
4078
5027
3666
-9413
2721
-2436
-3875
6504
4209
4833
1125
-4358
7924
-9869
3617
-789
8192
6223
7459
579
6940
3941
2176
-3724
5770
-8347
-9578
1054
302
-5430
9567
-1217
5185
2698
-6528
-9059
-3201
6861
7073
-9867
2905
541
-4664
207
6436
-917
-1269
-9368
-9222
2524
-1292
-7621
5894
-3317
9635
4390
8399
-7243
-2402
-9915
1836
-1966
-6762
-6570
1304
-6631
9504
9715
4388
-4935
3310
-7511
3921
-5172
-4850
3660
1318
6975
5445
-7009
-4516
9387
2027
-3060
7836
-8836
2865
-2214
-2179
-306
4637
-8284
6338
4343
3604
-8015
5657
-3859
-2880
-4751
-165
1943
4542
2432
-1544
2124
-5299
9372
2680
1302
-6670
-6269
985
691
-7030
-2202
8524
-4138
-4736
5051
-2709
5377
-5222
8585
-1813
-692
-4289
9582
-8991
8975
4587
428
3246
4064
794
8506
2997
5510
-166
8271
-3035
5691
5871
-6541
3961
4314
4588
9994
-3131
-1489
-4505
5088
-6363
-5716
4311
5588
-5918
-6555
3637
-3758
-304
1214
-5703
-8721
-8679
9457
8185
-3738
4177
111
-2520
9190
-4623
-9694
3126
714
-865
-8011
2785
-8659
-5717
4309
5181
3698
5620
7005
2711
-4472
3351
8996
2360
1348
7381
-9438
-2164
-3183
-2951
5032
-8653
9635
4185
-5073
-5251
-1861
458
4354
5458
-8222
3544
8877
-9856
-1621
-5835
7042
-1482
-6799
432
5306
8462
5570
-4223
-1034
-8826
-7896
7196
-2228
-78
-1418
-9486
7344
-9029
-6925
1317
2472
8990
7847
-7681
8282
32
7778
-3125
5570
-9198
1994
-5114
3391
7138
-5951
-3007
164
-5935
-9349
-3675
-5664
8448
-8298
-7760
-9330
-3059
-7266
5912
-3259
-6775
6218
-3632
-1765
-1543
-4907
-88
-6896
9003
-8861
-7077
7056
-9715
7816
-9719
8993
1072
4709
-2883
-3100
8177
7677
7050
-8922
164
-8597
-5186
786
3660
9259
-7601
1317
90
-9577
-8628
9026
8419
6803
8011
3916
-2068
-733
675
2318
720
-6709
9981
-5687
-7103
3422
4458
9424
-6541
-9590
-1675
9627
-3825
-5644
-137
-9538
8260
3565
-1916
-4059
-3611
4064
9568
4031
-2718
4184
6489
3877
-8248
-6522
1025
-7808
-702
-6222
-5215
-2362
1515
-6195
6270
4735
1094
-1092
5170
-1377
8522
4399
-4293
-4706
-2413
9116
-8340
-7605
-8195
6737
-3161
8535
-3617
2685
-574
-2468
-4312
-5923
-1895
3088
-9346
6434
714
9589
6517
2849
-3645
-9971
-4212
-6641
4227
9700
-4117
-4007
-4855
7698
8442
-725
-7108
14
-1509
-5268
4613
-380
-2592
4296
-3162
-2366
-9529
-2155
-9286
8192
-1304
7464
8765
466
-1006
2528
8297
-7479
-7388
-8619
158
5406
1084
-742
5967
-2707
-9179
5802
9071
-8270
5942
-264
1424
-8043
-5078
131
2051
48
2072
827
-9488
1841
2345
-6828
4667
-9075
3814
7239
-351
-2557
3964
9976
-2837
-3007
1200
-8777
6093
-9179
8447
3466
9487
-6319
289
-3597
-8787
-6308
4621
-1796
-5073
-8119
7261
4544
-6427
-3356
8929
-5116
3348
1203
6743
-5240
9139
-6544
8185
5608
-9766
7050
-134
-9731
7458
2313
-2061
-1487
-7647
-1713
9284
813
-4932
5088
8181
7727
654
501
9534
1866
0
6322
-9486
7112
9558
8626
-3609
-9639
9940
3006
-9686
-4625
-403
4289
-1291
6998
-9837
5732
9210
-3974
1266
946
-2122
1224
-7498
9190
8883
-3055
782
-7721
6905
9314
7345
-5831
-4022
-6390
7112
9609
3906
2574
-7069
8795
-5307
4673
-6043
1937
-1217
-1404
3628
-2488
-1968
4944
-8721
-3886
7028
9019
5732
3209
1055
1750
-8794
3873
-3316
8810
466
2171
6514
-6726
-2923
4963
-8233
9765
-6933
3408
373
-6936
7201
-7164
-6923
-3576
6063
-8038
-2876
9424
3207
-8230
-5133
3097
-9486
-5309
-5034
-2931
-2600
3932
5931
-8825
3964
348
-5669
8665
3006
-5742
7924
5803
-8878
-1543
4909
-3784
-4632
-509
4944
4294
-4672
9635
7814
6595
7766
2052
-3343
219
6791
8693
-720
8929
7269
9537
-1179
2196
-5528
-2269
-648
-5117
8699
3961
5588
4451
-3088
-2529
-762
4031
-3147
-9875
7673
-1916
-4189
-8061
6451
-9618
-4276
-2555
4838
-9209
-5876
-3335
249
829
-5269
2979
8990
646
6277
-9179
4150
9609
-5072
2771
-5117
1376
484
7411
-7374
-3753
-8688
-6489
1517
-7318
-9827
-2820
-8258
-3225
-9481
2645
-9435
7914
-6273
-1489
-6332
3631
-1603
4542
-1765
-5509
9044
-1584
9520
-5795
-6640
6500
-8518
-9410
402
2178
9475
6039
-8603
8118
1404
8226
-1590
-2891
-9705
-2063
60
7179
6925
8262
-8767
6223
9126
2382
4502
9184
-9243
-9361
400
-4235
-2979
1877
-3002
5506
9395
6173
8616
2573
9513
4540
4447
-7340
-5268
8462
-2997
7499
2845
2549
-4130
-8230
4722
6100
2513
5003
8879
-6670
-5208
8968
-2697
7556
-1689
8943
1268
-6793
-5718
-5894
9171
-88
8516
-8570
9019
3097
5201
9230
-4757
-600
663
-4806
1979
-2529
9333
-6086
7097
4064
5474
1952
-2684
-5835
6351
-859
-9467
4837
8304
-6718
-6097
-2126
3126
-3531
-8546
4563
6934
-194
4187
-1758
-4380
7207
3591
1253
101
-1872
-6318
1294
-6682
-7771
7835
1090
6906
1355
-8438
-6162
78
2567
-6246
-7530
648
6317
6985
-9966
-4352
-4901
-2413
-7549
4529
5474
-4935
-5923
221
-5034
4693
-3141
-5078
-2524
4532
127
-3133
5110
-8170
-7274
-3665
-7714
4049
6670
-3275
-7782
4542
-8408
-8462
9269
-3125
-2570
9985
1688
1365
4575
1655
-670
-4640
-6668
5566
-532
-9827
-9202
828
-3271
424
-9915
588
8231
-9884
-4045
8909
-8220
-1858
1869
588
3045
-7879
-8892
-4200
5516
4328
-1886
-552
-8926
3849
-1406
9456
-880
6828
5326
5097
-3493
-1603
-892
3008
7649
5136
-3059
-424
-9833
-6806
2324
-455
4437
4760
-8347
-7496
-4469
-5084
-8002
-3714
3001
-9528
4963
-3095
-9703
4192
5737
-5360
3992
-6126
5776
2805
-616
3527
2199
1212
3896
-1963
8491
-8383
316
2284
5088
516
4641
7298
-4353
-5427
5005
5201
-1019
9538
9475
9752
-8899
-2892
1567
3961
3853
-8413
4147
-8145
-9745
-2679
-4611
-6627
-4358
-4086
4451
-7888
4889
5621
-4910
6284
-2865
-9249
571
-2133
523
9194
-4957
-8928
9891
1064
2124
-8011
-5250
-2129
-7749
6845
593
-672
-5608
-7402
6174
4519
8181
9178
7345
-1829
1989
-4062
-719
-6001
-2688
3170
3291
4448
-1025
7069
6568
-306
6062
-3116
-4118
-4426
775
1687
-5014
1247
-9452
-1388
6488
8015
-3993
-5158
558
4730
7495
-1896
-3640
-6273
-974
4767
8909
4064
9186
5997
-1304
-7708
179
9816
9099
3897
-1097
-1927
-568
-3965
3443
-2107
4813
187
2176
-2615
8637
3296
7829
-4130
2578
-7318
5199
8984
-2847
-4732
87
180
6504
-9596
-4212
1865
6148
-9254
-976
826
-5984
-2023
5501
-1842
-8543
4961
-967
230
3214
5510
-3053
-9179
-8229
8188
5727
9085
-4504
-3035
-924
1394
-6109
9586
2262
5474
-1326
-2683
4866
4543
-3816
1518
-6390
-229
8952
-1135
5871
6979
-2753
-3998
-6600
-1011
-7391
-403
-9603
5546
1624
-9364
-3693
9137
5142
-7352
-3490
7160
-1096
6632
3757
4150
2882
-3281
-1760
-9
-6680
5571
-5656
-5892
-3488
-7652
4296
2460
4894
7455
-3623
-973
5848
-1871
8076
4617
-8848
-2349
2451
4352
1312
-3945
-7723
-4673
-3676
-6432
-985
1425
9437
8872
-8944
-6354
-239
6782
3600
8065
-6856
-8309
-737
6278
4336
9155
4581
-8654
-3945
289
2255
-5667
4507
3638
-7123
-874
-4744
-3594
-8667
2101
-1445
1611
-140
9816
516
-5348
-3655
-6291
-5728
-7268
-6135
-9762
3320
7255
-4813
-5200
5345
3853
3897
-4503
2058
-3944
3364
2648
-5406
-8132
1830
-132
5913
3228
8112
3859
-906
-2810
8109
-5072
484
2645
-8848
8436
-3017
9835
-3419
316
-3993
-4352
-1204
-8781
9538
1456
9199
4640
6224
-8610
4542
401
6223
9188
-2755
-5662
-8818
4574
9284
2085
-3018
7404
8524
2034
-9592
3680
2685
7536
9084
8654
5727
-9249
3241
2345
-4511
9997
7367
-6928
-7549
9372
9558
-2308
-8313
-5125
-338
-5348
3035
-15
-4581
-174
-8039
-3853
-4686
7248
8238
-2228
-1927
-18
-6495
-3812
5204
-8450
-7814
-6616
-5663
-9627
-5250
-3905
-3867
3994
-9121
6694
-1746
-5086
5967
-6751
1184
875
3244
9641
2766
-6279
7331
8762
7464
-5182
-3926
7178
6842
-4214
5460
6140
-1104
2886
9876
-4967
4302
-9041
-3166
-672
-1681
8523
35
-4848
-5195
5427
-2989
9405
-9610
8671
-7902
8651
-9996
1219
3384
9026
-9957
-7394
8009
-2382
-7124
-9217
4544
-3317
-2468
-2068
7740
-3399
3376
-3632
-7621
-6499
4776
-78
6260
-3876
8640
-5784
-4948
5736
-3686
3276
439
-6436
-239
977
-2134
4572
4208
-3399
-9161
-4615
-8868
3367
-8589
-6493
-9350
8932
2091
-6119
-3267
123
-5951
6167
-6522
4931
3134
-6349
9460
5566
-5438
1995
5871
-7791
2148
3726
-5219
2944
6726
-2299
5691
-6620
1405
9976
9132
-720
4209
-2867
6398
-8892
-68
3291
771
-1558
-3096
3831
-9722
-9365
-8607
-1094
-5720
-1709
9589
7770
7386
-5387
5186
513
-305
5561
-8097
-8589
-8140
4347
561
6291
3634
-9944
-7455
-6151
7228
1871
-8548
-9833
3097
2720
2574
767
3128
-2997
6073
540
-8717
7230
9270
6504
-5039
-1445
6290
-1161
-3267
-3794
9523
-635
1017
-7470
1624
4543
7758
1424
-3471
-172
-3086
7013
316
-2458
9267
-2266
-6641
1289
4354
4271
4722
-8660
-6221
4543
3463
-4125
-4586
475
5601
-8861
8615
28
-5086
-2105
7442
1245
-302
9187
8040
-3742
4471
7705
8660
-5112
-3425
-4931
-567
-3356
-134
-6836
1541
6762
3636
-746
-5067
-3613
3860
2877
6883
9475
-4161
6974
7068
2164
-6346
-9737
9908
7182
-8813
3476
484
3199
-2070
-2341
5743
5741
591
-3161
6036
8150
-8897
-1893
1209
-7582
-7437
7234
-6708
-2829
-2524
6964
7034
1513
6174
6934
280
9641
847
1159
952
1343
-8506
5915
-6436
432
6223
-1783
73
2528
-1577
-3572
-4815
-3531
6057
2199
6613
-5430
4415
4442
-9084
3873
7248
-6952
6677
8660
-9066
-3847
-4554
6222
-6395
-8095
-2450
-6443
-1303
1706
9779
-8038
-9366
4296
-5306
-5311
-4240
7298
-3953
-8356
-807
-2681
179
-4255
691
-7074
746
-2378
1064
-7575
7048
1294
530
-5539
9976
-1654
-3317
7271
-3234
3456
9513
4292
-4067
1497
-4529
-29
-6201
3571
-617
6430
-8576
9560
-3740
9199
7024
2091
-7461
-5109
-186
561
1622
5313
8671
2491
-7897
-307
3880
7732
-4879
7525
3223
9401
-7271
-2978
1184
226
-2584
-3546
1152
-2931
9762
-2330
-9794
5189
3745
331
6584
2574
-5654
6011
737
6159
-4744
-4419
8067
-1217
2049
-6350
9018
-2117
2132
8597
-9530
-7016
-3679
-9054
3181
2849
-2086
-3348
8710
1789
6364
3772
4935
-9973
8827
3680
-6365
-5652
-7387
-2676
-6706
-2755
5657
-9583
1432
-3077
-2330
7029
-304
602
2262
-6496
-7395
-4942
4024
-8975
-9008
-4506
-4966
-5699
2850
5063
-3003
652
9199
-8849
-4130
-512
-6350
5088
1916
-5914
5926
-7605
8516
-4227
-6641
-8193
-9620
520
4203
5474
-9010
-1603
-5129
-8932
6500
9889
-8667
9455
-5186
-9657
3134
5696
4035
6754
-8794
6135
7151
7609
-4933
3300
4617
-9708
4049
7688
-5385
-5745
7898
2378
-2837
7958
7575
1976
-7987
-4086
5402
2360
-1349
-9013
-3510
4900
8952
5720
5259
-4094
1579
-7419
-3854
-7321
9760
-4666
-6496
5766
3291
-4521
-2742
7986
-8860
6904
-3764
5608
7152
3269
-5951
8680
-8445
-9297
-7855
-7979
1926
-4375
-6930
-4530
-932
9657
-5440
-3343
438
-6522
7819
-8781
-4732
1184
-8015
-60
-404
-7454
-4174
-874
-2473
4297
-6099
-8267
-4154
1291
1337
9521
-360
8761
-3076
-1313
-7873
4858
-6503
-9938
9171
6806
-5437
-8349
-8773
-2910
-2055
9314
-2447
6948
-5709
-3733
-455
-6641
5948
-7454
7066
-5430
-3846
-3970
8497
9538
-6385
8989
8697
9554
3322
-7864
-2247
-6133
-976
2963
698
-5532
3254
1665
-3503
4442
-600
-688
-638
-1636
9267
-6915
-6318
-9737
-9513
-9936
-5136
-4873
-9185
-6924
6717
-6889
2647
1084
-5664
-5739
4960
3134
-6775
-2824
-4638
8305
-1265
-9971
8231
-8536
9504
-3028
5956
1952
-1958
3984
8940
8990
-8014
9926
-4620
-5456
-3836
-8705
-983
3707
-6394
-1662
-8145
-7360
4686
-2411
-2886
-4410
-9703
6730
-9620
3926
7521
-4869
5027
7658
9496
-2881
1321
-6336
-5026
8439
-5314
-6755
-7886
-4870
559
-4378
-3794
2146
6140
-7238
-304
-75
-1730
-8193
7004
1652
-4358
6418
2798
-1813
8853
3742
1482
-7531
-7669
-7519
3388
6846
-1472
-9808
-6186
9155
7090
9579
5941
4543
-2989
7654
-8039
-306
7921
5034
-9858
-1019
-9672
9329
-4707
-3981
-7497
1090
1759
9783
2651
-657
3964
-1534
9721
-2614
4173
-6195
4760
-1730
3156
-5517
-591
8665
1254
-34
-3754
-3650
3427
-9030
2341
-2886
9879
5418
-5117
-2585
-4872
-1514
8516
3475
-892
5998
9538
-3914
7048
-1684
-6143
-4606
-3896
4709
-6436
3799
-8333
-2814
-6402
948
5602
-1558
6703
7856
4964
-5314
-9638
5088
3013
-9818
-2155
8327
1943
9456
-2673
3199
5243
-976
4968
4844
-9603
-4086
7571
8850
5434
-6864
6014
461
3388
7815
-2890
8653
-6896
6056
8212
-720
-403
-3202
3920
808
6459
5359
-3007
458
9975
-9715
4948
-4358
5317
-6205
1103
-8227
-9874
-6636
-7901
-8096
2792
-9084
-3471
-3606
-6673
-9132
2276
9889
8200
-2616
3591
6487
-7753
4023
-2639
2886
2983
-1896
8402
-5073
-6235
-5312
2025
-7803
-692
8448
-1566
952
-5696
-4411
-2538
-1558
5528
-5639
-2117
5941
3402
4881
-3271
4270
-2193
-6418
2199
7254
4418
-7839
2134
-1857
-1365
-7136
4749
-2711
9771
-6132
-2867
-5523
3884
-1370
-583
8415
760
5956
6746
4737
-981
694
4709
1008
1248
7838
-6116
-1531
-2077
1021
2493
-9619
782
6649
7138
8279
-2468
9056
7409
-7647
-1780
8205
-2398
3676
-9796
-8037
-103
9284
4647
3161
-8833
-9719
-7455
3411
3919
-5616
-3668
-5525
-1197
-731
5532
2459
3469
2874
8523
-902
-8141
-1964
-2997
-3597
2432
9217
4408
2635
818
1310
3207
4743
-9537
-6621
3941
5668
2379
4056
1777
-4086
-5831
-8598
9538
-9343
-4263
-2318
3268
-396
-8185
9324
-2539
-9249
4824
-6719
8723
2122
3285
-4938
-8974
90
-3221
-7168
7423
-1886
-5278
-978
8009
-2219
8198
-3904
-7238
5767
-4277
-6698
-2543
2685
-2802
4404
2565
4407
6547
-487
7141
3284
4760
-2402
-6583
9579
-6541
-4751
-2929
-6946
-3927
8342
4883
5618
9780
-1842
1443
-7128
1994
5434
-7475
-5809
-5269
1557
-6164
-1863
-7795
1147
7936
-4227
-9430
-8671
-3276
1606
1039
-3384
1795
5046
3097
4338
5571
-2239
-6962
-8961
-2802
-76
-9737
-6844
-3064
-6309
3640
1256
-2344
5418
6521
-3679
301
151
-4146
7853
-6641
7740
7842
2638
8857
4338
2986
9994
7181
8693
1490
-1558
-4145
-4456
-3165
4217
7352
3486
-7519
-7905
5291
-7585
-9932
-3587
809
-7673
7013
-7836
865
4533
-1300
-6798
5129
-7357
5041
-6960
-4673
-8159
8881
-835
-7791
6852
1206
8260
-9179
5034
8671
-2172
2886
-5453
2920
-6770
-4457
4808
6857
4588
4603
7758
-4377
8712
-669
-8950
6486
-1841
3540
-9463
-1090
-9365
-5250
-1834
-4936
6637
-9932
6133
-8137
8883
-8628
-4366
7931
-388
-8364
-5798
1482
8185
-3987
-1094
-8253
-3597
2569
900
3676
-6739
-7941
-754
-1957
-2282
1011
7593
9197
-8852
-1273
-6293
-3859
-3810
9398
-9410
-4933
214
-2155
-6641
9186
2757
8224
-1967
3528
-1139
7204
-6047
-600
9513
2246
-4736
-1727
-5396
6184
-2684
5588
-7498
5997
1540
-1913
-8671
1889
-5197
7236
1671
-9097
-8095
2750
8771
2802
-3162
1397
8877
4543
8495
-7679
-3561
4372
9155
6837
2696
-1771
9644
-6864
2092
8853
-3490
6922
-2960
3748
-7069
2874
6998
8585
-6462
7560
2932
3623
-27
7302
5871
5194
1755
8544
1665
-7814
4950
8645
-600
-9900
-708
2723
-4601
-3200
-2615
6694
6095
-9013
9106
2330
1446
5684
-3176
-1406
1344
5320
7588
-294
1476
-3744
6024
-5024
-817
3152
2753
-2200
-5518
8345
3060
4338
-82
-8160
-2046
3742
-7621
-2070
1147
6488
-2921
9230
-9958
-475
-948
7051
9727
-7808
7723
6757
858
-6365
9414
859
5931
-8551
2196
-7941
-9454
9657
-9012
7108
-9603
-4545
-9037
-8549
-4873
-56
3112
-1542
145
7688
9994
-9367
9700
5650
-1100
-5022
-5728
9685
-5616
-9466
-9400
9083
-8607
-306
2432
460
-2639
4650
3881
-6114
4754
1026
523
-6394
7853
6707
6750
-4403
-5487
7050
-702
-4638
5844
-2395
7583
-2507
-6051
-3491
4472
-8534
5447
-6119
2612
7552
7190
-696
8667
1054
4599
-472
2398
-2452
-7911
5458
-2105
-111
6567
-4
3437
5972
7444
-266
-8441
-4804
7836
-9756
9348
8279
7545
-7009
1937
4616
-2351
-2390
-6961
9867
3814
9003
199
3691
8319
-3886
3158
5969
-3280
5153
3228
-3308
3274
9151
9886
-7621
9773
-6777
-6852
-3717
4041
-1300
-6269
2478
-1269
-6118
5359
-924
-6332
-6852
8537
-1157
-7605
9557
2652
1207
6403
8265
265
5135
-2986
5528
-4797
7751
-9669
8130
5254
7097
-3676
284
9534
-5912
-9198
-3493
2748
4650
4296
-2227
5587
-193
-2398
-9796
4344
-917
-4850
-3534
-3648
9752
3463
-8575
-3269
-6847
4776
-4374
-8219
-160
4049
2378
2814
6725
-1407
-1553
684
3942
-5252
7895
9462
-8659
-1452
-6321
-4363
-3758
-8973
-3195
8550
-3859
-6129
-563
5946
-2215
-3447
2888
5235
-8078
-8736
-1760
1120
2076
-1431
-4965
6477
6148
7082
8946
-3189
3748
3456
8892
6558
-4740
634
-6495
8729
5352
1678
-3406
2255
-3937
-3425
808
9493
-2357
-3170
-7439
3774
-7328
-5042
-5826
3110
4183
-3572
-2390
2491
-8621
-7811
-9161
-1916
3716
-4353
119
4707
-8691
3504
3887
-1627
-2616
1738
-4380
4844
6068
-6166
1585
-5026
9405
-3419
8451
-683
3014
3560
2378
-1785
-3242
-4552
5800
-9037
1091
4708
-9857
7286
5256
-1452
2548
847
-6051
2017
-7390
9984
-1120
-8406
8265
-4600
-1167
-6961
813
-1512
-7077
4643
-1376
-344
8817
-5932
-706
6486
3064
2801
2034
2037
-8347
-3161
-713
-7605
-2177
-3693
2922
1282
4269
8064
-9799
-568
561
1937
-5978
-8146
6482
8374
-4451
7688
948
974
-3724
4853
-6526
-1122
329
-9659
-720
-1622
-8628
1815
9040
-82
-7474
3294
-171
5259
-3274
932
4532
-3018
4471
2533
-2313
1058
-7785
9762
-7183
-4921
-4903
6666
-6451
-7131
4143
-9442
-8630
-9207
-7024
-4106
9132
464
-3860
7888
8697
-3434
6254
-1258
-9715
-271
-7374
2849
8143
-3361
-5419
9997
-4506
3252
9337
9667
-1709
7491
-5840
-447
-7969
8518
-781
4691
-1886
5156
6530
7521
2168
5366
483
6274
9234
-6485
6100
-1759
176
3826
-9830
8651
-7605
5100
-5590
4064
1604
9867
-678
-5370
-35
7305
2168
-8811
-403
7879
4581
5339
-5926
4808
-4842
340
-9243
4359
-5050
-9017
9326
-2452
-9346
7051
-2562
7979
-2434
7255
3833
-5344
3072
331
-6695
-6926
9827
-4959
-3182
-9361
414
-1403
-4380
-4521
-1090
8349
-759
9394
1536
-5511
4591
-7795
9728
9230
-9840
8651
-1547
-8381
-9147
6730
5238
4262
249
8145
4900
4599
1467
954
8334
6979
1224
2432
50
4264
4630
-1167
4800
-1191
693
-3563
-6378
974
-2419
1789
3054
-2412
-1584
2883
-3339
-7791
-5600
-5396
1087
-828
-2592
8601
5362
8826
-1666
8077
-6695
1072
-4943
-4530
-1482
-6621
8879
-968
9040
-2877
3079
-4962
6530
1862
1822
9834
4271
7949
-9690
829
9315
1779
4399
2132
-3274
4662
3216
7292
7094
7188
8535
7835
1584
2362
-741
1932
-9176
-9037
929
-8097
-5028
5516
4532
-7087
-5798
9477
-6265
4952
-8762
900
2224
-6735
-5709
-8114
2715
5121
-9818
-852
-481
-5998
6165
3261
-7318
4905
-3016
9473
-9638
-1957
-8007
9454
-944
-3523
3680
-5831
-5502
-1844
1256
1213
-6303
-836
3289
-9832
4182
-8256
-3817
-3260
-905
3444
1219
9958
2302
8846
9831
-6322
-4402
3247
7965
-1489
2095
-1489
-3968
-1407
-5327
-4629
-7238
5967
5235
-538
845
-9222
8505
-9408
1282
-5639
1090
2227
-5700
6752
-5841
9225
-4920
-9826
4707
9440
6405
-2155
7410
-3082
-5312
4098
9514
4105
933
-3406
9401
4961
4476
2391
5800
-1621
5362
3310
1075
-9713
4504
8389
-572
5032
-9536
-2292
-2675
7358
5502
1623
-6594
-8575
1732
-7946
-9237
-3681
-7628
2698
-1421
-826
-3343
-4227
4607
6370
9723
-6156
286
-7846
4557
5337
-4826
4439
7131
-9487
-3629
-9734
541
-2039
-1686
-3989
4621
-3225
-9449
-1304
6852
-2886
5955
986
-4897
2051
6028
564
-3758
520
-6564
-4150
8080
9337
9498
-8628
-7128
-4155
1962
-42
-1838
-2419
2463
-3157
-4741
735
-2341
-7683
-8854
-1269
-5438
101
3286
2036
9898
4737
6379
6670
2812
-629
-720
1738
1282
-4568
3094
1225
4830
9165
948
2359
-6679
-8283
7260
-7984
-5618
-4312
3110
9894
8692
5178
-2221
3321
-6621
3513
-3343
5165
-9915
2962
-1917
-3441
9126
7331
-149
-4957
3751
7108
-8462
7549
6428
8182
-3164
-8982
2156
-2407
5231
-4130
-4541
-6394
7410
6436
246
-8055
-6952
-2035
-8208
-1675
-5664
5967
-3316
-1702
3970
-193
-8735
-1404
-4522
5540
9664
-9207
5083
1555
-5726
-6202
-4865
-8330
3486
4085
-8462
9770
-3520
561
-9386
2091
-5039
1708
3863
5280
6362
9106
3905
6048
8854
9626
7771
-1686
-3655
-5259
1670
-1110
4294
-5372
9983
-4533
-2555
5110
-5307
1763
4413
800
-4503
-3134
7847
-503
507
6248
-600
-6361
-5078
824
9771
4266
5445
-2825
-4629
-8751
-686
-5318
188
-5664
-5211
-9060
-409
-2965
4414
-8843
-132
-9217
1870
3707
-4468
814
-2192
-9379
-5777
117
9230
-7224
-2424
-4269
5582
-5504
3975
-5638
7063
1871
-701
8400
-4698
6351
6492
-6672
6935
-9492
9530
-7087
-109
-6377
-4798
-4174
-5557
6934
4563
-7804
-5078
-3881
135
4439
-4419
2014
-6066
6434
-4834
-5385
-7795
-1765
3643
974
-8366
-7980
-5073
-4719
-285
-880
6935
4264
7604
4514
430
5418
1716
9348
-3089
-1376
-5798
6595
7134
-4543
579
-8865
7248
-7283
-9082
-8043
7349
-8298
-1028
5019
-8642
2466
3064
-3826
-6121
3742
-1070
999
9981
7604
-9111
-841
-4946
1753
2255
9383
-3685
1123
-6910
-720
9928
9579
-4657
-593
249
-4943
5687
6362
-140
2646
-3687
8106
-7402
-2523
-2820
8197
4635
2350
8919
4630
-9414
-3926
373
1583
8846
-8263
-6743
7845
-3059
3813
1723
-1686
-4079
-9156
-2458
5332
330
-4341
8612
-8231
6667
5789
2890
-2888
-4102
-7147
9802
6771
-2149
5854
6940
-2955
-8046
3456
9475
4263
932
-1884
4132
402
6571
8397
-6329
6911
-3090
6907
4845
6859
8098
-702
1513
-396
3981
9850
-1796
-8032
-7498
6184
-2886
-2185
-1778
3087
-9875
3639
4915
-816
9760
6882
329
6274
-7055
7868
8491
-1780
2052
-3756
8826
5168
-4727
7352
4325
1736
-3239
9691
-4130
-3162
4052
7464
-6357
2472
-115
-640
-3116
-3668
-996
5256
8226
-2539
-5444
-8750
7915
-326
3187
8267
-3293
8615
-9254
-5475
-2035
-1598
-1575
9163
-9145
5941
-5728
4184
-3300
-9183
-1543
2132
9550
-3209
5891
-2365
52
-6689
-3785
5153
3489
-6828
-8612
6768
9295
-2410
-3237
-1084
2587
9351
-6235
744
9664
5324
4543
-9722
-1792
-119
525
-671
1908
5032
-9013
7108
-4212
-3562
4597
-367
5713
8428
1465
6915
-6541
5979
-2341
-5188
6857
-2694
-6610
1678
-9186
-7905
9781
-3165
-2903
2509
-1337
800
3380
5456
8784
-2090
-9771
7778
-9249
-531
116
1175
-6513
2194
1862
-6579
-6439
7004
1638
-6327
5624
-5528
-1334
7138
1343
-8659
782
-6253
-5938
8645
2656
-301
-2349
-8975
-6787
5155
5231
-5570
6967
3702
123
-5167
-3043
3402
8371
-5952
-4915
-4801
9150
1975
-2202
4119
-6226
3916
-140
4439
9348
-1929
8364
2022
4469
-3183
4672
9115
-4793
7429
3462
-8779
-5926
4292
7525
7220
-578
2921
-1964
-6473
5005
3284
8692
-1133
-2762
-4744
9279
66
7571
5572
4294
9404
-5278
9864
-7402
-229
-9755
-8739
8926
-7615
5571
4451
5740
-3598
-4935
-1456
-3802
5455
-8932
6224
-565
8077
6725
-3671
9890
6851
1735
-3467
7239
885
5262
7640
-188
9126
-1792
-4506
-8270
-8613
-2434
8968
-6397
1592
5120
779
3291
-2283
1497
4754
-1910
-9944
9639
-5484
-487
8367
5257
8616
9485
-4227
-8593
-1160
385
-3945
5708
-157
-1784
-2639
-7679
-4953
-9632
-9700
-1787
-132
-2285
-7987
-147
-9041
-6377
8238
-3854
-5951
535
-2357
-2730
-2285
1851
-5448
9028
196
1716
-7113
-2707
-2035
3637
3112
-9502
4906
2203
1913
-4713
6075
3486
1541
-4948
1432
7737
1118
-1902
2451
-3007
-3904
-9350
7634
-7446
461
5041
-30
-9377
8814
-6346
9043
9467
7342
-4615
-8317
7764
-3477
-7356
-4967
6803
-8861
-9631
-6098
-3876
-1765
1043
-3667
-9530
2347
-6564
-7637
-8364
-4275
1525
6530
-5481
-486
-1603
-8823
9197
-214
5349
3017
1200
-9386
7375
-2775
7254
7178
-3681
-5467
-8198
1397
-311
-7226
3795
8448
-3789
4486
5075
4635
-6363
-3340
-422
5035
-4477
8356
-5102
-7568
5994
-5784
-7827
-6142
-3826
-9780
-4699
-4771
4845
6451
4168
8056
2684
5339
-4866
3730
7319
-6390
-4740
-4798
2805
-1011
5403
474
7720
-1090
2223
-3183
6154
-3741
-8381
2323
-3943
-1269
2455
-8625
-7987
6954
2034
9836
4141
7404
4850
40
3670
-1333
7412
7935
8415
-8526
2113
7452
6736
-8263
-4374
652
-6451
1490
9760
-256
-7714
2533
3199
-2773
-3681
5539
-1916
1909
-1964
-2762
-8349
-2932
7336
-6530
-7720
-8383
9332
-8828
-5689
-3333
6658
-5582
4404
1674
-7479
-6776
-8705
-8130
100
6783
9880
-4516
-2412
-3647
-4900
8715
6039
9330
-6499
-4598
8301
4273
812
-5745
-5918
-555
6650
3252
-7922
-6021
-3100
-4161
2552
7598
5871
3691
-2375
-8230
-7528
7566
-4716
2069
32
3784
4954
-7388
-616
-3875
-8626
7090
4066
2432
-4920
-7987
-2863
7798
-5504
-2362
9762
1989
6228
-8959
8495
-60
-7517
-5272
2690
-3511
2647
-4374
2647
-1540
-4472
-3451
-7292
-9577
9539
8487
4415
-5599
1337
-7103
-9811
-8913
3972
-4503
-9632
9104
3442
6059
5947
-5024
-432
4519
-1902
1754
-7455
-9956
8194
9202
-9535
8935
8935
-2793
-4277
81
692
-4098
528
1400
356
-7193
8322
7757
-5739
-9789
8946
-5459
-4716
3739
8679
-1252
-7177
-7496
319
8826
-6009
-8476
-1674
-4506
8222
-1760
7740
-2688
683
228
6131
9696
6071
-4872
8660
7342
1611
-9532
4360
5566
9607
-4967
4132
2026
-1884
8124
2100
-1490
407
-1649
-5627
1642
-8927
-3089
5917
1568
4104
228
3199
-5612
-806
2759
3463
5005
6718
747
-1238
4030
7309
-111
5055
9721
4574
7299
-5282
-5484
3838
-6836
4829
6656
-5251
-4818
-8492
2460
-1675
7048
8279
-5707
-134
-9
2284
-736
1147
7139
-9296
-9297
-940
5296
1755
-1217
-1003
8808
6737
-2262
2014
-701
-3095
-9138
-4808
-4565
3746
-9339
-1938
-4293
-5277
2921
-4506
-2679
-9032
2759
6611
-4943
2214
6118
6314
-7851
-1829
8826
-849
1430
-5086
-3185
-4886
-9932
4447
-403
6850
4962
1007
4177
4937
-1061
-4154
4201
6510
-5517
6718
9676
-6946
5571
-2277
-7299
-9536
-9659
1768
4024
2328
-621
-8437
4732
1337
8979
-4638
-4293
-6143)";
