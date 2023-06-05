#pragma once
#include "Fixed_Length_Memory_Pool.hpp"

//�������ݷ��������ݷ�

/*
��װFixLen_MemPool�࣬���Զ����������µ���ʵ��
�������������飬һ���ǿ����ڴ�أ���һ���������ڴ��

���䣺
ÿ�η��䶼�ӿ����ڴ������szArrBeg�����ڴ�������ڴ棬�������ʧ������--szArrLastSwap���Ŀ����ڴ�ؽ���
Ҳ����˵����szArrLastSwap��szArrEnd֮��ȫ�����������ڴ�أ���szArrBeg��szArrLastSwap֮��ȫ������δ�����ڴ�أ�szArrLastSwap�൱������ķָ���

�ڽ��������У�Ҫ�ȼ��szArrLastSwap�Ƿ�<=1���������������޿����ڴ���ˣ���ʱ�����Ҫ����szExpandMultiple�������ݣ�����һ���µ��ڴ��
������п����ڴ�أ��򽻻�������������NodeԪ�ص�ָ�벢�����ǵ�szArrIdx�޸�Ϊ��ǰ�����ڵ�����λ�õ�����

�������ڴ��ʱ�����ڴ�ص�block��Ϊ����ǰ�����ڴ��block�ܺ�*szExpandMultiple-��ǰ�����ڴ��block�ܺͣ�Ҳ���ǵ�ǰ�����ڴ��block�ܺ�����szExpandMultiple�������ӵ�block��
���szArrEnd����������ľ�̬������С�����½����ڴ�ط���szArrBeg����ԭ��szArrBeg����Node����szArrEnd����++szArrEnd������������Node���������������Ȼ�����ö��ַ�ͨ���ڴ����ʼ��ַ
��pNodeArrSortPool�ҵ��ʵ���λ�ý��в������򣬺����Ԫ��ȫ��ʹ��memmove�����ƶ�

�ͷţ�
��鵱ǰ�ͷ�ָ���ǲ���nullptr���Ǿ�ֱ�ӷ���true

����nullptr��ʹ�ͷ�ָ���ö��ַ�������pNodeArrSortPool�У���ΪpNodeArrSortPool�Ǹ����ڴ�ص�ַ����ģ����Կ��Կ����ҵ�ָ�������ڴ�أ�������Ϊ�ڴ�ص�ַ����ʼ��ַ��ָ���������ʼ��ַ
��������ַ֮�䣬���Զ��ַ�����һ���Ǿ�ȷ���ҵ�����ƥ��ķ�����Ҳ���ڸպ�ƥ�䣬�����ڴ����ʼ��ַ���Ǹ�block�պ������ͷţ�������ֻҪ���ֲ����е�beg��end���С����1���߸պ�ƥ�䣬
�����ǰ�ͷ�ָ����beg������ָ���ڴ���У���Ϊ����beg�ĵ�ַ������С��end�ĵ�ַ������Ϊ�ڴ�ذ����ڴ�ص���ʼ��ַ����ע�ⲻ���ڴ����ĵ�ַ�������ڳ�֮�ĵ�ַ�������Կ����Ƴ����ָ������beg������ָ���ڴ�أ�
Ȼ���������ڴ�ص��ͷź����ͷŸ�ָ��

���ͷ���ɺ�����ͷź�������false���򷵻�false

����false����ͨ����������õ���Node�ṹ�е�szArrIdx��øó���pNodeArrFreePool�е�λ�ã����ж�szArrLastSwap������λ�õĹ�ϵ�������szArrLastSwap>szArrIdx����ó����ڿ��������ڣ��������ֱ�ӷ���true
�����Լ���szArrLastSwap������ָ�뽻����������szArrIdx��Ȼ��++szArrLastSwap��ȷ���Լ��������������

*/

template <typename Pool_class, size_t szExpandMultiple = 2, size_t szAlignment = 4, size_t szAlignBlockNum = 2, typename Alloc_func = default_alloc, typename Free_func = default_free >
class AutoExpand_FixLen_MemPool
{
	static_assert(szExpandMultiple >= 2);
	static_assert(szAlignBlockNum == 1 || (szAlignBlockNum != 0 && szAlignBlockNum % 2 == 0));
protected:
	static constexpr size_t ArrMaxNum(void)//���¹��ƣ�����ʱ���㣩
	{
		size_t szBlockNum = 1;//�ڴ������
		size_t szCumulative = 0;//�ܴ�С

		size_t szNum = 0;
		while (szNum < 512)
		{
			//���㵱ǰ�ܴ�С
			szCumulative = Pool_class::szManageMemBlockRequireSize * szBlockNum;
			if (szCumulative / szBlockNum != Pool_class::szManageMemBlockRequireSize)//���
			{
				break;
			}
			
			szCumulative += szBlockNum;
			if (szCumulative < szBlockNum)//���
			{
				break;
			}
			
			szCumulative += sizeof(Pool_class) * szNum;
			if (szCumulative < sizeof(Pool_class) * szNum)//���
			{
				break;
			}

			szBlockNum *= szExpandMultiple;//����
			++szNum;
		}

		//�����СΪszNum
		return szNum;
	}

	static constexpr size_t PNODE_ARR_MAX_NUM = ArrMaxNum();//61

	struct Node
	{
		Pool_class csMemPool;
		size_t szArrIdx;//����ṹ�ڿ��г������е�����
	};

	using Type = Pool_class::RetPoint_Type;
	using Manage_Pool = FixLen_MemPool<Node, false, szAlignment, Alloc_func, Free_func>;

private:
	Node *pNodeArrFreePool[PNODE_ARR_MAX_NUM] = {0};//�����ڴ�أ�szArrLastSwap������ߣ���������ָ����Ϊ�����ڴ�أ��ұߣ�������ָ����Ϊ�����ڴ�أ�
	Node* pNodeArrSortPool[PNODE_ARR_MAX_NUM] = {0};//�����ڴ�أ����ڴ�ص�ַ����Node������Ҹߣ�

	static constexpr size_t szArrBeg = 0;//ͷ������
	size_t szArrEnd = 0;//β������
	size_t szArrLastSwap = 0;//��һ�ν���������

	Manage_Pool csMemPool = Manage_Pool(sizeof(Node), PNODE_ARR_MAX_NUM);//��һ���ڴ��������������ڲ������ͷ�

	size_t szMemBlockFixSize = 0;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����
	size_t szMemBlockNum = 0;//�ڴ�������ڴ�����
	size_t szMemBlockUse = 0;//�ڴ����ʹ���ڴ�����
private:
	Node *ConstructorNode(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum, size_t _szArrIdx)
	{
		Node *pNode = csMemPool.AllocMemBlock();//�����ڴ�

		//������
		new(&pNode->csMemPool) Pool_class(_szMemBlockFixSize, _szMemBlockPreAllocNum);//placement new
		//��ʼ������
		pNode->szArrIdx = _szArrIdx;

		return pNode;
	}

	void DestructorNode(Node *pNode)
	{
		//ȷ����ȷ����
		pNode->csMemPool.~Pool_class();
		pNode->szArrIdx = szArrBeg;

		csMemPool.FreeMemBlock(pNode);//�����ڴ�
	}

	void SwapFreePool(size_t szLeftIdx, size_t szRightIdx)//�������������ڴ��
	{
		std::swap(pNodeArrFreePool[szLeftIdx], pNodeArrFreePool[szRightIdx]);
		pNodeArrFreePool[szLeftIdx]->szArrIdx = szLeftIdx;
		pNodeArrFreePool[szRightIdx]->szArrIdx = szRightIdx;
	}

	void MoveFreePool(size_t szTargetIdx, size_t szSourceIdx)
	{
		if (szTargetIdx != szSourceIdx)
		{
			pNodeArrFreePool[szTargetIdx] = pNodeArrFreePool[szSourceIdx];//ֱ�ӿ�����Ŀ��λ��
			pNodeArrFreePool[szTargetIdx]->szArrIdx = szTargetIdx;//������������
			pNodeArrFreePool[szSourceIdx] = NULL;//�����Ա
		}
	}

	size_t BinarySearchSortPool(const void* pFind) const//ʹ��uintptr_t��������б��ж��ֲ��ң����ص�һ���ڴ�ػ���ַ������pFind��������б������������Ҹ�����
	{
		size_t szFindBeg = szArrBeg;
		size_t szFindEnd = szArrEnd;//ע������End����β���������ҷ���ֵ���������������ζ�����ȡ����

		while (szFindEnd - szFindBeg > 1)//<=1ʱ����ѭ������ʱpFind����Beg��End�м�
		{
			size_t szFindCur = (szFindBeg + szFindEnd) / 2;//���㵱ǰ���е�

			long lCmp = pNodeArrSortPool[szFindCur]->csMemPool.CmpPointAndPool(pFind);//�Ƚ�Ŀ��ָ����ڴ�ص�ַ
			if (lCmp < 0)//С��
			{
				szFindEnd = szFindCur;//Ŀ������ߣ��ض�End
			}
			else if (lCmp > 0)//����
			{
				szFindBeg = szFindCur;//Ŀ�����ұߣ��ض�Beg
			}
			else//����
			{
				return szFindCur;//ֱ������
			}
		}

		return szFindBeg;//Begǡ��С��pFind��Endǡ�ô���pFind����Ϊ���ڴ����ַ����ǡ�ô�����֤��pFind���Բ���End�У���Ȼ����Beg�������п���Ҳ����Beg��
	}

	void AllocAndInsertNodeToArr(size_t szNewPoolBlockNum, bool bInsertHead)//trueΪͷ�����룬falseΪszArrLastSwap����
	{
		//����ڵ�
		size_t szInsertIdx = bInsertHead ? szArrBeg : szArrLastSwap;
		Node *pInsertNode = ConstructorNode(szMemBlockFixSize, szNewPoolBlockNum, szInsertIdx);//�½��ڴ�أ���ʼ������
		szMemBlockNum += pInsertNode->csMemPool.GetMemBlockNum();//���ӵ�ǰ�ܽڵ����Ŀ

		//���뵽szInsertIdx������ԭ��ͷ���ڴ�طŵ�����β��
		MoveFreePool(szArrEnd, szInsertIdx);//��Ϊβ������Ч���ݣ�ֱ�Ӹ��Ƕ��������
		pNodeArrFreePool[szInsertIdx] = pInsertNode;//����szInsertIdx

		if (bInsertHead == false)
		{
			++szArrLastSwap;//�ƶ��߽�
		}

		//ע��˴���δ������һ���飬��ʱ���ı�szArrEnd���Ҵ˲�����̲�Ӱ��szArrLastSwap

		long lFindBeg = szArrBeg;
		long lFindEnd = (long)szArrEnd - 1;

		while (lFindBeg <= lFindEnd)//>ʱ����ѭ������ʱlFindBeg���Ƿ��ϵĲ����
		{
			long lFindCur = (lFindBeg + lFindEnd) / 2;//���㵱ǰ���е�

			long lCmp = pNodeArrSortPool[lFindCur]->csMemPool.CmpPointAndPool(pInsertNode->csMemPool.GetMemPool());//�Ƚ��½ڵ���ڴ�ص�ַ
			if (lCmp < 0)//С��
			{
				lFindEnd = lFindCur - 1;//Ŀ������ߣ��ض�End��Cur-1
			}
			else//����
			{
				lFindBeg = lFindCur + 1;//Ŀ�����ұߣ��ض�Beg��Cur+1
			}
		}

		//���ƶ�Ԫ�أ�ע��������������������飬Node������������������޹أ�������ģ�
		memmove(&pNodeArrSortPool[lFindBeg + 1], &pNodeArrSortPool[lFindBeg], sizeof(*pNodeArrSortPool) * (szArrEnd - lFindBeg));//szArrEnd 
		pNodeArrSortPool[lFindBeg] = pInsertNode;//����


		//�����鶼������ϣ�����szArrEnd
		++szArrEnd;
	}

	void RemoveAndFreeNodeFromArr(size_t szSortRemoveIdx)//ע�������IdxΪSort�����Idx������Free�����Idx
	{
		//�ȱ����ɾ��ָ��
		Node *pRemoveNode = pNodeArrSortPool[szSortRemoveIdx];

		//�ƶ�Ԫ�ش�Sort������ɾ��
		memmove(&pNodeArrSortPool[szSortRemoveIdx], &pNodeArrSortPool[szSortRemoveIdx + 1], sizeof(*pNodeArrSortPool) * (szArrEnd - (szSortRemoveIdx + 1)));
		pNodeArrSortPool[szArrEnd - 1] = NULL;//�����Ա

		// ע��˴���δ����һ������ɾ������ʱ���ı�szArrEnd

		//��Free������ɾ��������memmoveԪ��ɾ������ΪNode�е�Idx��Ԫ�ص�ǰ����λ������ֵ�󶨣���ʹ�õ�Ԫ���ƶ�����ɾ��
		size_t szFreeRemoveIdx = pRemoveNode->szArrIdx;

		//�����������szArrLastSwap�������szArrLastSwap�Ϻ��ұ�
		if (szFreeRemoveIdx < szArrLastSwap)
		{
			MoveFreePool(szFreeRemoveIdx, szArrLastSwap - 1);//�ƶ�szArrLastSwap�߽�ǰ��һ��Ԫ�ص�ɾ��λ��
			MoveFreePool(szArrLastSwap - 1, szArrEnd - 1);//�ƶ����һ��Ԫ�ص�szArrLastSwap�߽�ǰ��һ��Ԫ��λ��
			--szArrLastSwap;//�ƶ��߽�szArrLastSwap
		}
		else//>=
		{
			MoveFreePool(szFreeRemoveIdx, szArrEnd - 1);//�ƶ����һ��Ԫ�ص�ɾ��λ�ã���ɣ��������szArrLastSwap
		}

		//�����鶼ɾ����ϣ��ݼ�szArrEnd
		--szArrEnd;

		//���ٵ�ǰ�ܽڵ����Ŀ
		szMemBlockNum -= pRemoveNode->csMemPool.GetMemBlockNum();

		//�ͷŽڵ�
		DestructorNode(pRemoveNode);
	}

public:
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) ://��_szMemBlockPreAllocNum�������뵽�����2��ָ���η�
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockNum(Pool_class::Aligned(_szMemBlockPreAllocNum, szAlignBlockNum))
	{
		Node *pNewNode = ConstructorNode(szMemBlockFixSize, szMemBlockNum, szArrEnd);

		pNodeArrFreePool[szArrEnd] = pNewNode;
		pNodeArrSortPool[szArrEnd] = pNewNode;

		++szArrEnd;
		szArrLastSwap = szArrEnd;
	}

	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//�����࿽������

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept://�ƶ�����
		szArrEnd(_Move.szArrEnd),
		szArrLastSwap(_Move.szArrLastSwap),

		csMemPool(std::move(_Move.csMemPool)),

		szMemBlockFixSize(_Move.szMemBlockFixSize),
		szMemBlockNum(_Move.szMemBlockNum),
		szMemBlockUse(_Move.szMemBlockUse)
	{
		//��������
		memcpy(pNodeArrFreePool, _Move.pNodeArrFreePool, sizeof(pNodeArrFreePool));
		memcpy(pNodeArrSortPool, _Move.pNodeArrSortPool, sizeof(pNodeArrSortPool));

		//��ճ�Ա
		_Move.szArrEnd = 0;
		_Move.szArrLastSwap = 0;

		_Move.szMemBlockFixSize = 0;
		_Move.szMemBlockNum = 0;
		_Move.szMemBlockUse = 0;
	}

	~AutoExpand_FixLen_MemPool(void)
	{
		//�������������ڴ��	
		for (size_t i = szArrBeg; i < szArrEnd; ++i)
		{
			DestructorNode(pNodeArrSortPool[i]);//�������������ڴ�
		}
		//�������������õĶ���ͬһ���ڴ棬������������һ������

		szArrEnd = 0;
		szArrLastSwap = 0;

		szMemBlockFixSize = 0;
		szMemBlockNum = 0;
		szMemBlockUse = 0;
	}

	Type *AllocMemBlock(void)
	{
		Type *pFreeMemBlock;

		//ѭ��ֱ������ɹ������ڴ��
		while (true)
		{
			pFreeMemBlock = pNodeArrFreePool[szArrBeg]->csMemPool.AllocMemBlock();
			if (pFreeMemBlock != NULL)
			{
				//����ɹ����ٻ�������
				++szMemBlockUse;//������
				return pFreeMemBlock;
			}

			//������е��˴�������ڴ�ؿռ��Ѻľ�
			if (szArrLastSwap <= 1)//�Ѿ�û�п����ڴ�ؿɹ�����
			{
				break;
			}

			//��λ����һ�������ڴ��
			--szArrLastSwap;
			//����
			SwapFreePool(szArrBeg, szArrLastSwap);
		}

		//���е��������û�п��ÿ����ڴ���ˣ���̬����һ��
		if (szArrEnd >= PNODE_ARR_MAX_NUM)//��̬�����������ܷ���
		{
			return NULL;
		}
		
		//�����з���Ĵ�С
		size_t szNewPoolBlockNum = Pool_class::Aligned(szMemBlockNum * szExpandMultiple, szAlignBlockNum) - szMemBlockNum;

		//���䲢���뵽����
		AllocAndInsertNodeToArr(szNewPoolBlockNum, true);

		//������ϣ��ط����ڴ�
		pFreeMemBlock = pNodeArrFreePool[szArrBeg]->csMemPool.AllocMemBlock();
		if (pFreeMemBlock != NULL)
		{
			//����ɹ����ٻ�������
			++szMemBlockUse;//������
			return pFreeMemBlock;
		}

		return NULL;//�Բ��ⶼ��ʧ�ܣ�
	}

	bool FreeMemBlock(Type *pAllocMemBlock)
	{
		if (pAllocMemBlock == NULL)
		{
			return true;
		}

		//����ָ���������ڴ������
		size_t szMemBelongSort = BinarySearchSortPool(pAllocMemBlock);
		Node *pFreeNode = pNodeArrSortPool[szMemBelongSort];
		//�ͷ�
		if (pFreeNode->csMemPool.FreeMemBlock(pAllocMemBlock) == false)
		{
			return false;//�ͷ�ʧ�ܣ������������
		}

		//�ͷųɹ����ٻ�������
		--szMemBlockUse;//������

		//��ȡ�ڿ����ڴ���е�����
		size_t szFreeIdx = pFreeNode->szArrIdx;
		if (szArrLastSwap > szFreeIdx)//�ó����ڿ��������������ֱ�ӷ���
		{
			return true;
		}

		//�ó������ڴ�������szArrLastSwap����
		if (szArrLastSwap != szFreeIdx)//�������Լ�����
		{
			//�����������
			SwapFreePool(szFreeIdx, szArrLastSwap);
		}

		//����ʹ�����ı߽�Խ������
		++szArrLastSwap;

		return true;
	}

	bool Capacity(void)//��ģ�屶������1��
	{
		if (szArrEnd >= PNODE_ARR_MAX_NUM)
		{
			return false;//��������
		}

		//�����ڴ�
		size_t szNewPoolBlockNum = Pool_class::Aligned(szMemBlockNum * szExpandMultiple, szAlignBlockNum) - szMemBlockNum;//�������

		//���������
		AllocAndInsertNodeToArr(szNewPoolBlockNum, false);

		return true;
	}

	bool AddNewMemPool(size_t _szMemBlockPreAllocNum)//����_szMemBlockPreAllocNum��ģ�����Ҫ������һ�������ڴ��
	{
		if (szArrEnd >= PNODE_ARR_MAX_NUM)
		{
			return false;//��������
		}

		//�����ڴ�
		size_t szNewPoolBlockNum = Pool_class::Aligned(_szMemBlockPreAllocNum, szAlignBlockNum);//�������

		//���������
		AllocAndInsertNodeToArr(szNewPoolBlockNum, false);

		return true;
	}


	static bool default_remove(const Pool_class &c)
	{
		if (c.GetMemBlockUse() == 0 && c.GetMemBlockNum() < 64)
		{
			return true;//ɾ��
		}
		return false;//��ɾ��
	}

	template<typename Unary_Predicates = bool (*)(const Pool_class &)>
	size_t RemoveEligibleMemPool(Unary_Predicates upFunc = default_remove)
	{
		size_t szRemoveCount = 0;
		for (size_t i = szArrBeg; i < szArrEnd; ++i)
		{
			if (upFunc(pNodeArrSortPool[i]->csMemPool) == true)
			{
				RemoveAndFreeNodeFromArr(i);//ɾ���ڵ�
				++szRemoveCount;//����ɾ������
			}
		}

		return szRemoveCount;
	}

	static bool default_traverse(const Pool_class &c)
	{
		//use c
		return true;//��������
		return false;//��Ҫ�ٱ����ˣ�����ѭ��
	}

	template<typename Unary_Predicates = bool (*)(const Pool_class &)>
	size_t TraverseEligibleMemPool(Unary_Predicates upFunc = default_traverse)
	{
		for (size_t i = szArrBeg; i < szArrEnd; ++i)
		{
			if (upFunc(pNodeArrSortPool[i]->csMemPool) == false)
			{
				break;
			}
		}
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

	size_t GetPoolNum(void) const
	{
		return szArrEnd;
	}

	size_t GetFreePoolNum(void) const
	{
		return szArrLastSwap;
	}

	size_t GetFullPoolNum(void) const
	{
		return szArrEnd - szArrLastSwap;
	}
};