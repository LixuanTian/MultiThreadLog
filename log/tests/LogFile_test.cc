/*
 * @Author: tian ref muduo
 * @Date: 2021-05-18 11:21:06
 * @LastEditTime: 2021-05-18 11:31:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/tests/LogFile_test.cc
 */
#include "../LogFile.h"
#include "../Logging.h"
#include "../Timestamp.h"

#include <unistd.h>

std::unique_ptr<LogFile> g_logFile;

void outputFunc(const char* msg, int len)
{
  g_logFile->append(msg, len);
}

void flushFunc()
{
  g_logFile->flush();
}

int main(int argc, char* argv[])
{
  char name[256] = { '\0' };
  strncpy(name, argv[0], sizeof name - 1);
  g_logFile.reset(new LogFile(::basename(name), 200*1000));
  Logger::setOutput(outputFunc);
  Logger::setFlush(flushFunc);

  string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
  Timestamp start = Timestamp::now();
  for (int i = 0; i < 10000; ++i)
  {
    LOG_INFO << line << i;

    usleep(1000);
  }
  Timestamp now = Timestamp::now();
  printf("%f\n", timeDifference(now, start));
  return 0;
}
