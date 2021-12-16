#include <iostream>
#include <thread>
#include <future>
#include <vector>

using std::cout;
using std::endl;

int thread_func(int ti){
    cout << "thread [" << std::this_thread::get_id() << "] starts!\n";
    // 休息5000毫秒 = 5s
    cout << "sleap "<< ti <<"s\n";
    std::chrono::milliseconds dura(1000 * ti); 
    std::this_thread::sleep_for(dura);
    cout << "thread [" << std::this_thread::get_id() << "] ends!\n";
    return 1000 * ti;
}

int main(){
    cout << "main thread [" << std::this_thread::get_id() << "] starts\n";

    // 包装一个函数
    std::packaged_task<int(int)> myPt(thread_func); // 把函数包装起来
    std::thread t1(std::ref(myPt), 4); // 创建线程
    t1.join(); // 等待线程完成
    std::future<int> res = myPt.get_future(); // 获得结果
    cout << "res = "<<res.get()<<endl; // 这里可以直接取到结果，不会卡着

    // 包装一个lambda表达式，且直接调用
    std::packaged_task<int(int)> myPt2([](int ti){
        cout << "thread [" << std::this_thread::get_id() << "] starts!\n";
        cout << "sleap "<< ti <<"s\n";
        std::chrono::milliseconds dura(1000 * ti); 
        std::this_thread::sleep_for(dura);
        cout << "thread [" << std::this_thread::get_id() << "] ends!\n";
        return 1000 * ti;
    });
    myPt2(3); // 直接调用 没用新线程
    std::future<int> res2 = myPt2.get_future(); // 获得结果
    cout << "res2 = "<<res2.get()<<endl; // 这里可以直接取到结果，不会卡着

    // 容器批量操作
    std::packaged_task<int(int)> myPt3(thread_func);
    // 创建容器
    std::vector<std::packaged_task<int(int)>> myTasks;
    // 添加
    myTasks.push_back(std::move(myPt3)); // 插入时用移动语句
    // 拿出
    auto it = myTasks.begin();
    myPt3 = std::move(*it); // 拿出的时候也用移动语句
    myTasks.erase(it); // 弹出该迭代器

    cout << "main thread [" << std::this_thread::get_id() << "] ends\n";
    return 0;
}