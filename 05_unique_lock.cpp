#include <iostream>
#include <thread>
#include <list>

using namespace std;

class Server{
public:
    void MsgIn(){
        for(int i = 0; i < 100000; ++i){
            cout << "Msg In :" << i << endl;
            lock_guard<mutex> ServLg(ServMtx);
            msgRcvQueue.push_back(i);
        }
    }

    bool getFromQ(int &commond){
        lock_guard<mutex> ServLg(ServMtx);
        bool get = !msgRcvQueue.empty();
        if(get){
            commond = msgRcvQueue.front();
            msgRcvQueue.pop_front();
        }
        return get;
    }

    void MsgOut(){
        for(int i = 0; i < 100000; ++i){
            int commond;
            bool get = getFromQ(commond);
            if(get){
                cout << "Msg out" << commond << endl;
            }
            else{
                cout << "empty list, cicle " << i << endl;
            }
        }
    }


private:
    mutex ServMtx;
    list<int> msgRcvQueue;
};


int main()
{
    // 1. unique_lock取代lock_guard
        // unique_lock 比 lock_guard更灵活
        // lock_guard<mutex> l1(m1); // 原先的
        // unique_lock<mutex> u1(m1); // 替换的
    
    // 2. unique_lock 的第二个参数 
        // unique_lock<mutex> u1(m1, ...) 可以有三种
        // （1）adopt_lock -- 表示已经锁住了
                // 前提是已经lock了
        // （2）try_to_lock -- 如果没拿到锁也不会死卡住
            /* {
                unique_lock<mutex> u1(m1, try_lock);
                if(u1.owns_lock()){ // 拿到锁了
                    // 操作共享数据
                }else{ // 没拿到锁 -- 不至于干等
                    // 做点别的事情
                }
            } */
                // 前提是不能lock
        // （3）defer_lock -- 初始化时，不lock互斥量
                // 前提是不能lock

    // 3. unique_lock 成员函数
        // （1）lock()
                // 用了defer_lock后，可以主动锁住
        // （2）unlock()
                // 交替处理共享和非共享
            /* {
                unique_lock<mutex> u1(m1);
                // 处理共享数据
                u1.unlock(); // 解锁
                // 处理非共享数据
                u1.lock(); // 上锁
                // 处理共享数据
            } */ // 自动解锁
        // （3）try_lock()
                // 返回是否锁成功
        // （4）release()
                // 断开unique_lock 和 内部 mutex的关系，返回该mutex对象指针
                // 如果没解锁，有责任解锁
            /* {
                unique_lock<mutex> u1(m1);
                mutex* pm1 = u1.release(); // 释放了管理该mutex的权利
                pm1->unlock(); // 有责任解锁
            } */
    
    // 锁的粒度 -- 锁了的代码量

    // 4. unique_lock 所有权传递 -- 移动构造
            /* {
                unique_lock<mutex> u1(m1);
                unique_lock<mutex> u2(std::move(u1)); // 移动构造
            } */

            /* unique_lock<mutex> rtn_unq_lock(){
                unique_lock<mutex> temp(m1);
                return temp; // 返回局部变量，移动构造
            } */
            

    Server myServer;
    thread t1(&Server::MsgIn, &myServer);
    thread t2(&Server::MsgOut, &myServer);

    t1.join();
    t2.join();

    cout << "main thread\n";
    return 0;
}