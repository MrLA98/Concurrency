#include <iostream>
#include <thread>
#include <list>

using std::cout;
using std::endl;

class Server{
public:
    // 原先的写法
    /*
    void Msg_In(){
        for(int i = 0; i < 100000; ++i){
            {
                std::unique_lock<std::mutex> uLock(msgMut);
                msgQueue.push_back(i);
            }
            cout << "Msg In : " << i << endl;
        }
    }

    bool getMsg(int &commond){
        if(!msgQueue.empty()){ // 双重锁定 -- 效率提高但过于繁琐
            std::unique_lock<std::mutex> lg(msgMut);
            if(!msgQueue.empty()){
                commond = msgQueue.front();
                return true;
            }
        }
        return false;
    }

    void Msg_out(){
        for(int i = 0; i < 100000; ++i){
            int commond;
            if(getMsg(commond)){
                cout << "Msg out :" << commond << endl;
            }
            else{
                cout << "empty queue when i = " << i << endl;
            }
        }
    }
    */

   // 使用条件变量的写法
   void Msg_In(){
        for(int i = 0; i < 100000; ++i){
            cout << "$ Msg In ==> : " << i << endl;
            {
                std::unique_lock<std::mutex> uLock(msgMut);
                msgQueue.push_back(i);
                // 3. 该线程加入一个消息后就唤醒取消息的线程
                msgCV.notify_one(); // 只会唤醒一个wait中的线程，当多个线程在wait时，会随机唤醒一个
            }
        }
    }

    void Msg_out(){
        int commond = 0;
        while(true){
            std::unique_lock<std::mutex> uLock(msgMut);
            // 2. wait函数来判断条件
            msgCV.wait(uLock, [this]{
               return !msgQueue.empty();
            });
            // 3. 条件达成后的操作 -- 只要到这一行，条件一定成立且锁一定是锁着的
            commond = msgQueue.front();
            msgQueue.pop_front();
            uLock.unlock(); // 完事就解锁
            // 下方执行非共享数据的操作
            cout << "# <== Msg out : " << commond << endl;
        }
    }


private:
    std::list<int> msgQueue;
    std::mutex msgMut;
    std::condition_variable msgCV; // 1. 条件变量对象
};


int main(){
    // 一、条件变量 -- std::condition_variable
        // std::condition_variable msgCV; // 定义一个条件变量
        // unique_lock<mutex> uLock(m1); // 上锁
        // msgCV.wait(uLock, [lambda]); // 第二个参数为函数对象
            // 返回false，就解锁，阻塞在这里，直到别的线程调用msgCV.nitify_one()唤醒
                // 唤醒后uLock先尝试上锁--等别的解锁--直到上锁成功
                // 然后重新判断条件
            // 返回true，直接继续
        // msgCV.wait(uLock); // 一个参数，只放入unique_lock，等效于上面返回false
            // 但被唤醒后，先尝试上锁，上锁后直接继续，不再卡住

    // 二、两个大问题： "notify_one 和 wait 的配合问题"
            // msgCV.nitify_one()执行时，有可能另一个线程并没有wait
            // 也有可能有msgCV.nitify_one()的线程已经结束了，wait的线程还在卡
        // 因此目前的写法并不完美

    // 三、notify_all -- 将所有wait的部分都唤醒

    Server myServer;
    std::thread t_in(&Server::Msg_In, &myServer);
    std::thread t_out(&Server::Msg_out, &myServer);

    t_in.join();
    t_out.join();

    return 0;
}