#pragma once
/*
��λͼ�����ѷ����ڴ������ڵ㣬�ÿ����ڴ�ջ�ڵ������ڴ�λͼ


���ݳ�Ա��
void* mem_pool��һ���ڴ�أ����չ̶���С���ֿ�
void** mem_bitmap��һ���ڴ�λͼ���洢�ѷ����ڴ�������ڵ��ַ�������ַ��NULL��������ڴ�����
void** free_mem_stack��һ��ջ���飬�洢�������ڴ��ָ��
struct listnode{void*,listnode*} alloc_mem_list��һ�������洢�ѷ����ڴ��

size_t stack_top��һ�������±꣬�����洢ջ����ջ��
size_t fixed_length���û���ʼ��ʱȷ���ĳ���


�ӿڳ�Ա��
��ʼ�������û�����ĳ��ȳ�ʼ���ڴ�غͳ��ȣ���ϵͳ���䲢��ʼ��ȫ0���ڴ�λͼ����ϵͳ���䲢��ʼ��ջ���飬��ʼ��ջ����ջ���±꣬��ϵͳ���䲢��ʼ������
������ֱ���ͷ��ڴ�أ��������г�ԱΪ0



���䣺��ջ���鶥������һ���������ڴ�飬�����ѷ����ڴ�������������ڴ�λͼ��Ӧ���ڴ�����״̬�������ѷ����ڴ������Ľڵ��ַ
�ͷţ����ڴ�λͼ��Ѱ�Ҵ��ͷŵ�ַ��Ӧ�ڴ��ķ���״̬�����Ϊ��ʹ�ã������ø�λͼ�ڵ��д洢���ѷ�������ڵ��ַ����������ڵ�ִ��ɾ����������ѹ��������ڴ��ջ���鶥��

�ͷ�ȫ����ɾ���ѷ����ڴ�����������нڵ㣬ֱ������ջ����


ʵ��ϸ�ڣ�
��ǰ��ʱ�������ڴ����������������Ŀ����������ٴ�������⣨Ԥ����Ҫ�������ν����е��ڴ�أ�
ʹ��˫�����������������ͷ�ֱ�洢��ͬ�����ݣ��������ѷ����ڴ�飬�����ǿ����ڴ�飩�Ż���ջ���飬������һ��Ԫ�ش洢��ǰ�ڴ�����Ѿ������ȥ����ߵ�ַ�ĺ���ڴ���ַ�����������ʼ�������ڴ�������ֺ��ڿ�������

��ʼ��ջ���飺
			����1��ȫ���ʼ����Ȼ���ʼ��ջ��Ԫ��Ϊ�ڴ����ʼ��ַ�������ڴ�ʱ���ȼ��ջ�����һ��Ԫ���Ƿ�Ϊ0������������õ�ǰԪ�ص�ֵ������Ԫ�ص�ֵȻ�󵯳���ǰԪ�أ������ʼ�����ڿ������㣩
			����2����ȫ��ʼ��������ѭ��������ÿ��Ԫ�ض���ʼ�������ڴ��ַ����������ʱֱ��ʹ�ã��ڿ��ʼ�����޷�������ʱ��
��ʼ������
			����1��ʹ���α�����ģ��ľ�̬���������������������нڵ���������ʼ��
			����2��ʹ������������ʹ��ʱ��ϵͳ�����ڴ棬���ں����ͷŽڵ�ʱ�������ͷŽڵ��������������ͷţ������ͷţ������ط����ʱ��
����ʵ�֣�
			����1��ʹ�õ������ڴ�λͼ�洢�������ǰ���ڵ�����ǵ�ǰ�ڵ��Ա�ɾ���ڵ�ʱ�ܹ��޸�ǰ���ڵ㣬���������������������ط�ɾ���˵�ǰλͼ�洢��ǰ���ڵ㣬��ᵼ�´���
			����2��ʹ��˫�����ڴ�λͼֱ�Ӵ洢��ǰ�ڵ㣬ɾ��ʱֱ���޸Ľڵ�


��ʱ��
���䣺O(1)
�ͷţ�O(1)


*/

/*
����˼�����ѷ����ڴ�����������壬�����������
��λͼ���������ͷ��ڴ��ַ��״̬�Ƿ���ȷ���ÿ����ڴ�ջ�ڵ������ڴ�λͼ
Ҳ�����ڴ�λͼ���ڼ���ͷŵ�ַ�Ƿ�Ϸ���ջ���ڴ洢�������ڴ��


���ݳ�Ա��
void* mem_pool��һ���ڴ�أ����չ̶���С���ֿ�
bool* mem_bitmap��һ���ڴ�λͼ���洢�ڴ��ķ���״̬��true���ѷ��䣬false��δ���䣬���ڼ���û��ͷŹ����е��ڴ��ַ�Ƿ�Ϸ�
void** free_mem_stack��һ��ջ���飬�洢�������ڴ��ָ�룬�����ʱ��ͨ���洢�ĵ�ַ����ӳ�䵽�ڴ�λͼ�������ڴ����״̬

size_t stack_top��һ�������±꣬�����洢ջ����ջ��
size_t fixed_length���û���ʼ��ʱȷ���ĳ���


�ӿڳ�Ա��
��ʼ�������û�����ĳ��ȳ�ʼ���ڴ�غͳ��ȣ���ϵͳ���䲢��ʼ��ȫ0���ڴ�λͼ����ϵͳ���䲢��ʼ��ջ���飬��ʼ��ջ����ջ���±�
������ֱ���ͷ��ڴ�أ��������г�ԱΪ0

���䣺��ջ���鶥������һ���������ڴ�飬���ø��ڴ����ʼ��ַӳ����ڴ�λͼ���ڴ�����״̬
�ͷţ��Ƚ��е�ַ��飬���С���ڴ����ʼ��ַ������ڴ��β���ַ���Ǹõ�ַ���Ǵ��ڴ����ʼ��ַ��ʼ�ģ��û�ȷ�����ڴ���ַ������������ֱ�ӱ���������ڴ�λͼ��Ѱ�Ҵ��ͷŵ�ַ��Ӧ�ڴ��ķ���״̬�����Ϊ��ʹ�ã���ѹ��������ڴ��ջ���鶥�������򱨴�

�ͷ�ȫ����ֱ�������ڴ�λͼΪȫ0�������ݲ�������ջ����Ϊȫ0�����³�ʼ��ջ����

ʵ��ϸ�ڣ�
��ǰ��ʱ�������ڴ����������������Ŀ����������ٴ�������⣨Ԥ����Ҫ�������ν����е��ڴ�أ�

��ʼ��ջ���飺
			����1��ȫ���ʼ����Ȼ���ʼ��ջ��Ԫ��Ϊ�ڴ����ʼ��ַ�������ڴ�ʱ���ȼ��ջ�����һ��Ԫ���Ƿ�Ϊ0������������õ�ǰԪ�ص�ֵ������Ԫ�ص�ֵȻ�󵯳���ǰԪ�أ������ʼ�����ڿ������㣩
			����2����ȫ��ʼ��������ѭ��������ÿ��Ԫ�ض���ʼ�������ڴ��ַ����������ʱֱ��ʹ�ã��ڿ��ʼ�����޷�������ʱ��

��ʱ��
���䣺O(1)
�ͷţ�O(1)
*/

//#include <malloc.h>
#include <new>
#include <string.h>


#define SENTINEL_POINTER ((void *)(NULL + 1))//�ڱ�ָ��

struct default_alloc
{
	void *operator()(size_t szMemSize)
	{
		//return malloc(szMemSize);
		return new char[szMemSize];
	}
};

struct default_free
{
	void operator()(void *pMem)
	{
		//return free(pMem);
		return delete (char *)pMem;
	}
};

template <typename type, bool bLazyInit = true, size_t szAlignment = 4, typename alloc_func = default_alloc, typename free_func = default_free>//����ʱ�ķ������͡������ʼ�����ԣ��˲��Ի��޸Ĵ���Σ�����ʹ��ģ�� & constexpr if��
class FixLen_MemPool
{
	static_assert(szAlignment == 1 || (szAlignment != 0 && szAlignment % 2 == 0));
private:
	size_t szMemBlockFixSize = 0;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����
	
	bool *bArrMemBlockBitmap = NULL;//bool�����ڴ�λͼ
	void **pArrMemBlockStack = NULL;//ָ������ ջ
	size_t szStackTop = 0;//ջ��������ջ���������ɸߵ��ͣ���Ҳ��������ʹ�õ��ڴ����Ŀ�����ڴ�����ѷ����ȥ���ڴ����

	void *pMemPool = NULL;//ָ�� �ڴ��
	size_t szPoolSize = 0;//�ڴ�ش�С��size���ֽ�����
	size_t szMemBlockNum = 0;//�ڴ�������ڴ�����

	void *pBaseMem = NULL;//�ڴ����ַ(���ں����ͷ�)
	//size_t szBaseMemSize = 0;//���ڴ��С

protected:
	void *ThrowMalloc(size_t szMemSize)
	{
		alloc_func fAlloc;
		void *p = fAlloc(szMemSize);
		if (p == NULL)
		{
			throw std::bad_alloc();
		}
		return p;
	}

	void NoThrowFree(void *pMem)
	{
		free_func fFree;
		fFree(pMem);
	}

	size_t AlignedSize(size_t szSize)
	{
		return (szSize + szAlignment - 1) & ~(szAlignment - 1);
	}

	void* AlignedMem(void* pMem)
	{
		return (void *)(((uintptr_t)pMem + szAlignment - 1) & ~(szAlignment - 1));
	}

	const void *GetMemPool(void) const
	{
		return pMemPool;
	}
public:
	FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(type), size_t _szMemBlockPreAllocNum = 1024) :
		szMemBlockFixSize(_szMemBlockFixSize),
		szMemBlockNum(_szMemBlockPreAllocNum)
	{
		//λͼ��ջ���ڴ�ص���ʼ��ַȫ�����뵽szAlignment
		size_t szBitMapAlignedSize = AlignedSize(szMemBlockNum * sizeof(*bArrMemBlockBitmap));//����һ�������ַ���������ֵ������һ�������ַ�Ŀ�ʼ����ͬ
		size_t szStackAlignedSize = AlignedSize((szMemBlockNum + bLazyInit) * sizeof(*pArrMemBlockStack));//�˴�+bLazyInitȷ�������ʼ�������һ��ջ�ռ�
		szPoolSize = szMemBlockNum * szMemBlockFixSize;//�˴��ڴ�غ��޺�̽ṹ���������

		//һ���Է���
		size_t szBaseMemSize = szBitMapAlignedSize + szStackAlignedSize + szPoolSize + szAlignment - 1;//�����������ڴ�
		pBaseMem = ThrowMalloc(szBaseMemSize);//�����ڴ�

		//�ָ�
		bArrMemBlockBitmap = (bool *)AlignedMem(pBaseMem);
		pArrMemBlockStack = (void **)((uintptr_t)bArrMemBlockBitmap + szBitMapAlignedSize);
		pMemPool = (void *)((uintptr_t)pArrMemBlockStack + szStackAlignedSize);
	
		//��ʼ���ڴ�λͼ��ջ����
		FreeAllMemBlock();
	}

	FixLen_MemPool(const FixLen_MemPool &) = delete;//�����࿽������

	FixLen_MemPool(FixLen_MemPool &&_Move) noexcept ://�ƶ�����
		szMemBlockFixSize(_Move.szMemBlockFixSize),

		bArrMemBlockBitmap(_Move.bArrMemBlockBitmap),
		pArrMemBlockStack(_Move.pArrMemBlockStack),
		szStackTop(_Move.szStackTop),

		pMemPool(_Move.pMemPool),
		szPoolSize(_Move.szPoolSize),
		szMemBlockNum(_Move.szMemBlockNum),
		
		pBaseMem(_Move.pBaseMem)//,
		//szBaseMemSize(_Move.szBaseMemSize)
	{
		//�����ƶ������Ա
		_Move.szMemBlockFixSize = 0;
		
		_Move.bArrMemBlockBitmap = NULL;
		_Move.pArrMemBlockStack = NULL;
		_Move.szStackTop = 0;

		_Move.pMemPool = NULL;
		_Move.szPoolSize = 0;
		_Move.szMemBlockNum = 0;

		_Move.pBaseMem = NULL;
		//_Move.szBaseMemSize = 0;
	}

	~FixLen_MemPool(void) noexcept
	{
		szMemBlockFixSize = 0;

		bArrMemBlockBitmap = NULL;
		pArrMemBlockStack = NULL;
		szStackTop = 0;

		pMemPool = NULL;
		szPoolSize = 0;
		szMemBlockNum = 0;
		
		NoThrowFree(pBaseMem), pBaseMem = NULL;
		//szBaseMemSize = 0;
	}

	type *AllocMemBlock(void) noexcept
	{
		if (szStackTop >= szMemBlockNum)//û�п����ڴ����
		{
			return NULL;
		}

		//��ջ�е���һ�������ڴ��
		void *pFreeMemBlock = pArrMemBlockStack[szStackTop];
		//�ƶ�ջ����ջ
		++szStackTop;

		//�ж��ǲ��������ʼ����
		if constexpr (bLazyInit == true)
		{
			//���������Ҫ�����һ��ջ�����Ƿ񱻳�ʼ���������ǵĻ�Ҫ���г�ʼ��
			if (pArrMemBlockStack[szStackTop] == NULL)//ʹ���ˣ���ΪNULL�����ڱ�ֵ���˴������ټ��szStackTop < szMemBlockNum
			{
				pArrMemBlockStack[szStackTop] = (void *)((uintptr_t)pFreeMemBlock + szMemBlockFixSize);
			}
		}

		//���ڴ���ַӳ�䵽λͼ
		size_t szBitmapIndex = ((uintptr_t)pFreeMemBlock - (uintptr_t)pMemPool) / szMemBlockFixSize;
		//����Ϊ����״̬
		bArrMemBlockBitmap[szBitmapIndex] = true;

		return (type *)pFreeMemBlock;
	}

	bool FreeMemBlock(type *pAllocMemBlock) noexcept//�ͷŷ��ڴ�ط�����ڴ桢����ͷŻ᷵��false
	{
		if (pAllocMemBlock == NULL)//��ָ��
		{
			return true;//ֱ�ӳɹ�
		}

		if (CmpPointAndPool(pAllocMemBlock) != 0 ||//�����ڴ�ط�Χ
			((uintptr_t)pAllocMemBlock - (uintptr_t)pMemPool) % szMemBlockFixSize != 0)//���ڶ����ڴ��߽���
		{
			return false;
		}

		//����ӳ��
		size_t szBitmapIndex = ((uintptr_t)pAllocMemBlock - (uintptr_t)pMemPool) / szMemBlockFixSize;

		//�ڴ��Ѿ����ͷ�״̬������ظ��ͷţ�
		if (bArrMemBlockBitmap[szBitmapIndex] == false)
		{
			return false;
		}

		//����Ϊ�ͷ�״̬
		bArrMemBlockBitmap[szBitmapIndex] = false;
		//�ƶ�ջ����ջ
		--szStackTop;
		//���ͷŵ��ڴ��ѹ��ջ��
		pArrMemBlockStack[szStackTop] = pAllocMemBlock;

		return true;
	}

	void FreeAllMemBlock(void) noexcept
	{
		//����λͼΪδ����
		memset(bArrMemBlockBitmap, false, szMemBlockNum * sizeof(bool));

		//����ջ��Ϊ0
		szStackTop = 0;

		//����ջ����
		if constexpr (bLazyInit == true)
		{
			//�����ʼ��
			pArrMemBlockStack[szStackTop] = pMemPool;//������ʼ��ַ��ʣ�µ�ȫ0
			memset(&pArrMemBlockStack[szStackTop + 1], NULL, szMemBlockNum * sizeof(*pArrMemBlockStack));//���м䶼���ó�NULL
			pArrMemBlockStack[szMemBlockNum] = SENTINEL_POINTER;//����β��Ϊһ����NULL�ķǷ���ַ��ʵ���ϲ����ᱻ�����ȥ�����������ڱ���ǣ�
			//ע�⣺���������pArrMemBlockStack[szMemBlockNum]�����ᳬβ���ʣ���Ϊ��ʼ�������ڴ�ʱ�����bLazyInit
		}
		else
		{
			//��ȫ��ʼ��
			void *pMemBlockAddr = pMemPool;
			for (size_t i = 0; i < szMemBlockNum; ++i)
			{
				pArrMemBlockStack[i] = pMemBlockAddr;
				pMemBlockAddr = (void *)((uintptr_t)pMemBlockAddr + szMemBlockFixSize);
			}
		}
	}

	int CmpPointAndPool(const void *p) const//����-1����С���ڴ�ػ���ַ������0�������ڴ���У�����1���������ڴ��β��
	{
		if (p < pMemPool)
		{
			return -1;
		}
		else if (p >= (void *)((uintptr_t)pMemPool + szPoolSize))
		{
			return 1;
		}
		else//���ڴ����
		{
			return 0;
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
		return szStackTop;
	}
};

#undef SENTINEL_POINTER