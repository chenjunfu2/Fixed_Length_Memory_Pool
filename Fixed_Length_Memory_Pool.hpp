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


�µĿռ��Ż�������
ȡ��ջ���飬����top�����޸�Ϊָ�룬�����ڴ�ص����ڴ��Ĵ�С����Ϊһ��ָ���С
���ÿ����ڴ��ռ���Ϊ����ڵ㣬�洢һ��ָ��ָ����һ�������ڴ�飬�����������п����ڴ�飬
��ʼ������Ϊ��ȫ��ʼ����ɾ�������ʼ������

��ѡ�Ŀռ��Ż�������
���ڴ�λͼ��bool��Ϊunsigned char��������ÿһλ�洢ӳ�䣬ʹ���ڴ�λͼռ�ÿռ�Ϊԭ����1/8
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
	typename Type,//����ʱ�ķ�������
	size_t szAlignment = 4,//�ڴ����߽�
	bool bLessMemExpend = false,//���ٵ��ڴ�����ʵ��
	typename Alloc_func = default_alloc,//Ĭ�Ϸ�����
	typename Free_func = default_free//Ĭ���ͷ���
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
	size_t szMemBlockFixSize;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����

	bool *bArrMemBlockBitmap;//bool�����ڴ�λͼ

	union
	{
		void **pArrFreeMemBlockStack;//ָ������ ջ
		FreeBlock *pFreeMemBlockHead;//��������ͷָ��
	}unFree;
	
	union
	{
		size_t szStackTop;//ջ���±꣨ջ���������ɸߵ��ͣ�����ݼ�������������
		size_t szMemBlockUse;//�ڴ������ʹ�ÿ������ʹ�ü�����
	}unCount;

	size_t szMemBlockNum;//�ڴ�������ڴ�����
	size_t szMemPoolSize;//�ڴ�ش�С��size���ֽ�����

	void *pMemPool;//�ڴ����ʼ��ַ
	void *pBaseMem;//�ڴ����ַ(���ں����ͷ�)
	
protected:
	//�׳��쳣���ڴ���亯��
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

	//���׳��쳣���ڴ��ͷź���
	void NoThrowFree(void *pMem) const
	{
		Free_func fFree;
		fFree(pMem);
	}

	//����ģ����������С���߽�
	static size_t AlignedSize(size_t szSize)
	{
		return (szSize + szAlignment - 1) & ~(szAlignment - 1);
	}

	//����ģ�����ָ���ַ���߽�
	static void *AlignedMem(void *pMem)
	{
		return (void *)(((uintptr_t)pMem + szAlignment - 1) & ~(szAlignment - 1));
	}

	void Alloc(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum)
	{
		szMemBlockFixSize = _szMemBlockFixSize;
		szMemBlockNum = _szMemBlockPreAllocNum;

		//ȷ���û�����������ʱ��Ҫ����ʧ��
		if (szMemBlockFixSize < szMemBlockMinSize)//�����ڴ���С����Ϊ���޴�С
		{
			szMemBlockFixSize = szMemBlockMinSize;
		}

		if (szMemBlockNum < szMemBlockMinNum)//�����ڴ���������Ϊ���޸���
		{
			szMemBlockNum = szMemBlockMinNum;
		}

		//λͼ���ڴ�ص���ʼ��ַȫ�����뵽szAlignment
		size_t szBitMapAlignedSize = AlignedSize(szMemBlockNum * sizeof(*bArrMemBlockBitmap));//����һ�������ַ���������ֵ������һ�������ַ�Ŀ�ʼ
		size_t szStackAlignedSize = AlignedSize(szMemBlockNum * sizeof(*unFree.pArrFreeMemBlockStack));//ջ�����ַ
		szMemPoolSize = szMemBlockNum * szMemBlockFixSize;//�˴��ڴ�غ��޺�̽ṹ���������

		//һ���Է���
		size_t szBaseMemSize = szBitMapAlignedSize + szMemPoolSize + (szAlignment - 1);//�����������ڴ�
		pBaseMem = ThrowMalloc(szBaseMemSize);//�����ڴ�

		//�ָ�
		bArrMemBlockBitmap = (bool *)AlignedMem(pBaseMem);
		if constexpr (!bLessMemExpend)
		{
			unFree.pArrFreeMemBlockStack = (void **)((uintptr_t)bArrMemBlockBitmap + szBitMapAlignedSize);//bLessMemExpend���ں���Reset������
		}
		pMemPool = (void *)((uintptr_t)bArrMemBlockBitmap + szBitMapAlignedSize);
	}

public:
	using RetPoint_Type = Type;
	static constexpr size_t szManageMemBlockRequireSize = sizeof(*bArrMemBlockBitmap) + (bLessMemExpend == true ? 0 : sizeof(*unFree.pArrFreeMemBlockStack));//����һ���ڴ������Ĺ����ڴ��С
	static constexpr size_t szAlignmentSize = szAlignment;//�����ڴ�ı߽�
	static constexpr size_t szMemBlockMinSize = bLessMemExpend == true ? 1 : sizeof(FreeBlock);//�ڴ���С����
	static constexpr size_t szMemBlockMinNum = 1;//�ڴ���������

	//���캯��,��һ������Ϊ��ʼ�ڴ��Ԥ����ĳ�ʼ�ڴ�����,�ڶ�������Ϊ�����ڴ��Ĵ�С
	FixLen_MemPool(size_t _szMemBlockFixSize, size_t _szMemBlockPreAllocNum)
	{
		Alloc(_szMemBlockFixSize, _szMemBlockPreAllocNum);
		Reset();
	}

	//�ṩĬ�Ϲ��캯��
	FixLen_MemPool(void) = default;

	//���ú���
	FixLen_MemPool(const FixLen_MemPool &) = delete;//�����࿽������
	FixLen_MemPool &operator=(const FixLen_MemPool &) = delete;//���ø��Ƹ�ֵ����

	//�ƶ�����
	FixLen_MemPool(FixLen_MemPool &&_Move) noexcept ://�ƶ�����
		szMemBlockFixSize(_Move.szMemBlockFixSize),

		bArrMemBlockBitmap(_Move.bArrMemBlockBitmap),

		unFree(_Move.unFree),
		unCount(_Move.unCount),

		szMemBlockNum(_Move.szMemBlockNum),
		szMemPoolSize(_Move.szMemPoolSize),

		pMemPool(_Move.pMemPool),
		pBaseMem(_Move.pBaseMem)
	{
		//�������ָ��ΪNULL����ֹ���������ͷ�
		pBaseMem = NULL;
		//���ŵ���������Ա
		Clear();
	}

	//��������
	~FixLen_MemPool(void) noexcept
	{
		Clear();
	}

	//�������һ���ڴ��
	Type *AllocMemBlock(void) noexcept
	{
		void *pFreeMemBlock;

		if constexpr (bLessMemExpend)
		{
			if (unFree.pFreeMemBlockHead == NULL)//û�п����ڴ����
			{
				return NULL;
			}

			//����ͷ������Ϊ�����ڴ��
			pFreeMemBlock = unFree.pFreeMemBlockHead;
			//����ͷָ����һ�������ڴ��
			unFree.pFreeMemBlockHead = unFree.pFreeMemBlockHead->pNextFreeMemBlock;
			//����ʹ�ø���
			++unCount.szMemBlockUse;
		}
		else
		{
			if (unCount.szStackTop >= szMemBlockNum)//û�п����ڴ����
			{
				return NULL;
			}

			//��ջ�е���һ�������ڴ��
			pFreeMemBlock = unFree.pArrFreeMemBlockStack[unCount.szStackTop];
			//�ƶ�ջ����ջ
			++unCount.szStackTop;
		}

		//���ڴ���ַӳ�䵽λͼ
		size_t szBitmapIndex = ((uintptr_t)pFreeMemBlock - (uintptr_t)pMemPool) / szMemBlockFixSize;
		//����Ϊ����״̬
		bArrMemBlockBitmap[szBitmapIndex] = true;

		return (Type *)pFreeMemBlock;
	}

	//�����ѷ�����ڴ��
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


		if constexpr (bLessMemExpend)
		{
			//����ʹ�ø���
			--unCount.szMemBlockUse;
			//�����ͷ��ڴ��ָ������ͷ
			((FreeBlock *)pAllocMemBlock)->pNextFreeMemBlock = unFree.pFreeMemBlockHead;
			//���ͷŵ��ڴ���������ͷ
			unFree.pFreeMemBlockHead = (FreeBlock *)pAllocMemBlock;
		}
		else
		{
			//�ƶ�ջ����ջ
			--unCount.szStackTop;
			//���ͷŵ��ڴ��ѹ��ջ��
			unFree.pArrFreeMemBlockStack[unCount.szStackTop] = pAllocMemBlock;
		}

		return true;
	}

	//���첢���ض���
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

	//���������ն���
	bool FreeMemBlockDestructor(Type *pAllocMemBlock) noexcept
	{
		pAllocMemBlock->~Type();
		return FreeMemBlock(pAllocMemBlock);
	}

	//�ط����ڴ�أ�ע�⣬�ú����ᵼ���ڴ�ط�������ڴ��ȫ��ʧЧ��
	void Realloc(size_t _szMemBlockFixSize = sizeof(Type), size_t _szMemBlockPreAllocNum = 1024)
	{
		//������ԭ�ȵ��ڴ��
		Clear();

		//���·���
		Alloc(_szMemBlockFixSize, _szMemBlockPreAllocNum);

		//��ʼ���ڴ�λͼ��ջ����
		Reset();
	}

	//�����ڴ�أ�ע�⣬�ú����ᵼ���ڴ�ط�������ڴ��ȫ��ʧЧ��
	void Clear(void) noexcept
	{
		szMemBlockFixSize = 0;

		bArrMemBlockBitmap = NULL;

		memset(&unFree, 0, sizeof(unFree));
		memset(&unCount, 0, sizeof(unCount));

		szMemBlockNum = 0;
		szMemPoolSize = 0;

		pMemPool = NULL;
		NoThrowFree(pBaseMem), pBaseMem = NULL;
	}

	//������״̬Ϊ��ʼ��״̬��ע�⣬�ú����ᵼ���ڴ�ط�������ڴ��ȫ��ʧЧ��
	void Reset(void) noexcept
	{
		//����λͼΪδ����
		memset(bArrMemBlockBitmap, false, szMemBlockNum * sizeof(bool));

		if constexpr (bLessMemExpend)
		{
			//��������ͷָ���һ���ڴ��
			unFree.pFreeMemBlockHead = (FreeBlock *)pMemPool;
			//�����ڴ��ʹ�ü���Ϊ0
			unCount.szMemBlockUse = 0;

			//������ָ�봮�������ڴ��
			void *pCurrentMemBlock = unFree.pFreeMemBlockHead;
			for (size_t i = 0; i < szMemBlockNum - 1; ++i)
			{
				//������һ���ڴ��ĵ�ַ
				void *pNextMemBlock = (void *)((uintptr_t)pCurrentMemBlock + szMemBlockFixSize);
				//���õ�ǰ�ڴ��ָ����һ���ڴ��
				((FreeBlock *)pCurrentMemBlock)->pNextFreeMemBlock = (FreeBlock *)pNextMemBlock;
				//������һ���ڴ��
				pCurrentMemBlock = pNextMemBlock;
			}

			((FreeBlock *)pCurrentMemBlock)->pNextFreeMemBlock = NULL;//�������һ���ڴ��ָ��NULL
		}
		else
		{
			//����ջ��Ϊ0
			unCount.szStackTop = 0;

			//����ջ����
			void *pMemBlockAddr = pMemPool;
			for (size_t i = 0; i < szMemBlockNum; ++i)
			{
				unFree.pArrFreeMemBlockStack[i] = pMemBlockAddr;
				pMemBlockAddr = (void *)((uintptr_t)pMemBlockAddr + szMemBlockFixSize);
			}
		}
	}

	//���ڱȽ�ָ����ڴ�ع�ϵ�ĺ���,����-1����С���ڴ�ػ���ַ,����0�������ڴ����,����1���������ڴ��β��
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
		else//���ڴ����
		{
			return 0;
		}
	}

	//��ȡ�����ڴ��Ĵ�С
	size_t GetMemBlockFixSize(void) const noexcept
	{
		return szMemBlockFixSize;
	}

	//��ȡ�ڴ���ܹ��������ڴ����
	size_t GetMemBlockNum(void) const noexcept
	{
		return szMemBlockNum;
	}

	//��ȡ�ڴ���ܹ����õ��ڴ����
	size_t GetMemBlockUse(void) const noexcept
	{
		if constexpr (bLessMemExpend)
		{
			return unCount.szMemBlockUse;
		}
		else
		{
			return unCount.szStackTop;
		}
	}

	//��ȡ�ڴ�ص���ʼ��ַ(�����������ڴ��)
	const void *GetMemPool(void) const noexcept
	{
		return pMemPool;
	}

	//���ڼ����һ����������������뵽�ڶ��������������ֵ�߽�ĺ���
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