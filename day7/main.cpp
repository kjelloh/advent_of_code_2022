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

using Token = std::string;
using Tokens = std::vector<Token>;
using Model = std::vector<Tokens>;

Model parse(auto& in) {
    Model result{};
    std::string line{};
    while (std::getline(in,line)) {
        // std::cout << "\nline:" << std::quoted(line);
        std::istringstream is{line};
        Token token{};
        Tokens tokens{};
        while (is >> token) {
          tokens.push_back(token);
          // std::cout << " token:" << std::quoted(token);
        }
        result.push_back(tokens);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os,Tokens const& tokens) {
  for (auto const& token : tokens) os << "[" << token << "]";
  return os;
}

class DirTree {
public:
  using Vertex = std::string;
  using Vertices = std::vector<Vertex>;
  using AdjList = std::map<Vertex,Vertices>;
  using Size = std::map<Vertex,Result>;
  using Path = std::vector<Vertex>;
  DirTree(Model const& data_model) {
    Path current_dir{};
    for (auto const& tokens : data_model) {
      std::cout << "\ntokens:" << tokens;
      if (tokens[0]=="$") {
        std::cout << " COMMAND:" << std::quoted(tokens[1]);
        if (tokens[1] == "cd") {
          auto vertex = tokens[2];
          std::cout << "\n\tgo_to:" << vertex;
          if (vertex == "..") {
            current_dir.pop_back();
          }
          else {
            current_dir.push_back(vertex);
          }
        }
        else if (tokens[1] == "ls") {
          std::cout << "\n\tdo_list:";
        }
      }
      else if (tokens[0]=="dir") {
        auto vertex = tokens[1];
        std::cout << "\n\tDIR VERTEX:" << std::quoted(vertex);
        // insert vertex as connected to current path
        add_edge(current_dir,vertex,-1);
      }
      else {
        auto vertex = tokens[1];
        auto size = std::stoi(tokens[0]);
        std::cout << "\n\tFILE VERTEX:" << std::quoted(vertex) << " size:" << size;
        add_edge(current_dir,vertex,size);
      }
    }
  }
  friend std::ostream& operator<<(std::ostream& os,DirTree const& dir_tree);
  Vertices adj(Vertex const& vertex) const {
    Vertices result{};
    if (m_adj.contains(vertex)) result = m_adj.at(vertex);
    else {
      std::cerr << "\nIs leaf vertex:" << std::quoted(vertex);
    }
    return result;
  }
  int size(Vertex const& vertex) const {return (
    m_size.contains(vertex)?m_size.at(vertex):0);
  }
private:
  AdjList m_adj{};
  Size m_size{};
  void add_edge(Path const& path,Vertex const& vertex,int size) {
    auto from = path.back();
    m_adj[from].push_back(vertex);
    if (size>0) m_size[vertex] = size;
    std::cout << "\n\tadd_edge: " << std::quoted(from) << " - " << std::quoted(vertex);
  }
};

namespace path {
  std::string to_string(DirTree::Path const& path) {
    std::ostringstream os{};
    for (auto const& vertex : path) {   
      os << "[" << vertex << "]";
    }
    os << " count:" << path.size();
    return os.str();
  }
}

class DFS {
public:
  DFS(DirTree const& dir_tree) : m_dir_tree{dir_tree} {}
  void visit(DirTree::Vertex const& vertex,auto& f) {
    if (m_marked[vertex]) return;
    m_marked[vertex] = true;
    f(vertex);
    for (auto const& connected : m_dir_tree.adj(vertex)) {
      visit(connected,f);
    }
  }
private:
  DirTree const& m_dir_tree;
  std::map<DirTree::Vertex,bool> m_marked{};
};

std::ostream& operator<<(std::ostream& os,DirTree const& dir_tree) {
  auto print_vertex = [&os,&dir_tree](DirTree::Vertex const& vertex) {
    os << "\nvisit:" << std::quoted(vertex);
    auto size = dir_tree.size(vertex);
    if (size >=0) os << " size:" << size;
  };
  DFS dfs{dir_tree};
  dfs.visit("/",print_vertex);
  return os;
}

using PathToSize = std::map<DirTree::Path,int>;     

PathToSize rdfs(DirTree const& dir_tree,DirTree::Path const& source) {
  std::cout << "\nrdfs(" << path::to_string(source) << ")";
  PathToSize result{};
  if (dir_tree.adj(source.back()).size()==0) {
    // std::cout << " LEAF";
    result[source] = dir_tree.size(source.back());
  }
  else {
    // Visit all connected
    for (auto const& connected : dir_tree.adj(source.back())) {
      auto sub_path = source; sub_path.push_back(connected);
      auto x = rdfs(dir_tree,sub_path);
      int acc_size{};
      for (auto const& entry : x) {
        result.insert(entry);
        acc_size += dir_tree.size(entry.first.back());
      }
      result[source] += acc_size;
    }
  }
  // std::cout << "\nrdfs(" << path::to_string(source) << ") RETURN:";
  // for (auto const& [path,size] : result) {
  //   std::cout << "\n\tpath:" << path::to_string(path) << " size:" << size;
  // }
  return result;
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      DirTree dir_tree{data_model};
      std::cout << "\n" << dir_tree;
      std::vector<DirTree::Path> dirs{};
      auto path_to_size = rdfs(dir_tree,DirTree::Path{1,"/"});
      for (auto const& [path,size] : path_to_size) {
        std::cout << "\npath:" << path::to_string(path) << " size:" << size;
        if (dir_tree.adj(path.back()).size()>0 and size <= 100000) {
          result += size;
          std::cout << " COUNTED result=" << result;
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

char const* pTest = R"($ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k)";
char const* pData = R"($ cd /
$ ls
dir bgmjrlz
dir bhp
dir cbcwz
169838 fddw.bgw
dir fvhmzqc
dir hqmlnpn
248637 jtwpn.lnr
319470 lnmrrht.zbn
99548 pqpslbtn
dir rbztmqjn
102720 rqpt
dir vfrtt
$ cd bgmjrlz
$ ls
dir dhqzgdl
dir djtchhmw
dir tvq
$ cd dhqzgdl
$ ls
dir jjshzrhd
dir jlfz
dir vqvvwgt
dir zcqbt
$ cd jjshzrhd
$ ls
dir jjshzrhd
$ cd jjshzrhd
$ ls
81645 gfhfplmm
$ cd ..
$ cd ..
$ cd jlfz
$ ls
dir hgzg
189290 wgdffcr
$ cd hgzg
$ ls
121740 vmmcdr
$ cd ..
$ cd ..
$ cd vqvvwgt
$ ls
142209 djtchhmw.tgr
$ cd ..
$ cd zcqbt
$ ls
204760 mlsfnt
$ cd ..
$ cd ..
$ cd djtchhmw
$ ls
287664 cgjmd.vrb
307590 ghrntg.zsw
$ cd ..
$ cd tvq
$ ls
270869 jjshzrhd.lzb
$ cd ..
$ cd ..
$ cd bhp
$ ls
231648 gmj.srn
260008 hrtbfww.gts
287048 jgqjszsr
dir mps
dir sjt
dir zvbwsw
$ cd mps
$ ls
dir djtchhmw
153066 dnpv.vff
185096 slpz.phm
$ cd djtchhmw
$ ls
310736 ddhfvv.lzl
234532 hghrcqfd.hpn
276144 hrtbfww.gts
$ cd ..
$ cd ..
$ cd sjt
$ ls
dir btgrv
dir csn
dir ddhfvv
dir dfbjz
dir djtchhmw
dir qdms
45629 qgj.jjs
dir rdqbdtdh
235189 wcrsz.ccc
$ cd btgrv
$ ls
dir rnhq
$ cd rnhq
$ ls
205285 vmmcdr
$ cd ..
$ cd ..
$ cd csn
$ ls
234221 nhw.rml
$ cd ..
$ cd ddhfvv
$ ls
dir glbf
56038 gvbzwds.cff
dir qtc
308363 rdzj.sqr
dir vlrs
dir vzhm
dir wwghpsds
287177 zznmdh.nhn
$ cd glbf
$ ls
128210 btgrv.zqp
205284 dnt.jrd
135774 pmnbbb
$ cd ..
$ cd qtc
$ ls
292185 zsqz
$ cd ..
$ cd vlrs
$ ls
dir btgrv
dir jlbjlzzs
dir lnqcr
$ cd btgrv
$ ls
277089 bcpdvwqs.dmw
262922 hghrcqfd.hpn
$ cd ..
$ cd jlbjlzzs
$ ls
95245 vmmcdr
$ cd ..
$ cd lnqcr
$ ls
172326 qsrcb.fpd
$ cd ..
$ cd ..
$ cd vzhm
$ ls
203623 fvhmzqc.dmm
$ cd ..
$ cd wwghpsds
$ ls
60280 vmmcdr
$ cd ..
$ cd ..
$ cd dfbjz
$ ls
262505 blc.lhp
24423 ddhfvv
296606 fvhmzqc.ptz
98808 hghrcqfd.hpn
$ cd ..
$ cd djtchhmw
$ ls
278654 hghrcqfd.hpn
$ cd ..
$ cd qdms
$ ls
dir djtchhmw
154536 jjshzrhd.stf
dir sgh
$ cd djtchhmw
$ ls
246903 hghrcqfd.hpn
$ cd ..
$ cd sgh
$ ls
265535 btgrv.frs
299957 hffpl.qzw
$ cd ..
$ cd ..
$ cd rdqbdtdh
$ ls
dir btgrv
dir djtchhmw
182591 jjshzrhd
22987 jjshzrhd.qwp
dir jmc
185957 mthrpb.qmm
$ cd btgrv
$ ls
dir btgrv
9328 ddhfvv
7652 hghrcqfd.hpn
53498 wgdffcr
$ cd btgrv
$ ls
dir czgrcv
171099 hrtbfww.gts
22232 lqwvnz
dir mfhbd
193089 scld.jpg
75876 vmmcdr
226425 wgdffcr
$ cd czgrcv
$ ls
276374 lfctmv.dbp
268014 mpvb.rfg
180548 wgdffcr
$ cd ..
$ cd mfhbd
$ ls
67713 fvhmzqc.llz
$ cd ..
$ cd ..
$ cd ..
$ cd djtchhmw
$ ls
3290 ddhfvv.vsl
dir fvhmzqc
168684 hghrcqfd.hpn
dir jmqwll
80802 qmfhm.gtf
282318 rzn.chg
148018 wgdffcr
$ cd fvhmzqc
$ ls
dir bjpvn
87780 hghrcqfd.hpn
209282 vtv.wbt
$ cd bjpvn
$ ls
126911 rcq
$ cd ..
$ cd ..
$ cd jmqwll
$ ls
240811 vmmcdr
$ cd ..
$ cd ..
$ cd jmc
$ ls
320199 btgrv.ntz
11100 jwcvvfnb.grd
145758 wgdffcr
$ cd ..
$ cd ..
$ cd ..
$ cd zvbwsw
$ ls
dir fvhmzqc
$ cd fvhmzqc
$ ls
169680 rlnvr.bwd
$ cd ..
$ cd ..
$ cd ..
$ cd cbcwz
$ ls
82379 hrtbfww.gts
$ cd ..
$ cd fvhmzqc
$ ls
140571 hrtbfww.gts
dir hvnt
dir jbc
dir mzzfssn
dir npdccs
dir wrzzq
$ cd hvnt
$ ls
102394 hrtbfww.gts
29683 hsgstppl
dir rmrc
134244 rsjrj.gbr
231284 wqhndr.hlr
207733 wtjz
$ cd rmrc
$ ls
dir btgrv
259148 hrjqjdqq.tvm
$ cd btgrv
$ ls
240410 tqv
$ cd ..
$ cd ..
$ cd ..
$ cd jbc
$ ls
140479 fvhmzqc.pvm
$ cd ..
$ cd mzzfssn
$ ls
78226 brtbv.gtp
61906 btgrv
168944 nqll
111153 qmrsgwh
$ cd ..
$ cd npdccs
$ ls
65889 wfpvp.wsg
$ cd ..
$ cd wrzzq
$ ls
dir btgrv
82867 djtchhmw
dir dzzv
dir lpz
dir mqqlhnvh
$ cd btgrv
$ ls
dir dppvz
dir glmtpswv
dir qfgqfzm
dir qhb
$ cd dppvz
$ ls
296857 hrtbfww.gts
11272 jjshzrhd
$ cd ..
$ cd glmtpswv
$ ls
268244 cgntm.tcf
dir jjshzrhd
dir lqnb
128070 tzctcnq.gwr
110659 wnrblpbs.wqf
$ cd jjshzrhd
$ ls
dir cwdwh
173945 fzstdt.pdn
224834 nnvnqrh.zld
dir tfzp
$ cd cwdwh
$ ls
dir btgrv
315172 ddhfvv.vdc
109603 dsqjgv
dir hnp
284882 mnsb.cdh
247067 qtntt.jhn
200809 tvtbfn
$ cd btgrv
$ ls
74389 ndfzlfzf.lth
$ cd ..
$ cd hnp
$ ls
242535 ddhfvv
256542 nslg.qcc
143475 wbzjdrhd.gbr
$ cd ..
$ cd ..
$ cd tfzp
$ ls
46652 djtchhmw
167857 rtqcpsd
$ cd ..
$ cd ..
$ cd lqnb
$ ls
186160 ddhfvv
287136 wgdffcr
$ cd ..
$ cd ..
$ cd qfgqfzm
$ ls
212359 btgrv.hjj
$ cd ..
$ cd qhb
$ ls
15584 djtchhmw
$ cd ..
$ cd ..
$ cd dzzv
$ ls
141830 hrtbfww.gts
$ cd ..
$ cd lpz
$ ls
dir djtchhmw
14679 jjshzrhd
dir pbcrz
$ cd djtchhmw
$ ls
dir fvhmzqc
250329 hnhzlwrm.bqp
dir jjshzrhd
262196 jjshzrhd.gnz
dir qpc
dir svmmjlr
dir tddmdzd
236002 vmmcdr
101814 vwpnztc
$ cd fvhmzqc
$ ls
44990 fvhmzqc.ngb
235856 hlbhsz
219184 hrtbfww.gts
$ cd ..
$ cd jjshzrhd
$ ls
190311 clbzz
$ cd ..
$ cd qpc
$ ls
217404 jjshzrhd.nwv
142286 pgjgsh.sdd
$ cd ..
$ cd svmmjlr
$ ls
82636 cvbhsch
287045 fvhmzqc.rjh
48607 vmmcdr
$ cd ..
$ cd tddmdzd
$ ls
dir cjfjmjnh
dir fjwwn
7777 hghrcqfd.hpn
220410 tgfqgcc.ngd
$ cd cjfjmjnh
$ ls
dir gcs
$ cd gcs
$ ls
210998 nzzjl
79843 zltwtmnv
$ cd ..
$ cd ..
$ cd fjwwn
$ ls
17068 rht.hhw
$ cd ..
$ cd ..
$ cd ..
$ cd pbcrz
$ ls
41401 tzqjl.bhn
144639 zqght
$ cd ..
$ cd ..
$ cd mqqlhnvh
$ ls
dir bmcqbq
dir cqh
dir jjshzrhd
dir lbvrm
dir lwmbvsjj
dir rrcnhbn
67325 tfcl.npl
$ cd bmcqbq
$ ls
dir bwjhtvcm
dir wzmg
$ cd bwjhtvcm
$ ls
156583 qdjmmdq
$ cd ..
$ cd wzmg
$ ls
dir ddhfvv
dir tlhc
$ cd ddhfvv
$ ls
dir jjshzrhd
$ cd jjshzrhd
$ ls
202499 ddhfvv
$ cd ..
$ cd ..
$ cd tlhc
$ ls
dir lbpmtft
$ cd lbpmtft
$ ls
43196 lthlvv
$ cd ..
$ cd ..
$ cd ..
$ cd ..
$ cd cqh
$ ls
dir btgrv
35483 djtchhmw
318895 sfqdbd
62778 twdpcn.rzg
dir vwdqmtwf
$ cd btgrv
$ ls
dir hrbjmbf
dir npffrswd
$ cd hrbjmbf
$ ls
196766 crw.dht
$ cd ..
$ cd npffrswd
$ ls
dir jjshzrhd
$ cd jjshzrhd
$ ls
161401 dsqqh.pqg
$ cd ..
$ cd ..
$ cd ..
$ cd vwdqmtwf
$ ls
299604 qrdcnbt.wmh
$ cd ..
$ cd ..
$ cd jjshzrhd
$ ls
62584 dzfvzrf.tmc
255105 hwfh.tfd
$ cd ..
$ cd lbvrm
$ ls
dir btgrv
dir pvnbdwjj
21599 tjfd.jzf
315782 wgdffcr
dir znz
$ cd btgrv
$ ls
dir qjbhvdm
$ cd qjbhvdm
$ ls
115711 mjhcwn
$ cd ..
$ cd ..
$ cd pvnbdwjj
$ ls
156828 hrtbfww.gts
dir jjshzrhd
208415 vmmcdr
$ cd jjshzrhd
$ ls
131751 hwfh.tfd
$ cd ..
$ cd ..
$ cd znz
$ ls
dir ddhfvv
150828 gpcpnwj.fzv
119331 hrtbfww.gts
263285 nntqssp.hqg
dir pwtbr
236806 vmmcdr
$ cd ddhfvv
$ ls
265355 ddhfvv.bpb
$ cd ..
$ cd pwtbr
$ ls
212361 jjshzrhd.nmh
$ cd ..
$ cd ..
$ cd ..
$ cd lwmbvsjj
$ ls
275264 bvbq.rdf
dir ddhfvv
257257 fsql
210469 jmvchpn
57627 lrnhn
270278 vmmcdr
dir vrqmtl
$ cd ddhfvv
$ ls
244640 nhdztzsg
$ cd ..
$ cd vrqmtl
$ ls
123207 btgrv.qsg
152242 qsqt
259711 scvzvns.vvh
$ cd ..
$ cd ..
$ cd rrcnhbn
$ ls
dir jjshzrhd
$ cd jjshzrhd
$ ls
63581 btgrv.pbj
$ cd ..
$ cd ..
$ cd ..
$ cd ..
$ cd ..
$ cd hqmlnpn
$ ls
dir djtchhmw
dir dlqjbqbr
240226 fdmchrth
dir fvhmzqc
39519 hvtvcdv
140559 hwfh.tfd
243880 lvcwjnzb.ptf
dir nppp
162576 rmqmd.wdt
dir rscgvrdt
dir sdcbr
$ cd djtchhmw
$ ls
33004 ddhfvv.ghw
$ cd ..
$ cd dlqjbqbr
$ ls
dir fvhmzqc
dir jjshzrhd
dir ncwldt
dir wfw
$ cd fvhmzqc
$ ls
dir brfl
dir ccnlzrb
dir cjtl
dir dfvt
123279 fvhmzqc.qfp
dir ppshjv
dir smprwhmg
$ cd brfl
$ ls
291222 djtchhmw.grj
102020 hghrcqfd.hpn
197538 hwfh.tfd
82351 qqq.cqf
254314 ztthgs
$ cd ..
$ cd ccnlzrb
$ ls
dir rzmdmq
$ cd rzmdmq
$ ls
265088 rqtm.zmv
$ cd ..
$ cd ..
$ cd cjtl
$ ls
107485 djtchhmw.phc
$ cd ..
$ cd dfvt
$ ls
114165 dts.rlc
201057 frljlqzr.clp
$ cd ..
$ cd ppshjv
$ ls
305733 djtchhmw.ntn
$ cd ..
$ cd smprwhmg
$ ls
76792 ddhfvv
$ cd ..
$ cd ..
$ cd jjshzrhd
$ ls
19740 fvhmzqc
dir lqgsw
dir lsmccpj
17490 mlzznrc.mst
$ cd lqgsw
$ ls
297439 crqjmhrb.grs
$ cd ..
$ cd lsmccpj
$ ls
89758 btgrv.cpt
49280 ddhfvv.drt
dir gclrgz
dir gnrztgj
275064 jjshzrhd.jzv
98597 nbscl.wvs
225844 vmmcdr
$ cd gclrgz
$ ls
63611 djtchhmw.vfn
156340 gbhsz
$ cd ..
$ cd gnrztgj
$ ls
23195 btgrv
287815 fhthsd
$ cd ..
$ cd ..
$ cd ..
$ cd ncwldt
$ ls
70881 vmwllc.fbf
$ cd ..
$ cd wfw
$ ls
309325 ddhfvv.pqm
201372 fvhmzqc.rfl
143184 hghrcqfd.hpn
51325 vlq.wgr
$ cd ..
$ cd ..
$ cd fvhmzqc
$ ls
234497 ddhfvv.wlg
dir fvhmzqc
197961 fzbsr
dir jsfbvwb
$ cd fvhmzqc
$ ls
208299 bgpncvh.jhl
$ cd ..
$ cd jsfbvwb
$ ls
175184 ddhfvv.tpq
85214 djtchhmw.btf
2012 ttbpmsg.mlb
$ cd ..
$ cd ..
$ cd nppp
$ ls
223933 wgdffcr
$ cd ..
$ cd rscgvrdt
$ ls
207388 glgbngv.lcd
$ cd ..
$ cd sdcbr
$ ls
277288 btgrv.phv
49684 ddhfvv
195222 dntzwh
dir frcj
206408 gfttdcnq
147023 hrtbfww.gts
dir tcwpvrr
$ cd frcj
$ ls
37309 ctpjbmh
54747 hwfh.tfd
151065 phvllpq.gvh
$ cd ..
$ cd tcwpvrr
$ ls
63500 chsgcw.frm
dir ltwvvrv
113779 mgdqjg
177222 vzgpfpq.qln
$ cd ltwvvrv
$ ls
102472 vmmcdr
$ cd ..
$ cd ..
$ cd ..
$ cd ..
$ cd rbztmqjn
$ ls
216106 fnzwzdb.wrc
$ cd ..
$ cd vfrtt
$ ls
dir ddhfvv
dir ftrqt
dir fvhmzqc
186081 hwfh.tfd
dir jjshzrhd
dir mzfq
dir nhzz
dir zblsznh
$ cd ddhfvv
$ ls
28659 glfpm.hnp
274896 hrtbfww.gts
dir lwblgr
dir qmc
dir thwccb
dir tmlwvtmc
228636 zrn.ftn
$ cd lwblgr
$ ls
237410 djtchhmw.fjz
209100 hghrcqfd.hpn
317411 nsgtmddt.jvj
30033 pbhc.blz
8818 pwf.vjv
$ cd ..
$ cd qmc
$ ls
249328 cwftvdws
41124 pwmzz
99884 qbvpslt
$ cd ..
$ cd thwccb
$ ls
dir ddhfvv
$ cd ddhfvv
$ ls
49107 fvhmzqc.slp
$ cd ..
$ cd ..
$ cd tmlwvtmc
$ ls
dir djtchhmw
115288 gfdzqrb
248419 hrtbfww.gts
$ cd djtchhmw
$ ls
14211 qffwlmvm.fhp
dir tzbd
67495 wwttgflg.rcl
$ cd tzbd
$ ls
278463 btgrv.ldc
$ cd ..
$ cd ..
$ cd ..
$ cd ..
$ cd ftrqt
$ ls
95545 ddhfvv
180375 hwfh.tfd
$ cd ..
$ cd fvhmzqc
$ ls
161318 gtwj
$ cd ..
$ cd jjshzrhd
$ ls
91626 prmznc.gwp
dir wltvrv
$ cd wltvrv
$ ls
299813 hrtbfww.gts
$ cd ..
$ cd ..
$ cd mzfq
$ ls
101218 jjshzrhd.nwz
200289 njwfbc.bhb
$ cd ..
$ cd nhzz
$ ls
225881 hwfh.tfd
210133 mlb.wrt
$ cd ..
$ cd zblsznh
$ ls
dir fvhmzqc
214252 hrtbfww.gts
250855 qbjphgwn.vvj
dir tdpv
173807 wgdffcr
$ cd fvhmzqc
$ ls
dir djtchhmw
dir fsqdcwr
dir jjshzrhd
dir zljhz
$ cd djtchhmw
$ ls
dir sdrjlqqm
$ cd sdrjlqqm
$ ls
91244 fvhmzqc
$ cd ..
$ cd ..
$ cd fsqdcwr
$ ls
dir ddhfvv
dir nhmhgzt
dir pdhbd
$ cd ddhfvv
$ ls
199548 qwc
$ cd ..
$ cd nhmhgzt
$ ls
106393 ddhfvv.sjg
$ cd ..
$ cd pdhbd
$ ls
207023 hngmj.qls
$ cd ..
$ cd ..
$ cd jjshzrhd
$ ls
84955 vmmcdr
$ cd ..
$ cd zljhz
$ ls
dir dmqmc
dir jnlgsgn
dir mhtmt
dir mqtmpht
$ cd dmqmc
$ ls
dir djtchhmw
$ cd djtchhmw
$ ls
166369 hrtbfww.gts
$ cd ..
$ cd ..
$ cd jnlgsgn
$ ls
289581 vrbqvt.bgn
$ cd ..
$ cd mhtmt
$ ls
182104 hrtbfww.gts
285446 spgtjm.lhj
$ cd ..
$ cd mqtmpht
$ ls
179017 bgfgtqr.snr
dir czqj
dir gdc
104624 jjshzrhd
314246 mhvzncnt.vjd
$ cd czqj
$ ls
69494 hft.fsp
$ cd ..
$ cd gdc
$ ls
84423 bhqnfj.cts
$ cd ..
$ cd ..
$ cd ..
$ cd ..
$ cd tdpv
$ ls
dir frgfgd
dir sggm
274423 zzfsmdf
$ cd frgfgd
$ ls
205662 vmmcdr
$ cd ..
$ cd sggm
$ ls
260269 wzpjsjnq.nvt)";
