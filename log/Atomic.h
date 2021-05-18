/*
 * @Author: Tian reference muduo
 * @Date: 2021-05-14 10:19:46
 * @LastEditTime: 2021-05-15 12:36:08
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Atomic.h
 */

#ifndef ATOMIC_H
#define ATOMIC_H

#include "noncopyable.h"
#include <stdint.h>

/*
借助于GCC提供的原子操作函数组来实现了原子操作
*/

template<typename T>
class AtomicIntegerT : noncopyable {
public:
    AtomicIntegerT() : value_(0) {}
    
    T get() { //返回当前值
        // in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    T getAndAdd(T x) { //先返回当前值，然后再加上增量
        // in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
        return __sync_fetch_and_add(&value_, x);
    }

    T addAndGet(T x) { //先将当当前值加上增量，然后再返回
        return getAndAdd(x) + x;
    }

    T incrementAndGet() { //加一
        return addAndGet(1);
    }

    T decrementAndGet() { //减一
        return addAndGet(-1);
    }

    void add(T x) {
        getAndAdd(x);
    }

    void increment() {
        incrementAndGet();
    }

    void decrement() {
        decrementAndGet();
    }

    T getAndSet(T newValue) { //先获取当前值，然后再将其设置为新的值
        // in gcc >= 4.7: __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST)
        return __sync_lock_test_and_set(&value_, newValue);
    }

private:
    volatile T value_;
};

//定义两个常用的原子类型
typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

#endif
