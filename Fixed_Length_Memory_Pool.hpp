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

#include <malloc.h>
#include <string.h>
#include <algorithm>

#define SENTINEL_POINTER ((void *)(NULL + 1))//�ڱ�ָ��

template <typename type, bool bLazyInit = false>//����ʱ�ķ������͡������ʼ�����ԣ��˲��Ի��޸Ĵ���Σ�����ʹ��ģ�� & constexpr if��
class FixLen_MemPool
{
private:
	size_t szMemBlockFixSize = 0;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����

	void *pMemPool = NULL;//ָ�� �ڴ��
	size_t szPoolSize = 0;//�ڴ�ش�С��size���ֽ�����
	size_t szMemBlockNum = 0;//�ڴ�������ڴ�����

	bool *bArrMemBlockBitmap = NULL;//bool���� �ڴ�λͼ
	void **pArrFreeMemBlockStack = NULL;//ָ������ ջ
	size_t szStackTop = 0;//ջ��������ջ���������ɸߵ��ͣ���Ҳ��������ʹ�õ��ڴ����Ŀ�����ڴ�����ѷ����ȥ���ڴ����

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
		//��ʼ���ڴ�����
		szMemBlockNum = _szMemBlockPreAllocNum;
		//�����ڴ�ش�С
		szPoolSize = szMemBlockNum * szMemBlockFixSize;

		//�ȷ����ڴ��
		pMemPool = ThrowMalloc<char>(szPoolSize);//�ڴ�������ͣ�ֱ�Ӱ�char����
		//�ٷ����ڴ�λͼ
		bArrMemBlockBitmap = ThrowMalloc<bool>(szMemBlockNum);
		//���ŷ���ջ����
		if constexpr (bLazyInit == true)
		{
			pArrFreeMemBlockStack = ThrowMalloc<void *>(szMemBlockNum + 1);//�����ʼ��ʱ������һ���������ڱ����
		}
		else
		{
			pArrFreeMemBlockStack = ThrowMalloc<void *>(szMemBlockNum);//��ȫ��ʼ��ʱֻ����������С
		}
	
		//��ʼ���ڴ�λͼ��ջ����
		FreeAllMemBlock();
	}

	FixLen_MemPool(const FixLen_MemPool &) = delete;//�����࿽������

	FixLen_MemPool(FixLen_MemPool &&_Move) noexcept ://�ƶ�����
		szMemBlockFixSize(_Move.szMemBlockFixSize),

		pMemPool(_Move.pMemPool),
		szPoolSize(_Move.szPoolSize),
		szMemBlockNum(_Move.szMemBlockNum),

		bArrMemBlockBitmap(_Move.bArrMemBlockBitmap),
		pArrFreeMemBlockStack(_Move.pArrFreeMemBlockStack),
		szStackTop(_Move.szStackTop)
	{
		//�����ƶ������Ա
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
		if (szStackTop >= szMemBlockNum)//û�п����ڴ����
		{
			return NULL;
		}

		//��ջ�е���һ�������ڴ��
		void *pFreeMemBlock = pArrFreeMemBlockStack[szStackTop];
		//�ƶ�ջ����ջ
		++szStackTop;

		//�ж��ǲ��������ʼ����
		if constexpr (bLazyInit == true)
		{
			//���������Ҫ�����һ��ջ�����Ƿ񱻳�ʼ���������ǵĻ�Ҫ���г�ʼ��
			if (pArrFreeMemBlockStack[szStackTop] == NULL)//ʹ���ˣ���ΪNULL�����ڱ�ֵ���˴������ټ��szStackTop < szMemBlockNum
			{
				pArrFreeMemBlockStack[szStackTop] = (char *)pFreeMemBlock + szMemBlockFixSize;
			}
		}

		//���ڴ���ַӳ�䵽λͼ
		size_t szBitmapIndex = ((char *)pFreeMemBlock - (char *)pMemPool) / szMemBlockFixSize;
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
			((char *)pAllocMemBlock - (char *)pMemPool) % szMemBlockFixSize != 0)//���ڶ����ڴ��߽���
		{
			return false;
		}

		//����ӳ��
		size_t szBitmapIndex = ((char *)pAllocMemBlock - (char *)pMemPool) / szMemBlockFixSize;

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
		pArrFreeMemBlockStack[szStackTop] = pAllocMemBlock;

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
			pArrFreeMemBlockStack[szStackTop] = pMemPool;//������ʼ��ַ��ʣ�µ�ȫ0
			memset(&pArrFreeMemBlockStack[szStackTop + 1], NULL, szMemBlockNum * sizeof(void *));//���м䶼���ó�NULL
			pArrFreeMemBlockStack[szMemBlockNum] = SENTINEL_POINTER;//����β��Ϊһ����NULL�ķǷ���ַ��ʵ���ϲ����ᱻ�����ȥ�����������ڱ���ǣ�
			//ע�⣺���������pArrFreeMemBlockStack[szMemBlockNum]�����ᳬβ����
		}
		else
		{
			//��ȫ��ʼ��
			void *pMemBlockAddr = pMemPool;
			for (size_t i = 0; i < szMemBlockNum; ++i)
			{
				pArrFreeMemBlockStack[i] = pMemBlockAddr;
				pMemBlockAddr = (char *)pMemBlockAddr + szMemBlockFixSize;
			}
		}
	}

	int CmpPointAndPool(const void *p) const//����-1����С���ڴ�ػ���ַ������0�������ڴ���У�����1���������ڴ��β��
	{
		if (p < pMemPool)
		{
			return -1;
		}
		else if (p >= (char *)pMemPool + szPoolSize)
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