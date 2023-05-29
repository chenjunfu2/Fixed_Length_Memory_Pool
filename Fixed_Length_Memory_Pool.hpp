#pragma once
/*
用位图索引已分配内存块链表节点，用空闲内存栈节点索引内存位图


数据成员：
void* mem_pool：一个内存池，按照固定大小来分块
void** mem_bitmap：一个内存位图，存储已分配内存块的链表节点地址，如果地址是NULL，则代表内存块空闲
void** free_mem_stack：一个栈数组，存储待分配内存块指针
struct listnode{void*,listnode*} alloc_mem_list：一个链表，存储已分配内存块

size_t stack_top：一个数组下标，用来存储栈数组栈顶
size_t fixed_length：用户初始化时确定的长度


接口成员：
初始化：从用户传入的长度初始化内存池和长度，从系统分配并初始化全0的内存位图，从系统分配并初始化栈数组，初始化栈数组栈顶下标，从系统分配并初始化链表
析构：直接释放内存池，设置所有成员为0



分配：从栈数组顶部弹出一个待分配内存块，插入已分配内存块链表，并设置内存位图对应的内存块分配状态与插入的已分配内存块链表的节点地址
释放：从内存位图中寻找此释放地址对应内存块的分配状态，如果为已使用，则利用该位图节点中存储的已分配链表节点地址索引到链表节点执行删除操作，并压入待分配内存块栈数组顶部

释放全部：删除已分配内存块链表是所有节点，直接设置栈数组


实现细节：
当前暂时不考虑内存池用完的情况，等项目雏形做完后再处理此问题（预计需要用链表衔接所有的内存池）
使用双链表，可以让链表的两头分别存储不同的数据（正向是已分配内存块，反向是空闲内存块）优化掉栈数组，并增加一个元素存储当前内存池中已经分配出去的最高地址的后继内存块地址，用于懒惰初始化空闲内存块链表部分和勤快分配计算

初始化栈数组：
			方案1：全零初始化，然后初始化栈顶元素为内存池起始地址，分配内存时，先检查栈顶后的一个元素是否为0，如果是则利用当前元素的值计算后继元素的值然后弹出当前元素（懒惰初始化，勤快分配计算）
			方案2：完全初始化，利用循环给数组每个元素都初始化空闲内存地址，后续分配时直接使用（勤快初始化，无分配计算耗时）
初始化链表：
			方案1：使用游标数组模拟的静态链表，给整个链表待分配空闲节点链接做初始化
			方案2：使用真链表，并在使用时从系统分配内存，并在后续释放节点时插入已释放节点链表而不是真的释放（懒惰释放，减少重分配耗时）
链表实现：
			方案1：使用单链表，内存位图存储单链表的前驱节点而不是当前节点以便删除节点时能够修改前驱节点，但是这种情况下如果其它地方删除了当前位图存储的前驱节点，则会导致错误
			方案2：使用双链表，内存位图直接存储当前节点，删除时直接修改节点


耗时：
分配：O(1)
释放：O(1)


*/

/*
经过思考，已分配内存块链表并无意义，进行重新设计
用位图索引检查待释放内存地址和状态是否正确，用空闲内存栈节点索引内存位图
也就是内存位图用于检查释放地址是否合法，栈用于存储待分配内存块


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
*/

#include <malloc.h>
#include <string.h>
#include <algorithm>

#define SENTINEL_POINTER ((void *)(NULL + 1))//哨兵指针

template <typename type, bool bLazyInit = false>//分配时的返回类型、懒惰初始化策略（此策略会修改代码段，所以使用模板 & constexpr if）
class FixLen_MemPool
{
private:
	size_t szMemBlockFixSize = 0;//用户初始化时需要的定长内存长度（size：字节数）

	void *pMemPool = NULL;//指针 内存池
	size_t szPoolSize = 0;//内存池大小（size：字节数）
	size_t szMemBlockNum = 0;//内存池中总内存块个数

	bool *bArrMemBlockBitmap = NULL;//bool数组 内存位图
	void **pArrFreeMemBlockStack = NULL;//指针数组 栈
	size_t szStackTop = 0;//栈顶索引（栈生长方向：由高到低），也代表着已使用的内存块数目，即内存池中已分配出去的内存块数

protected:
	template<typename T>
	T *ThrowMalloc(size_t szNum)
	{
		T *p = (T *)malloc(sizeof(T) * szNum);
		if (p == NULL)
		{
			throw std::bad_alloc();
		}
		return p;
	}

	template<typename T>
	T *ThrowRealloc(void *pOld, size_t szNewNum)
	{
		T *pNew = (T *)realloc(pOld, sizeof(T) * szNewNum);
		if (pNew == NULL)
		{
			throw std::bad_alloc();
		}

		return pNew;
	}

	const void *GetMemPool(void) const
	{
		return pMemPool;
	}
public:
	FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(type), size_t _szMemBlockPreAllocNum = 1024) :
		szMemBlockFixSize(_szMemBlockFixSize)
	{
		//初始化内存块个数
		szMemBlockNum = _szMemBlockPreAllocNum;
		//计算内存池大小
		szPoolSize = szMemBlockNum * szMemBlockFixSize;

		//先分配内存池
		pMemPool = ThrowMalloc<char>(szPoolSize);//内存池无类型，直接按char分配
		//再分配内存位图
		bArrMemBlockBitmap = ThrowMalloc<bool>(szMemBlockNum);
		//接着分配栈数组
		if constexpr (bLazyInit == true)
		{
			pArrFreeMemBlockStack = ThrowMalloc<void *>(szMemBlockNum + 1);//懒惰初始化时需分配多一个用于做哨兵标记
		}
		else
		{
			pArrFreeMemBlockStack = ThrowMalloc<void *>(szMemBlockNum);//完全初始化时只需分配所需大小
		}
	
		//初始化内存位图、栈数组
		FreeAllMemBlock();
	}

	FixLen_MemPool(const FixLen_MemPool &) = delete;//禁用类拷贝构造

	FixLen_MemPool(FixLen_MemPool &&_Move) noexcept ://移动构造
		szMemBlockFixSize(_Move.szMemBlockFixSize),

		pMemPool(_Move.pMemPool),
		szPoolSize(_Move.szPoolSize),
		szMemBlockNum(_Move.szMemBlockNum),

		bArrMemBlockBitmap(_Move.bArrMemBlockBitmap),
		pArrFreeMemBlockStack(_Move.pArrFreeMemBlockStack),
		szStackTop(_Move.szStackTop)
	{
		//清理移动对象成员
		_Move.szMemBlockFixSize = 0;
		
		_Move.pMemPool = NULL;
		_Move.szPoolSize = 0;
		_Move.szMemBlockNum = 0;

		_Move.bArrMemBlockBitmap = NULL;
		_Move.pArrFreeMemBlockStack = NULL;
		_Move.szStackTop = 0;
	}

	~FixLen_MemPool(void) noexcept
	{
		szMemBlockFixSize = 0;

		free(pMemPool), pMemPool = NULL;
		szPoolSize = 0;
		szMemBlockNum = 0;

		free(bArrMemBlockBitmap), bArrMemBlockBitmap = NULL;
		free(pArrFreeMemBlockStack), pArrFreeMemBlockStack = NULL;
		szStackTop = 0;
	}

	type *AllocMemBlock(void) noexcept
	{
		if (szStackTop >= szMemBlockNum)//没有空闲内存块了
		{
			return NULL;
		}

		//从栈中弹出一个空闲内存块
		void *pFreeMemBlock = pArrFreeMemBlockStack[szStackTop];
		//移动栈顶出栈
		++szStackTop;

		//判断是不是懒惰初始化的
		if constexpr (bLazyInit == true)
		{
			//如果是则需要检查下一个栈数据是否被初始化过，不是的话要进行初始化
			if (pArrFreeMemBlockStack[szStackTop] == NULL)//使用了（不为NULL）的哨兵值，此处无需再检查szStackTop < szMemBlockNum
			{
				pArrFreeMemBlockStack[szStackTop] = (char *)pFreeMemBlock + szMemBlockFixSize;
			}
		}

		//从内存块地址映射到位图
		size_t szBitmapIndex = ((char *)pFreeMemBlock - (char *)pMemPool) / szMemBlockFixSize;
		//设置为分配状态
		bArrMemBlockBitmap[szBitmapIndex] = true;

		return (type *)pFreeMemBlock;
	}

	bool FreeMemBlock(type *pAllocMemBlock) noexcept//释放非内存池分配的内存、多次释放会返回false
	{
		if (pAllocMemBlock == NULL)//空指针
		{
			return true;//直接成功
		}

		if (CmpPointAndPool(pAllocMemBlock) != 0 ||//超出内存池范围
			((char *)pAllocMemBlock - (char *)pMemPool) % szMemBlockFixSize != 0)//不在定长内存块边界上
		{
			return false;
		}

		//计算映射
		size_t szBitmapIndex = ((char *)pAllocMemBlock - (char *)pMemPool) / szMemBlockFixSize;

		//内存已经是释放状态（多次重复释放）
		if (bArrMemBlockBitmap[szBitmapIndex] == false)
		{
			return false;
		}

		//设置为释放状态
		bArrMemBlockBitmap[szBitmapIndex] = false;
		//移动栈顶入栈
		--szStackTop;
		//将释放的内存块压入栈中
		pArrFreeMemBlockStack[szStackTop] = pAllocMemBlock;

		return true;
	}

	void FreeAllMemBlock(void) noexcept
	{
		//设置位图为未分配
		memset(bArrMemBlockBitmap, false, szMemBlockNum * sizeof(bool));

		//设置栈顶为0
		szStackTop = 0;

		//设置栈数据
		if constexpr (bLazyInit == true)
		{
			//懒惰初始化
			pArrFreeMemBlockStack[szStackTop] = pMemPool;//设置起始地址，剩下的全0
			memset(&pArrFreeMemBlockStack[szStackTop + 1], NULL, szMemBlockNum * sizeof(void *));//把中间都设置成NULL
			pArrFreeMemBlockStack[szMemBlockNum] = SENTINEL_POINTER;//设置尾部为一个非NULL的非法地址（实际上并不会被分配出去，仅用于做哨兵标记）
			//注意：这种情况下pArrFreeMemBlockStack[szMemBlockNum]并不会超尾访问
		}
		else
		{
			//完全初始化
			void *pMemBlockAddr = pMemPool;
			for (size_t i = 0; i < szMemBlockNum; ++i)
			{
				pArrFreeMemBlockStack[i] = pMemBlockAddr;
				pMemBlockAddr = (char *)pMemBlockAddr + szMemBlockFixSize;
			}
		}
	}

	int CmpPointAndPool(const void *p) const//返回-1代表小于内存池基地址，返回0代表在内存池中，返回1代表大等于内存池尾部
	{
		if (p < pMemPool)
		{
			return -1;
		}
		else if (p >= (char *)pMemPool + szPoolSize)
		{
			return 1;
		}
		else//在内存池中
		{
			return 0;
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
		return szStackTop;
	}
};

#undef SENTINEL_POINTER