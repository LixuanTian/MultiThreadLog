/*
 * @Author: tian ref muduo
 * @Date: 2021-05-17 09:17:45
 * @LastEditTime: 2021-05-17 09:21:23
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/ProcessInfo.h
 */

#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include "StringPiece.h"
#include "Types.h"
#include "Timestamp.h"
#include <vector>
#include <sys/types.h>

//实现读取进程信息的函数
/*
/proc文件系统：包含了各种用于展示内核信息的文件，并允许进程通过常规I/O系统读取，
  需要注意的是/proc下保存的文件与子目录并非存储与磁盘，而是由内核在进程访问此类信息时动态创建的。
/proc/PID：对于系统中每个进程，内核提供了相应的目录，命名为/proc/PID，其中PID是进程ID。
  此目录下的文件与子目录包含了进程的相关信息。
/proc/PID/fd：为进程打开的每个文件描述符包含了一个符号连接，每个符号连接的名称与描述符的数值相匹配。
/proc/PID/task：针对进程中每个线程，内核提供/proc/PID/task/TID命名的子目录（TID是线程id）。
*/

  pid_t pid(); //返回进程的ID
  string pidString(); //将进程ID转为字符串
  uid_t uid(); //返回用户ID 
  string username(); //返回用户名
  uid_t euid(); //返回有效用户ID 
  Timestamp startTime(); //时间戳
  int clockTicksPerSecond();
  int pageSize();
  bool isDebugBuild();  // constexpr

  string hostname();
  string procname();
  StringPiece procname(const string& stat);

  /// read /proc/self/status
  string procStatus(); //读取进程状态

  /// read /proc/self/stat
  string procStat();

  /// read /proc/self/task/tid/stat
  string threadStat();

  /// readlink /proc/self/exe
  string exePath();

  int openedFiles(); //统计进程打开的文件数目
  int maxOpenFiles(); //最大的打开文件数目

  struct CpuTime //统计进程的cpu使用时间
  {
    double userSeconds; //用户态时间
    double systemSeconds; //内核态时间

    CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }

    double total() const { return userSeconds + systemSeconds; }
  };
  CpuTime cpuTime(); //返回进程使用CPU的时间

  int numThreads();
  std::vector<pid_t> threads(); //返回进程中线程的信息（pid_t）

#endif  
