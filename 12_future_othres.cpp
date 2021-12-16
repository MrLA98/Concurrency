#include <iostream>
#include <thread>
#include <future>
#include <vector>

using std::cout;
using std::endl;

// 一个线程的入口函数
int thread_func(int ti){
    cout << "thread [" << std::this_thread::get_id() << "] starts!\n";
    // 休息5000毫秒 = 5s
    cout << "sleap "<<ti<<"s\n";
    std::chrono::milliseconds dura(ti*1000); 
    std::this_thread::sleep_for(dura);
    cout << "thread [" << std::this_thread::get_id() << "] ends!\n";
    return ti*1000;
}

int main(){
    //std::future<int> myFu = std::async(std::launch::deferred,thread_func, 1);
    std::future<int> myFu = std::async(thread_func, 1);
    // 等待一定时间，返回执行状态
    std::future_status status = myFu.wait_for(std::chrono::seconds(2)); 
    if(status == std::future_status::timeout){ // 超时了,然后主线程卡在return 0处
        cout << "time out!" << endl;
    }
    else if(status == std::future_status::ready){ // 成功返回
        cout << "succeed to return\n";
        cout <<"res = "<< myFu.get() << endl;
    }
    else if(status == std::future_status::deferred){ // 如果是std::async(std::launch::deferred,...
        cout << "defered to process, using get() to process in main thread\n";
        cout <<"res = "<< myFu.get() << endl;
    }

    // shared_future
    std::shared_future<int> myFu_s(myFu.share()); // 通过future来创建 -- 之后myFu就空了
    //std::shared_future<int> myFu_s(std::move(myFu)); // 右值传递 -- 之后myFu就空了
    cout << myFu.valid() << endl; // 查看future对象是否不为空，ture表示有值，false表示空
    // get()函数本质是一个移动实现，会将future对象中的内容移动到res中，然后future变空
    int res1 = myFu_s.get();
    int res2 = myFu_s.get();
    int res3 = myFu_s.get();
    // ... get多少次都没事，因为是复制过去的 


    return 0;
}