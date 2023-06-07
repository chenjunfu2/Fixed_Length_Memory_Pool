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
```cpp
struct Link_Node//示例链表节点
{
	int iData;
	Link_Node *pNext;
};

struct Test//示例节点
{
	char c;
};

//构造定长内存池

//分配时返回void* 注意使用void时无法默认传入sizeof(void)参数，需要手动传递内存块大小
FixLen_MemPool<void> pPool(16);

//分配时返回long*
FixLen_MemPool<long> longPool;

//分配时返回Link_Node*
FixLen_MemPool<Link_Node> linkPool;

//分配时返回Test* 这里sizeof(Test)==1，内存没有对齐会导致后续访问速度变慢，可以手动传入指定大小来强制对齐
//在模板内指定了懒惰初始化和内存池起始地址（也就是第一个分配的内存块）对齐到8字节边界
size_t szAlignedSize = FixLen_MemPool<Test>::Aligned(sizeof(Test), 8);//使用了静态函数Aligned计算内存对齐
FixLen_MemPool<Test, true, 8> testPool(szAlignedSize);//这样实际分配的内存是8字节，使用1字节，剩余7字节用于对齐，同时模板指定第一个内存块的起始地址是对齐8字节边界的

//使用定长内存池

puts("void:");
//void*
void *pTemp = pPool.AllocMemBlock();//请求一个内存块
size_t szpFixSize = pPool.GetMemBlockFixSize();//获取分配时指定的定长块大小，以字节为单位
memset(pTemp, 0xCF, szpFixSize);//使用内存块...
pPool.FreeMemBlock(pTemp);//释放内存块

size_t szpUse = pPool.GetMemBlockUse();
printf("use:%zu\n", szpUse);//预期这里应该打印出use:0

puts("\nlong:");
//long*
long *lArr[16];
for (int i = 0; i < 16; ++i)
{
	lArr[i] = longPool.AllocMemBlock();//请求一些内存块
}

size_t szlUse = longPool.GetMemBlockUse();//获取使用情况
printf("use:%zu\n", szlUse);//预期这里应该打印出use:16

puts("\nLink_Node:");
//Link_Node*
Link_Node *pHead = NULL;

//插入函数
auto InsertHead = [&](int iData) -> void
{
	Link_Node *pNewNode = linkPool.AllocMemBlockConstructor(Link_Node(iData, pHead));//分配内存并构造

	pHead = pNewNode;//接入链表

	printf("insert head:%d\n", iData);
};

//移除函数
auto RemoveHead = [&](void) -> int
{
	if (pHead == NULL)
	{
		return -1;
	}

	Link_Node *pRemoveNode = pHead;
	pHead = pHead->pNext;//移出链表
	int rData = pRemoveNode->iData;//保存data

	linkPool.FreeMemBlockDestructor(pRemoveNode);//释放内存并析构

	printf("remove head:%d\n", rData);
	return rData;
};

//链表的插入和移除
puts("Insert:");
InsertHead(16);
for (int i = 0; i < 9; ++i)
{
	InsertHead(i);
}

puts("Remove:");
RemoveHead();

puts("Insert:");
InsertHead(32);

puts("RemoveAll:");
while (RemoveHead() != -1)
{
	continue;
}

puts("\nTest:");
//分配时返回Test*
for (int i = 0; i < testPool.GetMemBlockNum(); ++i)//全部分配完
{
	testPool.AllocMemBlock();//哎呀，泄露了，可以手动重置类回收所有已分配指针
}

const char *cp = testPool.GetMemBlockUse() == testPool.GetMemBlockNum() ? "true" : "false";
printf("use all:%s\n", cp);//预期这里应该打印出use all:true

testPool.Reset();//直接重置整个类，回到初始状态

cp = testPool.GetMemBlockUse() == testPool.GetMemBlockNum() ? "true" : "false";
printf("now\nuse all:%s\n", cp);//预期这里应该打印出use all:false

puts("\nsize_t:");
//分配时返回size_t*
//移动构造
auto MoveFunc = [](void)
{
	//分配时返回size_t* 这里手动指定了初始内存块个数为2048而不是默认的1024
	return FixLen_MemPool<size_t>(sizeof(size_t), 2048);
};

auto sizePool = MoveFunc();
sizePool.AllocMemBlock();//哎呀，泄露了，泄露的指针在类析构后会被回收，无需担心

size_t szBlockNum = sizePool.GetMemBlockNum();
printf("block num:%zu\n", szBlockNum);//预计打印出block num:2048
```

**Automatic_Expand_Fixed_Length_Memory_Pool**：  
```cpp
//自动扩容类初始化参数、使用方式与定长池类一致，除了模板和部分新增成员函数
using Pool = FixLen_MemPool<size_t>;

//模板第一个参数需要接受一个FixLen_MemPool类型，这个类管理模板第一个参数指定的内存池
//模板参数Pool表明每次创建的内存池类为Pool，2表明每次扩容都以当前大小乘以2倍扩容，4表明每次扩容后的大小都对齐到4字节边界上
AutoExpand_FixLen_MemPool<Pool, 2, 4> sizeAutoPool(sizeof(size_t), 8);//指定管理的第一个内存池包含8个内存块

auto printInfo = [&](const char *p) -> void
{
	printf("%s\n", p);
	printf("use:%zu\n", sizeAutoPool.GetMemBlockUse());//获取使用数
	printf("all:%zu\n", sizeAutoPool.GetMemBlockNum());//获取总内存块数

	printf("pool num:%zu\n", sizeAutoPool.GetPoolNum());//获取总内存池数
	printf("free pool:%zu\n", sizeAutoPool.GetFreePoolNum());//获取空闲内存池数
	printf("full pool:%zu\n", sizeAutoPool.GetFullPoolNum());//获取满内存池数
};

int iCur = 0;
auto printEvery = [&](const Pool &c) -> bool
{
	printf("Cur:%d ", iCur++);
	printf("use:%zu,all:%zu\n", c.GetMemBlockUse(), c.GetMemBlockNum());

	return true;
};

for (int i = 0; i < 1023; ++i)
{
	sizeAutoPool.AllocMemBlock();//哎呀，泄露了，后续使用谓词手动重置所有管理的类或析构内存池
}

printInfo("Alloc");
sizeAutoPool.TraverseEligibleMemPool(printEvery);
iCur = 0;
putchar('\n');

sizeAutoPool.Capacity();//按照模板预设手动扩容一次

printInfo("Capacity");
sizeAutoPool.TraverseEligibleMemPool(printEvery);
iCur = 0;
putchar('\n');

auto ResetUse = [&](const Pool &c) -> bool
{
	return c.GetMemBlockUse() != 0;//重置
};

sizeAutoPool.ResetEligibleMemPool(ResetUse);
printInfo("ResetAll");
sizeAutoPool.TraverseEligibleMemPool(printEvery);
iCur = 0;
putchar('\n');

auto clearFunc = [](const Pool &c) -> bool//清理内存池的函数
{
	if (c.GetMemBlockUse() == 0)//只要没使用，就析构
	{
		return true;//删除
	}
	return false;
};

sizeAutoPool.RemoveEligibleMemPool(clearFunc);
printInfo("RemoveAll");
sizeAutoPool.TraverseEligibleMemPool(printEvery);
iCur = 0;
putchar('\n');

//通过参数1对齐到预设的字节边界上后扩容一次（注意，手动扩容可能引发问题，导致内存池管理空间提前耗尽）
//这个函数的目的在于在使用内存池清理函数之后管理类内部完全不存在内存池时手动添加一个，否则内存池的MemBlockNum为0会导致无法扩容
sizeAutoPool.AddNewMemPool(1);
printInfo("AddNew");
sizeAutoPool.TraverseEligibleMemPool(printEvery);
iCur = 0;
putchar('\n');
```
