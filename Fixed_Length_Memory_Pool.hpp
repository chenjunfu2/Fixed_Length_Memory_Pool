#pragma once
/*
��λͼ�����ѷ����ڴ������ڵ㣬�ÿ����ڴ�ջ�ڵ������ڴ�λͼ


���ݳ�Ա��
void* mem_pool��һ���ڴ�أ����չ̶���С���ֿ�
void** mem_bitmap��һ���ڴ�λͼ���洢�ѷ����ڴ�������ڵ��ַ�������ַ��nullptr��������ڴ�����
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

#include <new>
#include <algorithm>

template <typename type, bool bLazyInit = false>//����ʱ�ķ������͡������ʼ�����ԣ��˲��Ի��޸Ĵ���Σ�����ʹ��ģ��+constexpr if��
class FixLen_MemPool
{
private:
	struct//�����ڴ�������ã��ö����ƴ�����������ǰ������������ʹ�������ṹ��Ӱ����
	{
		void *pMemPool = nullptr;//ָ�� �ڴ��
		size_t szPoolSize = 0;//�ڴ�ش�С��size���ֽ�����
		size_t szMemBlockNum = 0;//�ڴ�������ڴ�����

		bool *bArrMemBlockBitmap = nullptr;//bool���� �ڴ�λͼ
		void **pArrFreeMemBlockStack = nullptr;//ָ������ ջ
		size_t szStackTop = 0;//ջ��������ջ���������ɸߵ��ͣ���Ҳ��������ʹ�õ��ڴ����Ŀ�����ڴ�����ѷ����ȥ���ڴ����
	};

	size_t szMemBlockFixSize = 0;//�û���ʼ��ʱ��Ҫ�Ķ����ڴ泤�ȣ�size���ֽ�����

public:
	FixLen_MemPool(size_t _szMemBlockFixSize = sizeof(type), size_t _szMemBlockPreAllocNum = 1024) :
		szMemBlockFixSize(_szMemBlockFixSize)
	{
		//��ʼ���ڴ�����
		szMemBlockNum = _szMemBlockPreAllocNum;

		//�ȷ����ڴ��
		szPoolSize = szMemBlockNum * szMemBlockFixSize;
		pMemPool = new char[szPoolSize];

		//�ٷ����ڴ�λͼ
		bArrMemBlockBitmap = new bool[szMemBlockNum];
		std::fill_n(bArrMemBlockBitmap, szMemBlockNum, false);//����λͼΪδ����

		//���ŷ���ջ����
		pArrFreeMemBlockStack = new void *[szMemBlockNum];
		if constexpr (bLazyInit == true)//����ջ����
		{
			//�����ʼ��
			std::fill_n(pArrFreeMemBlockStack, szMemBlockNum, nullptr);
			pArrFreeMemBlockStack[szStackTop] = pMemPool;//������ʼ��ַ��ʣ�µ�ȫ0
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

	FixLen_MemPool(const FixLen_MemPool &) = delete;//�����࿽������

	FixLen_MemPool(FixLen_MemPool &&_Move) noexcept ://�ƶ�����
		szMemBlockFixSize(_Move.szMemBlockFixSize)
	{
		_Move.szMemBlockFixSize = 0;

		//ǳ���Ƴ�Ա
		pMemPool = _Move.pMemPool;
		szPoolSize = _Move.szPoolSize;
		szMemBlockNum = _Move.szMemBlockNum;

		bArrMemBlockBitmap = _Move.bArrMemBlockBitmap;
		pArrFreeMemBlockStack = _Move.pArrFreeMemBlockStack;
		szStackTop = _Move.szStackTop;

		//�����ƶ������Ա
		_Move.pMemPool = nullptr;
		_Move.szPoolSize = 0;
		_Move.szMemBlockNum = 0;

		_Move.bArrMemBlockBitmap = nullptr;
		_Move.pArrFreeMemBlockStack = nullptr;
		_Move.szStackTop = 0;
	}

	~FixLen_MemPool(void) noexcept
	{
		delete[] pMemPool;
		pMemPool = nullptr;

		szPoolSize = 0;
		szMemBlockNum = 0;

		delete[] bArrMemBlockBitmap;
		bArrMemBlockBitmap = nullptr;

		delete[] pArrFreeMemBlockStack;
		pArrFreeMemBlockStack = nullptr;

		szStackTop = 0;

		szMemBlockFixSize = 0;
	}

	type *AllocMemBlock(void) noexcept
	{
		if (szStackTop >= szMemBlockNum)//û�п����ڴ����
		{
			return nullptr;
		}

		//��ջ�е���һ�������ڴ��
		void *pFreeMemBlock = pArrFreeMemBlockStack[szStackTop];
		//�ƶ�ջ����ջ
		++szStackTop;

		//�ж��ǲ��������ʼ����
		if constexpr (bLazyInit == true)
		{
			//���������Ҫ�����һ��ջ�����Ƿ񱻳�ʼ���������ǵĻ�Ҫ���г�ʼ��
			if (szStackTop < szMemBlockNum && pArrFreeMemBlockStack[szStackTop] == nullptr)
			{
				pArrFreeMemBlockStack[szStackTop] = (char *)pFreeMemBlock + szMemBlockFixSize;
			}
		}

		//���ڴ���ַӳ�䵽λͼ
		size_t szBitmapIndex = ((size_t)pFreeMemBlock - (size_t)pMemPool) / szMemBlockFixSize;
		//����Ϊ����״̬
		bArrMemBlockBitmap[szBitmapIndex] = true;

		return pFreeMemBlock;
	}

	bool FreeMemBlock(type *pAllocMemBlock) noexcept//�ͷŷ��ڴ�ط�����ڴ桢����ͷŻ᷵��false
	{
		if (pAllocMemBlock == nullptr)//��ָ��
		{
			return true;//ֱ�ӳɹ�
		}

		if (pAllocMemBlock < pMemPool || (size_t)pAllocMemBlock >= (size_t)pMemPool + szPoolSize ||//�����ڴ�ط�Χ
			((size_t)pAllocMemBlock - (size_t)pMemPool) % szMemBlockFixSize != 0)//���ڶ����ڴ��߽���
		{
			return false;
		}

		//����ӳ��
		size_t szBitmapIndex = ((size_t)pAllocMemBlock - (size_t)pMemPool) / szMemBlockFixSize;

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
		std::fill_n(bArrMemBlockBitmap, szMemBlockNum, false);

		//����ջ��Ϊ0
		szStackTop = 0;
		//����ջ����
		if constexpr (bLazyInit == true)
		{
			//�����ʼ��
			std::fill_n(pArrFreeMemBlockStack, szMemBlockNum, nullptr);
			pArrFreeMemBlockStack[szStackTop] = pMemPool;//������ʼ��ַ��ʣ�µ�ȫ0
		}
		else
		{
			//��ȫ��ʼ��
			char *pMemBlockAddr = (char *)pMemPool;
			for (size_t i = 0; i < szMemBlockNum; ++i)
			{
				pArrFreeMemBlockStack[i] = pMemBlockAddr;
				pMemBlockAddr += szMemBlockFixSize;
			}
		}
	}
};