#include <list>
#include <iostream>
#include <thread>
#include <mutex> // 互斥量

using namespace std;

class server
{
public:
    void getMsg()
    {
        for (int i = 0; i < 1000000; ++i)
        {
            cout << "msg in:" << i << endl;
            // myMutex.lock(); // 上锁 -- 原始版本
            lock_guard<mutex> myGuard(myMutex); // 上锁+自动解锁 -- 取代版本
            // 如果两个锁
            // std::lock(myMutex1, myMutex2); // 一次锁两个，但是需要两个mutex的mutex.unlock()
            msgRcvQueue.push_back(i);           // 操作共享数据
            // myMutex.unlock(); // 解锁 -- 原始版本
        }
    }

    bool out(int &commond)
    {
        // myMutex.lock(); // 上锁 -- 原始版本
        lock_guard<mutex> myGuard(myMutex); // 上锁+自动解锁 -- 取代版本
        bool get = !msgRcvQueue.empty();    // 操作共享数据
        if (get)
        {
            commond = msgRcvQueue.front(); // 操作共享数据
            msgRcvQueue.pop_front();       // 操作共享数据
        }
        // myMutex.unlock(); // 解锁 -- 原始版本
        return get;
    }

    void outMsg()
    {
        for (int i = 0; i < 1000000; ++i)
        {
            int commond;
            bool get = out(commond);
            if (!get)
            {
                cout << "circle : " << i << "empty msgRcvQueue!\n";
            }
            else
            {
                cout << "msg out : " << commond << endl;
            }
        }
    }

private:
    list<int> msgRcvQueue; // 共享数据
    mutex myMutex;         // 互斥量
};

int main()
{
    server myserver;
    thread myINserver(&server::getMsg, &myserver);
    thread myOUTserver(&server::outMsg, &myserver);
    // 解决共享数据的条件竞争问题 -- 需要c++的 “互斥量” -- mutex
        // 某个线程操作共享数据时，用互斥量将其锁住，其他线程需要等其操作完成才能操作共享数据
        // 原则是只保护需要保护的数据 -- 不能多也不能少
        // 成员函数 lock() 和 unlock() 要成对使用

    // lock_guard 直接取代lock()和unlock()
        // 可以取代上锁，且自动解锁，但是使用lock_guard时，不能再用lock和unlock了
        // 本质是局部变量的生成和毁灭时，在构造中调用lock，析构中调用unlock，使用可以使用局部{}来控制上锁和解锁时机

    // 死锁：
        // >=两个锁在>=两个线程中不同顺序上锁，造成互相等待资源的情况
        // 解决方法：如果几个线程中使用了同样的几个锁，确保上锁顺序是相同的

    // lock函数模版 -- 用的很少
        // std::lock() 一次锁住两个及以上的互斥量 -- 至少两个
        // 如果让他锁住的几个里，有一个或多个已经被锁住，他会立刻释放掉没被锁的让别的线程用，等别的用完
        // 因此输入的互斥量一定是同时被锁住，或都没锁

    // lock_gaurd() 函数模版
        // 用 下面三行来实现： 同时锁住两个变量，且自动解锁
        // lock(m1, m2);
        // lock_gaurd<mutex> lm1(m1, adopt_lock);
        // lock_gaurd<mutex> lm2(m2, adopt_lock);
            // std::adopt_lock -> 表示已经lock了，不用再lock了

    myINserver.join();
    myOUTserver.join();

    return 0;
}
