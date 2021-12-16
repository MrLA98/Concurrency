#include <iostream>
#include <thread>
#include <future>
#include <vector>

using std::cout;
using std::endl;

// 通过一个promise对象的引用来返回结果
void thread_func(std::promise<int> &ret , int para){
    cout << "thread [" << std::this_thread::get_id() << "] starts!\n";
    // 假设在计算 -- 花费时间和para成比例
    cout << "sleap "<< para <<"s\n";
    std::chrono::milliseconds dura(1000 * para); 
    std::this_thread::sleep_for(dura);
    para += para * para / 4;
    // 计算结束，保存结果
    ret.set_value(para);

    cout << "thread [" << std::this_thread::get_id() << "] ends!\n";
}

void thread_func2(std::future<int> &Fu){
    cout << "thread [" << std::this_thread::get_id() << "] starts!\n";
    cout << "res from thread1 = " << Fu.get() << endl;
    cout << "thread [" << std::this_thread::get_id() << "] ends!\n";
}

int main(){
    cout << "main thread [" << std::this_thread::get_id() << "] starts\n";

    std::promise<int> ret; // 声明对象
    std::thread t1(thread_func, std::ref(ret), 3); // 引用方法传入参数
    t1.join(); // 等待线程
    // 用future直接获取
    std::future<int> myFu = ret.get_future(); // 获取线程返回值
    //cout << "res = " << myFu.get() << endl; 

    // 别的线程获取
    std::thread t2(thread_func2, std::ref(myFu));
    t2.join();

    cout << "main thread [" << std::this_thread::get_id() << "] ends\n";

    return 0;
}
