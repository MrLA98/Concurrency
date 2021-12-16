# Concurrency
## 1. 线程基础
> 该部分都在头文件` <thread>` 中
>
> 主要内容：
>
> ```cpp
> std::thread t1(func, para);
> std::thread t2(&A::func, &Aobj, para);
> t1.join();
> t1.detech();
> t1.joinable();
> ```

### 1.1 创建线程
- 基本模式： `std::thread obj(func)`，func为可调对象，可以是：
  1. 函数
  2. 函数对象 -- 重载()运算符
  3. lambda表达式
  4. 类成员函数 -- 需要额外输入参数(指定的类对象)

### 1.2 线程参数
- 基本模式：`thread obj(func, para1, para2, ...)`
  - 不管是传入一个对象，还是其应用，thread对象都会复制一份
  - 可以使用 `std::ref` 将传入对象强制转化为引用
  - 传入一个对象来隐式转换时，需要在传入时进行显式转换
  - 如果传递指针就尽可能不要使用detech，有可能所指的地址在线程结束前就被释放了

### 1.3 线程的加入
- 加入： `obj.join()` 主线程会等待线程结束后再结束

- 分离： `obj.detech()` 线程挂到后台，和主线程不再有关系

- 判断： `bool b = obj.joinable()` 是否可以使用 `join()`
  - 如果可以使用 `join()`，就也可以使用 `detech()`

## 2. 线程间共享数据
> 该部分都在头文件 `<thread>` 中
>
> 主要内容：
>
> ```cpp
> std::mutex mut1;
> mut1.lock();
> mut1.unlock();
> std::lock_gaurd<mutex> lGaurd(mut1);
> std::lock_gaurd<mutex> lGaurd(mut1, std::adopt_lock);
> std::lock(mut1, mut2, ...);
> std::unique_lock<mutex> u1(mut1);
> unique_lock<mutex> u1(mut1, adopt_lock);
> unique_lock<mutex> u2(mut1, defer_lock);
> unique_lock<mutex> u3(mut1, try_to_lock);
> u2.lock();
> u2.unlock();
> u3.owns_lock();
> mutex* pm = u3.release();
> u2.try_lock();
> std::once_flag flag1;
> std::call_once(flag1, func);
> std::condition_variable myCV;
> myMut.notify_one();
> myMut.notify_all();
> myMut.wait(u1,[bool_lambda]);
> ```

### 2.1. 互斥量和锁
- 当多个线程访问共享数据时，要防止条件互斥
- 因此，同一时间只有一个线程能够访问共享数据，通过互斥量和锁实现
  - `std::mutex myMutex` 定义一个互斥量
  - `myMutex.lock()` 上锁
  - `myMutex.unlock()` 解锁
- 一个互斥量在某个线程被锁上，另一个线程就会在锁处等待，直至第一个线程解锁
- 防止忘了解锁：使用`std::lock_gaurd<mutex> l1(myMutex)` 
  - 在这句命令时，上锁，结束一个作用域时，自动解锁
  - 可以使用`{}`来控制解锁时机

### 2.2 死锁及其预防
- 当有多个锁需要同时上锁，且在多个位置以不同顺序上锁时，就可能会产生死锁
- 防止死锁的主要方法就是：
  - 保证每个上锁的位置，都是以同样顺序上锁的
- 可以使用 `std::lock(mut1, mut2, mut...)`
  - 同时锁住多个互斥量，一旦之间某个互斥量锁住了，就会自动释放其他锁，等待全部解锁时再一同锁住
  - 缺点是需要手动解锁
- 配合 `std::adopt_lock` 使用，即可自动解锁
```cpp
std::lock(mut1, mut2, mut...); // 同时上锁
// 自动解锁
std::lock_gaurd<mutex> l1(mut1, std::adopt_lock);
std::lock_gaurd<mutex> l2(mut2, std::adopt_lock);
...
```

### 2.3 unique_lock 类
- 和 `std::lock_gaurd` 类似，但更加灵活，效率相对低一点
- 使用方法：`std::unique_lock<mutex> u1(mut1);`
  - 创建一个 `unique_lock` 与 `mut1` 关联，并上锁，自动解锁
- 参数和方法：
  1. `unique_lock<mutex> u1(mut1, adopt_lock);`
     - 表示关联`mut1`，且说明已经上锁了 
  2. `unique_lock<mutex> u2(mut1, defer_lock);`
     - 表示关联`mut1`，但还没上锁
     - 后续可以使用 `u2.lock()` 和 `u2.unlock()` 来灵活上锁解锁
  3. `unique_lock<mutex> u3(mut1, try_to_lock);`
     - 表示关联mut1，尝试锁
     - 用 `bool b = u3.owns_lock()` 可以知道是否锁成功了，进行分支处理
  4. `u3.try_lock()` 也是返回是否锁成功，前提是`u3`初始化时用的 `defer_lock` ,总之在使用之前，`u3`持有的互斥量是未锁的
  5. `mutex* pm = u3.release()`，直接断开`u3`和`mut1`之间的关系，并返回一个指向`mut1`的指针，但此时就需要`pm`来负责解锁了。

### 2.4 单例模式的多线程
- 单例模式的基本写法
```cpp
class Singtn{
public:
    static Singtn* getInstance(){ // 单例接口
        if(!instance){
          createInstance();
        }
        return instance;
    }

    void print(){ // 成员函数
        cout << "my singleton!\n";
    }

    class TrashRecicle{ // 垃圾回收
    public:
        ~TrashRecicle(){ // 析构中释放内存
            if(Singtn::instance){
                delete Singtn::instance;
                Singtn::instance = NULL;
            }
        }
    };

private:
    static Singtn* instance; // 单例指针

    Singtn(){} // 私有构造

    static void createInstance(){ // 单例初始化
        instance = new Singtn();
        static TrashRecicle tr;
    }
};

// 指针初始化
Singtn* Singtn::instance = NULL;

// 使用方法
Singtn* pSgt = Singtn::getInstance();
pSgt->print();
```

- 多线程使用单例模式时，有可能多个线程同时调用`createInstance()`函数

__两种解决方法：__
1. 双重锁（推荐）
```cpp
std::mutex mut1;
static Singtn* Singtn::getInstance(){ // 修改单例接口
    if(!instance){ // 双重锁定
      std::lock_gaurd<mutex> lg1(m1);
      if(!instance){
        createInstance();
      }
    }
    return instance;
}
```
  2. `std::call_once()` （代码更简单，但效率略低）
```cpp
std::once_flag flag1;
static Singtn* Singtn::getInstance(){ // 修改单例接口
    std::call_once(flag1, createIncetance);
    return instance;
}
```
### 2.5 条件变量
- 某些操作需要共享数据满足一定条件时才能进行
  - 但如果仅仅通过条件分支和循环来判断的话
  - 只要不满足条件，就会不停循环，资源占用率大，且在做无效工作
  - 希望直接在不满足条件时休眠，而满足条件时被唤醒
- 使用条件变量
```cpp
/* -------------------背景---------------------- */
/*   1. “工作2”只有在“工作1”执行一次或多次后，才可以执行
     2. “工作2”执行后，“工作1”的成果可能会回到没做过的状态
       以至于“工作2”重新处于不能进行的状态           */
/* -------------------------------------------- */

// 1. 创建条件变量和互斥量
std::condition_variable myCV;
std::mutex myMut;

// 线程1，做“工作1”
void thread_func1()
{
  std::unique_lock<std::mutex> uLock(myMut); // 上锁
  sharedData.doSomething_1(); // “工作1”处理共享数据
  // 处理过一次了，满足“工作2”执行的条件了，将其唤醒
  myMut.notify_one(); // 3. 唤醒正在休眠的线程
}

// 线程2，做工作2
void thread_func2()
{
  std::unique_lock<std::mutex> uLock(myMut); // 上锁
  // 2. 判断1是否执行过，使得工作2可以执行
  myMut.wait(uLock, [sharedData]{
    return sharedData.thing_1_done();
  }); 
  // lambda表达式返回true才会继续;返回false会先将uLock解锁，然后在本行休眠，直到线程1使用notiy_one()将其唤醒, 唤醒后：
    // (1)重新尝试上锁
    // (2)上锁后重新判断lambda表达式，为true才继续,否则重新休眠
  sharedData.doSomething_2(); // “工作2”处理共享数据
}
```
- 如果有多个正在wait的线程，`notify_one()` 只能随机唤醒一个。如果希望唤醒多个，则需要使用 `notify_all()` 函数。

## 3. future 类
> 以下方法和类都在头文件`<future>`里
>
> 主要内容：
>
> ```cpp
> std::future<int> fu = std::async(func, para...);
> future<int> fu = async(std::launch::async,func, para...);
> fu = async(std::launch::deferred,func, para...);
> fu.get();
> fu.wait();
> std::future_status myStt;
> myStt = fu.wait_for(std::chrono::seconds(10));
> std::package_task<int(int)> pkt(thread_func);
> std::thread t1(std::ref(pkt), para..); 
> pkt(para); 
> fu = pkt.get_future();
> std::promise<int> res;
> res.set_value(val);
> fu = res.get_future();
> myStt == std::future_status::timeout;
> myStt == std::future_status::ready;
> myStt == std::future_status::deferred;
> std::shared_future<int> myFu_s(myFu.share());
> std::shared_future<int> myFu_s(std::move(myFu));
> std::shared_future<int> myFu_s(res.get_future());
> std::shared_future<int> myFu_s(pkt.get_future());
> ```
### 3.1 用 `std::async()` 返回future对象
__两种使用方法：__
- `future<int> fu = async(func, para...)`
  - 直接从一个函数启动线程，输入参数
- `future<int> fu = async(std::launch::async,func, para...)`
  - 从这句开始直接启动线程
  - 在 `fu.get()` 或 `fu.wait()` 处等待线程结束
  - 如果没有，则在主线程`return 0;`处等待线程结束
- `future<int> fu = async(std::launch::deferred,func, para...)`
  - 在 `fu.get()` 或 `fu.wait()` 处才开始线程，没有就不执行
  - 而且实际上是在主线程中执行

### 3.2 future对象的方法
- `int res = fu.get()` 
  - 通过移动的方法，把fu中储存的结果给res
  - 因此只能执行一次
- `fu.wait()`
  - 类似于`thread.join()`,等待线程结束

### 3.3 package_task类
- 用途 -- 将函数包装起来，可以返回future对象
- 使用
```cpp
// 线程入口函数
int thread_func(int para);
// 包装
std::package_task<int(int)> pkt(thread_func)
// ------------------^^^上面是共同操作^^^------------------------
// 用法1：
std::thread t1(std::ref(pkt),para); // 创建线程 -- 注意要用引用
t1.join(); // 主线程等待
// 用法2： -- 实际上相当于调用函数，没有开始新线程
pkt(para); // 调用入口函数
// ------------------vvv下面是共同操作vvv-------------------------
std::future<int> fu = pkt.get_future(); // 得到返回值
int res = fu.get();
```
- 拷贝构造函数没有，所以不能复制，只能移动
- 且`get_future()` 只能执行一次

### 3.4 promise类
- 用途 -- 将promise对象的引用传入void入口函数
  - 再将其转化为future对象

- 用法
```cpp
void thread_func(proimse<int> &res, int para){
  int val = dosomething(para); // 处理
  res.set_value(val); // 获得结果
}
promise<int> res; // 声明对象
thread t1(thread_func, std::ref(res), 12); // 创建线程
t1.join();
std::future<int> fu = res.get_future(); // 获得返回值
int res = fu.get();
```
- 注意，同样的，拷贝构造函数没有，所以不能复制，只能移动
- 且`get_future()` 只能执行一次

### 3.5 future_statues 枚举类
- 首先说明，线程是用 _9.1_ 的 `async()` 方法创建的
- 其次说明，这是future对象的成员方法 -- `fu.wait_for(time);` 的返回类型，有且只有一下三种情况：
  - `std::future_status::timeout`
    - 执行时间 > time时返回
    - 还会继续执行，在get/wait处等待，或在`return 0;`处等待
  - `std::future_status::ready`
    - time时间内执行结束
  - `std::future_status::deferred`
    - 表示创建线程时使用的 `async(std::launch::deferred, fucn)`
    - 等待get/wait才开启线程，否则不开启
- 最后，time的类型是 `std::chrono::seconds(t)` -- 表示t秒

### 3.6 shared_future 类
- 从`future`对象构造
  - `std::shared_future<int> myFu_s(myFu.share());` 
    - 成员函数
  - `std::shared_future<int> myFu_s(std::move(myFu));`
    - 传递右值
  - 之后myFu都变成空了
    - `myFu.valid() == false`
- 从`pakcage_task`或`promise`对象构造
  - `std::shared_future<int> myFu_s(res.get_future());`
    - 从package_task构造
  - `std::shared_future<int> myFu_s(pkt.get_future());`  
    - 从promise构造
  - 算是自动类型转换
- 新对象 `myFu_s.get()` 使用多少次都可以，以为是拷贝，而不是移动