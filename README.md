# Concurrency
## 1. 创建线程
- 基本模式： `std::thread obj(func)`，func为可调对象，可以是：
  1. 函数
  2. 函数对象 -- 重载()运算符
  3. lambda表达式
  4. 类成员函数 -- 需要额外输入参数(指定的类对象)

## 2. 线程参数
- 基本模式：`thread obj(func, para1, para2, ...)`
  - 不管是传入一个对象，还是其应用，thread对象都会复制一份
  - 可以使用 `std::ref` 将传入对象强制转化为引用
  - 传入一个对象来隐式转换时，需要在传入时进行显式转换
  - 如果传递指针就尽可能不要使用detech，有可能所指的地址在线程结束前就被释放了

## 3. 线程的加入
- 加入： `obj.join()` 主线程会等待线程结束后再结束

- 分离： `obj.detech()` 线程挂到后台，和主线程不再有关系

- 判断： `bool b = obj.joinable()` 是否可以使用 `join()`
  - 如果可以使用 `join()`，就也可以使用 `detech()`

## 4. 互斥量和锁
- 当多个线程访问共享数据时，要防止条件互斥
- 因此，同一时间只有一个线程能够访问共享数据，通过互斥量和锁实现
  - `std::mutex myMutex` 定义一个互斥量
  - `myMutex.lock()` 上锁
  - `myMutex.unlock()` 解锁
- 一个互斥量在某个线程被锁上，另一个线程就会在锁处等待，直至第一个线程解锁
- 防止忘了解锁：使用`std::lock_gaurd<mutex> l1(myMutex)` 
  - 在这句命令时，上锁，结束一个作用域时，自动解锁
  - 可以使用`{}`来控制解锁时机

## 5. 死锁
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

## 6. unique_lock
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

## 7. 单例模式的多线程
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
## 8. 条件变量
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
