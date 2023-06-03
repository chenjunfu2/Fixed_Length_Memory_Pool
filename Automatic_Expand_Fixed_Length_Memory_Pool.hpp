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

template <typename Pool_class, size_t szExpandMultiple = 2, size_t szAlignment = 4, typename Alloc_func = default_alloc, typename Free_func = default_free>
class AutoExpand_FixLen_MemPool
{
	static_assert(szExpandMultiple >= 2);
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

	static constexpr size_t PNODE_ARR_MAX_SIZE = ArrMaxNum();//61
	using Type = Pool_class::RetPoint_Type;
	struct Node
	{
		Pool_class csMemPool;
		size_t szArrIdx;//����ṹ�ڿ��г������е�����
	};

private:
	Node* pNodeArrFreePool[PNODE_ARR_MAX_SIZE];//�����ڴ�أ�szArrLastSwap������ߣ���������ָ����Ϊ�����ڴ�أ��ұߣ�������ָ����Ϊ�����ڴ�أ�
	Node* pNodeArrSortPool[PNODE_ARR_MAX_SIZE];//�����ڴ�أ����ڴ�ص�ַ����Node������Ҹߣ�

	static constexpr size_t szArrBeg = 0;//ͷ������
	size_t szArrEnd = 0;//β������
	size_t szArrLastSwap = 0;//��һ�ν���������

	FixLen_MemPool<Node, false, 4, Alloc_func, Free_func> csMemPool = (sizeof(Node), PNODE_ARR_MAX_SIZE);//��һ���ڴ��������������ڲ������ͷ�

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

	void SwapFreePool(size_t szLeftIdx,size_t szRightIdx)//�������������ڴ��
	{
		std::swap(pNodeArrFreePool[szLeftIdx], pNodeArrFreePool[szRightIdx]);
		pNodeArrFreePool[szLeftIdx]->szArrIdx = szLeftIdx;
		pNodeArrFreePool[szRightIdx]->szArrIdx = szRightIdx;
	}

	size_t BinarySearchSortPool(const void* pFind)//ʹ��uintptr_t��������б��ж��ֲ��ң����ص�һ���ڴ�ػ���ַ������pFind��������б������������Ҹ�����
	{
		size_t szFindBeg = szArrBeg;
		size_t szFindEnd = szArrEnd;
		size_t szFindCur;

		while (szFindEnd - szFindBeg > 1)//<=1ʱ����ѭ������ʱpFind����Beg��End�м�
		{
			szFindCur = (szArrEnd - szArrBeg) / 2;//���㵱ǰ���е�

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

public:
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) ://��_szMemBlockPreAllocNum�������뵽�����2��ָ���η�
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockNum(_szMemBlockPreAllocNum)
	{
		Node *pNewNode = ConstructorNode(szMemBlockFixSize, szMemBlockNum, szArrEnd);

		pNodeArrFreePool[szArrEnd] = pNewNode;
		pNodeArrSortPool[szArrEnd] = pNewNode;

		++szArrEnd;
		szArrLastSwap = szArrEnd;
	}

	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//�����࿽������

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept//�ƶ�����
	{
		//����ʵ�֣��Ȳ���
	}

	~AutoExpand_FixLen_MemPool(void)
	{
		//һ����������������ϵͳ������
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
		if (szArrEnd >= PNODE_ARR_MAX_SIZE)//��̬�����������ܷ���
		{
			return NULL;
		}

		//�½��ڴ�أ���ʼ������Ϊͷ������
		Node *pNewNode = ConstructorNode(szMemBlockFixSize, szMemBlockNum * szExpandMultiple - szMemBlockNum, szArrBeg);
		szMemBlockNum *= szExpandMultiple;

		//���뵽free����ͷ��������ԭ��ͷ���ڴ�طŵ�����β��
		pNodeArrFreePool[szArrEnd] = pNodeArrFreePool[szArrBeg];//��Ϊβ������Ч���ݣ�ֱ�Ӹ��Ƕ��������
		pNodeArrFreePool[szArrEnd]->szArrIdx = szArrEnd;//������������
		pNodeArrFreePool[szArrBeg] = pNewNode;//����ͷ��
		//ע��˴���δ������һ���飬��ʱ���ı�szArrEnd

		//��������sort���飨Ŀǰ��ʱ�������������κ��ڴ�ص������
		size_t szSortInsert = BinarySearchSortPool(pNewNode->csMemPool.GetMemPool()) + 1;//��ȡ���±߽�����+1�Ĳ��������
		//���ƶ�Ԫ�أ�ע��������������������飬Node������������������޹أ�������ģ�
		memmove(pNodeArrSortPool[szSortInsert + 1], pNodeArrSortPool[szSortInsert], sizeof(*pNodeArrSortPool) * (szArrEnd - szSortInsert));//szArrEnd 
		pNodeArrSortPool[szSortInsert] = pNewNode;//����

		//�����鶼������ϣ�����szArrEnd
		++szArrEnd;

		//������ϣ��ط����ڴ�
		pFreeMemBlock = pNodeArrFreePool[szArrBeg]->csMemPool.AllocMemBlock();
		if (pFreeMemBlock != NULL)
		{
			++szMemBlockUse;//������
			return pFreeMemBlock;
		}

		return NULL;//�Բ��ⶼ��ʧ�ܣ�
	}

	bool FreeMemBlock(Type *pAllocMemBlock)
	{
		





	}

	void FreeAllMemBlock(void)//ע�⣬ֻ���ͷ��ڴ���ѷ����ڴ棬������ɾ���ڴ��
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