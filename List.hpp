#pragma once
#include <new>
#include <initializer_list>

template<typename DataType>
class List
{
private:
	//�ڵ�����
	struct Node
	{
		Node *pPrev;//��һ���ڵ�
		Node *pNext;//��һ���ڵ�
		DataType tData;//����
	};

	Node *pHead;//ͷ��
	Node *pTail;//β��
public:
	//������
	class iterator//������
	{
	private:
		friend class List;//��������Ϊ��Ԫ��
		Node *pCurrent;
	protected:
		//����(ֻ��������ͼ̳���ʹ��Node����)
		iterator(Node *_pcurrent = nullptr) : pCurrent(_pcurrent)
		{}
	public:
		//��������
		iterator(const iterator &_Iterator) :pCurrent(_Iterator.pCurrent)
		{}

		//�ƶ�����
		iterator(iterator &&_Iterator) :pCurrent(_Iterator.pCurrent)
		{
			_Iterator.pCurrent = nullptr;
		}

		//Ĭ��������ƽ�������ࣩ
		~iterator(void) = default;

		//ǰ׺����
		iterator &operator++(void)//��int��ǰ׺����
		{
			if (pCurrent != nullptr)
			{
				pCurrent = pCurrent->pNext;
			}
			return *this;
		}

		//ǰ׺�ݼ�
		iterator &operator--(void)//��int��ǰ׺�ݼ�
		{
			if (pCurrent != nullptr)
			{
				pCurrent = pCurrent->pPrev;
			}
			return *this;
		}

		//�����û�ȡData
		DataType &operator*(void) const
		{
			return pCurrent->tData;//���ж��ˣ�����û�������nullptrֱ���ó���ը
		}

		DataType *operator->(void) const//��ȡ��ַ
		{
			return &pCurrent->tData;
		}
		
		//�����ж�
		bool operator!=(iterator &_rRight) const
		{
			return pCurrent != _rRight.pCurrent;
		}
		
		//����ж�
		bool operator==(iterator &_rRight) const
		{
			return pCurrent == _rRight.pCurrent;
		}

		//��������Ч�Լ�⣨���pCurrent�ǲ���nullptr��
		operator bool(void) const
		{
			return pCurrent != nullptr;
		}

		//��������Ч�Լ�⣨���pCurrent�ǲ���nullptr��
		bool operator!(void) const
		{
			return pCurrent == nullptr;
		}
	};

	//Ĭ���޲ι���
	List(void) :pHead(nullptr), pTail(nullptr)
	{}

	//�ƶ�����
	List(List &&_List) :pHead(_List.pHead), pTail(_List.pTail)
	{
		_List.pHead = nullptr;
		_List.pTail = nullptr;
	}

	//����
	~List(void)
	{
		RemoveAll();
	}

	//ɾ������Ԫ�أ��������
	bool RemoveAll(void)
	{
		if (pHead == nullptr && pTail == nullptr)
		{
			return false;
		}

		Node *pDel = pHead;
		while (pDel != nullptr)//����ɾ������Ԫ��
		{
			pHead = pHead->pNext;
			delete pDel;
			pDel = pHead;
		}
		
		//�ÿ�����
		pHead = nullptr;
		pTail = nullptr;

		return true;
	}

	//�Ƿ�Ϊ��
	bool IsEmpty(void)
	{
		return pHead == nullptr && pTail == nullptr;
	}

	//ͷ��
	bool InsertHead(DataType &&tData)
	{
		//���䲢��ʼ��
		Node *pInsert = new Node{nullptr,pHead,std::move(tData)};
		if (pInsert == nullptr)
		{
			return false;
		}

		//����ͷ�ڵ�ָ��ǰһ���ڵ��ָ��Ϊ��ǰ����Ľڵ�
		if (pHead != nullptr)
		{
			pHead->pPrev = pInsert;
		}

		//����ͷ�ڵ�Ϊ�½ڵ�
		pHead = pInsert;

		//����β�ڵ�
		if (pTail == nullptr)
		{
			pTail = pInsert;
		}

		return false;
	}

	//β��
	bool InsertTail(DataType &&tData)
	{
		//���䲢��ʼ��
		Node *pInsert = new Node{pTail,nullptr,std::move(tData)};
		if (pInsert == nullptr)
		{
			return false;
		}

		//����β�ڵ�ĺ�һ���ڵ�ָ��Ϊ��ǰ����Ľڵ�
		if (pTail != nullptr)
		{
			pTail->pNext = pInsert;
		}

		//����β�ڵ�Ϊ�½ڵ�
		pTail = pInsert;

		//����ͷ�ڵ�
		if (pHead == nullptr)
		{
			pHead = pInsert;
		}

		return true;
	}

	//�м����(��itposָ���Ԫ�غ����)
	bool InsertMid(DataType &&tData, iterator &itPos)
	{
		Node *pInsert = itPos.pCurrent;//�����λ�ú����
		if (pInsert == nullptr)
		{
			return false;
		}

		//���䲢��ʼ��
		Node *pInsert = new Node{pInsert,pInsert->pNext,std::move(tData)};
		if (pInsert == nullptr)
		{
			return false;
		}

		//���������һ���ڵ㣬��ô��һ���ڵ����һ���ڵ�Ϊ�½ڵ�
		if (pInsert->pNext != nullptr)
		{
			pInsert->pNext->pPrev = pInsert;
		}
		else//�����������һ���ڵ㣬����ǰ�ڵ���β�ڵ�
		{
			pTail = pInsert;//����β�ڵ�Ϊ�²���Ľڵ�
		}

		//���ò�������һ���ڵ�Ϊ�½ڵ�
		pInsert->pNext = pInsert;

		//����itPosʹ��ָ���²���Ľڵ�
		++itPos;

		return true;
	}

	//ͷɾ
	bool RemoveHead(void)
	{
		if (pHead == nullptr)
		{
			return false;
		}

		//����ͷ��ָ��
		Node *pRemove = pHead;

		//����ָ��
		pHead = pHead->pNext;
		if (pHead != nullptr)//����Ԫ��
		{
			pHead->pPrev = nullptr;//ȡ����ǰһ��Ԫ��ָ�������
		}
		else//ɾ����
		{
			pTail = nullptr;//β��ָ��ҲҪȡ�������һ��Ԫ�ص�����
		}

		//ɾ��
		delete pRemove;
		pRemove = nullptr;

		return true;
	}

	//βɾ
	bool RemoveTail(void)
	{
		if (pHead == nullptr)
		{
			return false;
		}

		//����β��ָ��
		Node *pRemove = pTail;

		//����ָ��
		pTail = pTail->pPrev;
		if (pTail != nullptr)//����Ԫ��
		{
			pTail->pNext = nullptr;//ȡ���Ժ�һ��Ԫ��ָ�������
		}
		else//ɾ����
		{
			pHead = nullptr;//ͷ��ָ��ҲҪȡ�������һ��Ԫ�ص�����
		}

		//ɾ��
		delete pRemove;
		pRemove = nullptr;

		return true;
	}

	//�м�ɾ��
	bool RemoveMid(iterator &itPos)
	{
		Node *pRemove = itPos.pCurrent;
		if (pRemove == nullptr)
		{
			return false;
		}

		//����������һ��Ԫ��
		++itPos;

		//����ǰ�ڵ�ָ���һ���ڵ�ָ��ΪҪɾ���ڵ��һ���ڵ��ָ��
		if (pRemove->pPrev != nullptr)
		{
			pRemove->pPrev->pNext = pRemove->pNext;
		}
		else//Ҫɾ���ڵ���ͷ�ڵ�
		{
			pHead = pRemove->pNext;//����ͷ�ڵ�ָ��Ҫɾ���ڵ����һ���ڵ�
		}

		//���ú�ڵ�ָ��ǰһ���ڵ��ָ��ΪҪɾ���ڵ�ǰһ���ڵ��ָ��
		if (pRemove->pNext != nullptr)
		{
			pRemove->pNext->pPrev = pRemove->pPrev;
		}
		else//Ҫɾ���ڵ���β�ڵ�
		{
			pTail = pRemove->pPrev;//����β�ڵ�ָ��Ҫɾ���ڵ����һ���ڵ�
		}
		
		//ɾ��
		delete pRemove;
		pRemove = nullptr;

		return true;
	}

	//��ȡͷ��һ��
	DataType &GetHead(void)
	{
		return pHead->tData;
	}

	//��ȡβ��һ��
	DataType &GetTail(void)
	{
		return pTail->tData;
	}

	//��������ʼ
	iterator begin(void) const
	{
		return iterator(pHead);
	}

	//����������
	iterator end(void) const
	{
		return iterator(nullptr);
	}

	//�����������ʼ
	iterator rbegin(void) const
	{
		return iterator(pTail);
	}

	//�������������
	iterator rend(void) const
	{
		return iterator(nullptr);
	}
};