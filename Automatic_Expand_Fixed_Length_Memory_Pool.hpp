#pragma once
#include "Fixed_Length_Memory_Pool.hpp"

//链表扩容法、树扩容法

#define ARR_MAX_SIZE 63

template <typename Pool_class, bool bAutoDelUnuse = false, size_t szExpandMultiple = 2, typename Alloc_func = default_alloc, typename Free_func = default_free>
class AutoExpand_FixLen_MemPool
{
	static_assert(szExpandMultiple >= 2);
private:
	using Type = Pool_class::RetPoint_Type;
	struct Node
	{
		Pool_class csMemPool;
		size_t szIndex;//这个结构在空闲池数组中的索引
	};

	FixLen_MemPool<Node, false, 4, Alloc_func, Free_func> csMemPool = (sizeof(Node), ARR_MAX_SIZE);//用一个内存池来管理后续的内部分配释放

	Node* pstArrFreePool[ARR_MAX_SIZE];//空闲内存池
	Node* pstArrSortPool[ARR_MAX_SIZE];//排序内存池

	size_t szFreePoolEnd = 0;//尾后索引
	size_t szSortPoolEnd = 0;//尾后索引
	size_t szFreePoolLastSwap = 0;//上一次交换的索引

	size_t szMemBlockFixSize = 0;//用户初始化时需要的定长内存长度（size：字节数）

	size_t szMemBlockNum = 0;//内存池中总内存块个数
	size_t szMemBlockUse = 0;//内存池中使用内存块个数
private:
	Node *ConstructorNode(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum, size_t _szIndex)
	{
		Node *pNode = csMemPool.AllocMemBlock();//请求内存

		//构造类
		new(&pNode->csMemPool) Pool_class(_szMemBlockFixSize, _szMemBlockPreAllocNum);//placement new
		//初始化索引
		pNode->szIndex = _szIndex;

		return pNode;
	}

	void DestructorNode(Node *pNode)
	{
		//确保正确析构
		pNode->csMemPool.~Pool_class();
		pNode->szIndex = 0;

		csMemPool.FreeMemBlock(pNode);//回收内存
	}

public:
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) :
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

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept ://移动构造
		//csPoolList(std::move(_Move.csPoolList)),

		szMemBlockFixSize(_Move.szMemBlockFixSize),
		szMemBlockNum(_Move.szMemBlockNum),
		szMemBlockUse(_Move.szMemBlockUse)
	{
		_Move.szMemBlockFixSize = 0;
		_Move.szMemBlockNum = 0;
		_Move.szMemBlockUse = 0;
	}

	~AutoExpand_FixLen_MemPool(void)
	{
		
	}

	Type *AllocMemBlock(void)
	{
		
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

#undef ARR_MAX_SIZE