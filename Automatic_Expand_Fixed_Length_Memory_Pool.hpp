#pragma once
#include "Fixed_Length_Memory_Pool.hpp"
#include "List.hpp"

//链表扩容法、树扩容法

template <typename type, bool bLazyInit = true, bool bAutoDelUnuse = false, long ExpandMultiple = 2>
class AutoExpand_FixLen_MemPool
{
	static_assert(ExpandMultiple > 1);//必须要大于1否则无法扩容
private:
	using BaseClass = FixLen_MemPool<type, bLazyInit>;
	using Pool_List = List<BaseClass>;

	Pool_List csPoolList = {};
	size_t szMemBlockFixSize = 0;//用户初始化时需要的定长内存长度（size：字节数）

	size_t szPoolSize = 0;//内存池总大小
	size_t szMemBlockNum = 0;//内存池中总内存块个数
	size_t szMemBlockUse = 0;//内存池中使用内存块个数

private:
	void ExpandAndAddNodeBack()
	{
		//新的内存池与原内存池总大小的和为原内存池总大小的ExpandMultiple倍
		csPoolList.InsertTail(BaseClass(szMemBlockFixSize, szMemBlockNum * ExpandMultiple - szMemBlockNum));
		
		//设置成员变量
		szPoolSize += csPoolList.GetTail().GetPoolSize();
		szMemBlockNum += csPoolList.GetTail().GetMemBlockNum();
		szMemBlockUse += csPoolList.GetTail().GetMemBlockUse();
	}

	void ContractAndDelNodeIt(Pool_List::iterator &it)
	{
		//设置成员变量
		szPoolSize -= it->GetPoolSize();
		szMemBlockNum -= it->GetMemBlockNum();
		szMemBlockUse -= it->GetMemBlockUse();

		//移除节点
		csPoolList.RemoveMid(it);
	}

public:
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(type), size_t _szMemBlockPreAllocNum = 1024) :
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockNum(_szMemBlockPreAllocNum)
	{}

	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//禁用类拷贝构造

	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept ://移动构造
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

		//循环结束后则代表现有内存池全部分配不了，则新建一个内存池并分配
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
					if (it->GetMemBlockUse() == 0)//如果删除了这个指针后内存池使用计数为0则直接释放此内存池
					{
						ContractAndDelNodeIt(it);
					}
				}
				return true;//释放成功，返回
			}
		}

		return false;
	}

	void FreeAllMemBlock(void)//注意，只是释放内存池已分配内存，并不会删除内存池
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

	void DeleteAllUnusePool(void)//删除所有未使用的内存池类
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