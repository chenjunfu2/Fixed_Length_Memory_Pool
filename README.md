# 定长内存快速分配器  
  
## 介绍：  
**Fixed_Length_Memory_Pool**：  
一个用于分配定长内存的内存池，不可扩容  
分配耗时：固定O(1)  
释放耗时：固定O(1)  
  
**Automatic_Expand_Fixed_Length_Memory_Pool**：  
与上面相比，它多了扩容机制，是一个用于管理上面内存池的管理器  
分配耗时：最差O(log n)（保证n<64）（m次分配内O(1)，m次分配时O(log n)，m\*=扩容倍数，m起始值为用户指定，依此循环，扩容次数为n）  
释放耗时：固定O(log n)（保证n<64）（这里的n与上面相同）  
  
## 场景：  
这两个内存池都是在大量定长内存场景下优化分配和释放速度，如**链表、队列节点**，**树、图节点**，**哈希节点**等的分配与释放  
  
## 优化：  
这两个内存池都是通过减少对系统分配释放函数的调用，并使用简单的调度算法和检查代码，来减少内存分配释放的开销  
  
## 原理：  
**Fixed_Length_Memory_Pool**：  
这个内存池在初始化时按照指定的单个块大小和总块数来预分配内存并按固定大小切分，  
分配时从栈中弹出返回给用户，释放时检查并回收入栈，在其生命周期内，管理的内存块大小不变  
  
**Automatic_Expand_Fixed_Length_Memory_Pool**：  
这个自动扩容内存池依赖并管理上面的定长内存池，使其组成内存池阵列，通过用户设置的预分配项来构造第一个内存池，  
分配时通过向其管理的内存池阵列中的空闲池，请求内存返回给用户，并在池内存用尽后自动构造一个新池扩容，  
释放时通过二分查找在内存池阵列中找到对应释放指针的地址所属内存池，并交由其检查指针合法性后回收，  
在其生命周期内，管理的总内存池数会变化  

## 使用：  
**Fixed_Length_Memory_Pool**：  
## FixLen_MemPool类  
一个定长内存池类，通过预设的初始值来分配并管理一个大内存块，并在后续使用中提供快速的内存请求操作  
  
### 语法  
```cpp
template <
	typename Type,
	bool bLazyInit,
	size_t szAlignment,
	typename Alloc_func,
	typename Free_func
>
class FixLen_MemPool;
```
  
### 参数  
`Type`  
类型: **typename**  
一个模板类型，用于指定后续内存分配时，返回的指针指向的类型  
  
`bLazyInit`  
类型: **bool**  
这个值用于指定这个类是否进行懒惰初始化，如果为true，则类进行懒惰初始化，否则进行完全初始化，此参数用于初始化和重置时的优化  
  
`szAlignment`  
类型: **size_t**  
这个值用于指定类初始化时分配的地址的内存对齐边界，类内所有动态分配内存的起始地址都会对齐到此边界上，此参数不为0，必须为1或为2的倍数  
  
`Alloc_func`  
类型: **typename**  
用于提供类初始化时的默认分配器，需要提供一个重载了函数调用运算符的类类型，并保证只在类初始化时创建使用并立即销毁  
  
`Free_func`  
类型: **typename**  
用于提供类析构时的默认释放器，，需要提供一个重载了函数调用运算符的类类型，并保证只在类析构时创建使用并立即销毁  
  
### 返回值  
**无**  
  
### 注解  
* 如果Type类型为void，则需要在构造函数中手动指定内存块大小  
* 如果Type不为类类型，则不应该使用带有构造尾缀或析构尾缀的操作函数  
* 如果构造函数中指定的大小比Type小（void除外），则后续对分配的内存进行解引用、读写操作的行为未定义  
* 如果启用了懒惰初始化，则在后续的内存分配中会存在额外开销  
  
### 示例  
以下是一些简易的声明  
```cpp
FixLen_MemPool<void> pPool;
FixLen_MemPool<long, true> longPool;
FixLen_MemPool<char, false, 8> charPool;
```
  
## FixLen_MemPool构造函数  
用于构造FixLen_MemPool类  
  
### 语法  
```cpp
FixLen_MemPool(
	size_t _szMemBlockFixSize,
	size_t _szMemBlockPreAllocNum
);
```
  
### 参数  
`_szMemBlockFixSize`  
类型: **size_t**  
定长内存块的大小，不应为0  
  
`_szMemBlockPreAllocNum`  
类型: **size_t**  
预分配的定长内存块个数，不应为0  
  
### 返回值  
**无**  
  
### 注解  
* 如果任意一个参数为0，为了保证类初始化完毕，将使用默认参数1替代0，类在分配器分配失败后会抛出`std::bad_alloc`异常  
  
### 示例  
以下是一些简易的示例  
```cpp
FixLen_MemPool<void> pPool(32);
FixLen_MemPool<long> longPool(sizeof(long), 2048);
FixLen_MemPool<char> charPool(sizeof(char) * 4);
```
  
## FixLen_MemPool移动构造函数  
简易说明  
  
### 语法  
```cpp
FixLen_MemPool(
	FixLen_MemPool &&_Move
);
```
  
### 参数  
`_Move`  
类型: **FixLen_MemPool &&**  
被移动的对象  
  
### 返回值  
**无**  
  
### 注解  
* 被移动的对象在移动后失效，移动目标对象继承所有被移动对象的数据值，使用已被移动的对象是未定义行为  
  
### 示例  
示例说明  
```cpp
auto MoveFunc = [](void)
{
	return FixLen_MemPool<size_t>(sizeof(size_t), 2048);
};

auto sizePool = MoveFunc();
```
  
## FixLen_MemPool析构函数  
用于析构FixLen_MemPool类  
  
### 语法  
```cpp
~FixLen_MemPool(
	void
);
```
  
### 参数  
**无**
  
### 返回值  
**无**
  
### 注解  
* 此析构函数可以被安全的多次调用，除非类模板中的默认释放器在多次使用或释放NULL指针时会出错  
  
### 示例  
以下是手动调用析构函数的示例  
```cpp
FixLen_MemPool<void> pPool(16);
pPool.~FixLen_MemPool<void>();
```
  
## AllocMemBlock成员函数  
用于分配一个预设大小的内存块  
  
### 语法  
```cpp
Type *AllocMemBlock(
	void
);
```
  
### 参数  
**无**
  
### 返回值  
类型: **Type \***  
成功分配内存则返回实际内存地址，否则返回NULL  
  
### 注解  
* 如果内存池中可分配内存块耗尽，则此函数永远返回NULL，直到用户释放一个已分配地址。
* 如果使用内存块时，读写操作超过类构造时传入的固定大小，则行为未定义  
  
### 示例  
以下是一个分配示例  
```cpp
FixLen_MemPool<void> pPool(16);//构造内存池对象
void *Temp = pPool.AllocMemBlock();//分配一个内存块
/*使用Temp*/
pPool.FreeMemBlock(Temp);//回收内存块
```
  
## FreeMemBlock成员函数  
简易说明  
  
### 语法  
```cpp
bool FreeMemBlock(
	Type *pAllocMemBlock
);
```
  
### 参数  
`pAllocMemBlock`  
类型: **Type \***  
通过成员函数AllocMemBlock分配的内存指针，或者为NULL  
  
### 返回值  
类型: **bool**  
如果释放成功，返回true，否则返回false  
  
### 注解  
* 此函数会检查释放内存地址的合法性，如果地址不在内存池中、不在定长内存块边界上，或者该内存地址已释放过，且其不为NULL，则该函数失败  
  
### 示例  
示例说明  
```cpp
FixLen_MemPool<void> pPool(16);//构造内存池对象

void *Temp = pPool.AllocMemBlock();//分配一个内存块

pPool.FreeMemBlock(Temp);//正常释放，返回true
pPool.FreeMemBlock(NULL);//释放空指针，返回true

pPool.FreeMemBlock(Temp);//多次释放，返回false
pPool.FreeMemBlock((void *)1);//释放非法地址，返回false
```
  
## AllocMemBlockConstructor成员函数  
与AllocMemBlock效果一致，但是提供了变长参数模板用于分配并构造类  
  
### 语法  
```cpp
template<
	typename... Args
>
Type *AllocMemBlockConstructor(
	Args&&... args
);
```
  
### 参数  
`Args`  
类型: **typename...**  
变长模板类型  
  
`args`  
类型: **Args &&...**  
变长参数，会完美转发到构造new的构造函数参数中  
  
### 返回值  
类型: **Type \***  
返回构造好的Type类指针，如果失败返回NULL  
  
### 注解  
* 会分配内存并通过参数构造Type类并返回其指针，如果内存池中无可用内存块，则返回NULL  
  
### 示例  
示例代码  
```cpp
struct Test//示例节点
{
	char c;
};
FixLen_MemPool<Test> testPool;//构造类

Test *pTest = testPool.AllocMemBlockConstructor('A');//分配并使用'A'初始化结构体中的成员c
/*使用pTest*/
testPool.FreeMemBlockDestructor(pTest);//使用对应的函数析构并释放
```
  
## FreeMemBlockDestructor成员函数  
与FreeMemBlock效果一致，但是会析构内存块代表的类  
  
### 语法  
```cpp
bool FreeMemBlockDestructor(
	Type *pAllocMemBlock
);
```
  
### 参数  
`pAllocMemBlock`  
类型: **Type \***  
参数说明  
  
### 返回值  
类型: **bool**  
与FreeMemBlock返回值一致
  
### 注解  
* 除了对释放地址所代表的类进行析构外，其余行为与FreeMemBlock一致  
  
### 示例  
示例说明  
```cpp
struct Test//示例节点
{
	char c;
};
FixLen_MemPool<Test> testPool;//构造类

Test *pTest = testPool.AllocMemBlockConstructor('A');//分配并使用'A'初始化结构体中的成员c
/*使用pTest*/
testPool.FreeMemBlockDestructor(pTest);//使用对应的函数析构并释放
```
  
## Reset成员函数  
用于将类的状态重置为构造函数后的状态  
  
### 语法  
```cpp
void Reset(
	void
);
```
  
### 参数  
**无**
  
### 返回值  
**无**
  
### 注解  
* 用于重置类，如果模板中指定了懒惰初始化，则重置也为懒惰重置
* 这个重置函数会重置类状态为所有内存块未分配，在使用了这个函数后，仍读写以前分配的内存块，则行为未定义  
  
### 示例  
重置类状态  
```cpp
FixLen_MemPool<void> pPool(16);//初始化类
pPool.AllocMemBlock();//分配内存但是忘记保存返回地址，造成内存泄漏
pPool.Reset();//重置后相当于回收所有已分配内存块
```
  
*更新待续*
