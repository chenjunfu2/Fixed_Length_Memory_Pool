#pragma once
#include "Fixed_Length_Memory_Pool.hpp"

//链表扩容法、树扩容法

/*
封装FixLen_MemPool类，并自动扩容申请新的类实例
类内有两个数组，一个是空闲内存池，另一个是排序内存池

分配：
每次分配都从空闲内存池索引szArrBeg处的内存池请求内存，如果请求失败则与--szArrLastSwap处的空闲内存池交换
也就是说，从szArrLastSwap到szArrEnd之间全部是已满的内存池，从szArrBeg到szArrLastSwap之间全部都是未满的内存池，szArrLastSwap相当于数组的分隔符

在交换过程中，要先检测szArrLastSwap是否<=1，如果是则代表已无可用内存池了，这时候就需要按照szExpandMultiple倍增扩容，分配一个新的内存池
如果还有可用内存池，则交换数组中这两个Node元素的指针并给他们的szArrIdx修改为当前它所在的数组位置的索引

分配新内存池时，新内存池的block数为：当前所有内存池block总和*szExpandMultiple-当前所有内存池block总和，也就是当前所有内存池block总和扩容szExpandMultiple倍后增加的block数
如果szArrEnd不等于数组的静态声明大小，则新建的内存池放在szArrBeg处，原先szArrBeg处的Node调到szArrEnd，并++szArrEnd，设置这两个Node里所在数组的索引然后利用二分法通过内存池起始地址
在pNodeArrSortPool找到适当的位置进行插入排序，后面的元素全部使用memmove往后移动

释放：
检查当前释放指针是不是nullptr，是就直接返回true

不是nullptr，使释放指针用二分法查找在pNodeArrSortPool中，因为pNodeArrSortPool是根据内存池地址排序的，所以可以快速找到指针所属内存池，但是因为内存池地址是起始地址，指针可以在起始地址
到结束地址之间，所以二分法并不一定是精确查找到完美匹配的方案（也存在刚好匹配，比如内存池起始地址的那个block刚好正在释放），所以只要二分查找中的beg和end相差小等于1或者刚好匹配，
则代表当前释放指针在beg索引所指的内存池中（因为大于beg的地址，但是小于end的地址，又因为内存池按照内存池的起始地址排序（注意不是内存池类的地址，是类内池之的地址），所以可以推出这个指针所属beg索引所指的内存池）
然后调用这个内存池的释放函数释放该指针

在释放完成后，如果释放函数返回false，则返回false

不是false，则通过这个索引得到的Node结构中的szArrIdx获得该池在pNodeArrFreePool中的位置，并判断szArrLastSwap和自身位置的关系，如果是szArrLastSwap>szArrIdx，则该池已在空闲区域内，无需调换直接返回true
否则将自己和szArrLastSwap索引的指针交换，并设置szArrIdx，然后++szArrLastSwap，确保自己被换入空闲区域

*/

template <typename Pool_class, size_t szExpandMultiple = 2, size_t szAlignment = 4, typename Alloc_func = default_alloc, typename Free_func = default_free>
class AutoExpand_FixLen_MemPool
{
	static_assert(szExpandMultiple >= 2);
protected:
	static constexpr size_t ArrMaxNum(void)//大致估计（编译时计算）
	{
		size_t szBlockNum = 1;//内存块总数
		size_t szCumulative = 0;//总大小

		size_t szNum = 0;
		while (szNum < 512)
		{
			//计算当前总大小
			szCumulative = Pool_class::szManageMemBlockRequireSize * szBlockNum;
			if (szCumulative / szBlockNum != Pool_class::szManageMemBlockRequireSize)//溢出
			{
				break;
			}
			
			szCumulative += szBlockNum;
			if (szCumulative < szBlockNum)//溢出
			{
				break;
			}
			
			szCumulative += sizeof(Pool_class) * szNum;
			if (szCumulative < sizeof(Pool_class) * szNum)//溢出
			{
				break;
			}

			szBlockNum *= szExpandMultiple;//倍增
			++szNum;
		}

		//数组大小为szNum
		return szNum;
	}

	static constexpr size_t PNODE_ARR_MAX_SIZE = ArrMaxNum();//61
	using Type = Pool_class::RetPoint_Type;
	struct Node
	{
		Pool_class csMemPool;
		size_t szArrIdx;//这个结构在空闲池数组中的索引
	};

private:
	Node* pNodeArrFreePool[PNODE_ARR_MAX_SIZE];//空闲内存池（szArrLastSwap索引左边，不包括其指代都为空闲内存池，右边，包括其指代都为已满内存池）
	Node* pNodeArrSortPool[PNODE_ARR_MAX_SIZE];//排序内存池（按内存池地址排序Node，左低右高）

	static constexpr size_t szArrBeg = 0;//头部索引
	size_t szArrEnd = 0;//尾后索引
	size_t szArrLastSwap = 0;//上一次交换的索引

	FixLen_MemPool<Node, false, 4, Alloc_func, Free_func> csMemPool = (sizeof(Node), PNODE_ARR_MAX_SIZE);//用一个内存池来管理后续的内部分配释放

	size_t szMemBlockFixSize = 0;//用户初始化时需要的定长内存长度（size：字节数）
	size_t szMemBlockNum = 0;//内存池中总内存块个数
	size_t szMemBlockUse = 0;//内存池中使用内存块个数
private:
	Node *ConstructorNode(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum, size_t _szArrIdx)
	{
		Node *pNode = csMemPool.AllocMemBlock();//请求内存

		//构造类
		new(&pNode->csMemPool) Pool_class(_szMemBlockFixSize, _szMemBlockPreAllocNum);//placement new
		//初始化索引
		pNode->szArrIdx = _szArrIdx;

		return pNode;
	}

	void DestructorNode(Node *pNode)
	{
		//确保正确析构
		pNode->csMemPool.~Pool_class();
		pNode->szArrIdx = szArrBeg;

		csMemPool.FreeMemBlock(pNode);//回收内存
	}

	void SwapFreePool(size_t szLeftIdx,size_t szRightIdx)//交换两个空闲内存池
	{
		std::swap(pNodeArrFreePool[szLeftIdx], pNodeArrFreePool[szRightIdx]);
		pNodeArrFreePool[szLeftIdx]->szArrIdx = szLeftIdx;
		pNodeArrFreePool[szRightIdx]->szArrIdx = szRightIdx;
	}

	size_t BinarySearchSortPool(const void* pFind)//使用uintptr_t在排序池列表中二分查找，返回第一个内存池基地址不大于pFind的排序池列表的索引（左低右高排序）
	{
		size_t szFindBeg = szArrBeg;
		size_t szFindEnd = szArrEnd;
		size_t szFindCur;

		while (szFindEnd - szFindBeg > 1)//<=1时结束循环，此时pFind就在Beg到End中间
		{
			szFindCur = (szArrEnd - szArrBeg) / 2;//计算当前的中点

			long lCmp = pNodeArrSortPool[szFindCur]->csMemPool.CmpPointAndPool(pFind);//比较目标指针和内存池地址
			if (lCmp < 0)//小于
			{
				szFindEnd = szFindCur;//目标在左边，截断End
			}
			else if (lCmp > 0)//大于
			{
				szFindBeg = szFindCur;//目标在右边，截断Beg
			}
			else//等于
			{
				return szFindCur;//直接命中
			}
		}

		return szFindBeg;//Beg恰好小于pFind且End恰好大于pFind，因为是内存基地址排序，恰好大于则证明pFind绝对不在End中，必然返回Beg，即便有可能也不在Beg中
	}

public:
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) ://把_szMemBlockPreAllocNum向上舍入到最近的2的指数次方
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockNum(_szMemBlockPreAllocNum)
	{
		Node *pNewNode = ConstructorNode(szMemBlockFixSize, szMemBlockNum, szArrEnd);

		pNodeArrFreePool[szArrEnd] = pNewNode;
		pNodeArrSortPool[szArrEnd] = pNewNode;

		++szArrEnd;
		szArrLastSwap = szArrEnd;
	}

	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//禁用类拷贝构造

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept//移动构造
	{
		//懒得实现，先不做
	}

	~AutoExpand_FixLen_MemPool(void)
	{
		//一会在做，丢给操作系统清理先
	}

	Type *AllocMemBlock(void)
	{
		Type *pFreeMemBlock;

		//循环直到分配成功或无内存池
		while (true)
		{
			pFreeMemBlock = pNodeArrFreePool[szArrBeg]->csMemPool.AllocMemBlock();
			if (pFreeMemBlock != NULL)
			{
				++szMemBlockUse;//计数菌
				return pFreeMemBlock;
			}

			//如果运行到此代表这个内存池空间已耗尽
			if (szArrLastSwap <= 1)//已经没有空闲内存池可供交换
			{
				break;
			}

			//定位到下一个空闲内存池
			--szArrLastSwap;
			//交换
			SwapFreePool(szArrBeg, szArrLastSwap);
		}

		//运行到此则代表没有可用空闲内存池了，动态分配一个
		if (szArrEnd >= PNODE_ARR_MAX_SIZE)//静态数组满，不能分配
		{
			return NULL;
		}

		//新建内存池，初始化索引为头部索引
		Node *pNewNode = ConstructorNode(szMemBlockFixSize, szMemBlockNum * szExpandMultiple - szMemBlockNum, szArrBeg);
		szMemBlockNum *= szExpandMultiple;

		//插入到free数组头部，并将原先头部内存池放到数组尾部
		pNodeArrFreePool[szArrEnd] = pNodeArrFreePool[szArrBeg];//因为尾部无有效数据，直接覆盖而无需调换
		pNodeArrFreePool[szArrEnd]->szArrIdx = szArrEnd;//设置索引关联
		pNodeArrFreePool[szArrBeg] = pNewNode;//插入头部
		//注意此处还未插入另一数组，暂时不改变szArrEnd

		//插入排序到sort数组（目前暂时不考虑类中无任何内存池的情况）
		size_t szSortInsert = BinarySearchSortPool(pNewNode->csMemPool.GetMemPool()) + 1;//获取到下边界索引+1的插入点索引
		//先移动元素（注意这里操作的是排序数组，Node里的索引与排序数组无关，无需更改）
		memmove(pNodeArrSortPool[szSortInsert + 1], pNodeArrSortPool[szSortInsert], sizeof(*pNodeArrSortPool) * (szArrEnd - szSortInsert));//szArrEnd 
		pNodeArrSortPool[szSortInsert] = pNewNode;//插入

		//两数组都插入完毕，递增szArrEnd
		++szArrEnd;

		//操作完毕，重分配内存
		pFreeMemBlock = pNodeArrFreePool[szArrBeg]->csMemPool.AllocMemBlock();
		if (pFreeMemBlock != NULL)
		{
			++szMemBlockUse;//计数菌
			return pFreeMemBlock;
		}

		return NULL;//卧槽这都能失败？
	}

	bool FreeMemBlock(Type *pAllocMemBlock)
	{
		





	}

	void FreeAllMemBlock(void)//注意，只是释放内存池已分配内存，并不会删除内存池
	{
		
	}

	size_t GetMemBlockFixSize(void) const
	{
		return szMemBlockFixSize;
	}

	size_t GetMemBlockNum(void) const
	{
		return szMemBlockNum;
	}

	size_t GetMemBlockUse(void) const
	{
		return szMemBlockUse;
	}
};