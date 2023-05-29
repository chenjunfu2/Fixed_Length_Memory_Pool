#pragma once
#include "Fixed_Length_Memory_Pool.hpp"
#include "List.hpp"

//�������ݷ��������ݷ�

template <typename type, bool bLazyInit = true, bool bAutoDelUnuse = false, long ExpandMultiple = 2>
class AutoExpand_FixLen_MemPool
{
	static_assert(ExpandMultiple > 1);//����Ҫ����1�����޷�����
private:
	using BaseClass = FixLen_MemPool<type, bLazyInit>;
	using Pool_List = List<BaseClass>;

	Pool_List csPoolList = {};
	size_t szMemBlockFixSize = 0;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����

	size_t szPoolSize = 0;//�ڴ���ܴ�С
	size_t szMemBlockNum = 0;//�ڴ�������ڴ�����
	size_t szMemBlockUse = 0;//�ڴ����ʹ���ڴ�����

private:
	void ExpandAndAddNodeBack()
	{
		//�µ��ڴ����ԭ�ڴ���ܴ�С�ĺ�Ϊԭ�ڴ���ܴ�С��ExpandMultiple��
		csPoolList.InsertTail(BaseClass(szMemBlockFixSize, szMemBlockNum * ExpandMultiple - szMemBlockNum));
		
		//���ó�Ա����
		szPoolSize += csPoolList.GetTail().GetPoolSize();
		szMemBlockNum += csPoolList.GetTail().GetMemBlockNum();
		szMemBlockUse += csPoolList.GetTail().GetMemBlockUse();
	}

	void ContractAndDelNodeIt(Pool_List::iterator &it)
	{
		//���ó�Ա����
		szPoolSize -= it->GetPoolSize();
		szMemBlockNum -= it->GetMemBlockNum();
		szMemBlockUse -= it->GetMemBlockUse();

		//�Ƴ��ڵ�
		csPoolList.RemoveMid(it);
	}

public:
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(type), size_t _szMemBlockPreAllocNum = 1024) :
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockNum(_szMemBlockPreAllocNum)
	{}

	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//�����࿽������

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept ://�ƶ�����
		csPoolList(std::move(_Move.csPoolList)),

		szMemBlockFixSize(_Move.szMemBlockFixSize),
		szPoolSize(_Move.szPoolSize),
		szMemBlockNum(_Move.szMemBlockNum),
		szMemBlockUse(_Move.szMemBlockUse)
	{
		_Move.szMemBlockFixSize = 0;
		_Move.szPoolSize = 0;
		_Move.szMemBlockNum = 0;
		_Move.szMemBlockUse = 0;
	}

	~AutoExpand_FixLen_MemPool()
	{
		csPoolList.RemoveAll();

		szMemBlockFixSize = 0;
		szPoolSize = 0;
		szMemBlockNum = 0;
		szMemBlockUse = 0;
	}

	type *AllocMemBlock(void)
	{
		type *pFreeMemBlock;
		for (auto &it : csPoolList)
		{
			pFreeMemBlock = it.AllocMemBlock();
			if (pFreeMemBlock != NULL)
			{
				++szMemBlockUse;
				return pFreeMemBlock;
			}
		}

		//ѭ������������������ڴ��ȫ�����䲻�ˣ����½�һ���ڴ�ز�����
		ExpandAndAddNodeBack();

		pFreeMemBlock = csPoolList.GetTail().AllocMemBlock();
		if (pFreeMemBlock != NULL)
		{
			++szMemBlockUse;
			return pFreeMemBlock;
		}
		
		return NULL;
	}

	bool FreeMemBlock(type *pAllocMemBlock)
	{
		for (typename Pool_List::iterator it = csPoolList.begin(); it != csPoolList.end(); ++it)
		{
			if (it->FreeMemBlock(pAllocMemBlock))
			{
				--szMemBlockUse;
				if constexpr (bAutoDelUnuse)
				{
					if (it->GetMemBlockUse() == 0)//���ɾ�������ָ����ڴ��ʹ�ü���Ϊ0��ֱ���ͷŴ��ڴ��
					{
						ContractAndDelNodeIt(it);
					}
				}
				return true;//�ͷųɹ�������
			}
		}

		return false;
	}

	void FreeAllMemBlock(void)//ע�⣬ֻ���ͷ��ڴ���ѷ����ڴ棬������ɾ���ڴ��
	{
		for (auto &it : csPoolList)
		{
			if (it.GetMemBlockUse() != 0)
			{
				szMemBlockUse -= it.GetMemBlockUse();
				it.FreeAllMemBlock();
			}
		}
	}

	void DeleteAllUnusePool(void)//ɾ������δʹ�õ��ڴ����
	{
		for (typename Pool_List::iterator it = csPoolList.begin(); it != csPoolList.end(); ++it)
		{
			if (it->GetMemBlockUse() == 0)
			{
				ContractAndDelNodeIt(it);
			}
		}
	}

	size_t GetMemBlockFixSize(void) const
	{
		return szMemBlockFixSize;
	}

	size_t GetPoolSize(void) const
	{
		return szPoolSize;
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