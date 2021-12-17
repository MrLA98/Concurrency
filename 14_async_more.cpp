#include <iostream>
#include <thread>
#include <future>

using std::cout;
using std::endl;

int thread_func(int para){
    cout << "thread [" << std::this_thread::get_id() << "] starts"<<endl;
    cout << "sleep " << para << "s\n";
    std::this_thread::sleep_for(std::chrono::seconds(para));
    cout << "thread [" << std::this_thread::get_id() << "] ends"<<endl;
    return para*1000;
}

int main(){
    // 1. 如果系统资源紧张，thread 有可能创建失败，在join的时候会崩溃
        // 而async有四种选择：
            // deferred不会创建新线程，直到wait或get处，在调用wait/get的线程上调用线程函数
            // async会强制立即创建新线程，并立即进入主线程线程函数 -- 有可能会系统崩溃
            // any = deferred | async 表示可能是这两个中的一个，有不确定性，系统自己选择
            // 不带参数，系统默认是any，系统自己决定
        // 当选any或不带参数时，在系统资源紧张的情况下，系统就会选择用deferred方式，故不会崩溃

        // 注意：一个任务中，线程数量不宜超过100～200
        

    cout << "main thread [" << std::this_thread::get_id() << "] starts"<<endl;
    // std::future<int> myFu = std::async(std::launch::deferred,thread_func, 5);
    // std::future<int> myFu = std::async(std::launch::async,thread_func, 5);
    // std::future<int> myFu = std::async(std::launch::any,thread_func, 5);
    std::future<int> myFu = std::async(thread_func, 5);
    std::future_status fst = myFu.wait_for(std::chrono::seconds(0));
    if(fst == std::future_status::deferred){
        // 说明线程资源紧张，要等待get
    }else{ // 不紧张，已经启动了线程
        if(fst == std::future_status::ready){
            // 执行完成
        }else if(fst == std::future_status::timeout){ 
            // 超时
        }
    }
    
    cout << "hhh\n";
    int res = myFu.get();
    cout << "res = " << res <<endl;
    cout << "main thread [" << std::this_thread::get_id() << "] ends"<<endl;
    return 0;
}
