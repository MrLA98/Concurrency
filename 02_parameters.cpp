// #include <iostream>
// #include <thread>
// #include <string>

// using namespace std;

// // detach的话，i的地址和val、mval的地址不一样
// // 所以引用传递的i是一个复制的值的引用，是安全的，不用担心其在主线程里被销毁
// // 而传递的指针是一样的地址，所以传递指针是不安全的
// // void myprint(const int &i, char *buf){
// void myprint(const int &i, const string &buf) {
//                                 // -- 隐式将char*生成一个新的string对象传入，从而变成安全的 
//                                 // -- 这个时机有可能是char*被回收以后，所以又可能不安全
//     cout << i << endl; 
//     cout << buf << endl;
// }

// int main(){
//     // 1. 传递临时对象作为线程参数
//         // -- 建议整形类的直接值传递
//         // -- 传递类对象，避免隐式类型转换，在创建线程时，显式生成对象，引用传递 
//             // -- 因为隐式类型转换是在子线程中创建的
//         // -- 但即使传入的是对象的引用，thread还是会将该对象拷贝一份
//             // -- 如果确实需要引用该对象，需要使用std::ref()
//     // 2. 传入智能指针unique_ptr时，需要使用std::move
//     // 3. 传入类对象的函数指针来启动线程
//     int val = 1, &mval = val;
//     char mybuf[] = "this is a test!";
//     // thread mytObj(myprint, mval, mybuf);
//     thread mytObj(myprint, mval, string(mybuf)); // 安全的方法，在这个时候直接生成一个string对象传入
//     mytObj.join();
//     //mytObj.detach();
//     cout << "main thread!\n";


//     return 0;
// }