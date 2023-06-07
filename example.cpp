#include "Fixed_Length_Memory_Pool.hpp"

#include <stdio.h>
#include <string.h>

//使用示例
int Example_Fixed_Length_Memory_Pool(void)
{
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

	return 0;
}


#include "Automatic_Expand_Fixed_Length_Memory_Pool.hpp"

int Example_Automatic_Expand_Fixed_Length_Memory_Pool(void)
{
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

	printInfo("Init");
	sizeAutoPool.TraverseEligibleMemPool(printEvery);
	iCur = 0;
	putchar('\n');

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

	//因为上面手动清理了所有内存池，使得类恢复回初始状态，所以手动分配第一个内存池
	//也可以不分配，后续调用Alloc函数时会自动新建第一个
	sizeAutoPool.AddFirstMemPool(1);
	printInfo("AddNew");
	sizeAutoPool.TraverseEligibleMemPool(printEvery);
	iCur = 0;
	putchar('\n');
	
	return 0;
}