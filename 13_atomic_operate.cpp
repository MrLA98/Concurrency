#include <iostream>
#include <thread>
#include <ctime>

using std::cout;
using std::endl;

//int g_count = 0; // 1、2的全局变量
//std::mutex mut;  // 2 中的锁
std::atomic<int> g_count = 0; // 3的原子变量
std::atomic<bool> aflag = false;

void thread_func(){
    // 1. 原始方法 -- 60ms
    /*
    for(int i = 0; i < 10000000; ++i){
        ++g_count; // 输出的结果不是 20000000，小于该值
                   // 因为++操作的汇编被另一个线程打断了
    }*/

    // 2. 解决方法1 -- 上锁 -- 900ms
    /*
    for(int i = 0; i < 10000000; ++i){
        mut.lock(); // ++前上锁
        ++g_count;
        mut.unlock(); // ++后解锁
        // 确保了++操作不会被打断
        // 但运行速度大大减慢了 耗时变成了十倍
    }*/

    // 3. 解决方法2 -- 原子操作
    for(int i = 0; i < 10000000; ++i){
        g_count++; // 现在自加操作是原子变量
        //g_count += 1; // 原子操作
        //g_count = g_count + 1; // 不是原子操作
    }
}

void thread_circle(){
    while(aflag == false){
        cout << "thread [" << std::this_thread::get_id() <<"] is working...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 休眠一秒
    }
    cout << "thread [" << std::this_thread::get_id() <<"] ends!\n";
}

int main(){
    // 原子操作 -- 多线程中不会被打断的程序片段
        // 某个操作要么是完成状态，要么是未完成状态，不会出现中间状态
        // 即使其汇编语句有很多行，也要保证要么一行没执行，要么全部执行完成，中间不能被打断
    // 无锁技术 -- 比互斥量效率更高
        // 互斥量 -- 加锁大段代码
        // 原子操作 -- 把对某个变量的操作变成不可分割的操作
    // std::atomic<int> val = 0; 
        // val就是一个原子变量，他的操作都是原子操作

    // 案例一、对整型变量操作
    clock_t start = clock();
    std::thread t1(thread_func);
    std::thread t2(thread_func);
    t1.join();
    t2.join();
    clock_t end = clock();
    cout << "g_count = " << g_count << endl; // 输出的结果不是
    cout << "spent :" << (1000.0 * (end-start)) / CLOCKS_PER_SEC << "ms\n";

    // 案例二、原子变量控制线程结束
    std::thread t3(thread_circle);
    std::thread t4(thread_circle);
    std::this_thread::sleep_for(std::chrono::seconds(7)); // 主线程休眠7秒
    aflag = true; // 原子操作
    t3.join();
    t4.join();

    // 可以看出，原子操作的读写不需要锁保护，因为不会被打断
    // 但由于功能较小，一般用于多线程统计之类的辅助性工作
    
    return 0;
}