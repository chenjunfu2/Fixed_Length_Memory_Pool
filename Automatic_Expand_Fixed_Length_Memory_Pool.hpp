#pragma once
#include "Fixed_Length_Memory_Pool.hpp"

//�������ݷ��������ݷ�


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
		szBlock *= 2;//����
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
		size_t szFreePoolIdx;//����ṹ�ڿ��г������е�����
	};

	FixLen_MemPool<Node, false, 4, Alloc_func, Free_func> csMemPool = (sizeof(Node), PNODE_ARR_MAX_SIZE);//��һ���ڴ��������������ڲ������ͷ�

	Node* pstArrFreePool[PNODE_ARR_MAX_SIZE];//�����ڴ��
	Node* pstArrSortPool[PNODE_ARR_MAX_SIZE];//�����ڴ��

	static constexpr size_t szFreePoolBeg = 0;//ͷ������
	static constexpr size_t szSortPoolBeg = 0;//ͷ������
	size_t szFreePoolEnd = 0;//β������
	size_t szSortPoolEnd = 0;//β������
	size_t szFreePoolLastSwap = 0;//��һ�ν���������

	size_t szMemBlockFixSize = 0;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����

	size_t szMemBlockNum = 0;//�ڴ�������ڴ�����
	size_t szMemBlockUse = 0;//�ڴ����ʹ���ڴ�����
private:
	Node *ConstructorNode(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum, size_t _szFreePoolIdx)
	{
		Node *pNode = csMemPool.AllocMemBlock();//�����ڴ�

		//������
		new(&pNode->csMemPool) Pool_class(_szMemBlockFixSize, _szMemBlockPreAllocNum);//placement new
		//��ʼ������
		pNode->szFreePoolIdx = _szFreePoolIdx;

		return pNode;
	}

	void DestructorNode(Node *pNode)
	{
		//ȷ����ȷ����
		pNode->csMemPool.~Pool_class();
		pNode->szFreePoolIdx = szFreePoolBeg;

		csMemPool.FreeMemBlock(pNode);//�����ڴ�
	}

	void SwapFreePool(size_t szLeftIdx,size_t szRightIdx)//�������������ڴ��
	{
		std::swap(pstArrFreePool[szLeftIdx], pstArrFreePool[szRightIdx]);
		pstArrFreePool[szLeftIdx]->szFreePoolIdx = szLeftIdx;
		pstArrFreePool[szRightIdx]->szFreePoolIdx = szRightIdx;
	}

public:
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) ://��_szMemBlockPreAllocNum�������뵽�����2��ָ���η�
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

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept//�ƶ�����
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

			//������е��˴�������ڴ�ؿռ��Ѻľ�
			if (szFreePoolLastSwap <= 1)//�Ѿ�û�п����ڴ�ؿɹ�����
			{
				break;
			}

			//��λ����һ�������ڴ��
			--szFreePoolLastSwap;
			//����
			SwapFreePool(szFreePoolBeg, szFreePoolLastSwap);
		}

		//���е��������û�п��ÿ����ڴ���ˣ���̬����һ��
		if (szFreePoolEnd < PNODE_ARR_MAX_SIZE)//��̬����û�������ܷ���
		{
			//�ѵ�ǰ�ڴ�ص���β����ֱ�Ӹ���
			pstArrFreePool[szFreePoolEnd] = pstArrFreePool[szFreePoolBeg];
			pstArrFreePool[szFreePoolEnd]->szFreePoolIdx = szFreePoolEnd;
			--szFreePoolEnd;
			//�½��ڴ��
			pstArrFreePool[szFreePoolBeg] = ConstructorNode(szMemBlockFixSize, szMemBlockNum, szFreePoolBeg);
			szMemBlockNum *= szExpandMultiple;





		}








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

#undef PNODE_ARR_MAX_SIZE