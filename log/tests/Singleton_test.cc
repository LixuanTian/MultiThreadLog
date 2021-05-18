/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 14:32:16
 * @LastEditTime: 2021-05-15 14:32:31
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/tests/Singleton_test.cc
 */
#include "../Singleton.h"
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

class TestNoDestroy : noncopyable
{
 public:
  // Tag member for Singleton<T>
  void no_destroy();

  TestNoDestroy()
  {
    printf("tid=%d, constructing TestNoDestroy %p\n", CurrentThread::tid(), this);
  }

  ~TestNoDestroy()
  {
    printf("tid=%d, destructing TestNoDestroy %p\n", CurrentThread::tid(), this);
  }
};

void threadFunc()
{
  printf("tid=%d, %p name=%s\n",
         CurrentThread::tid(),
         &Singleton<Test>::instance(),
         Singleton<Test>::instance().name().c_str());
  Singleton<Test>::instance().setName("only one, changed");
}

int main()
{
  Singleton<Test>::instance().setName("only one");
  Thread t1(threadFunc);
  t1.start();
  t1.join();
  printf("tid=%d, %p name=%s\n",
         CurrentThread::tid(),
         &Singleton<Test>::instance(),
         Singleton<Test>::instance().name().c_str());
  Singleton<TestNoDestroy>::instance();
  printf("with valgrind, you should see %zd-byte memory leak.\n", sizeof(TestNoDestroy));
}
