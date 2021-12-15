// #include <thread>
// #include <vector>
// #include <iostream>

// using namespace std;

// // 多个线程使用只读数据时没有问题的，安全
// vector<int> ReadOnly = {1,2,3};

// // 有读有写，不安全，需要特殊处理

// void myPrint(int count){
//     cout << "thread starts, id = " << std::this_thread::get_id() << endl;
//     cout << "print readOnly = " << ReadOnly[0] << ReadOnly[1] << ReadOnly[2] << endl;
//     cout << "thread ends, count = " << count << endl;
// }

// int main(){
//     vector<thread> mythreads;
//     for(int i = 0; i < 10; ++i){
//         mythreads.push_back(thread(myPrint, i));
//     }
//     for(auto &it : mythreads){
//         it.join();
//     }

//     cout << "main thread!\n";
//     return 0;
// }