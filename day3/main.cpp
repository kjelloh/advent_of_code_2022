#include <iostream>
#include <iomanip> // E.g., std::quoted
#include <iterator>
#include <string>
#include <sstream> // E.g., std::istringstream, std::ostringstream
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <deque>
#include <ranges> // E.g., std::subrange, std::view 
#include <utility> // E.g., std::pair,..
#include <algorithm> // E.g., std::find, std::all_of,...
#include <numeric> // E.g., std::accumulate
#include <limits> // E.g., std::numeric_limits

extern char const* pTest;
extern char const* pData;

using Result = size_t;
using Answers = std::vector<std::pair<std::string,Result>>;

using Model = std::vector<std::pair<std::string,std::string>>;

std::string to_string(auto v) {
  return {v.begin(),v.end()};
}

// Model parse(std::string_view s) {
//   Model result{};
//   for (auto const& line : std::views::split(s, "\n")) {
//     result.push_back(to_string(line));
//   }
//   return result;
// }

Model parse(auto& in) {
    Model result{};
    std::string line{};
    while (std::getline(in,line)) {
        auto pos = line.size() / 2;
        result.push_back({line.substr(0,pos),line.substr(pos)});
    }
    return result;
}

std::set<char> to_set(std::string s) {
  std::set<char> result{};
  for (auto ch : s) result.insert(ch);
  return result;
}

std::set<char> to_intersection(std::set<char> first,std::set<char> second) {
  std::set<char> result{};
  auto iter = std::set_intersection(
     first.begin()
    ,first.end()
    ,second.begin()
    ,second.end()
    ,std::inserter(result,result.begin()));
  return result;
}

std::set<char> to_intersection(std::string const& left,std::string const& right) {
  return to_intersection(to_set(left),to_set(right));
}

Result to_priority(char ch) {
  if (std::islower(ch)) return static_cast<Result>(ch-'a'+ 1);
  else return static_cast<Result>(ch-'A'+ 27);
}

namespace part1 {
  Result solve_for(char const* pData) {
      Result result{};
      std::stringstream in{ pData };
      auto data_model = parse(in);
      for (auto const& [left,right] : data_model) {
        std::cout << "\nleft:" << std::quoted(left) << " right:" << std::quoted(right);
        auto intersection = to_intersection(left,right);
        std::cout << "\n\tintersection:";
        for (auto ch : intersection) {
          std::cout << " " << ch << " priority:" << to_priority(ch);
          result += to_priority(ch);
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
      auto iter = data_model.begin();
      while (iter != data_model.end()) {
        std::vector<std::set<char>> r_sets{};
        for (int i=0;i<3;++i) {
          auto line = iter->first + iter->second;
          std::cout << "\nrucksack:" << line;
          auto r_set = to_set(line);
          r_sets.push_back(r_set);
          ++iter;
        }
        std::set<char> conjunction{};
        for (auto const& r_set : r_sets) {
          std::copy(r_set.begin(),r_set.end(),std::inserter(conjunction,conjunction.begin()));
        }
        std::cout << "\n\tconjunction:";
        for (auto ch : conjunction) {
          std::cout << " " << ch;
        }
        std::set<char> intersection{conjunction};
        for (auto const& r_set : r_sets) {
          intersection = to_intersection(intersection,r_set);
        }
        std::cout << "\n\tintersection:";
        for (auto ch : intersection) {
          std::cout << " " << ch;
          result += to_priority(ch);
        }
      } // while
      return result;
  }
}

int main(int argc, char *argv[])
{
  Answers answers{};
  // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
  // answers.push_back({"Part 1     ",part1::solve_for(pData)});
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

char const* pTest = R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw)";
char const* pData = R"(shzsFcPssFhjFssBzdpRcNHNZrpdJdJVJZ
fwvMCntfCCbSbSbtDgDNrDtDtJHZVH
GbCwwbwwnGrLhBzjFFFsWPhL
PpCqRsqqmmtCwMJC
LHFrLLHDSNHlfWNhDzmjzzJlJzPJMvPJjQ
SGSWDNrhZhPDSWDZLgVVRgbRppgpGVnpnn
GRRjbVjmJZlgMRzzrN
FpDptHpfHfnpPTvDFTWpFPnPcMfNCClNrzcVcrMMzVsCZlsZ
TFTQDnvLHPFDtVbLwbjdGjdwwJ
lhljvvhCjjzhjszzBPmnmGVZMGzG
FbTcTwbtSFdtcMPnTBPQVnnBZT
SFMpHDtNDSSbSdwppvgJWjJCJJgWgvlJHH
wzNCWpzCzJnWWpRRNdJrgHLhjfbLrHrchV
lBMStmPmmLQDPQZlshrdhgrfrcrrddgHgs
mvGDGQSvDPBlGMLGCvCWpNvpzRWFwqRw
stBttBThtDZqPWssPWZp
gRggwwggCGFSBBvPRpHZZrHdZLZq
ccFJGCNJmmGQzbTDhnQhBBnB
HJqMqtZbJMmJTqtLtVMqhpfphNdQfhfzzjhhlHll
rWSBrnwFwWCvwWCwBgPgCgzjQccQhhgRzcdQzjfcNfzR
CWBCwCvCvvwssWLMtJJGMdMZJsGV
nFwSFQwsNrrsssSwCrhrCNnfcCRgJRMJTJcMfRzMCMCRvW
DdbGdLZLttllWWvTzgzzgR
ZqGzPdLtDjBjDZGPZVmnhQFwqrFQhVFnss
sNNpCjttjsJjSpgpWjslCTnqqSVffrnhSfDhmhrhfm
dBwcGzbPBHbbwZcwJbcTTFDFFFDVrdVmFdnDqf
HzGcczQPHGwzPzGHRctWlvRgtvJlvNlJvRNj
cFNCFdvcCHvFBCZcwBfRSpttGhDmCghGShmSRt
QjLnTTzQVzTTnLMqhDgPhGDDSjGPrgSh
TTJGnJJlLQdNWZWJNBJZ
WHBpHcMDZHLDbHLtGCnmRmLNGmRqvsCC
PzTFzPPTJzrSbGsvnmqfsqfqRz
dJSQQdVFQgjTrjQPWcWHbBVcZVccZtWp
JDtnRtJzNzTTNlHc
rQPJFrLPGMMwrGPFwjFMVLjSTWHdWBTdSWdWZlcWTHlZ
MGFrCvLLwrwPFVVhvLMGGtnqfsmRJgDnqbRgfbqmfC
jnTtFjcSSvctJjznzvFmpqqPMqQDRVpRqPzqQzVQ
bhHBfrWpfHsfGNllRrRCPqDCqPqq
gsGZZpbWgbwHWGNgfZNjvLSTTjtnTgjSSSSJmL
RLQNdVNnRQdQHVVLGpspNqvtsqptqpTtsp
MlRWwbRBBFMFjCTFTTFDvj
WmlWBmBwwmrndnmLRHRQ
WnftJWlfnWSHGCjWWWSCFqFGBDqBwMcDmmMmGmqD
pTNhpTrPhhhRPzbhrppLhThLgqDmwccwqPBmMMqnFBcwwmBB
ZrhQTpzdjSVVtnvZ
jgtnJtBjtlTdJBZJVQBngQGDCGWpPGCcPWCbWdWMbcpP
wHstNNttSHPDmHcMCp
rrFFSvLLNfsFtNSqrtfrhsNjjVTBVgVvnTBvTQvjTgjVZz
qhZwlqFqFwlJwrDHqHcDvgcNzv
RCCTQmjCbQTBtRTbjJRDpBrgDHPPpPDvHccDPc
VmjVWstQJhlJlGVJ
GggpGwZmgvgJMvbJFQQDbDFbBbFCQDCW
rtrLzNLtNSPnNqDSQDcQCWlqBQ
VtzdRPtztLtVRtZmmMTRwCGZpMwp
vtvqjsCqtshfjcWFHWGjGFJj
zGrnzDDMpPcTHcSTVTJP
DbDwMbZRDrZdBBnMznZMGZDfwtlgQhsqCttClsqvsLfCff
JLzLtLsrzsQdvrWRwMHwcc
qPmCTzlPjljjFTZmWwcwwvHMMRWwHvMm
PFqZnVCqTCNjCzNgQsbGBLzLQQ
CBnppDHllVpPCBshBHpjDTSmZcSrfwvmJcDDTJfw
dFRLdLFQzNSTBTSNmBJv
FzFFzRMBFWtQlPlsjjPVMnhC
CVCfwnfdVvBdBbTNTT
LNzsHPNWsDjTZqDHqT
PtLgQsGQLSzWLstPgGWcgQLSNrpplffrnrNhpVCwlGVlrwMn
jPPVqPsHffzVnHzvSgMcCJGGMSVCll
pdbpDpBLNmNNppJgcvgSllGjDSGQ
hrbBwLpjLhhhNZLhNrhZZLHzfsztFzzsrtHfRFnFfRHf
tdjBdbmSfdHBdHHmZlWjFrnlWQlqvMFvFn
pDNDJhLhPVPLLLJphJLwNcwnQTcWWTqTrqMWTZvqMrlvFM
gpVLhNwpgZJCghCLDNwphgmsBdzHHHmSstmfggzdbR
TfMpfMBVftLMDBSjWDHgzHbgwLgHHvdzggzs
QJnZcFFnZRHdHjJvwgdg
RjjRRnmNmmZNjZqZnQcVffBrWqVTqrtffTSTVV
fZTdTVcVjrjdBzdTnGtgnnGSHHNFGn
thMWPtPMslmGnWnNnS
thvbMvQMRphhLCjrzBjZVdcQfC
MpmgZFgMGdrFrBCVnJ
JsbJlTTlvLQbVffRRvBBRVjd
LWlbhHlJhLTJmmGcMMHNmNgN
bhvmhPrbhqNqQRRGzQjVvvRL
wTwBZDBTwwggfnngcDfdsVVFQCdzCzDVRsFdQs
pngWMcgzMgpZWncnMpWNrbNHrNbmHhltWlbl
nPndBjLPscWSccBVGnScsSzMdhMppMthdMgpMgrzvhhp
CCFTFDwqZqCCJmhvpDzztVzDNztp
qQFJTbRVbmCfwTwfmnnssWBGnLnWlRLSGn
JRlJDSvLRRCdvmDSvdlbZNVBSWZGNgWsZGNgZBVs
QrjPMqMnLzzjLjFnNNgBpsgtgGGGVZ
hrjrFqjqFrQfMHPhQzDvvCLJdwwwmvbJbwDH
HDGrDDDpNsGQNdZQ
jpjgtgjSjpjllfZZtZsvNdtshqqq
cbgMfjclWTJcMwjWJfpfmVPLPBnVBHnmVbnmLBbD
rPrMZNsNrsvrwqvFFFdgQWNzLJJzRW
pStppStHmcmHpgVSllVcbVbWWDdLFhdbzdRRFhJFLLRF
cltCHmCBmtSlgjpllgGvTwPZPMfZvPsCMCwZvC
FRQQMdlFMDWRFQRQMQQDWdFbSSSVJSBbJSlBVVBnPJnzJL
rsftPfhsrgwznSzzHSLgJG
fhNsjrjhvsTTvdjcCRMRMRPcCW
tRtJttHFrjtDQHHBQMMBgMBSghhZQb
vqWPLpLvqrmPdmqwvqfmPhNBBBlSnbwbgnlnlhNSZZ
pGpdfzLLspddmqsqPvfvvPpGTVcJJCDRjHrccRtDjcRDFD
GJMHCdTMWJRhSTlhhSPllt
fVvqpfBFrqvqNzzgVDFrpDPmSVtQSlSmhjwltlRtmVhn
pzpBNDBzfDrsNsDRJJRdCssMLdLZWZ
hFfvWWvdpCwwcwFhphpcZCMmllHLfmbQlbrQLBJmGgQrQm
nVSNGjGzzSVNTsjzSJrbSrHBSHlrrmQHJB
PttTNsTRVnNjNqRnzRzRWCCCcpMCWGPMwwFZvFwW
DvZbFnDDsqDBwwRQgNBm
HhWpWWRMWChlChdHLlGlGtQtggSNPSSpgPmNgPJQtw
CMlWGMhhCVHlLCdHTHdrGHdbzjVqnzcvqqjRjFbbzbFFqR
ZZgCNqqBmjZsNgZCqJgNBdrLFHbBrWlPdHWFbPnHPW
TVwTDfzDwSDzmcSTcrzdbllnHPHdFlLzbF
tvDQwVtVvDVmtRsNMgRpJg
BBpDCpNJnmnpnDDmDGGmtTzqHcGTvTTjTbGjHLVcLb
swNNhPwwHzTVwwHw
rPRlPRhSQmmBDpnNfl
pbRhffPzcPDmfcNTpVBLpBjMGBGjZLLg
ssrCsqrszgJjZMqZLQ
SzCwnsllCrssvdrvwzPmDmPPbFRbSThPTPDD
QWLfcfczQQpcDTpLPfdZRRvRRVqbFWvZbvtqvv
NsGGJBhCmNdZVqsbdssZ
rMwwwBBJMrdzPfQMpzMnLQ
rdtCQhrCtQQprtTWQCHFjPgGBPdFPgvBqRRPqB
lsVsSnVSbLmmgBcgTLTFGgvq
wTTDTszsbzMDppJrJhDQ
ZlmsGLBVCBBZFCFFHqcHVvQhqVQSSHpH
dbbTRMrRwwzDfrTbFtMvcptvHFQQpqtc
gdJTDWgfwDwTwmgmNPnNsgFBlZ
PWhWhGFzzzrLdHCPccbJQJcHPD
NRpVTpTgRWVlHJNHMcHQMb
pSRSpVSZWRSTZjgTRTWnFLdZLrrndhdzZvtvzn
LgctLgVBVLhlPjqRhBLVcVlhbDDcGnNGfwCrbNDnrGbGCJNw
HmppHMWWmQmMqZZHWQrDDfDCffJrDGJrCb
qpWZsZZZMZWTLPhTRgTtThRP
hfhQfFQWzBfhfTQdmzdLDtDjtvHLjt
qsgpcqMNRmgpqsCwpCmZDvjwvdddHZHvZDtrrd
SgmNmqScbTSJbhJQ
dvMTQvTnZJsrQdbbSvMVZMblDwlflfDGgwwHcfGjPfjjrG
FqBLpBpFpFzRzqNFmgjGlDRHcwPPGwgcgs
tLNtWshsLLqWMJhTQVVbhvJd
bgZLMZgzbbLCcPCbMZbcNMgBqSTqSWVtSzvvTTBTqBvRBW
FhQpJQnGrlhGlrnTqRtTRJqSwDwtVR
FqlnnFnqFHnGHHdNZdbZbCNMdPLMPb
HHFnbftcfnfbbTbTnHTNVZZzJlPQlFrFzVJFZsdr
mvpGCBgwqCvLCqvMQWWzsQQWlPzwzsrV
hpGSGgqSvqbHcnhfVfct
lGVrnHsGcnVHzscrlGjHcrHqqWPlJCPJClTLLqCSPLPdqS
fRbwbtMQZtMMRFMSqfJTTWCTJPJCmd
ggdvtvdbVVGnpDGg
BnBjTcbnvhjjlMnNJJfnDnQDGdNDfP
qwFqVSWwqLpWFmFVCSqFpDDCJNJRQTRfRDGPfdfDQN
zwHwWVVWWFSqqwWTLqzzztHMvBhlcghblMlcttMllh
PFFNPNPmlFllbctNLmcjBstrsVrQHJSSHHSnnB
fddfDhdwGhTWWTDMwMggssjsjndsBsjsnSrVqSVV
MCvvTWvRMwCvGPpzCcmbplpCVC
thTqlPPTNbGNhGdqRRhRrNtFWnDnvvFZDpnFvfQDZtvWvv
HcMzVcVVcHrgHzcMcmmgfQvFQnMjnWDjfnvjQFfQ
VSmHJLHBJrTrJTlT
NjnsHjLLjNRddNdBFBSR
ftsbqfDcDqsrDtqsfSVBhJVFJgdBRVFS
wvDqwtDlsDDczjzjHvLzLQQM
qDwstwDtRfpJfVhBVZBMvnlRvv
zSFzQHFWdgZBVTZhTzrp
NHdggjGjWHQFPWHNPPbpJfPDtCwCtqDqJfbt
pvnbqHvnTvlCCpjsBsMGBGWWPp
RJSJhJCRVJmJwScrhSJdfwFsBGhZBjhGFFFFGgFPGhZW
cdRrdmwtfcdSmLtcSCQlvQNqQTlqqvtTlv
rnSlSrgWjVGpTTRhSffpRd
HtgHPsNNgNHszPcTBphMdhHhBcTc
JNNbZPZZsszNmtDbPgsmJlwFvWVnCwrlmWlnjnGvCC
WrVBVgVGGQCrSTTqvVjDqDjv
FmwRRwwRQhhLFMjFMzdqSSzS
RcJtbnnLtQWrGHcfrP
vpzssjmVjVZWNZzzQwtQwccpQhgtQCct
qDdfLMnMrrTbBLqTqltlTfbGQnghgRwggGgRnhhccCJJcG
MtdLfSSSddMftlrjjzsSWVSjFvjNvs
qTRPpRPzJglzGJzpGRHWHljwDtbwffjtbhjfwNfHmwwf
SZLVdsvrrdFdBcdZvsBdDCNtbmftNwfNbNhCNvtz
MLzzddLsQRppRlQGPq
PDDpdJgtpppGgttgdGdgJFzLjVcvVnnCTrVrRPTLvwnTTC
ZSbHBsSNlZcsfNnnvRrnVjrHwvCC
NmSmsfsfhmzcDmctJW
NbrLfrrLqpqWQHtBzbFttJgcgB
CmwjPPjjjShPvljwvwwjPBFttBtcHzFJcHTRHJRRmT
CCljjDGhvPCVdVSCdPvrrNfnnQsGqMpqqMqnFW
bdPdbcDZlddsZbHjrrgrmZmCZhCGjv
BffLfLVFVMMBRfwMpfzhFGFhGWvWvrhNvvNj
RMBpRSnffBJjSbJqdPHsDcbqtl
BgwGwDDZttDDTNND
WzNNnFRWFtTFlFsh
WJjPpPqqzWRbrqnNqvVvgvvdcBwgdrVBZG
FFbMVMFPvJppgvcvrZMjHlCJWHmHHBlqhCmqChCl
RGQVdVVLnLsQnQnnqWBlBmDRBDWWlhBD
SftLzQndGfVgFfjvvM
npvLlFLTWWqdLnJCmBmmpjQjjmjB
tfgDwzwVVVVtgtrsJtrbjSPQjQmjNBCNBNhPHDHC
ggVzVtMRgzMrvJLqFnnRnnRT
gZFZssWgNZTDwHDWzsFwWDQMMpqqpBPMjFtMPSQFqqqM
vrmvhdnVvQpftStnMN
JCdLddhhdJdcCdrrmCGhlgNsWDWDwWsgwHgHLZHW
vSsSGjSPvjvRSGpFprFbqFpppRfp
ZdmlndtBZbwrwfpWFn
JmdHdBBHtgllZldBhJZldLLBjPVQTfvGPNzQQSjjzPgTGNTs
TjTjBjVrTsLRRrMBsMMgzLqGGqgQHQdCQGgpgd
nbZcmNnPNcbNftvhlhZpgQgCqdSpgCHCqPFzSH
WfcNvtmmNmQlvNcbsWWjMwMVMRDVMDJM
hHHnfZSwHDgHcfclSGSnvrnvBCvWWntvzvzbWWVq
dTJTmspFTsFdRRLvtvLzvvVqtPVtrb
VFMNFpRJNTppTpsJVRcMGgfGfgZwghgGfcGh
lLGvwsMJLCMVnTrCrVdHRd
tbzqtDNNBpNWBtqzfRrFFnrVTTdrQVSVGp
tztfzmfzzPDzgWNNBbhGMlJLsvhJJjGJhGmM
rHrVJQVQVJLggDQQLbTvdCCSTdWLLLbCbS
pNtnwPthmZGRpmPFtqbMSzqffFSdTvbSzW
GpvvshwtmwsZDljjssHjVBVj
SmhJdtJhhzQSrzVhtQbtBRNfnFNSnDNGRfFGGMgR
lHwqPjqwTjLHCWLvPpvNrNMvnNGNfNGNBffGRN
lCrPTrwpPZWlqPlqpWWqZjsmJzbzVtVhhdsJcQdddZVJ
QqpCWHdQdVQlWcQCqcfRjnZZZPDnSPqPhhqZ
tmmzgWGgwJwwStSZZDRnZssR
GbFbLLvgmMMwGgmLCrppQrWVlWrFTHHd
qdqCgSVdVSVqfwsdZhpJspZsph
RjZBbmRlrlmmJwLNNNhLpwhB
vZlRrtTZCzCMfSPT
JBjhCNwjrlJlHJJRsscZrTcvLLgTsLPP
dDztmntCSgbLgqTzgc
fGVWnSMFtVGMNNNQllBjWHJC
dSDhVVdVZtnSgHQGThQvFNQQqF
LcfLRpMpcBpbrJfsbsscBNWRNPRGHvqPdTGPGqPWFH
spmrCcrdJJpLLmcmLLLDlzSzCCjVwZVtgnSZzn
ZJtgPTHtZPZQGbtNzzprVWWVrbrpCD
BRlfcRmBhSMVBqSVfBvNWrDrjWCjjzprCDCl
LfMRSmfqfSLcnnMqVSfccnhZwJFHZFTGZQGFLggwZTZGPJ
BChWddRRRcfmDbfhDP
MgpMFFsvMfGwvLgPjQPzPPmDcztDtw
NFgJpqvpLgqFnWWVNSfnNSCR
zMMMRmMfJpfhpzQJLMVtjtjPntgBtlZlVgJP
SdNbZvZbvbHTNbZbSWTdrTVBglBDlWBjDPDgntPqBDPt
rrNcrFwNdSrfzwMzZMLQQs
JPmCSfHTGJdTCbHgpgqLgRhghhffhg
ZWSSFVSVFQghQvwpphgh
lsDtZjVMMSdCNdbGCbjb
PBQPvDvVVRvQDqLDzJTlzwjz
tGcZTcdgGcncdrFrsTjzJSJqJqqwHSzzSZwq
CgtgdFgcFCMnMgsGFGGQWPQpQTCBvbNpNVVWVT
FHVFWMHMgVhnLWWMpnppfcdZNcPplnfn
RSvSCBSqGgDRjqCpPlPpppTpPjlcNP
zRzsGgJDqJwLVWVFwM
ThhWhNthVWTWqbWbFWbTdBtWSdMlHSlGlCGCdsCMClmnSlMn
DPPpvvfDHfLgDHvzpvPDsssMsmmzsMcClScMMcGG
rgPLHHJJHgZfvvZQQZfrpfFqBBwFBTNTBNBwtthQwVqt
JJgSWDSmSDQCFrhbRLSwLS
VznqzVNsMsZLdqslbRChtbHdHRrwHb
LVfNLMsLTmWDpBpf
cbTsnNpcnnchllFQlMRgJhRP
WddmdMVSBMWSBWjwCJVCPRwPFCQRRC
DWSSfjdSrTpDnHHMbZ
fgsVqqwQQtHhCrDfJH
pvbnBZWBbvWbTdthrJbDmqrmHq
TvZSNSNNSvFMBpqpnLnTBZBFGQwlQFggcFzcVGRlswsRll
zfMcQHzPtRNvlllc
BLnMhbZMLJLNNVtCdNgZgt
BJGFpqMBhBLLMqnwBhbrbhLssjFzssjfDzsmFmjmQFQPjT
JPBJPnpBFrqBJHtjlCjHJcCthM
wQZggQWQGfZFVmmGfDRjlvvNcvlNDhcNttlctt
SfmWfwVFwVGZWQVGSTdTbBpTPSqBbLnnrn
RJqBRJbqpqqJGvqHMmcfczfcjvHQfm
llgVnSWSlsssTnlWjhTcsZMcZcZMZMcccMNmcH
TnFhhllnWCnVCTllLnhhVSrbDDPrdpjRRqjBPRBBpbFJ
mRwRRNDjNTqwDNjNnNRTsQLcQWpQWZJLlLpQWs
PMFGCSzzgbBVzCGShVQZcgWsQLLftttQtlZZ
SlGVldCGGbBPMhPCVSBrNNdjRqjDrNmDnmrRwN
mqGGqGHnqGBCMrnGCbbbLgTTFFNNghHNTj
SdRfcsDPPcDdRzWPWltSlscwTSbShhgpQhgbFLbTQFjwhN
DPWWZzzztsDDtfzlscsPdcWZnVMNVqqGJnBrVqCrMrVvZnBJ
ZgglFCrrrlrWCJswHmwRVmFSwSsP
zhzqBLcjjnpzMzjhTtcqnVGbwssVRmqbHNPbwPsVNH
ftBjzLtptRWdvZlQQZQf
nGpsMncVRMGSnfsBllZdppwrTljZrQ
gcgHmtbCthHWhwBFBWZBlWlWrd
bCDDqHhcqbbtqcqtvJMzsGRsvVfPsfnzJV
TclPvSGZsPZRjhjWDgjp
JtnwHFtJqtwfQfgWgRWhdhjtgdRM
JBwnHwgFFqVJrsGmPvNTPsVvSN
ZJnfZNnDNZJLzNntDtDNNzNWTVBPrrvRRGdBcVRfPPcvfdMr
CFgjFmggQSQQmSggVMMvRdTvBVRjrdrc
mbsqQFqFgwwmgmSbwQWWLDWzpLcLnzZzLbLL
PnwSFSLSTwbbHdtstW
RrDZVVfJNZCmDCfVDVlblZHbddtHScbWbMjt
NmzqhzCCqmzffhCCqrhhLLnPvpnTPPgpGTTBSL
ShhfLSDDFMPQddpMrDgNbjzffqqqzgcjbqZR
sCstmwJwVBtmTltVmTVbRbcbcRvqvrZvBRvZbR
VwCnwnVrrrWShWPHHDdQFL
pbpDpWjZMmFCmmmb
jTjtJLJgJncCFmnJFC
LvhvhTQhBSdRNtLNsSszlGrHSGjZDlGf
JrhvTNJJhhCrtVtcrNLwDBSBwqzDwQVbBLQS
RnCgHmHHGMdPsGMfDlDqlSQbQnQQDbzD
RdPMPsmWHmjfMffPcCWrptcprpFTFrFp)";
