#include <list>
#include <iostream>
#include <thread>

using namespace std;

class server{
public:
    void getMsg(){
        for(int i = 0; i < 1000000; ++i){
            cout << "get msg" << i << endl;
            msgRcvQueue.push_back(i);
        }
    }

    void outMsg(){
        for(int i = 0; i < 100000; ++i){
            if(msgRcvQueue.empty()){
                cout << "circle : "<< i <<"empty msgRcvQueue!\n";
            }else{
                int commond = msgRcvQueue.front();
                msgRcvQueue.pop_front();
                cout << "out msg : "<< i<<endl;
            }
        }
    }

private:
    list<int> msgRcvQueue; // 共享数据
};

int main(){
    server myserver;
    thread myINserver(&server::getMsg, &myserver);
    thread myOUTserver(&server::outMsg, &myserver);
    // 解决共享数据的条件竞争问题 -- 需要c++的 “互斥量”
        // 某个线程操作共享数据时，用互斥量将其锁住，其他线程需要等其操作完成才能操作共享数据

    myINserver.join();
    myOUTserver.join();

    return 0;
}
