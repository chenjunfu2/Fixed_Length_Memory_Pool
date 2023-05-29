#pragma once
#include <new>
#include <initializer_list>

template<typename DataType>
class List
{
private:
	//节点声明
	struct Node
	{
		Node *pPrev;//上一个节点
		Node *pNext;//下一个节点
		DataType tData;//数据
	};

	Node *pHead;//头部
	Node *pTail;//尾部
public:
	//迭代器
	class iterator//迭代器
	{
	private:
		friend class List;//设置主类为友元类
		Node *pCurrent;
	protected:
		//构造(只允许主类和继承类使用Node构造)
		iterator(Node *_pcurrent = nullptr) : pCurrent(_pcurrent)
		{}
	public:
		//拷贝构造
		iterator(const iterator &_Iterator) :pCurrent(_Iterator.pCurrent)
		{}

		//移动构造
		iterator(iterator &&_Iterator) :pCurrent(_Iterator.pCurrent)
		{
			_Iterator.pCurrent = nullptr;
		}

		//默认析构（平凡析构类）
		~iterator(void) = default;

		//前缀递增
		iterator &operator++(void)//无int是前缀递增
		{
			if (pCurrent != nullptr)
			{
				pCurrent = pCurrent->pNext;
			}
			return *this;
		}

		//前缀递减
		iterator &operator--(void)//无int是前缀递减
		{
			if (pCurrent != nullptr)
			{
				pCurrent = pCurrent->pPrev;
			}
			return *this;
		}

		//解引用获取Data
		DataType &operator*(void) const
		{
			return pCurrent->tData;//不判断了，如果用户解引用nullptr直接让程序炸
		}

		DataType *operator->(void) const//获取地址
		{
			return &pCurrent->tData;
		}
		
		//不等判断
		bool operator!=(iterator &_rRight) const
		{
			return pCurrent != _rRight.pCurrent;
		}
		
		//相等判断
		bool operator==(iterator &_rRight) const
		{
			return pCurrent == _rRight.pCurrent;
		}

		//迭代器有效性检测（检测pCurrent是不是nullptr）
		operator bool(void) const
		{
			return pCurrent != nullptr;
		}

		//迭代器有效性检测（检测pCurrent是不是nullptr）
		bool operator!(void) const
		{
			return pCurrent == nullptr;
		}
	};

	//默认无参构造
	List(void) :pHead(nullptr), pTail(nullptr)
	{}

	//移动构造
	List(List &&_List) :pHead(_List.pHead), pTail(_List.pTail)
	{
		_List.pHead = nullptr;
		_List.pTail = nullptr;
	}

	//析构
	~List(void)
	{
		RemoveAll();
	}

	//删除所有元素，清空链表
	bool RemoveAll(void)
	{
		if (pHead == nullptr && pTail == nullptr)
		{
			return false;
		}

		Node *pDel = pHead;
		while (pDel != nullptr)//遍历删除所有元素
		{
			pHead = pHead->pNext;
			delete pDel;
			pDel = pHead;
		}
		
		//置空链表
		pHead = nullptr;
		pTail = nullptr;

		return true;
	}

	//是否为空
	bool IsEmpty(void)
	{
		return pHead == nullptr && pTail == nullptr;
	}

	//头插
	bool InsertHead(DataType &&tData)
	{
		//分配并初始化
		Node *pInsert = new Node{nullptr,pHead,std::move(tData)};
		if (pInsert == nullptr)
		{
			return false;
		}

		//设置头节点指向前一个节点的指针为当前分配的节点
		if (pHead != nullptr)
		{
			pHead->pPrev = pInsert;
		}

		//设置头节点为新节点
		pHead = pInsert;

		//设置尾节点
		if (pTail == nullptr)
		{
			pTail = pInsert;
		}

		return false;
	}

	//尾插
	bool InsertTail(DataType &&tData)
	{
		//分配并初始化
		Node *pInsert = new Node{pTail,nullptr,std::move(tData)};
		if (pInsert == nullptr)
		{
			return false;
		}

		//设置尾节点的后一个节点指针为当前分配的节点
		if (pTail != nullptr)
		{
			pTail->pNext = pInsert;
		}

		//设置尾节点为新节点
		pTail = pInsert;

		//设置头节点
		if (pHead == nullptr)
		{
			pHead = pInsert;
		}

		return true;
	}

	//中间插入(在itpos指向的元素后插入)
	bool InsertMid(DataType &&tData, iterator &itPos)
	{
		Node *pInsert = itPos.pCurrent;//在这个位置后插入
		if (pInsert == nullptr)
		{
			return false;
		}

		//分配并初始化
		Node *pInsert = new Node{pInsert,pInsert->pNext,std::move(tData)};
		if (pInsert == nullptr)
		{
			return false;
		}

		//如果存在下一个节点，那么下一个节点的上一个节点为新节点
		if (pInsert->pNext != nullptr)
		{
			pInsert->pNext->pPrev = pInsert;
		}
		else//如果不存在下一个节点，即当前节点是尾节点
		{
			pTail = pInsert;//设置尾节点为新插入的节点
		}

		//设置插入点的下一个节点为新节点
		pInsert->pNext = pInsert;

		//递增itPos使其指向新插入的节点
		++itPos;

		return true;
	}

	//头删
	bool RemoveHead(void)
	{
		if (pHead == nullptr)
		{
			return false;
		}

		//保存头部指针
		Node *pRemove = pHead;

		//设置指针
		pHead = pHead->pNext;
		if (pHead != nullptr)//还有元素
		{
			pHead->pPrev = nullptr;//取消对前一个元素指针的引用
		}
		else//删完了
		{
			pTail = nullptr;//尾部指针也要取消对最后一个元素的引用
		}

		//删除
		delete pRemove;
		pRemove = nullptr;

		return true;
	}

	//尾删
	bool RemoveTail(void)
	{
		if (pHead == nullptr)
		{
			return false;
		}

		//保存尾部指针
		Node *pRemove = pTail;

		//设置指针
		pTail = pTail->pPrev;
		if (pTail != nullptr)//还有元素
		{
			pTail->pNext = nullptr;//取消对后一个元素指针的引用
		}
		else//删完了
		{
			pHead = nullptr;//头部指针也要取消对最后一个元素的引用
		}

		//删除
		delete pRemove;
		pRemove = nullptr;

		return true;
	}

	//中间删除
	bool RemoveMid(iterator &itPos)
	{
		Node *pRemove = itPos.pCurrent;
		if (pRemove == nullptr)
		{
			return false;
		}

		//让它引用下一个元素
		++itPos;

		//设置前节点指向后一个节点指针为要删除节点后一个节点的指针
		if (pRemove->pPrev != nullptr)
		{
			pRemove->pPrev->pNext = pRemove->pNext;
		}
		else//要删除节点是头节点
		{
			pHead = pRemove->pNext;//设置头节点指向要删除节点的下一个节点
		}

		//设置后节点指向前一个节点的指针为要删除节点前一个节点的指针
		if (pRemove->pNext != nullptr)
		{
			pRemove->pNext->pPrev = pRemove->pPrev;
		}
		else//要删除节点是尾节点
		{
			pTail = pRemove->pPrev;//设置尾节点指向要删除节点的上一个节点
		}
		
		//删除
		delete pRemove;
		pRemove = nullptr;

		return true;
	}

	//获取头部一项
	DataType &GetHead(void)
	{
		return pHead->tData;
	}

	//获取尾部一项
	DataType &GetTail(void)
	{
		return pTail->tData;
	}

	//迭代器开始
	iterator begin(void) const
	{
		return iterator(pHead);
	}

	//迭代器结束
	iterator end(void) const
	{
		return iterator(nullptr);
	}

	//反向迭代器开始
	iterator rbegin(void) const
	{
		return iterator(pTail);
	}

	//反向迭代器结束
	iterator rend(void) const
	{
		return iterator(nullptr);
	}
};