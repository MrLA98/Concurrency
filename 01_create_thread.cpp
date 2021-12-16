#include <iostream>
#include <vector>
#include <string>
#include <thread> //头文件

using namespace std;

// 初始函数
void doSomeThing(){
    cout << "my thread start!\n";
    cout << "my thread end!\n";
}

// 类来创建线程
class DoSometing{
public:
    int mi; // 如果用引用，会出现bug
    DoSometing(int i):mi(i){
        cout<<"有参构造\n";
    }
    DoSometing(const DoSometing &ta):mi(ta.mi){
        cout << "拷贝构造\n";
    }
    void operator()(){
        //cout << "my thread start!\n";
        cout << "mi = " << mi << endl;
        //cout << "my thread end!\n";
    }
    void thread_func(){
        cout << "thread func\n";
    }
    ~DoSometing(){
        cout << "析构\n";
    }
};


int main(){
    // ---------用函数创建线程---------
    // thread myThread(doSomeThing); // 创建线程，立即开始执行

    // -------用函数对象来创建线程-------
    //int i = 6;
    //DoSometing dododo(i); // 可调函数对象
    //thread myThread(dododo); // 拷贝构造->线程对象 -- 但是这个对象里最好不要有指针或引用

    // ---------lambda表达式---------
    auto lambda3 = [](){
        cout << "thread3 start\n";
        cout << "thread3 end\n";
    };
    thread myThread(lambda3);

    // ---------类对象的函数指针--------
    // DoSometing obj1(10);
    // thread mythread2(&DoSometing::thread_func, obj1);

    // -------------join-------------
    // 加入/汇合，阻塞
    // 让主线程等待该线程执行完成，然后汇合
    myThread.join(); 
    // cout << myThread.joinable() << endl;

    // ------------detach------------
    // 分离，不和主线程汇合，分离后一旦主线程结束，子线程后台运行
    // 子线程被c++运行时刻接管，运行完成后，运行时库负责清理资源（守护线程）
    // 不太稳定 -- 每次进行的次序都不一样 -- 该线程失去控制 -- 不推荐使用
    // 使用detach后就不能再join回来了，否则会有异常
    // myThread.detach();

    // -----------joinable-----------
    // 判断是否可以成功使用join()
    // cout << myThread.joinable() << endl;

    cout << "main" << " thread ! " << endl;
    cout << "main" << " thread ! " << endl;
    cout << "main" << " thread ! " << endl;
    cout << "main" << " thread ! " << endl;

     
    // 主线程结束 -- 结束前，其他线程也一定要结束
    // 主线程一定要等待子线程完毕再结束
    return 0; 
}