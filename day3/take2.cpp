#include <iostream>
#include <iomanip> // E.g., std::quoted
#include <string>
#include <string_view>
#include <sstream> // E.g., std::istringstream, std::ostringstream
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <deque>
#include <array>
#include <ranges> // E.g., std::subrange, std::view 
#include <utility> // E.g., std::pair,..
#include <algorithm> // E.g., std::find, std::all_of,...
#include <numeric> // E.g., std::accumulate
#include <limits> // E.g., std::numeric_limits
#include <chrono>
#include <cassert>
#include <bitset>
#include <cmath> // pow
#include <functional> // E.g., std::reference_wrapper
#include <iterator>

extern char const* pTest;
extern char const* pData;

namespace test {bool does_comply();}

using Integer = int;
using Result = Integer;
using Answers = std::vector<std::pair<std::string,Result>>;

using Items = std::bitset<52>;
auto to_items = [](auto s){
  std::cout << "\n" << std::string_view{s.begin(),s.end()};
  Items result{};
  for (char ch : s) {
    int index= (ch<='Z')?ch-'A'+26:ch-'a';
    result[index] = true;
    // std::cout << "\n" << ch << " " << index;
  }
  std::cout << " -> " << result.to_string();
  return result;
};

int to_priority(Items const& items) {
  int result{};
  while (!items[result]) ++result;
  std::cout << "\n" << result+1;
  return result+1;
}


namespace part1 {
  auto left_half = [](auto r){return r | std::views::take(r.size()/2);};
  auto right_half = [](auto r){return r | std::views::drop(r.size()/2);};
  int to_priority(Items const& l_items,Items const& r_items) {
    int result{};
    auto items = (l_items & r_items);
    std::cout << "\n" << l_items.to_string();
    std::cout << "\n" << r_items.to_string();
    std::cout << "\n" << items.to_string();
    return ::to_priority(items);
  }
  Result solve_for(char const* pData) {
      using namespace std::literals;
      std::cout << "\n\nPART 1";
      Result result{};
      std::string_view in{ pData };
      auto left = in | std::views::split("\n"sv) | std::views::transform(left_half) | std::views::transform(to_items);
      auto right = in | std::views::split("\n"sv) | std::views::transform(right_half) | std::views::transform(to_items);
      std::vector<int> priorities{};
      std::transform(left.begin(),left.end(),right.begin(),std::back_inserter(priorities),to_priority);
      result = std::accumulate(priorities.begin(),priorities.end(),0);
      return result;
  }
}

namespace part2 {
  Result solve_for(char const* pData) {
      using namespace std::literals;
      std::cout << "\n\nPART 2";
      Result result{};
      std::string_view in{ pData };
      auto items = in | std::views::split("\n"sv) | std::views::transform(to_items);
      auto begin = items.begin();
      auto end = std::next(begin,3);
      std::vector<int> priorities{};
      while (begin != items.end()) {
        auto sticker = Items((1L << 52)-1); // all ones
        for (auto iter=begin;iter!=end;++iter) {
          sticker &= *iter;
        }
        priorities.push_back(to_priority(sticker));
        begin = end;
        end = (begin!=items.end())?std::next(begin,3):begin;
      }
      result = std::accumulate(priorities.begin(),priorities.end(),0);
      return result;
  }
}

namespace test {
  bool does_comply() {
    return false;
  }
}

int main(int argc, char *argv[])
{
  if (argc>1 and std::string{argv[1]}=="test") {
    if (test::does_comply()) std::cout << "\nTest: You have arrived at your destination :)";
    else std::cout << "\nTest: Not there yet...";
  }
  else {
    Answers answers{};
    std::chrono::time_point<std::chrono::system_clock> start_time{};
    std::vector<std::chrono::time_point<std::chrono::system_clock>> exec_times{};
    exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 1 Test",part1::solve_for(pTest)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 1     ",part1::solve_for(pData)});
    // exec_times.push_back(std::chrono::system_clock::now());
    // answers.push_back({"Part 2 Test",part2::solve_for(pTest)});
    // exec_times.push_back(std::chrono::system_clock::now());
    answers.push_back({"Part 2     ",part2::solve_for(pData)});
    exec_times.push_back(std::chrono::system_clock::now());
    std::cout << "\n\nANSWERS";
    for (int i=0;i<answers.size();++i) {
      std::cout << "\nduration:" << std::chrono::duration_cast<std::chrono::milliseconds>(exec_times[i+1] - exec_times[i]).count() << "ms"; 
      std::cout << " answer[" << answers[i].first << "] " << answers[i].second;
    }
    std::cout << "\n";
  }
  return 0;
}

char const* pTest = R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw)";
char const* pData = R"(GbccTtTSGGbgrcWBGGrdgTnVQnCmNpCJlNnNPVfClcnN
vMzvZhzhwDLVmQnClwwNQp
FRsZFzjQFsqRzRRjDZbdtTgdHBBWGrdBdHHs
HCLTmbCLgzNBNPSSlT
JJGMWRJMrrdwWWGjGWMLRGLjBzNQsBzPPfflzDPBsBffDrQz
pwJdLMjdMddWjLtwZWMMwGtHhnvnCnhvqVFFZnvbgbqVCZ
tvMCDCSVVvDDBQFRbqWMMsWgFWgc
BLLPTpBmfLPrHLLfLsbhRqbzRRcRHgqssR
dfdNLmPTdNZmZdZPfpmTJLPPSvQjtSGVwQSDJSjSwDQBVCGw
wZWTWNFqzwZbWNpSgGMVMtTHsgGs
nlnPnPvLQjzdtsjBHBMMGSHg
LdnrrLnhRdLLmLDRPvmdQnJDJWNqcCqZJZqfFqfcfzcq
vPTbfWggzvGVqjsVqV
dDcJHZcZHmMFQQMshsjcRqVChjNtqh
dDHJDmFnrJmQFnBdMdQHJdZZlWTTPPjTLWbTzLWlTTfwjzBT
VfmDHDfZzfLcZLLLHBFQtRJTcdjgdTgFjjtR
WRPhMlGSshPRGgvFMtddTjjCQt
SPhWPsbNWShsWllswGpzLmzfZwmZfqLVRrDBZB
MtZgRgJJbbGjgDDgbjRjRbGcNdWwncBFdLBBMhFBQwnWnw
vlpsNVsCzzfHpvTpzlSSSvppcQdfnwnWhndWndwfQLhnhcFQ
pCsCCTHVvSzvPvHvzpPVTVHHRJJDgJZjJjqNmjZmDtJRZPNR
GTGTbhhPjJJjBhhZsGzmfHSNsmHHgSdL
FcFpMDFDRFfsRHSjmLjR
CpFjqcCjwjnpwhTPTWBQZZTb
zdzzwDlnTDQQQQnqQqqsFqnrSBSprbpjNJJBJPPdpJfbZb
hgMcRVGMtHgRcNSPrpfgfjJpBp
CLRHVHhtvtvGPWFFDqPDLnqP
jssjjjHCSGCMNJTWWCJT
DvcvBtVrrDSNWcMfLRfM
ppDZSpBhBvBmvDHnFsHHPnGPGbQp
VVVLsLWnjVVBGgScjtBjjLLgmJdPmJmfmhmGmmmNmJGhPNJP
QbqlZqQTZvMHmshvFhPfffFp
CsTRrQrwbCbrZqQTQlRjtDBSBtwBjgVWLBtgBt
FLsSFRTPscHZmGRGGc
npNNptgttCNpgLbnQMgnQnMNqVhGqZrmrmmqrmcrqmCVZwqH
WMWbtpjLgnLNvSfPPzjvjPdv
FcFFhZlhlMrHlSFSrHZMJZSVmmLmVLLCsBtLBCzCBVDRcV
PMGPbndvGfGstLzDCmLB
NwjPqdvPpvgddqgwHrZhJlTlThpWJMZp
SdjStScTWTwwvwwfjRhQPQQQDlLBGpLrPrLrLc
gCqJbNsVsNMgzMJnnqzNlCLGGlZZPrLLlLLtpPDl
JsFJFMmbJqqnJbhSHdjWwjtmHWvS
zBFDGGbNzDWRbDccsWslHlWWsJcS
zMVqTwzPfVfVMwmlcZTZZlSmTlmc
MrMvMCnrMVMCPrPnDFFGgQdFgRdzznbd
rJtJnrnSShJgcCsjjNNMSSDzRmzm
HWDWPBPDBfFVBffqplvlmNlQllvzQNQqlN
FVGHHFVbwBpBPwFFGfBpHVDgrcJCbZcLdCgtcCcJLJrd
MGHGGFFqbFTGmFwLmQsQflFN
WcvBdpjhdZdNwdZwLZ
vWtgVcpvjthtNcjntDhhpSJMSqHzqTzqCVHTSqHPGT
cVHZfjfZMcrSDQMJRCBCQw
PtGddtslsWQDBdwCDDdw
WsvTFnshPTGhGhhlPNGTCnsjgFVfmgfHZfVHgcHZVVFmFV
JFFqfJBgrHBffVHlsBFqfWNgjTtztNnttWWvWNwzwt
hGZbcBcZZSQmZLQRTbvNttTzjtTvbpzv
cmmhRchPZhZSSmdmGPDDdJdBVMffHlqlslfF
qWwTNwNHMHNNMRqMdRMQQMHLmmvzrTmrzPvzJvZvZlvzjZ
FphBpnBhVBSFvLljzZPpmrPL
nGsBbssbcbdlwggdNl
RLSRTLSFFFLPSWpzzTJdzsQpbd
DvqqcwVMDDcfrrnwDcwnvCdpQQphJhJjhdhpzsJhMQ
fGcvDZffcGGZDHGrGrtJRPlPmJSlPLRgNBHg
QlFFmGQFDQrrWlRlWGrnQVCLNvvPwLCwBvCcCcJCLCCm
tHtfsjSMCNPwzvCf
MjqStqMHsMSgjShjTttgphsTlrbDWGDrGlRTlNDbQrQRWRbD
QbChcCJCbHQCjbGCjQfsdsrtTqrfTLrcFftd
DwRzVzzZnzZRwvgRhRWtqsLLWtRdqLLdqd
NMwMzBVVPPSGQhBl
ttTPHWdrJjCdjnFMtLLtLNvQltLh
pSDBwZRBBsgfDGGsGpBVMFPQQlFMFQQFQfNvLNfF
PsppZzBVzwgDwBwwgpSSBssWjqrdCnjjHdmCTHznWCJJWn
WcdHdPcdZrLPDPBQDg
pMjMMqfmJlqNflMlFNRfLBwnLzTTTDJJwDTTGTLJ
hlhbqpbNNbVVdbZtSB
RDBWGRDnzBWBJDNBttSLlclldtQQcTTLFF
rTPVjZZsCZrVhdFMcgLgwFSgQh
PsjHVffbsTCHrCvTPfDJGJHNzzNJWnnnmzDB
LQdFgTLdQjVsQFTRBjMZrmBjWGMGSW
flvJJlJpbNnppCpMGGfBBZSZRfFmGr
NNbNDNlbDpHlbDDplvzvnCbzqsQPFQsTTccsqdQqLgdHLwQw
DnGDNDTFdFwDzCZZRmhThCRRRv
SgrPLrrLsBPbHBCmtVZVCdCcctHH
rSbgBrsqgsPppMBqfpPsLpPGlNGGDwNFNJWDldlllDwJMG
PWbvNWvpvJPnWDGqDjDczj
QwfFFVVQSMlDlQfFZhsHrBrhHHTcjnczqjzqrG
mwMSgfmDmSSFgfFNbmLpbRbJbvbRpC
lsggLLLDGldGTGBBhNTCwRwVnJnNCCnbRV
QQpWrpHtrHrpNRRJNtfbJCVR
PvQQFPzccvBglclNscls
NsszMMNGWLcWBhMF
gTtwvbqfnDTdpvqDftpnnntDZvLFQFBLmRWFRhJZJhLLBRQB
DwrpDbngprPWGllSNrSS
nCqdLPZPMMZLNvtGhRmGhGPmtW
TSrVZVSZVwFTgSVtrtchvWRRrtWtcr
gjbjBjgTjfgfVfHHppBLMnqLMDnqClsZJLLD
hrqShCPCpHHBVBGWQFVQGFGnzQDf
tgvZsbwsbcMbRsgccjDGFvGFfWJLLzFFQJ
TTZmMcgmbmWZMctbbtsHrrqqSHHrCrPBBSCPrT
HHHNZLGLpBpRSvWlGlqhPghqDGnnFr
QCNCMTJdjMjdjsQTbdQmmCQDngFqnggPFcPcnPFcDqcbDn
MNJfzNsfJdJjdzwMNjjTJttSHVStRtZVwHvWRWtZHt
DSbvDdDbbwHgCSgZPwpbPgmTTJhsTTChqTJssQssFmJJ
zzjMNNGMMRcNNhvnvFqmtJJv
WWffvlVrcGzGlcjLvfrVRLHgHgpDPSbPpwwHbWbBbPPH
FCCjjFlFtCjzlpTHtJsQTTcpTT
DWLhWSgDWWdSWLwmmpHHQTHcBTBvvwHvHl
mgGRhrLLgWqnjrfCNlzP
cLsslBlsqNNTHlTVNbLZZLRCQbZZdQdpbP
JGfJhhwfwBBSJPRdZddpZRQbfR
hWmWGgDhJrFhBcWsssWHvHll
lmmvlJFtMHFtQzVSRbPGzLJRgG
BcTcrNBrrwwqDBqNqwcrhLpLPVzRhPRPPPgSGVPLbS
TrDqcnsTcsmnvHtdGtMW
CcnDQpSDcnFcPBrmbPQGBsGB
gCtCfRZTBWbjPRbr
qgvHqgJhMfZTtvHgfTghJgMJDpdppFSLLCcSDvLLcdDwvLcw
ffFgGRMWSTGcnDgllDDpDp
dvSdHBrVSLNVLjdlsllcsDqpsZ
SHHHNrLJLvtNQJVvmMfGRGGRCJWJRwzWMh
JNpNDfDBDHVzwHHzpzBWVBPsvsFNCbmbqsFFNsjCmvsmNC
rnnrtLhnrrQZMvtFbWmqtllcFb
GQRdGQLLhMSQhZLZdgdwwHzPDzSVWzVDwJDVpz
LdcGjgdcrMDSFGVfnnGG
HNsCCQFCPvFFBJnnSBJVfDVJwf
HHFRqHPpNppmQPcpLjzrdgtbgztT
GlZZbclGZsDvlGhsShRnCnMQtjtQjnCQsQRM
PggFVcdFNFNNVVFLPdPdrwpWBMMnqMpnttJMnjMnQqtqQtqq
FdgcdcLwfTmSGTmhlbzG
RGvhGrLhhRhlpChZrGSprBdPPHJJSBgSSHqBWBBffH
mQmjmwtTMTVLzHnTPWffPHHJBf
jMmmwMcVcFLFrlRshZbCrF
SnNgNgBlNZSZdZtMrlnSnnQtjpwFwpvFJwFqpwSbqjjqGRpv
CLCcWHLhLTzsDPcCWMLGpFJbGFwsbvGwJwjpRv
zMCTPhmHWzfhQQmndNllNrQg
dbdBdZrQsrdrGslrrSpLvwHmlTmmwScTHv
FgnJqLDLWqNnNpppmpCSSmCJTw
NNhhnRNfzMhgnMDFfGdGLbBVVdQRtPVZGt
BBQJNTTzTcfRhtjhffqDDWCC
vZnsLsVLSvPwPFFnwPlSPgZWqGjChgWCCWWCMCgGMh
srLLnLmlPwrrPwmwwvlRqzQRJBmppQTTQpTdBN
PLDpZGpWbNGWLDfQmsQDwwsmhm
vTzMMbgCfgHQsmQt
VMlRznlzVnTcFzbMcrpJcNrJdjdpZrLdcZ
SftvFcDSvDHsFtctMSvbdjbpqpRRpRTJrMdrrb
QzQZWZnQgQZwBBwsJdqPjdjrnTpJjs
NGmwmgszhZwwGGgZGmggWLVSVHlNVVtDcDltFVVVlVHt
WCfFBfBHHjHHjgHBjJFVcVRwQMbVrRhrJbRRJM
sZMsDqzZPRrRrVswdc
vvTzDzpDTvpDvZPvSnNZZlSHMCHjjFtWmlttlCjmCF
hJZwhrvhBJRrPQPwRRZLllgLqfcqpTggpcTWMTff
HHDzMztbVgTzNpgf
nGbmtjDMFjDjCHbbbHHHdBQQPBrZvJQRwvwRPZQJGs
NNSrMSHRqWpWNNrNMvLffTBBDmsvcmcJLM
lwPPhcddcGPlBDTDlmDvJJsv
bZhzCdPGGFzVVPwVwbNtcqHrpnpZptSZqRrt
GvvSWhmhWBNcBDNc
FzlRRTljjRTjRRmZfbflRTlFFrrMrcBcDVqBVsNDDJsMFr
bttRfzfRHzjlmlnCbTtzbRShgwHGGvppLdpvwLLGLLhd
MHGMWdBFFNsFFHpWSFddMmqVmVBggmlVfbVffjgZml
hcJsTTscvsLDzDJmqVgfqbqnbmfJbJ
PvRTzsPwLcwCprSdwdNW
qfJnJdLpJzrcqCrCzcGfpRSSVBPRSjSSllTNRBdTRS
DbsbtggsbbsghhgvnWWSlVjPSjmmPBtjPNlTmS
vHHHHDHvZHQvWbWsZDgWhDwWzpfGfzfcpFJzczwFJrfffnGC
sQvsRQsFZvfpGhjhQqjpZvjGJHgngPBNHnCBJBCmSBmBNG
HTHwbtdTDDnCTPTT
zwMlVdzbzLzMWvQZRQZfZZlHsR
QhzWwRBPHgFrWWrH
SDgJCCDCsVpMMqTtFpfpqG
gJNCCddSZNSlljQzPPNBzR
dLzVVjfLGCCdRPrdmBtwWttr
NnbNsbTHJnbHbSHlNQsNtwrJRwBMMBhrPJWZRRtM
QSslpFvpSSsQPFCDqqgzcjCj
fcpGshsfNcNZsmRjNqCtnFgbCgHrrggmrn
QvzBlBBQBdJTBzBwVVMgbrwwLFtLtgLFHCHrbF
dlQQMBSSTPZfPcfssZNC
gNGVMzVpVVTdPDWdRdNT
BfjbnCBjBzffHrbrzBDddQWTZZQTTJTQTHHS
zrFncfBjcjnrrlCLwFgpmvFmwGmVLh
MbngccTfWgbWcTTzZghmLshhLRttpthRDLtf
CdFdJHCJjBvBSCNCNJBjjdjpsPDwDtwvptRPmLmzRwhhLR
qJSCCBFHQBFFldrVZZbggnGTzcZQ
lPrpppllcwwpHprppNdfLbQJnWdLJnncdN
tSjjjSSDGgghRbbSTfTbTFTLQn
jCBgDMbBMGghZzCZmmlrrpwp
FhCDFvvPwCjcLhDjhnvjnsdfZTlflQlflLsppdQfld
zPNSmmHrSSHWBNSMMVGzfGfZTZQZzdpdRGZR
WSNVVMMVtHSVbMNWBHqmwcvhcgwgvwtPvgtPDjtw
jSSSjzZMmgSzzwmZBtHcHmtNdncHtnpNcn
VsLsRsJJsTfRVfLRLJlfLlWqNbDcddncvpvbdvcnpqdpdtHq
GQQTsJGGJLlRGJFWffWLhgZwrZBZFZrhtBjrjjZw
ZfzJPvPnLvRJRfZLDfjfrBcqrgsgDBrcrGgslsms
SNhpqSNhpVTNQSMNgWmrlccVGBBmwrBw
QHHFhhNdTNHHfZPFqtqPRtjq
PMZSPSZZGMspsLhLRqRVzfGjvF
tcwwgcgbcbCrtbbtmQQcCqRffFLhRgqjFjRfhFqhqz
CLQtcbcmwmbdrbBrlrCwQTsTPsPsZNBPNWJpZWpTss
lMTrcHrhChWnRzJrznnr
DDJbPwjLJpfBQjPVBpbsGVGVWnZnsqnZsnzsqZ
wJQLbpPJDLfgPbDNCHNlghNCMhcNcl
tlVZhlVWtnBltVtssZBBbPbcpdPwbPWfvcbLvbbb
NwCTFNFDNdSNPpLpfN
CGwRjwDjzFFGRGjjFRjlBtZqMzVVtVqhMZMBZn
HhFdMFHhgrdjcZtZjr
zvvQQvzwzDMjZTjtcrTDtt
BwMwSvQSVlzQlMQzwzNgGHPGGHFCCCgGhsHLCS
zMVtBhhVhhDhtzBtMTTfDrPbmRRmPbQmrQbNQGRQtR
vLlJHgnLpDvHHvHvmPbSQbQRGmJmPRrb
pspwHClCwqplsHqDsMMTFWsWfjzszf
CmmjLwWSWGCHCjwSmStJBgQcccBhwgQtgthQ
WZVFTpqWsMsZpFddzszbVzJQBnRtrQthchdBgtgRtdrc
bbsMTWsMVsZqNZMpqWDqbMsCjvlCfjGCPlLLPLCmSCfCLN
VzsjjVGhpjJrJHCppprt
WtMnqtWdSQDtMRSnLNHHwHwQvrJrJCPN
tTtWSScTddBqdRMmlsbFBfhVBhfjjF
gLMWzdTgLFQHdlMgMRwcwhqqvPcPhVFRDF
tBnGrSCZNZCrtGBsSNGtBPhcgfchqqDPwVPRvNRqwN
CmrgstjZngtBzbjJlQWWHjjM
qttwGWHtVPzJJPqbmb
NrRvfTTghNrpLrrpLTrNrRrhvJmzmzlbbVVbdbdZlDdvzMPb
rfcprNcfgpLrVNnnCcnnscstFGCF
ZZhTfggZsbshGrfshMrNMCSRMMWqCqMNRq
TVTJPDTFccqMCcJw
BBLBmLTLDHFvsQpfgnZhbQvG
ffSrFvVVmVCQSfVDFzDvDDmmnGWCRqGRWNNqlttnRsNtGnWW
PgZQgPJJpTpTHRGtNRGWqZMWWR
wdTdwgbPJTJgTgLSFBbrQvSrFrVS
ppssshsscCVCHhVWVpznnQRBnZnBbzczFPRS
dqqfJGWttfWGlwwPSbFbZnRFPFtFZS
wGwdGdddLfGgMTJfwLMlJMpTCChjHhjTHpjjhmsDHmHW
PZQBhRPQBQrWHFHqHFHCqh
STQSvvvppzSVHJJFWjHC
TTgTvbsbszcNnnvbncvRGPBRtRgQrDPLfftPPR
dMltttpQhmQVZdmhsdrvNCHvlWbHWvHCWrlr
PzzLTGpGPDzFBzqFGFqFvHWHvRJbbrbWvCvjJCLv
BpqTBzpzfGGTTPZtSddtQmVffSst
bwHbRZldhQQfDWWGDjBf
CzvgpsNMsvCvFvpszpnMsFgBTDBDWPnPVJJZZDJnfjDTff
CpcLsFrMZbhRcdmt
HgjpWlhzpWjhWTQPFdPBRQzTMQ
JsfwrqLttwJVLGhRQGGPBd
CttfrqDmDDtCsbZCHjhZHSHNlgcW
QSdCWlCRhWRdlrlZrDssZsGDbv
pjPrpjqFNrZNGnBbsNDG
wjjVHjfLQRCgdLrC
PjMpRdBdjMSGsjpdprqtwCrNGrrNlthhrG
WQzDzLZDgzZcqlqqrtJclJnh
zbWHQHDfDWZHfLZHfffWVZpRSPpdVvBSPMqVMPjdvspS
TMBJLTJlFHBjFFtMGngpvvpgvQmtNSNngv
bVhsZswRCbbVZWVfVZwVSpmSQPPvNHwPHmgmSSNN
dCCVZZcbWVVcCbbfsLrdjFMJdDDHBTMrjr
vNWcTWnCqNCPPjhhHsQrfgszrTJRQsfRQD
wLdwMBLFBBQJpszJBqzB
lLwVmMSmttVMlSNqcbcbSbNcvHbh
PVfJfDWrPVPPLcPPFWcjPrqlqqQsljRpplqBQpRvSQvs
NdggMTCChMgdChNmdtTbtmsQSRhQslhlpFRpFwllwQvw
CdnGzbGbgMGMdTCZZDDJcZFDDnWrPH
fsshhnfLZSvcVbdcZVJj
RCCSmDFFpRqHQDgWvbGjgjDdbG
pFmFtCSBCSMBBLwrPsBPNlNB
fWWcwbbwbWfGCPgPfvbwgvgcQQqQLsGLJQTZHHrZRsrLqlJs
VnszmsDBpMFpzNFlrlRLRRHZqRHr
VpdzDMNzNDjpsdzdnzDcPCwtWCjhbthvfgtgwP
SPQtSWDLLltQQctHLSBSWHlWgFwhMRsgwggrFJPgdgwwGJhJ
nCqmfVqfVjTznCMhsGRRRgGFMffw
mCnNTVzVvjmqNtlDtbttDlBM
LjctjtppFWmgthgs
nBrNvzTqlDJlbbZgvmhMZVZb
JJrnTrrgGDqDPwwSPHPpfjRdPc
sTQmCmmVqmJHSTjGdMMfMNNvNHvc
rFbzlLLWWPzwlWrlbwzrWbRvdfFjdjpvjfFNNMccphCvhN
rrWzrwzPBBBCZTJgZg
gffvjftWddzZtbvdNvgZLwBBMJLSWMDMDDBRWRmS
PqPqpqcCnCpVqlClTQQmPMDwPRJJBLLLhS
VCGqlHmmHsjtHNsZ
mmMlVllWmhmmBzzLGMWlBmpstptPRRZpPMFJSpRsFRFs
DgjnndQcNTCCCDNcdSRSdtZPfwfwJSJJ
gQjCQqQjHNnjDCgHNcZGZLhHzrLVLGzlrGmb
JfwfJpBgJSMphZqtqDDG
QcQrssrGCcMCVcMc
RnljPRnPjWbGRbjnjbvmSzwHfHgwfJHzdLFSwBFW
sBjbHCBCnjvsJCHBsbvwwJGfRNFFFfFGTcrVFffNHRTP
zDqdpqMgMtgzthgDtQmzGPTVSTVrVGTFSVFFqNRF
zDLdphmmLhMhDhQdlzgLLbjCnWswWWlrZJBswCJJZl
FMNrQFgrVwmrpJMwMTMPflbsHPTtlSbftSjCbC
zGnGnhnGzDqRLnZLHNHbbbHDlltNSjCl
nzddcRzzBnRRvRRhvnQFpmpgggJVcVmMQmgN
CCpMlhwwpJpdBlsdcjvtZDFrtmRqmDrsmv
gzVPbjSPfSPTTTPnWVSbbvDQZZDZrFWDvFDvmvQQmZ
LTbLTPgnTzLVPNNGnNTgVNPlGdHHCphMwHMwjMphlpjccl
rMMrqcrmJqJqmCsTPWWGGPzPlPPrGL
fnwqwwZwRnVlWWnzWBWlDP
jvVHvqfpJmhtHJtH
NLMVQjRNTJCTJtZTJc
DlGlGHvFHGDgcFCtfhCJFtZc
DBHGGGSDvGDPHWBGdBbSvgWDNVMjLLRnmNmjPLNPNcRQVnjj
tsGdTJdJtNllzjGRzm
HZvvDLLWqbBBMRMRNjVhHRmn
vZDCvqqgBDZZjbZDrWqBvpdpFpcdpCJcPTSJJtptpP)";
