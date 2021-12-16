#include <iostream>
#include <thread>

using std::cout;
using std::thread;
using std::mutex;
using std::unique_lock;

std::once_flag sgtFlag;

class Singtn{
public:
    static Singtn* getInstance(){ // 单例接口
        std::call_once(sgtFlag, createInstance); // createInstance函数只执行一次
        return instance;
    }

    void print(){ // 成员函数
        cout << "my singleton!\n";
    }

    class TrashRecicle{ // 垃圾回收
    public:
        ~TrashRecicle(){
            if(Singtn::instance){
                delete Singtn::instance;
                Singtn::instance = NULL;
            }
        }
    };

private:
    static Singtn* instance; // 单例指针

    Singtn(){}

    static void createInstance(){ // 单例初始化
        instance = new Singtn();
        static TrashRecicle tr;
    }
};

// 指针初始化
Singtn* Singtn::instance = NULL; // 一定要记得在类外初始化静态成员变量

void threadFunc(){
    cout << "thread begins\n";
    Singtn* pSgt = Singtn::getInstance();
    pSgt->print();
    cout << "thread ends\n";
}

int main(){
    // 三、std::call_once() -- 可以取代双重锁定 -- 但效率会差一点
        // 保证一个函数植被执行一次 -- 等同于互斥量，效率上比互斥量更高
        // 需要与一个标记结合使用 -- std::once_flag
        // 成功调用函数以后，once_flag会被置为标记，标记过以后就不能再调用这个函数了
        // 两个线程到达call_once后，只有一个线程进入执行对应的函数，
            // 另一个会等待其执行完毕，并且执行完毕后不再执行该函数

    thread t1(threadFunc);
    thread t2(threadFunc);
    t1.join();
    t2.join();

    return 0;
}