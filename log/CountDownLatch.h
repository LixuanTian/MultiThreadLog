/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 11:15:59
 * @LastEditTime: 2021-05-15 12:34:36
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/CountDownLatch.h
 */


#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "Condition.h"
#include "Mutex.h"

/*
功能(用于线程之间的同步问题)
  到计門阀
  可用于所有子线程等待主线程发起“开始”命令才开始运行
  也可以用于主线程等待子线程初始化完毕才开始工作
*/

class CountDownLatch : noncopyable {
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_; //计数功能
};

#endif