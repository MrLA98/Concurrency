#include <iostream>
#include <thread>
#include <future>

using std::cout;
using std::endl;

// 用类的成员函数
class thread_class{
public:
    int thread_func(int para){
        cout << "thread [" << std::this_thread::get_id() << "] starts!\n";
        cout << "sleap "<< para << "s\n";
        std::chrono::milliseconds dura(1000 * para); 
        std::this_thread::sleep_for(dura);
        cout << "thread [" << std::this_thread::get_id() << "] ends!\n";
        return 1000 * para;
    }
};


// 一个线程的入口函数
int thread_func(){
    cout << "thread [" << std::this_thread::get_id() << "] starts!\n";
    // 休息5000毫秒 = 5s
    cout << "sleap 5s\n";
    std::chrono::milliseconds dura(5000); 
    std::this_thread::sleep_for(dura);
    cout << "thread [" << std::this_thread::get_id() << "] ends!\n";
    return 5000;
}

int main(){
    
    // 一、std::async, std::future -- 从入口创建后台任务并返回值
        // 头文件 -- <future>
        // 核心目的 -- 希望线程返回一个结果
        // 基本原理 -- async 从给定的入口函数启动一个线程，并返回一个future类型的对象
        // 说法 -- 启动一个异步任务
        // future -- 类模版
            // 其get()方法可以得到线程的返回值 -- 提供了一种访问异步操作结果的方法
                // get()方法会卡住，等待线程结束 类似于thread.join()
                // ！！！但是不能多次使用
            // 其wait()方法会等待线程结束，但不会像get()一样返回一个值
            // 如果不用get()和wait()，程序会在async绑定时开始，在主线程return 0 处等待线程结束
                // 使用了，程序则在get或wait调用时开始
                // 多个线程使用async时，也是从绑定时开始执行的
        // async(func) -- 一个函数，从func启动一个线程，返回一个future对象

        // async的特殊参数：放在第一个参数位置
            // 1. std::launch::deferred -- 线程入口函数的调用，推迟到future对象的get或wait方法调用时
                // 如果后面没有get或wait，那就直接不进入线程入口函数，实际上根本没有创建线程
                // 实际上，没有创建新的线程，入口函数是在主函数中执行的
            // 2. std::launch::async -- 在async函数绑定时就开始执行
    
    cout << "main thread [" << std::this_thread::get_id() << "] starts\n";
    std::future<int> thread_return1 = std::async(std::launch::async,thread_func); // 这里绑定关系

    //thread_class tcObj;
    // 使用类对象成员函数的方法、传参方法和thread相似
    //std::future<int> thread_return2 = std::async(&thread_class::thread_func, &tcObj, 3); 

    cout << "continuing...\n";
    /* 
        主线程正常干点别的
    */
    int result1 = thread_return1.get(); 
    cout << "thread1_return = " << result1 << endl;
    //int result2 = thread_return2.get();
    //thread_return2.wait();
    //cout << "thread2_return = " << result2 << endl;

    cout << "main thread [" << std::this_thread::get_id() << "] ends\n";

    return 0;
}
