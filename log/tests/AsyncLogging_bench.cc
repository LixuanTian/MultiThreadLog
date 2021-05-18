/*
 * @Author: tian ref muduo
 * @Date: 2021-05-18 17:04:21
 * @LastEditTime: 2021-05-18 20:27:26
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/tests/AsyncLogging_bench.cc
 */

#include "../AsyncLogging.h"
#include "../Logging.h"
#include "../Timestamp.h"

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>
#include <vector>
#include <memory>

off_t kRollSize = 500*1000*1000;

AsyncLogging* g_asyncLog = NULL; //全局日志变量

void asyncOutput(const char* msg, int len) {
  g_asyncLog->append(msg, len);
}

void bench() {
  int cnt = 0;
  const int kBatch = 10000;
  
  for (int i = 0; i < kBatch; ++i) {
    LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
      << " "
      << cnt;
    ++cnt;
  }
}

int main(int argc, char* argv[]) { 
  printf("pid = %d\n", getpid());

  char name[256] = { '\0' };
  strncpy(name, argv[0], sizeof name - 1);
  AsyncLogging log(::basename(name), kRollSize);
  log.start();
  g_asyncLog = &log;
  Logger::setOutput(asyncOutput);
  
  int threads = 100;
  std::vector<std::unique_ptr<Thread>> thr;
  thr.reserve(threads);
  Timestamp start = Timestamp::now();
  for (int i = 0; i < threads; ++i) {
    thr.emplace_back(new Thread(bench));
    thr.back()->start();
  }
  for (auto& v : thr) {
    v->join();
  }
  Timestamp now = Timestamp::now();
  printf("%f\n", timeDifference(now, start));
  struct timespec ts = { 0, 500*1000*1000 };
  nanosleep(&ts, NULL);
  return 0;
}
