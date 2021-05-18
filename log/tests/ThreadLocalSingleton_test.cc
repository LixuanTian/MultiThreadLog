/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 15:10:54
 * @LastEditTime: 2021-05-15 15:11:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/tests/ThreadLocalSingleton_test.cc
 */
#include "../ThreadLocalSingleton.h"
#include "../CurrentThread.h"
#include "../Thread.h"

#include <stdio.h>

class Test : noncopyable
{
 public:
  Test()
  {
    printf("tid=%d, constructing %p\n", CurrentThread::tid(), this);
  }

  ~Test()
  {
    printf("tid=%d, destructing %p %s\n", CurrentThread::tid(), this, name_.c_str());
  }

  const string& name() const { return name_; }
  void setName(const string& n) { name_ = n; }

 private:
  string name_;
};

void threadFunc(const char* changeTo)
{
  printf("tid=%d, %p name=%s\n",
         CurrentThread::tid(),
         &ThreadLocalSingleton<Test>::instance(),
         ThreadLocalSingleton<Test>::instance().name().c_str());
  ThreadLocalSingleton<Test>::instance().setName(changeTo);
  printf("tid=%d, %p name=%s\n",
         CurrentThread::tid(),
         &ThreadLocalSingleton<Test>::instance(),
         ThreadLocalSingleton<Test>::instance().name().c_str());

  // no need to manually delete it
  // ThreadLocalSingleton<Test>::destroy();
}

int main()
{
  ThreadLocalSingleton<Test>::instance().setName("main one");
  Thread t1(std::bind(threadFunc, "thread1"));
  Thread t2(std::bind(threadFunc, "thread2"));
  t1.start();
  t2.start();
  t1.join();
  printf("tid=%d, %p name=%s\n",
         CurrentThread::tid(),
         &ThreadLocalSingleton<Test>::instance(),
         ThreadLocalSingleton<Test>::instance().name().c_str());
  t2.join();

  pthread_exit(0);
}
