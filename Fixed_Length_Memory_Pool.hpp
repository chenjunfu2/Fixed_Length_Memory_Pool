#pragma once
/*
��λͼ���������ͷ��ڴ��ַ��״̬�Ƿ���ȷ���ÿ����ڴ�ջ�ڵ������ڴ�λͼ
Ҳ�����ڴ�λͼ���ڼ���ͷŵ�ַ�Ƿ�Ϸ���ջ���ڴ洢�������ڴ��

ʵ��������ڴ�ؾ���ͨ�����ٶ�ϵͳ���õĴ�����������������
����ڴ�ذ����������ݽṹ���ڹ���block�ķ�����ͷţ���һ����һ������blockջ�������ʱ����ջ���Ŀ���block����������û������յ�ʱ����û����ص�ָ��ѹ��ջ��
��һ����һ���ڴ�λͼ��������֤�ͷŹ����е��û������ָ�뵱ǰ��״̬��ʲô�����ͷ�orδ�ͷţ���ֹ�û�����ͷ�ͬһ��ָ�뵼��ջ�ڴ��ڶ���ظ�����������ջ
�����ʱ���ջ�е������ڴ���ͬ�������ڴ�����ڴ�λͼ�е�ӳ�䣬�����ó�true�ѷ��䣬�����ͷŹ����м���û�ָ���Ƿ����ڴ���С��Ƿ����ڴ��߽�����
�ͻ����ӳ�����Ȼ�����ڴ�λͼ�е�boolֵ���бȶԣ�ֻҪ���ֵ��true�ѷ��䣬������û��ͷŵ���һ����ȷ��ָ�룬��ô�ͽ��ڴ�λͼ�����ֵ���ó�false��ѹ�����ջ

ʵ�����ڴ�λͼ�����Ͼ�����������ȫ�ͷż��ģ������100%�����û�������ȫ�ޱ�ʹ�������λͼ��һ��ջ�����䵯�����ͷ�ѹ�뼴�ɣ�Ȼ��ɶʱ���û�һ����С�ĳ����ը�ˣ�����

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
#include <utility>


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

#define SENTINEL_POINTER ((void *)(NULL + 1))//�ڱ�ָ��

template <
	typename Type,//����ʱ�ķ�������
	bool bLazyInit = false,//�����ʼ�����ԣ��˲��Ի��޸Ĵ���Σ�����ʹ��ģ�� & constexpr if��
	size_t szAlignment = 4,//�ڴ����߽�
	typename Alloc_func = default_alloc,//Ĭ�Ϸ�����
	typename Free_func = default_free>//Ĭ���ͷ���
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

	void NoThrowFree(void *pMem) const
	{
		Free_func fFree;
		fFree(pMem);
	}

	static size_t AlignedSize(size_t szSize)
	{
		return (szSize + szAlignment - 1) & ~(szAlignment - 1);
	}

	static void *AlignedMem(void *pMem)
	{
		return (void *)(((uintptr_t)pMem + szAlignment - 1) & ~(szAlignment - 1));
	}
public:
	using RetPoint_Type = Type;
	static constexpr size_t szLazyInitExtraRequireSize = bLazyInit * sizeof(*pArrMemBlockStack);//�����ʼ������Ķ����ڴ�
	static constexpr size_t szManageMemBlockRequireSize = sizeof(*bArrMemBlockBitmap) + sizeof(*pArrMemBlockStack);//����һ���ڴ������Ĺ����ڴ��С
	static constexpr size_t szAlignmentSize = szAlignment;//�����ڴ�ı߽�

	FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024) :
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
		Reset();
	}

	FixLen_MemPool(const FixLen_MemPool &) = delete;//�����࿽������
	FixLen_MemPool &operator=(const FixLen_MemPool &) = delete;//���ø��Ƹ�ֵ����

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

	Type *AllocMemBlock(void) noexcept
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

		return (Type *)pFreeMemBlock;
	}

	bool FreeMemBlock(Type *pAllocMemBlock) noexcept//�ͷŷ��ڴ�ط�����ڴ桢����ͷŻ᷵��false
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

	//���첢���ض����ַ
	template<typename... Args>
	Type *AllocMemBlockConstructor(Args&&... args) noexcept
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

	//���������ն����ַ
	bool FreeMemBlockDestructor(Type *pAllocMemBlock) noexcept
	{
		pAllocMemBlock->~Type();
		return FreeMemBlock(pAllocMemBlock);
	}

	void Reset(void) noexcept
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

	long CmpPointAndPool(const void *pMem)  const noexcept//����-1����С���ڴ�ػ���ַ������0�������ڴ���У�����1���������ڴ��β��
	{
		if (pMem < pMemPool)
		{
			return -1;
		}
		else if (pMem >= (void *)((uintptr_t)pMemPool + szPoolSize))
		{
			return 1;
		}
		else//���ڴ����
		{
			return 0;
		}
	}

	size_t GetMemBlockFixSize(void) const noexcept
	{
		return szMemBlockFixSize;
	}

	size_t GetMemBlockNum(void) const noexcept
	{
		return szMemBlockNum;
	}

	size_t GetMemBlockUse(void) const noexcept
	{
		return szStackTop;
	}

	const void *GetMemPool(void) const noexcept
	{
		return pMemPool;
	}

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

#undef SENTINEL_POINTER