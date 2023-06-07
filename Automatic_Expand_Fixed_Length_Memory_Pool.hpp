#pragma once
#include "Fixed_Length_Memory_Pool.hpp"

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

template <
	typename Pool_class,//�ڴ������
	size_t szExpandMultiple = 2,//���ݱ���
	size_t szAlignBlockNum = 2,//�ڴ���ڴ���������ı߽�
	size_t szAlignment = 4,//�����ڴ�صĶ���߽�
	typename Alloc_func = default_alloc,//�����ڴ�ص�Ĭ�Ϸ�����
	typename Free_func = default_free>//�����ڴ�ص�Ĭ���ͷ���
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
	Node *pNodeArrSortPool[PNODE_ARR_MAX_NUM] = {0};//�����ڴ�أ����ڴ�ص�ַ����Node������Ҹߣ�

	static constexpr size_t szArrBeg = 0;//ͷ������
	size_t szArrEnd = 0;//β������
	size_t szArrLastSwap = 0;//��һ�ν���������

	Manage_Pool csMemPool = Manage_Pool(sizeof(Node), PNODE_ARR_MAX_NUM);//��һ���ڴ��������������ڲ������ͷ�

	size_t szMemBlockPreAllocNum = 0;//�û���ʼ��ʱָ����Ԥ�����С
	size_t szMemBlockFixSize = 0;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����
	size_t szMemBlockNum = 0;//�ڴ�������ڴ�����
	size_t szMemBlockUse = 0;//�ڴ����ʹ���ڴ�����
private:
	//���첢����һ���ڵ㣬���ڹ����µ��ڴ��
	Node *ConstructorNode(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum, size_t _szArrIdx) noexcept
	{
		Node *pNode = csMemPool.AllocMemBlock();//�����ڴ�

		//������
		new(&pNode->csMemPool) Pool_class(_szMemBlockFixSize, _szMemBlockPreAllocNum);//placement new
		//��ʼ������
		pNode->szArrIdx = _szArrIdx;

		return pNode;
	}

	//����������һ���ڵ㣬�������������ڴ��
	void DestructorNode(Node *pNode) noexcept
	{
		//ȷ����ȷ����
		pNode->csMemPool.~Pool_class();
		pNode->szArrIdx = szArrBeg;

		csMemPool.FreeMemBlock(pNode);//�����ڴ�
	}

	//����FreePool�����ڵ������ڵ�
	void SwapFreePool(size_t szLeftIdx, size_t szRightIdx) noexcept//�������������ڴ��
	{
		std::swap(pNodeArrFreePool[szLeftIdx], pNodeArrFreePool[szRightIdx]);
		pNodeArrFreePool[szLeftIdx]->szArrIdx = szLeftIdx;
		pNodeArrFreePool[szRightIdx]->szArrIdx = szRightIdx;
	}

	//�ƶ�FreePool�����ڵ�һ��Դ�ڵ㵽Ŀ������λ��
	void MoveFreePool(size_t szTargetIdx, size_t szSourceIdx) noexcept
	{
		if (szTargetIdx != szSourceIdx)
		{
			pNodeArrFreePool[szTargetIdx] = pNodeArrFreePool[szSourceIdx];//ֱ�ӿ�����Ŀ��λ��
			pNodeArrFreePool[szTargetIdx]->szArrIdx = szTargetIdx;//������������
			pNodeArrFreePool[szSourceIdx] = NULL;//�����Ա
		}
	}

	//��SortPool�����ڶ��ֲ���ָ���������ڴ��
	size_t BinarySearchSortPool(const void *pFind) const noexcept//ʹ��uintptr_t��������б��ж��ֲ��ң����ص�һ���ڴ�ػ���ַ������pFind��������б������������Ҹ�����
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

	//�����µ��ڴ�ز�������������
	void AllocAndInsertNodeToArr(size_t szNewPoolBlockNum) noexcept
	{
		//����ڵ�
		Node *pInsertNode = ConstructorNode(szMemBlockFixSize, szNewPoolBlockNum, szArrLastSwap);//�½��ڴ�أ���ʼ������
		szMemBlockNum += pInsertNode->csMemPool.GetMemBlockNum();//���ӵ�ǰ�ܽڵ����Ŀ

		//���뵽szArrLastSwap������ԭ��ͷ���ڴ�طŵ�����β��
		MoveFreePool(szArrEnd, szArrLastSwap);//��Ϊβ������Ч���ݣ�ֱ�Ӹ��Ƕ��������
		pNodeArrFreePool[szArrLastSwap] = pInsertNode;//����szArrLastSwap
		++szArrLastSwap;//�ƶ��߽�

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

	//�Ƴ��ѷ�����ڴ�ز���������ɾ��
	void RemoveAndFreeNodeFromArr(size_t szSortRemoveIdx) noexcept//ע�������IdxΪSort�����Idx������Free�����Idx
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
		if (szFreeRemoveIdx < szArrLastSwap)//�����
		{
			MoveFreePool(szFreeRemoveIdx, szArrLastSwap - 1);//�ƶ�szArrLastSwap�߽�ǰ��һ��Ԫ�ص�ɾ��λ��
			MoveFreePool(szArrLastSwap - 1, szArrEnd - 1);//�ƶ����һ��Ԫ�ص�szArrLastSwap�߽�ǰ��һ��Ԫ��λ��
			--szArrLastSwap;//�ƶ��߽�szArrLastSwap
		}
		else//>=���ϻ��ұ�
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

	void ResetAndMoveNodeFromFreeArr(size_t szSortResetIdx)
	{
		size_t szFreeResetIdx = pNodeArrSortPool[szSortResetIdx]->szArrIdx;

		szMemBlockUse -= pNodeArrFreePool[szFreeResetIdx]->csMemPool.GetMemBlockUse();//���ٵ�ǰ��ʹ�ü���
		pNodeArrFreePool[szFreeResetIdx]->csMemPool.Reset();//���ýڵ�

		//�����������szArrLastSwap�������szArrLastSwap�Ϻ��ұ�
		if (szFreeResetIdx < szArrLastSwap)//�����
		{
			return;//ֱ�ӷ���
		}

		if (szFreeResetIdx != szArrLastSwap)//�������Լ�����
		{
			SwapFreePool(szFreeResetIdx, szArrLastSwap);//�������λ�ñ߽�
		}
		
		++szArrLastSwap;//�ƶ��߽�
	}

public:
	using RetPoint_Type = Type;
	static constexpr size_t szMaxMemPool = PNODE_ARR_MAX_NUM;//��������ܴ��µ��ڴ�ظ���
	static constexpr size_t szManageMemPoolRequireSize = sizeof(Node) * 2;//����һ���ڴ������Ĺ����ڴ��С
	static constexpr size_t szAlignBlockNumSize = szAlignBlockNum;//�ڴ��������뵽�Ĵ�С
	static constexpr size_t szAlignmentSize = szAlignment;//�����ڴ�ı߽�
	static constexpr size_t szExpandMultipleSize = szExpandMultiple;//���ݵı���

	//���캯��,��һ������Ϊ�����ڴ��Ĵ�С,Ĭ��ֵ��Type�Ĵ�С,�ڶ�����������ʼ�ڴ��Ԥ����ĳ�ʼ�ڴ�����,Ĭ��ֵ��1024
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) ://��_szMemBlockPreAllocNum�������뵽�����2��ָ���η�
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockPreAllocNum(_szMemBlockPreAllocNum)
	{}

	//���ú���
	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//�����࿽������
	AutoExpand_FixLen_MemPool &operator=(const AutoExpand_FixLen_MemPool &) = delete;//���ø��Ƹ�ֵ����

	//�ƶ�����
	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept ://�ƶ�����
		szArrEnd(_Move.szArrEnd),
		szArrLastSwap(_Move.szArrLastSwap),

		csMemPool(std::move(_Move.csMemPool)),

		szMemBlockPreAllocNum(_Move.szMemBlockPreAllocNum),
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

		szMemBlockPreAllocNum = 0;
		_Move.szMemBlockFixSize = 0;
		_Move.szMemBlockNum = 0;
		_Move.szMemBlockUse = 0;
	}

	//����
	~AutoExpand_FixLen_MemPool(void) noexcept
	{
		//�������������ڴ��	
		for (size_t i = szArrBeg; i < szArrEnd; ++i)
		{
			DestructorNode(pNodeArrSortPool[i]);//�������������ڴ�
		}
		//�������������õĶ���ͬһ���ڴ棬������������һ������

		szArrEnd = 0;
		szArrLastSwap = 0;

		szMemBlockPreAllocNum = 0;
		szMemBlockFixSize = 0;
		szMemBlockNum = 0;
		szMemBlockUse = 0;
	}

	//�������һ���ڴ��
	Type *AllocMemBlock(void)
	{
		Type *pFreeMemBlock;

		//ѭ��ֱ������ɹ������ڴ��
		while (true)
		{
			if (szArrLastSwap == szArrBeg)//�����ڴ������ȫ�ľ���û�п����ڴ�ؿɹ�����
			{
				break;
			}

			pFreeMemBlock = pNodeArrFreePool[szArrBeg]->csMemPool.AllocMemBlock();
			if (pFreeMemBlock != NULL)
			{
				//����ɹ����ٻ�������
				++szMemBlockUse;//������
				return pFreeMemBlock;
			}

			//����ʧ�ܣ���λ����һ�������ڴ��
			--szArrLastSwap;
			//����
			SwapFreePool(szArrBeg, szArrLastSwap);
		}

		//���е��������û�п��ÿ����ڴ���ˣ���̬����һ��

		//����һ��
		if (Capacity() == false)
		{
			return NULL;//�޷����ݣ���û����
		}

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

	//�����ѷ�����ڴ��
	bool FreeMemBlock(Type *pAllocMemBlock) noexcept
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
		if (szFreeIdx < szArrLastSwap)//�ó����ڿ��������������ֱ�ӷ���
		{
			return true;
		}

		//�ó������ڴ�������szArrLastSwap����
		if (szFreeIdx != szArrLastSwap)//�������Լ�����
		{
			//�����������
			SwapFreePool(szFreeIdx, szArrLastSwap);
		}

		//����ʹ�����ı߽�Խ������
		++szArrLastSwap;

		return true;
	}

	//���첢���ض���
	template<typename... Args>
	Type *AllocMemBlockConstructor(Args&&... args)
	{
		Type *pFreeMemBlock = AllocMemBlock();
		if (pFreeMemBlock == NULL)
		{
			return NULL;
		}

		//����new
		new(pFreeMemBlock) Type(std::forward<Args>(args)...);

		return pFreeMemBlock;
	}

	//���������ն���
	bool FreeMemBlockDestructor(Type *pAllocMemBlock) noexcept
	{
		pAllocMemBlock->~Type();
		return FreeMemBlock(pAllocMemBlock);
	}

	//����ģ�����ָ�������ݱ�������һ��
	bool Capacity(void)//��ģ�屶������1��
	{
		if (szArrEnd >= PNODE_ARR_MAX_NUM)//��̬�����������ܷ���
		{
			return false;
		}

		//�����ڴ�
		if (szMemBlockNum != 0)//��Ϊ0��ֱ��ͨ��������
		{
			size_t szNewPoolBlockNum = Pool_class::Aligned(szMemBlockNum * szExpandMultiple, szAlignBlockNum) - szMemBlockNum;//�������
			//���䲢���뵽����
			AllocAndInsertNodeToArr(szNewPoolBlockNum);

			return true;
		}
		else//Ϊ0��ͨ���û�����ʱ��Ԥ�贴����һ���ڴ��
		{
			return AddFirstMemPool(szMemBlockPreAllocNum);
		}
	}

	//���ղ���ָ�����ڴ��������ģ���еĶ����С����һ���µ��ڴ��
	bool AddFirstMemPool(size_t _szMemBlockPreAllocNum)//����_szMemBlockPreAllocNum��ģ�����Ҫ������һ�������ڴ��
	{
		if (szArrBeg != szArrEnd)
		{
			return false;//���ǵ�һ��
		}

		//�������
		size_t szFirstPoolBlockNum = Pool_class::Aligned(_szMemBlockPreAllocNum, szAlignBlockNum);
		//�����ڴ�
		Node *pNewNode = ConstructorNode(szMemBlockFixSize, szFirstPoolBlockNum, szArrBeg);
		//���Ӽ���
		szMemBlockNum += pNewNode->csMemPool.GetMemBlockNum();

		//����ͷ��
		pNodeArrFreePool[szArrBeg] = pNewNode;
		pNodeArrSortPool[szArrBeg] = pNewNode;

		//���ó�Ա
		++szArrEnd;
		szArrLastSwap = szArrEnd;

		return true;
	}

	using Up_Func = bool (*)(const Pool_class &c);

	//ʾ������
	static bool default_remove(const Pool_class &c)
	{
		if (c.GetMemBlockUse() == 0 && c.GetMemBlockNum() < 64)
		{
			return true;//ɾ��
		}
		return false;//��ɾ��
	}

	//�Ƴ����������з���һԪν���жϽ�����ڴ��(����trueɾ��������,����true��������)
	template<typename Unary_Predicates = Up_Func>
	size_t RemoveEligibleMemPool(Unary_Predicates upFunc = default_remove) noexcept//ע��������ｫ�ڵ�ȫ��ɾ�������´�����ʱ���������
	{
		size_t szRemoveCount = 0;

		size_t i = szArrBeg;
		while (i < szArrEnd)
		{
			if (upFunc(pNodeArrSortPool[i]->csMemPool) == true)
			{
				RemoveAndFreeNodeFromArr(i);//ɾ���ڵ�
				++szRemoveCount;//����ɾ������
				continue;//ɾ��֮��ǰi������һ���ڵ㣬�������ֱ��continue
			}

			++i;
		}

		return szRemoveCount;
	}

	//ʾ������
	static bool default_reset(const Pool_class &c)
	{
		if (c.GetMemBlockUse() != 0)
		{
			return true;//����
		}
		return false;//������
	}

	//�������з���һԪν���жϽ�����ڴ��(����true����,����true��������)
	template<typename Unary_Predicates = Up_Func>
	size_t ResetEligibleMemPool(Unary_Predicates upFunc = default_reset) noexcept//ע��������ｫ�ڵ�ȫ��ɾ�������´�����ʱ���������
	{
		size_t szResetCount = 0;

		for (size_t i = szArrBeg; i < szArrEnd; ++i)
		{
			if (upFunc(pNodeArrSortPool[i]->csMemPool) == true)
			{
				ResetAndMoveNodeFromFreeArr(i);
				++szResetCount;//�������ü���
			}
		}

		return szResetCount;
	}

	//ʾ������
	static bool default_traverse(const Pool_class &c)
	{
		if (c.GetMemBlockUse() != 0)
		{
			//use c
			return true;//��������
		}
		return false;//��Ҫ�ٱ����ˣ�����ѭ��
	}

	//�������з���һԪν���жϽ�����ڴ��(����true��������,����false��������)
	template<typename Unary_Predicates = Up_Func>
	size_t TraverseEligibleMemPool(Unary_Predicates upFunc = default_traverse) const noexcept
	{
		size_t i;
		for (i = szArrBeg; i < szArrEnd; ++i)
		{
			if (upFunc(pNodeArrSortPool[i]->csMemPool) == false)
			{
				break;
			}
		}

		return i;//���ر���������
	}

	//��ȡ�����ڴ��Ĵ�С
	size_t GetMemBlockFixSize(void) const noexcept
	{
		return szMemBlockFixSize;
	}

	//��ȡ�����ڴ���ܹ��������ڴ����
	size_t GetMemBlockNum(void) const noexcept
	{
		return szMemBlockNum;
	}

	//��ȡ�����ڴ���ܹ����õ��ڴ����
	size_t GetMemBlockUse(void) const noexcept
	{
		return szMemBlockUse;
	}

	//��ȡ���й�����ڴ�ظ���
	size_t GetPoolNum(void) const noexcept
	{
		return szArrEnd;
	}

	//��ȡ���й�����ڴ���л��ܷ�����ڴ����ڴ�ظ���
	size_t GetFreePoolNum(void) const noexcept
	{
		if (szArrBeg == szArrEnd)
		{
			return 0;
		}

		Pool_class &csTemp = pNodeArrFreePool[szArrBeg]->csMemPool;
		bool bFull = csTemp.GetMemBlockUse() == csTemp.GetMemBlockNum() && szArrLastSwap != szArrBeg;
		return szArrLastSwap - bFull;
	}

	//��ȡ���й�����ڴ����ȫ���������ڴ����ڴ�ظ���
	size_t GetFullPoolNum(void) const noexcept
	{
		if (szArrBeg == szArrEnd)
		{
			return 0;
		}

		Pool_class &csTemp = pNodeArrFreePool[szArrBeg]->csMemPool;
		bool bFull = csTemp.GetMemBlockUse() == csTemp.GetMemBlockNum() && szArrLastSwap != szArrBeg;
		return szArrEnd - szArrLastSwap + bFull;
	}
};