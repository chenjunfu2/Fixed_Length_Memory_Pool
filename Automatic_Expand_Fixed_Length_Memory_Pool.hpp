#pragma once
#include "Fixed_Length_Memory_Pool.hpp"

/*
封装FixLen_MemPool类，并自动扩容申请新的类实例
类内有两个数组，一个是空闲内存池，另一个是排序内存池

分配：
每次分配都从空闲内存池索引szArrBeg处的内存池请求内存，如果请求失败则与--szArrLastSwap处的空闲内存池交换
也就是说，从szArrLastSwap到szArrEnd之间全部是已满的内存池，从szArrBeg到szArrLastSwap之间全部都是未满的内存池，szArrLastSwap相当于数组的分隔符

在交换过程中，要先检测szArrLastSwap是否<=1，如果是则代表已无可用内存池了，这时候就需要按照szExpandMultiple倍增扩容，分配一个新的内存池
如果还有可用内存池，则交换数组中这两个Node元素的指针并给他们的szArrIdx修改为当前它所在的数组位置的索引

分配新内存池时，新内存池的block数为：当前所有内存池block总和*szExpandMultiple-当前所有内存池block总和，也就是当前所有内存池block总和扩容szExpandMultiple倍后增加的block数
如果szArrEnd不等于数组的静态声明大小，则新建的内存池放在szArrBeg处，原先szArrBeg处的Node调到szArrEnd，并++szArrEnd，设置这两个Node里所在数组的索引然后利用二分法通过内存池起始地址
在pNodeArrSortPool找到适当的位置进行插入排序，后面的元素全部使用memmove往后移动

释放：
检查当前释放指针是不是nullptr，是就直接返回true

不是nullptr，使释放指针用二分法查找在pNodeArrSortPool中，因为pNodeArrSortPool是根据内存池地址排序的，所以可以快速找到指针所属内存池，但是因为内存池地址是起始地址，指针可以在起始地址
到结束地址之间，所以二分法并不一定是精确查找到完美匹配的方案（也存在刚好匹配，比如内存池起始地址的那个block刚好正在释放），所以只要二分查找中的beg和end相差小等于1或者刚好匹配，
则代表当前释放指针在beg索引所指的内存池中（因为大于beg的地址，但是小于end的地址，又因为内存池按照内存池的起始地址排序（注意不是内存池类的地址，是类内池之的地址），所以可以推出这个指针所属beg索引所指的内存池）
然后调用这个内存池的释放函数释放该指针

在释放完成后，如果释放函数返回false，则返回false

不是false，则通过这个索引得到的Node结构中的szArrIdx获得该池在pNodeArrFreePool中的位置，并判断szArrLastSwap和自身位置的关系，如果是szArrLastSwap>szArrIdx，则该池已在空闲区域内，无需调换直接返回true
否则将自己和szArrLastSwap索引的指针交换，并设置szArrIdx，然后++szArrLastSwap，确保自己被换入空闲区域
*/

template <
	typename Pool_class,//内存池类型
	size_t szExpandMultiple = 2,//扩容倍数
	size_t szAlignBlockNum = 2,//内存池内存块个数对齐的边界
	size_t szAlignment = 4,//管理内存池的对齐边界
	typename Alloc_func = default_alloc,//管理内存池的默认分配器
	typename Free_func = default_free>//管理内存池的默认释放器
class AutoExpand_FixLen_MemPool
{
	static_assert(szExpandMultiple >= 2);
	static_assert(szAlignBlockNum == 1 || (szAlignBlockNum != 0 && szAlignBlockNum % 2 == 0));
protected:
	static constexpr size_t ArrMaxNum(void)//大致估计（编译时计算）
	{
		size_t szBlockNum = 1;//内存块总数
		size_t szCumulative = 0;//总大小

		size_t szNum = 0;
		while (szNum < 512)
		{
			//计算当前总大小
			szCumulative = Pool_class::szManageMemBlockRequireSize * szBlockNum;
			if (szCumulative / szBlockNum != Pool_class::szManageMemBlockRequireSize)//溢出
			{
				break;
			}

			szCumulative += szBlockNum;
			if (szCumulative < szBlockNum)//溢出
			{
				break;
			}

			szCumulative += sizeof(Pool_class) * szNum;
			if (szCumulative < sizeof(Pool_class) * szNum)//溢出
			{
				break;
			}

			szBlockNum *= szExpandMultiple;//倍增
			++szNum;
		}

		//数组大小为szNum
		return szNum;
	}

	static constexpr size_t PNODE_ARR_MAX_NUM = ArrMaxNum();//61

	struct Node
	{
		Pool_class csMemPool;
		size_t szArrIdx;//这个结构在空闲池数组中的索引
	};

	using Type = Pool_class::RetPoint_Type;
	using Manage_Pool = FixLen_MemPool<Node, false, szAlignment, Alloc_func, Free_func>;

private:
	Node *pNodeArrFreePool[PNODE_ARR_MAX_NUM] = {0};//空闲内存池（szArrLastSwap索引左边，不包括其指代都为空闲内存池，右边，包括其指代都为已满内存池）
	Node *pNodeArrSortPool[PNODE_ARR_MAX_NUM] = {0};//排序内存池（按内存池地址排序Node，左低右高）

	static constexpr size_t szArrBeg = 0;//头部索引
	size_t szArrEnd = 0;//尾后索引
	size_t szArrLastSwap = 0;//上一次交换的索引

	Manage_Pool csMemPool = Manage_Pool(sizeof(Node), PNODE_ARR_MAX_NUM);//用一个内存池来管理后续的内部分配释放

	size_t szMemBlockPreAllocNum = 0;//用户初始化时指定的预分配大小
	size_t szMemBlockFixSize = 0;//用户初始化时需要的定长内存长度（size：字节数）
	size_t szMemBlockNum = 0;//内存池中总内存块个数
	size_t szMemBlockUse = 0;//内存池中使用内存块个数
private:
	//构造并返回一个节点，用于构造新的内存池
	Node *ConstructorNode(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum, size_t _szArrIdx) noexcept
	{
		Node *pNode = csMemPool.AllocMemBlock();//请求内存

		//构造类
		new(&pNode->csMemPool) Pool_class(_szMemBlockFixSize, _szMemBlockPreAllocNum);//placement new
		//初始化索引
		pNode->szArrIdx = _szArrIdx;

		return pNode;
	}

	//析构并回收一个节点，用于析构无用内存池
	void DestructorNode(Node *pNode) noexcept
	{
		//确保正确析构
		pNode->csMemPool.~Pool_class();
		pNode->szArrIdx = szArrBeg;

		csMemPool.FreeMemBlock(pNode);//回收内存
	}

	//交换FreePool数组内的两个节点
	void SwapFreePool(size_t szLeftIdx, size_t szRightIdx) noexcept//交换两个空闲内存池
	{
		std::swap(pNodeArrFreePool[szLeftIdx], pNodeArrFreePool[szRightIdx]);
		pNodeArrFreePool[szLeftIdx]->szArrIdx = szLeftIdx;
		pNodeArrFreePool[szRightIdx]->szArrIdx = szRightIdx;
	}

	//移动FreePool数组内的一个源节点到目标索引位置
	void MoveFreePool(size_t szTargetIdx, size_t szSourceIdx) noexcept
	{
		if (szTargetIdx != szSourceIdx)
		{
			pNodeArrFreePool[szTargetIdx] = pNodeArrFreePool[szSourceIdx];//直接拷贝到目标位置
			pNodeArrFreePool[szTargetIdx]->szArrIdx = szTargetIdx;//设置索引关联
			pNodeArrFreePool[szSourceIdx] = NULL;//清理成员
		}
	}

	//在SortPool数组内二分查找指针所属的内存池
	size_t BinarySearchSortPool(const void *pFind) const noexcept//使用uintptr_t在排序池列表中二分查找，返回第一个内存池基地址不大于pFind的排序池列表的索引（左低右高排序）
	{
		size_t szFindBeg = szArrBeg;
		size_t szFindEnd = szArrEnd;//注意这里End代表尾后索引并且返回值和数组访问无论如何都不会取到它

		while (szFindEnd - szFindBeg > 1)//<=1时结束循环，此时pFind就在Beg到End中间
		{
			size_t szFindCur = (szFindBeg + szFindEnd) / 2;//计算当前的中点

			long lCmp = pNodeArrSortPool[szFindCur]->csMemPool.CmpPointAndPool(pFind);//比较目标指针和内存池地址
			if (lCmp < 0)//小于
			{
				szFindEnd = szFindCur;//目标在左边，截断End
			}
			else if (lCmp > 0)//大于
			{
				szFindBeg = szFindCur;//目标在右边，截断Beg
			}
			else//等于
			{
				return szFindCur;//直接命中
			}
		}

		return szFindBeg;//Beg恰好小于pFind且End恰好大于pFind，因为是内存基地址排序，恰好大于则证明pFind绝对不在End中，必然返回Beg，即便有可能也不在Beg中
	}

	//分配新的内存池并插入两个数组
	void AllocAndInsertNodeToArr(size_t szNewPoolBlockNum) noexcept
	{
		//分配节点
		Node *pInsertNode = ConstructorNode(szMemBlockFixSize, szNewPoolBlockNum, szArrLastSwap);//新建内存池，初始化索引
		szMemBlockNum += pInsertNode->csMemPool.GetMemBlockNum();//增加当前总节点的数目

		//插入到szArrLastSwap，并将原先头部内存池放到数组尾部
		MoveFreePool(szArrEnd, szArrLastSwap);//因为尾部无有效数据，直接覆盖而无需调换
		pNodeArrFreePool[szArrLastSwap] = pInsertNode;//插入szArrLastSwap
		++szArrLastSwap;//移动边界

		//注意此处还未插入另一数组，暂时不改变szArrEnd，且此插入过程不影响szArrLastSwap

		long lFindBeg = szArrBeg;
		long lFindEnd = (long)szArrEnd - 1;

		while (lFindBeg <= lFindEnd)//>时结束循环，此时lFindBeg就是符合的插入点
		{
			long lFindCur = (lFindBeg + lFindEnd) / 2;//计算当前的中点

			long lCmp = pNodeArrSortPool[lFindCur]->csMemPool.CmpPointAndPool(pInsertNode->csMemPool.GetMemPool());//比较新节点的内存池地址
			if (lCmp < 0)//小于
			{
				lFindEnd = lFindCur - 1;//目标在左边，截断End到Cur-1
			}
			else//大于
			{
				lFindBeg = lFindCur + 1;//目标在右边，截断Beg到Cur+1
			}
		}

		//先移动元素（注意这里操作的是排序数组，Node里的索引与排序数组无关，无需更改）
		memmove(&pNodeArrSortPool[lFindBeg + 1], &pNodeArrSortPool[lFindBeg], sizeof(*pNodeArrSortPool) * (szArrEnd - lFindBeg));//szArrEnd 
		pNodeArrSortPool[lFindBeg] = pInsertNode;//插入

		//两数组都插入完毕，递增szArrEnd
		++szArrEnd;
	}

	//移除已分配的内存池并从数组中删除
	void RemoveAndFreeNodeFromArr(size_t szSortRemoveIdx) noexcept//注意这里的Idx为Sort数组的Idx而不是Free数组的Idx
	{
		//先保存待删除指针
		Node *pRemoveNode = pNodeArrSortPool[szSortRemoveIdx];

		//移动元素从Sort数组中删除
		memmove(&pNodeArrSortPool[szSortRemoveIdx], &pNodeArrSortPool[szSortRemoveIdx + 1], sizeof(*pNodeArrSortPool) * (szArrEnd - (szSortRemoveIdx + 1)));
		pNodeArrSortPool[szArrEnd - 1] = NULL;//清除成员

		// 注意此处还未从另一数组中删除，暂时不改变szArrEnd

		//从Free数组中删除（不可memmove元素删除，因为Node中的Idx与元素当前所在位置索引值绑定），使用单元素移动操作删除
		size_t szFreeRemoveIdx = pRemoveNode->szArrIdx;

		//两种情况：在szArrLastSwap左边与在szArrLastSwap上和右边
		if (szFreeRemoveIdx < szArrLastSwap)//在左边
		{
			MoveFreePool(szFreeRemoveIdx, szArrLastSwap - 1);//移动szArrLastSwap边界前的一个元素到删除位置
			MoveFreePool(szArrLastSwap - 1, szArrEnd - 1);//移动最后一个元素到szArrLastSwap边界前的一个元素位置
			--szArrLastSwap;//移动边界szArrLastSwap
		}
		else//>=在上或右边
		{
			MoveFreePool(szFreeRemoveIdx, szArrEnd - 1);//移动最后一个元素到删除位置，完成，无需更改szArrLastSwap
		}

		//两数组都删除完毕，递减szArrEnd
		--szArrEnd;

		//减少当前总节点的数目
		szMemBlockNum -= pRemoveNode->csMemPool.GetMemBlockNum();

		//释放节点
		DestructorNode(pRemoveNode);
	}

	void ResetAndMoveNodeFromFreeArr(size_t szSortResetIdx)
	{
		size_t szFreeResetIdx = pNodeArrSortPool[szSortResetIdx]->szArrIdx;

		szMemBlockUse -= pNodeArrFreePool[szFreeResetIdx]->csMemPool.GetMemBlockUse();//减少当前总使用计数
		pNodeArrFreePool[szFreeResetIdx]->csMemPool.Reset();//重置节点

		//两种情况：在szArrLastSwap左边与在szArrLastSwap上和右边
		if (szFreeResetIdx < szArrLastSwap)//在左边
		{
			return;//直接返回
		}

		if (szFreeResetIdx != szArrLastSwap)//无需与自己交换
		{
			SwapFreePool(szFreeResetIdx, szArrLastSwap);//换入空闲位置边界
		}
		
		++szArrLastSwap;//移动边界
	}

public:
	using RetPoint_Type = Type;
	static constexpr size_t szMaxMemPool = PNODE_ARR_MAX_NUM;//类中最多能存下的内存池个数
	static constexpr size_t szManageMemPoolRequireSize = sizeof(Node) * 2;//管理一个内存池所需的管理内存大小
	static constexpr size_t szAlignBlockNumSize = szAlignBlockNum;//内存块个数对齐到的大小
	static constexpr size_t szAlignmentSize = szAlignment;//对齐内存的边界
	static constexpr size_t szExpandMultipleSize = szExpandMultiple;//扩容的倍数

	//构造函数,第一个参数为定长内存块的大小,默认值是Type的大小,第二个参数是起始内存池预分配的初始内存块个数,默认值是1024
	AutoExpand_FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) ://把_szMemBlockPreAllocNum向上舍入到最近的2的指数次方
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockPreAllocNum(_szMemBlockPreAllocNum)
	{}

	//禁用函数
	AutoExpand_FixLen_MemPool(const AutoExpand_FixLen_MemPool &) = delete;//禁用类拷贝构造
	AutoExpand_FixLen_MemPool &operator=(const AutoExpand_FixLen_MemPool &) = delete;//禁用复制赋值重载

	//移动构造
	AutoExpand_FixLen_MemPool(AutoExpand_FixLen_MemPool &&_Move) noexcept ://移动构造
		szArrEnd(_Move.szArrEnd),
		szArrLastSwap(_Move.szArrLastSwap),

		csMemPool(std::move(_Move.csMemPool)),

		szMemBlockPreAllocNum(_Move.szMemBlockPreAllocNum),
		szMemBlockFixSize(_Move.szMemBlockFixSize),
		szMemBlockNum(_Move.szMemBlockNum),
		szMemBlockUse(_Move.szMemBlockUse)
	{
		//拷贝数组
		memcpy(pNodeArrFreePool, _Move.pNodeArrFreePool, sizeof(pNodeArrFreePool));
		memcpy(pNodeArrSortPool, _Move.pNodeArrSortPool, sizeof(pNodeArrSortPool));

		//清空成员
		_Move.szArrEnd = 0;
		_Move.szArrLastSwap = 0;

		szMemBlockPreAllocNum = 0;
		_Move.szMemBlockFixSize = 0;
		_Move.szMemBlockNum = 0;
		_Move.szMemBlockUse = 0;
	}

	//析构
	~AutoExpand_FixLen_MemPool(void) noexcept
	{
		//依次析构回收内存池	
		for (size_t i = szArrBeg; i < szArrEnd; ++i)
		{
			DestructorNode(pNodeArrSortPool[i]);//依次析构回收内存
		}
		//两个数组内引用的都是同一段内存，所以清理其中一个即可

		szArrEnd = 0;
		szArrLastSwap = 0;

		szMemBlockPreAllocNum = 0;
		szMemBlockFixSize = 0;
		szMemBlockNum = 0;
		szMemBlockUse = 0;
	}

	//请求分配一个内存块
	Type *AllocMemBlock(void)
	{
		Type *pFreeMemBlock;

		//循环直到分配成功或无内存池
		while (true)
		{
			if (szArrLastSwap == szArrBeg)//空闲内存池已完全耗尽，没有空闲内存池可供交换
			{
				break;
			}

			pFreeMemBlock = pNodeArrFreePool[szArrBeg]->csMemPool.AllocMemBlock();
			if (pFreeMemBlock != NULL)
			{
				//分配成功，召唤计数菌
				++szMemBlockUse;//计数菌
				return pFreeMemBlock;
			}

			//分配失败，定位到下一个空闲内存池
			--szArrLastSwap;
			//交换
			SwapFreePool(szArrBeg, szArrLastSwap);
		}

		//运行到此则代表没有可用空闲内存池了，动态分配一个

		//扩容一次
		if (Capacity() == false)
		{
			return NULL;//无法扩容，那没辙了
		}

		//操作完毕，重分配内存
		pFreeMemBlock = pNodeArrFreePool[szArrBeg]->csMemPool.AllocMemBlock();
		if (pFreeMemBlock != NULL)
		{
			//分配成功，召唤计数菌
			++szMemBlockUse;//计数菌
			return pFreeMemBlock;
		}

		return NULL;//卧槽这都能失败？
	}

	//回收已分配的内存块
	bool FreeMemBlock(Type *pAllocMemBlock) noexcept
	{
		if (pAllocMemBlock == NULL)
		{
			return true;
		}

		//计算指针所属的内存池索引
		size_t szMemBelongSort = BinarySearchSortPool(pAllocMemBlock);
		Node *pFreeNode = pNodeArrSortPool[szMemBelongSort];
		//释放
		if (pFreeNode->csMemPool.FreeMemBlock(pAllocMemBlock) == false)
		{
			return false;//释放失败，无需后续处理
		}

		//释放成功，召唤计数菌
		--szMemBlockUse;//计数菌

		//获取在空闲内存池中的索引
		size_t szFreeIdx = pFreeNode->szArrIdx;
		if (szFreeIdx < szArrLastSwap)//该池已在空闲区域，无需操作直接返回
		{
			return true;
		}

		//该池在满内存区域，与szArrLastSwap调换
		if (szFreeIdx != szArrLastSwap)//无需与自己交换
		{
			//换入空闲区域
			SwapFreePool(szFreeIdx, szArrLastSwap);
		}

		//递增使其代表的边界越过自身
		++szArrLastSwap;

		return true;
	}

	//构造并返回对象
	template<typename... Args>
	Type *AllocMemBlockConstructor(Args&&... args)
	{
		Type *pFreeMemBlock = AllocMemBlock();
		if (pFreeMemBlock == NULL)
		{
			return NULL;
		}

		//构造new
		new(pFreeMemBlock) Type(std::forward<Args>(args)...);

		return pFreeMemBlock;
	}

	//析构并回收对象
	bool FreeMemBlockDestructor(Type *pAllocMemBlock) noexcept
	{
		pAllocMemBlock->~Type();
		return FreeMemBlock(pAllocMemBlock);
	}

	//按照模板参数指定的扩容倍数扩容一次
	bool Capacity(void)//按模板倍数扩容1次
	{
		if (szArrEnd >= PNODE_ARR_MAX_NUM)//静态数组满，不能分配
		{
			return false;
		}

		//分配内存
		if (szMemBlockNum != 0)//不为0，直接通过其扩容
		{
			size_t szNewPoolBlockNum = Pool_class::Aligned(szMemBlockNum * szExpandMultiple, szAlignBlockNum) - szMemBlockNum;//计算对齐
			//分配并插入到数组
			AllocAndInsertNodeToArr(szNewPoolBlockNum);

			return true;
		}
		else//为0，通过用户构造时的预设创建第一个内存池
		{
			return AddFirstMemPool(szMemBlockPreAllocNum);
		}
	}

	//按照参数指定的内存块数根据模板中的对齐大小分配一个新的内存池
	bool AddFirstMemPool(size_t _szMemBlockPreAllocNum)//按照_szMemBlockPreAllocNum和模板对齐要求新增一个空闲内存池
	{
		if (szArrBeg != szArrEnd)
		{
			return false;//不是第一个
		}

		//计算对齐
		size_t szFirstPoolBlockNum = Pool_class::Aligned(_szMemBlockPreAllocNum, szAlignBlockNum);
		//分配内存
		Node *pNewNode = ConstructorNode(szMemBlockFixSize, szFirstPoolBlockNum, szArrBeg);
		//增加计数
		szMemBlockNum += pNewNode->csMemPool.GetMemBlockNum();

		//插入头部
		pNodeArrFreePool[szArrBeg] = pNewNode;
		pNodeArrSortPool[szArrBeg] = pNewNode;

		//设置成员
		++szArrEnd;
		szArrLastSwap = szArrEnd;

		return true;
	}

	using Up_Func = bool (*)(const Pool_class &c);

	//示例函数
	static bool default_remove(const Pool_class &c)
	{
		if (c.GetMemBlockUse() == 0 && c.GetMemBlockNum() < 64)
		{
			return true;//删除
		}
		return false;//不删除
	}

	//移除并析构所有符合一元谓词判断结果的内存池(返回true删除并析构,返回true继续遍历)
	template<typename Unary_Predicates = Up_Func>
	size_t RemoveEligibleMemPool(Unary_Predicates upFunc = default_remove) noexcept//注意如果这里将节点全部删除，则下次扩容时会出现问题
	{
		size_t szRemoveCount = 0;

		size_t i = szArrBeg;
		while (i < szArrEnd)
		{
			if (upFunc(pNodeArrSortPool[i]->csMemPool) == true)
			{
				RemoveAndFreeNodeFromArr(i);//删除节点
				++szRemoveCount;//增加删除计数
				continue;//删除之后当前i就是下一个节点，无需递增直接continue
			}

			++i;
		}

		return szRemoveCount;
	}

	//示例函数
	static bool default_reset(const Pool_class &c)
	{
		if (c.GetMemBlockUse() != 0)
		{
			return true;//重置
		}
		return false;//不重置
	}

	//重置所有符合一元谓词判断结果的内存池(返回true重置,返回true继续遍历)
	template<typename Unary_Predicates = Up_Func>
	size_t ResetEligibleMemPool(Unary_Predicates upFunc = default_reset) noexcept//注意如果这里将节点全部删除，则下次扩容时会出现问题
	{
		size_t szResetCount = 0;

		for (size_t i = szArrBeg; i < szArrEnd; ++i)
		{
			if (upFunc(pNodeArrSortPool[i]->csMemPool) == true)
			{
				ResetAndMoveNodeFromFreeArr(i);
				++szResetCount;//增加重置计数
			}
		}

		return szResetCount;
	}

	//示例函数
	static bool default_traverse(const Pool_class &c)
	{
		if (c.GetMemBlockUse() != 0)
		{
			//use c
			return true;//继续遍历
		}
		return false;//不要再遍历了，结束循环
	}

	//遍历所有符合一元谓词判断结果的内存池(返回true继续遍历,返回false结束遍历)
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

		return i;//返回遍历的总数
	}

	//获取定长内存块的大小
	size_t GetMemBlockFixSize(void) const noexcept
	{
		return szMemBlockFixSize;
	}

	//获取所有内存池总共包含的内存块数
	size_t GetMemBlockNum(void) const noexcept
	{
		return szMemBlockNum;
	}

	//获取所有内存池总共已用的内存块数
	size_t GetMemBlockUse(void) const noexcept
	{
		return szMemBlockUse;
	}

	//获取所有管理的内存池个数
	size_t GetPoolNum(void) const noexcept
	{
		return szArrEnd;
	}

	//获取所有管理的内存池中还能分配出内存块的内存池个数
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

	//获取所有管理的内存池中全部分配完内存块的内存池个数
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