#pragma once
#include "Fixed_Length_Memory_Pool.hpp"

//链表扩容法、树扩容法


int mainw(void)
{
	size_t szBlock = 1;
	size_t szFixSize = 1;
	size_t szManage = 9;

	size_t i = 0;
	size_t szCurrent = 0;
	size_t Temp;

	for (i = 0; i < 64; ++i)
	{
		Temp = (szManage + szBlock) * szFixSize;
		if (Temp > SIZE_MAX - szCurrent)
		{
			break;
		}

		szCurrent += Temp;
		szBlock *= 2;//倍增
		printf("i:%02zu Cur:%zu\n", i, szCurrent);
	}

	printf("\nTmp:%zu\n", Temp);

	return 0;
}

#define PNODE_ARR_MAX_SIZE 42

template <typename Pool_class, bool bAutoDelUnuse = false, size_t szExpandMultiple = 2, typename Alloc_func = default_alloc, typename Free_func = default_free>
class AutoExpand_FixLen_MemPool
{
	static_assert(szExpandMultiple >= 2);
private:
	using Type = Pool_class::RetPoint_Type;
	struct Node
	{
		Pool_class csMemPool;
		size_t szFreePoolIdx;//这个结构在空闲池数组中的索引
	};

	FixLen_MemPool<Node, false, 4, Alloc_func, Free_func> csMemPool = (sizeof(Node), PNODE_ARR_MAX_SIZE);//用一个内存池来管理后续的内部分配释放

	Node* pstArrFreePool[PNODE_ARR_MAX_SIZE];//空闲内存池
	Node* pstArrSortPool[PNODE_ARR_MAX_SIZE];//排序内存池

	static constexpr size_t szFreePoolBeg = 0;//头部索引
	static constexpr size_t szSortPoolBeg = 0;//头部索引
	size_t szFreePoolEnd = 0;//尾后索引
	size_t szSortPoolEnd = 0;//尾后索引
	size_t szFreePoolLastSwap = 0;//上一次交换的索引

	size_t szMemBlockFixSize = 0;//用户初始化时需要的定长内存长度（size：字节数）

	size_t szMemBlockNum = 0;//内存池中总内存块个数
	size_t szMemBlockUse = 0;//内存池中使用内存块个数
private:
	Node *ConstructorNode(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum, size_t _szFreePoolIdx)
	{
		Node *pNode = csMemPool.AllocMemBlock();//请求内存

		//构造类
		new(&pNode->csMemPool) Pool_class(_szMemBlockFixSize, _szMemBlockPreAllocNum);//placement new
		//初始化索引
		pNode->szFreePoolIdx = _szFreePoolIdx;

		return pNode;
	}

	void DestructorNode(Node *pNode)
	{
		//确保正确析构
		pNode->csMemPool.~Pool_class();
		pNode->szFreePoolIdx = szFreePoolBeg;

		csMemPool.FreeMemBlock(pNode);//回收内存
	}

	void SwapFreePool(size_t szLeftIdx,size_t szRightIdx)//交换两个空闲内存池
	{
		std::swap(pstArrFreePool[szLeftIdx], pstArrFreePool[szRightIdx]);
		pstArrFreePool[szLeftIdx]->szFreePoolIdx = szLeftIdx;
		pstArrFreePool[szRightIdx]->szFreePoolIdx = szRightIdx;
	}

public:
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) ://把_szMemBlockPreAllocNum向上舍入到最近的2的指数次方
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockNum(_szMemBlockPreAllocNum)
	{
		Node *pNewNode = ConstructorNode(szMemBlockFixSize, szMemBlockNum, szFreePoolEnd);

		pstArrFreePool[szFreePoolEnd] = pNewNode;
		pstArrSortPool[szSortPoolEnd] = pNewNode;

		++szFreePoolEnd;
		++szSortPoolEnd;

		szFreePoolLastSwap = szFreePoolEnd;
	}

	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//禁用类拷贝构造

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept//移动构造
	{

	}

	~AutoExpand_FixLen_MemPool(void)
	{
		
	}

	Type *AllocMemBlock(void)
	{
		while (true)
		{
			Type *pFreeMemBlock = pstArrFreePool[szFreePoolBeg]->csMemPool.AllocMemBlock();
			if (pFreeMemBlock != NULL)
			{
				return pFreeMemBlock;
			}

			//如果运行到此代表这个内存池空间已耗尽
			if (szFreePoolLastSwap <= 1)//已经没有空闲内存池可供交换
			{
				break;
			}

			//定位到下一个空闲内存池
			--szFreePoolLastSwap;
			//交换
			SwapFreePool(szFreePoolBeg, szFreePoolLastSwap);
		}

		//运行到此则代表没有可用空闲内存池了，动态分配一个
		if (szFreePoolEnd < PNODE_ARR_MAX_SIZE)//静态数组没满，还能分配
		{
			//把当前内存池调到尾部并直接覆盖
			pstArrFreePool[szFreePoolEnd] = pstArrFreePool[szFreePoolBeg];
			pstArrFreePool[szFreePoolEnd]->szFreePoolIdx = szFreePoolEnd;
			--szFreePoolEnd;
			//新建内存池
			pstArrFreePool[szFreePoolBeg] = ConstructorNode(szMemBlockFixSize, szMemBlockNum, szFreePoolBeg);
			szMemBlockNum *= szExpandMultiple;





		}








	}

	bool FreeMemBlock(Type *pAllocMemBlock)
	{
		
	}

	void FreeAllMemBlock(void)//注意，只是释放内存池已分配内存，并不会删除内存池
	{
		
	}

	void DeleteAllUnusePool(void)//删除所有未使用的内存池类
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

#undef PNODE_ARR_MAX_SIZE