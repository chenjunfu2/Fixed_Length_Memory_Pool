#pragma once

/*
用位图索引检查待释放内存地址和状态是否正确，用空闲内存栈节点索引内存位图
也就是内存位图用于检查释放地址是否合法，栈用于存储待分配内存块

实际上这个内存池就是通过减少对系统调用的次数来获得性能增益的
这个内存池包括两个数据结构用于管理block的分配和释放，第一个是一个空闲block栈，分配的时候让栈顶的空闲block弹出分配给用户，回收的时候把用户返回的指针压入栈顶
另一个是一个内存位图，用于验证释放过程中的用户传入的指针当前的状态是什么：已释放or未释放，防止用户多次释放同一个指针导致栈内存在多个重复数据甚至爆栈
分配的时候从栈中弹出的内存块会同步计算内存块在内存位图中的映射，并设置成true已分配，而在释放过程中检查用户指针是否在内存池中、是否与内存块边界对齐后
就会进行映射计算然后与内存位图中的bool值进行比对，只要这个值是true已分配，则代表用户释放的是一个正确的指针，那么就将内存位图的这个值设置成false并压入空闲栈

实际上内存位图本质上就是用来做安全释放检测的，如果能100%信任用户，则完全无比使用做这个位图，一个栈，分配弹出、释放压入即可，然后啥时候用户一个不小心程序就炸了（不是

数据成员：
void* mem_pool：一个内存池，按照固定大小来分块
bool* mem_bitmap：一个内存位图，存储内存块的分配状态，true是已分配，false是未分配，用于检查用户释放过程中的内存地址是否合法
void** free_mem_stack：一个栈数组，存储待分配内存块指针，分配的时候通过存储的地址计算映射到内存位图并设置内存分配状态

size_t stack_top：一个数组下标，用来存储栈数组栈顶
size_t fixed_length：用户初始化时确定的长度


接口成员：
初始化：从用户传入的长度初始化内存池和长度，从系统分配并初始化全0的内存位图，从系统分配并初始化栈数组，初始化栈数组栈顶下标
析构：直接释放内存池，设置所有成员为0

分配：从栈数组顶部弹出一个待分配内存块，设置该内存块起始地址映射的内存位图的内存块分配状态
释放：先进行地址检查，如果小于内存池起始地址或大于内存池尾后地址或是该地址不是从内存块起始地址开始的，用户确定的内存块地址的整数倍，则直接报错，否则从内存位图中寻找此释放地址对应内存块的分配状态，如果为已使用，则压入待分配内存块栈数组顶部，否则报错

释放全部：直接设置内存位图为全0，并根据策略设置栈数组为全0或重新初始化栈数组

实现细节：
当前暂时不考虑内存池用完的情况，等项目雏形做完后再处理此问题（预计需要用链表衔接所有的内存池）

初始化栈数组：
			方案1：全零初始化，然后初始化栈顶元素为内存池起始地址，分配内存时，先检查栈顶后的一个元素是否为0，如果是则利用当前元素的值计算后继元素的值然后弹出当前元素（懒惰初始化，勤快分配计算）
			方案2：完全初始化，利用循环给数组每个元素都初始化空闲内存地址，后续分配时直接使用（勤快初始化，无分配计算耗时）

耗时：
分配：O(1)
释放：O(1)


新的空间优化方案：
取消栈数组，数组top索引修改为指针，设置内存池单个内存块的大小下限为一个指针大小
利用空闲内存块空间作为链表节点，存储一个指针指向下一个空闲内存块，依次链接所有空闲内存块，
初始化方案为完全初始化，删除懒惰初始化方案

可选的空间优化方案：
讲内存位图由bool改为unsigned char，并利用每一位存储映射，使得内存位图占用空间为原来的1/8
*/

#include <new>
#include <string.h>
#include <utility>

struct default_alloc
{
	void *operator()(size_t szMemSize) const
	{
		return new char[szMemSize];
	}
};

struct default_free
{
	void operator()(void *pMem) const noexcept
	{
		return delete (char *)pMem;
	}
};

template <
	typename Type,//分配时的返回类型
	size_t szAlignment = 4,//内存对齐边界
	typename Alloc_func = default_alloc,//默认分配器
	typename Free_func = default_free//默认释放器
>
class FixLen_MemPool
{
private:
	static_assert(szAlignment == 1 || (szAlignment != 0 && szAlignment % 2 == 0));
	struct FreeBlock
	{
		FreeBlock *pNextFreeMemBlock;
	};

private:
	size_t szMemBlockFixSize = 0;//用户初始化时需要的定长内存长度（size：字节数）

	bool *bArrMemBlockBitmap = NULL;//bool数组内存位图
	FreeBlock *pFreeMemBlockHead = NULL;//空闲链表头指针

	void *pMemPool = NULL;//内存池起始地址
	size_t szMemPoolSize = 0;//内存池大小（size：字节数）

	size_t szMemBlockNum = 0;//内存池中总内存块个数
	size_t szMemBlockUse = 0;//内存池中已使用块个数（使用计数）

	void *pBaseMem = NULL;//内存基地址(用于后续释放)

	static constexpr void *SENTINEL_POINTER = (void *)(NULL + 1);//哨兵指针
	
protected:
	//抛出异常的内存分配函数
	void *ThrowMalloc(size_t szMemSize) const
	{
		Alloc_func fAlloc;
		void *p = fAlloc(szMemSize);
		if (p == NULL)
		{
			throw std::bad_alloc();
		}
		return p;
	}

	//不抛出异常的内存释放函数
	void NoThrowFree(void *pMem) const
	{
		Free_func fFree;
		fFree(pMem);
	}

	//按照模板对齐参数大小到边界
	static size_t AlignedSize(size_t szSize)
	{
		return (szSize + szAlignment - 1) & ~(szAlignment - 1);
	}

	//按照模板对齐指针地址到边界
	static void *AlignedMem(void *pMem)
	{
		return (void *)(((uintptr_t)pMem + szAlignment - 1) & ~(szAlignment - 1));
	}

public:
	using RetPoint_Type = Type;
	static constexpr size_t szManageMemBlockRequireSize = sizeof(*bArrMemBlockBitmap);//管理一个内存块所需的管理内存大小
	static constexpr size_t szAlignmentSize = szAlignment;//对齐内存的边界
	static constexpr size_t szMemBlockMinSize = sizeof(FreeBlock);//内存块大小下限

	//构造函数,第一个参数为起始内存池预分配的初始内存块个数,第二个参数为定长内存块的大小
	FixLen_MemPool(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum)
	{
		Realloc(_szMemBlockFixSize, _szMemBlockPreAllocNum);
	}

	//提供默认构造函数
	FixLen_MemPool(void) = default;

	//禁用函数
	FixLen_MemPool(const FixLen_MemPool &) = delete;//禁用类拷贝构造
	FixLen_MemPool &operator=(const FixLen_MemPool &) = delete;//禁用复制赋值重载

	//移动构造
	FixLen_MemPool(FixLen_MemPool &&_Move) noexcept ://移动构造
		szMemBlockFixSize(_Move.szMemBlockFixSize),

		bArrMemBlockBitmap(_Move.bArrMemBlockBitmap),
		pFreeMemBlockHead(_Move.pFreeMemBlockHead),

		pMemPool(_Move.pMemPool),
		szMemPoolSize(_Move.szMemPoolSize),
		szMemBlockNum(_Move.szMemBlockNum),
		szMemBlockUse(_Move.szMemBlockUse),

		pBaseMem(_Move.pBaseMem)
	{
		//务必设置指针为NULL，否则移动对象析构后会将其释放
		pBaseMem = NULL;
		//其余移动对象成员析构后自动清理
	}

	//析构函数
	~FixLen_MemPool(void) noexcept
	{
		Clear();
	}

	//请求分配一个内存块
	Type *AllocMemBlock(void) noexcept
	{
		if (pFreeMemBlockHead == NULL)//没有空闲内存块了
		{
			return NULL;
		}

		//链表头弹出作为空闲内存块
		void *pFreeMemBlock = pFreeMemBlockHead;
		//链表头指向下一个空闲内存块
		pFreeMemBlockHead = pFreeMemBlockHead->pNextFreeMemBlock;

		//从内存块地址映射到位图
		size_t szBitmapIndex = ((uintptr_t)pFreeMemBlock - (uintptr_t)pMemPool) / szMemBlockFixSize;
		//设置为分配状态
		bArrMemBlockBitmap[szBitmapIndex] = true;
		//设置使用个数
		++szMemBlockUse;

		return (Type *)pFreeMemBlock;
	}

	//回收已分配的内存块
	bool FreeMemBlock(Type *pAllocMemBlock) noexcept//释放非内存池分配的内存、多次释放会返回false
	{
		if (pAllocMemBlock == NULL)//空指针
		{
			return true;//直接成功
		}

		if (CmpPointAndPool(pAllocMemBlock) != 0 ||//超出内存池范围
			((uintptr_t)pAllocMemBlock - (uintptr_t)pMemPool) % szMemBlockFixSize != 0)//不在定长内存块边界上
		{
			return false;
		}

		//计算映射
		size_t szBitmapIndex = ((uintptr_t)pAllocMemBlock - (uintptr_t)pMemPool) / szMemBlockFixSize;

		//内存已经是释放状态（多次重复释放）
		if (bArrMemBlockBitmap[szBitmapIndex] == false)
		{
			return false;
		}

		//设置释放内存块指向链表头
		((FreeBlock *)pAllocMemBlock)->pNextFreeMemBlock = pFreeMemBlockHead;
		//将释放的内存块插入链表头
		pFreeMemBlockHead = (FreeBlock *)pAllocMemBlock;
		//设置为释放状态
		bArrMemBlockBitmap[szBitmapIndex] = false;
		//设置使用个数
		--szMemBlockUse;

		return true;
	}

	//构造并返回对象
	template<typename... Args>
	Type *AllocMemBlockConstructor(Args&&... args) noexcept
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

	//重分配内存池，注意，该函数会导致内存池分配出的内存块全部失效！
	void Realloc(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024)
	{
		Clear();//先清理原先的内存池

		szMemBlockFixSize = _szMemBlockFixSize;
		szMemBlockNum = _szMemBlockPreAllocNum;

		//确保用户传入错误参数时不要构造失败
		if (szMemBlockFixSize < szMemBlockMinSize)//设置内存块大小至少为下限大小
		{
			szMemBlockFixSize = szMemBlockMinSize;
		}

		if (szMemBlockNum == 0)
		{
			szMemBlockNum = 1;
		}

		//位图、内存池的起始地址全部对齐到szAlignment
		size_t szBitMapAlignedSize = AlignedSize(szMemBlockNum * sizeof(*bArrMemBlockBitmap));//从上一个对齐地址加上这个数值就是下一个对齐地址的开始
		szMemPoolSize = szMemBlockNum * szMemBlockFixSize;//此处内存池后无后继结构，无需对齐

		//一次性分配
		size_t szBaseMemSize = szBitMapAlignedSize + szMemPoolSize + (szAlignment - 1);//计算总所需内存
		pBaseMem = ThrowMalloc(szBaseMemSize);//请求内存

		//分割
		bArrMemBlockBitmap = (bool *)AlignedMem(pBaseMem);
		pMemPool = (void *)((uintptr_t)bArrMemBlockBitmap + szBitMapAlignedSize);

		//初始化内存位图、栈数组
		Reset();
	}

	//清理内存池，注意，该函数会导致内存池分配出的内存块全部失效！
	void Clear(void) noexcept
	{
		szMemBlockFixSize = 0;

		bArrMemBlockBitmap = NULL;
		pFreeMemBlockHead = NULL;

		pMemPool = NULL;
		szMemPoolSize = 0;
		szMemBlockNum = 0;
		szMemBlockUse = 0;

		NoThrowFree(pBaseMem), pBaseMem = NULL;
	}

	//重置类状态为初始化状态，注意，该函数会导致内存池分配出的内存块全部失效！
	void Reset(void) noexcept
	{
		//设置位图为未分配
		memset(bArrMemBlockBitmap, false, szMemBlockNum * sizeof(bool));

		//设置链表头指向第一个内存块
		pFreeMemBlockHead = (FreeBlock *)pMemPool;
		//设置内存块使用计数为0
		szMemBlockUse = 0;

		//依次用指针串联空闲内存块
		void *pCurrentMemBlock = pFreeMemBlockHead;
		for (size_t i = 0; i < szMemBlockNum - 1; ++i)
		{
			//计算下一个内存块的地址
			void *pNextMemBlock = (void *)((uintptr_t)pCurrentMemBlock + szMemBlockFixSize);
			//设置当前内存块指向下一个内存块
			((FreeBlock *)pCurrentMemBlock)->pNextFreeMemBlock = (FreeBlock *)pNextMemBlock;
			//处理下一个内存块
			pCurrentMemBlock = pNextMemBlock;
		}

		((FreeBlock *)pCurrentMemBlock)->pNextFreeMemBlock = NULL;//设置最后一个内存块指向NULL
	}

	//用于比较指针和内存池关系的函数,返回-1代表小于内存池基地址,返回0代表在内存池中,返回1代表大等于内存池尾部
	long CmpPointAndPool(const void *pMem) const noexcept
	{
		if (pMem < pMemPool)
		{
			return -1;
		}
		else if (pMem >= (void *)((uintptr_t)pMemPool + szMemPoolSize))
		{
			return 1;
		}
		else//在内存池中
		{
			return 0;
		}
	}

	//获取定长内存块的大小
	size_t GetMemBlockFixSize(void) const noexcept
	{
		return szMemBlockFixSize;
	}

	//获取内存池总共包含的内存块数
	size_t GetMemBlockNum(void) const noexcept
	{
		return szMemBlockNum;
	}

	//获取内存池总共已用的内存块数
	size_t GetMemBlockUse(void) const noexcept
	{
		return szMemBlockUse;
	}

	//获取内存池的起始地址(用于排序多个内存池)
	const void *GetMemPool(void) const noexcept
	{
		return pMemPool;
	}

	//用于计算第一个参数向上舍入对齐到第二个参数代表的数值边界的函数
	static size_t Aligned(size_t szSize, size_t szAlign)
	{
		if (szAlign == 0 || szAlign == 1)
		{
			return szSize;
		}
		else if (szAlign % 2 == 0)
		{
			return (szSize + szAlign - 1) & ~(szAlign - 1);
		}
		else
		{
			return szSize < szAlign ? szAlign : szSize + (szAlign - (szSize % szAlign));
		}
	}
};