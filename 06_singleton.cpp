#include <iostream>
#include <thread>

using std::cout;
using std::thread;
using std::mutex;
using std::unique_lock;

mutex sgtMutex; // 13. 考虑上锁
std::once_flag sgtFlag; // 16. 使用call_once的方法

// 单例模式 -- 某个类能且只能创建一个实例对象
// 单例类示例
class mySingtn{
public:
    static mySingtn* getInstance(){ // 3. 静态成员函数返回静态成员变量
        if(!instance){ // 15. 双重锁定 -- 只会成功进入并new一次
            // 没进入，说明一定已经new过了，但进入表示有可能没new过，等待以后重新判断，这样就不会重复new
            unique_lock<mutex> uLock(sgtMutex); // 14. 考虑加锁 -- 会导致效率低 -- 考虑双重锁定
            if(!instance){ // 5. 返回静态成员变量，如果是空，就创建成员变量 -- 内部调用私有的构造函数
                instance = new mySingtn();
                static TrashRecicle tr; // 8. 在第一次生成单例对象时，生成一个静态的垃圾回收对象
            } 
        }
        return instance;
    }

    class TrashRecicle{ // 7. 定义一个类用来垃圾回收
    public:
        ~TrashRecicle(){ // 9. 静态对象被销毁时，调用其析构函数
            if(mySingtn::instance){ // 10. 嵌套类可以访问私有成员
                delete mySingtn::instance; // 11. 释放内存
                mySingtn::instance = NULL; // 12. 指针置空
            }
        }
    };

    void print(){
        cout << "my singleton\n";
    }

private:
    mySingtn(){} // 1. 构造函数私有

    static mySingtn* instance; // 2. 私有的静态成员变量
};

mySingtn* mySingtn::instance = NULL; // 4. 静态变量需要在类外初始化

// 线程函数
void threadFunc(){
    mySingtn* pMs = mySingtn::getInstance(); // 6. 在使用时，定义一个指针获得静态成员对象
    pMs->print();
}

int main(){
    thread t1(threadFunc);
    thread t2(threadFunc);
    t1.join();
    t2.join();

    // 二、多线程使用单例模式
        // 最好是在主线程中创建单例模式
        // 如果是在线程中创建，则要考虑上锁的问题
        // 双重锁定问题
    return 0;
}