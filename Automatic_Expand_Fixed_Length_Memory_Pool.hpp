#pragma once
#include "Fixed_Length_Memory_Pool.hpp"

//�������ݷ��������ݷ�

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
		size_t szIndex;//����ṹ�ڿ��г������е�����
	};

	FixLen_MemPool<Node, false, 4, Alloc_func, Free_func> csMemPool = (sizeof(Node), ARR_MAX_SIZE);//��һ���ڴ��������������ڲ������ͷ�

	Node* pstArrFreePool[ARR_MAX_SIZE];//�����ڴ��
	Node* pstArrSortPool[ARR_MAX_SIZE];//�����ڴ��

	size_t szFreePoolEnd = 0;//β������
	size_t szSortPoolEnd = 0;//β������
	size_t szFreePoolLastSwap = 0;//��һ�ν���������

	size_t szMemBlockFixSize = 0;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����

	size_t szMemBlockNum = 0;//�ڴ�������ڴ�����
	size_t szMemBlockUse = 0;//�ڴ����ʹ���ڴ�����
private:
	Node *ConstructorNode(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum, size_t _szIndex)
	{
		Node *pNode = csMemPool.AllocMemBlock();//�����ڴ�

		//������
		new(&pNode->csMemPool) Pool_class(_szMemBlockFixSize, _szMemBlockPreAllocNum);//placement new
		//��ʼ������
		pNode->szIndex = _szIndex;

		return pNode;
	}

	void DestructorNode(Node *pNode)
	{
		//ȷ����ȷ����
		pNode->csMemPool.~Pool_class();
		pNode->szIndex = 0;

		csMemPool.FreeMemBlock(pNode);//�����ڴ�
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

	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//�����࿽������

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept ://�ƶ�����
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

	void FreeAllMemBlock(void)//ע�⣬ֻ���ͷ��ڴ���ѷ����ڴ棬������ɾ���ڴ��
	{
		
	}

	void DeleteAllUnusePool(void)//ɾ������δʹ�õ��ڴ����
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