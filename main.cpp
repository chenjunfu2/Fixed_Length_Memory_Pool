#include "Fixed_Length_Memory_Pool.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

//#define printf
//0x7fffffff//��������
//0x80000000//ӳ������

#define BLOCK_SIZE 4
#define BLOCK_NUM 134217728

#define CLOCKTOSEC(start,end) ((long double)((end) - (start)) / (long double)CLOCKS_PER_SEC)

void *pArr[BLOCK_NUM];

//���ܲ���
int main(void)
{
	printf("info:\n    block size=%ldB\n     block num=%ldc\n      mem size=%.4lfMB\n", BLOCK_SIZE, BLOCK_NUM, (long double)BLOCK_SIZE * (long double)BLOCK_NUM / (long double)1024 / (long double)1024);

	clock_t my;
	{
		FixLen_MemPool<void> a(BLOCK_SIZE, BLOCK_NUM);
		
		clock_t start = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			pArr[i] = a.AllocMemBlock();
		}
		clock_t alloc = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			a.FreeMemBlock(pArr[i]);
		}
		clock_t end = clock();

		printf("my:\n    alloc=%.4lfs\n     free=%.4lfs\n      all=%.4lfs\n\n", CLOCKTOSEC(start, alloc), CLOCKTOSEC(alloc, end), CLOCKTOSEC(start, end));
		my = end - start;
	}

	clock_t n;
	{
		clock_t start = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			pArr[i] = new char[BLOCK_SIZE];
		}
		clock_t alloc = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			delete[] (char *)pArr[i];
		}
		clock_t end = clock();

		printf("new:\n    alloc=%.4lfs\n     free=%.4lfs\n      all=%.4lfs\n\n", CLOCKTOSEC(start, alloc), CLOCKTOSEC(alloc, end), CLOCKTOSEC(start, end));
		n = end - start;
	}

	printf("ratio:\n    new all/my all=%.6lfr\n", (long double)n / (long double)my);

	return 0;
}

//��������ͷŲ���
int maind(void)
{
	//FixLen_MemPool<void> a(10, 1024);
	FixLen_MemPool<void, true> a(10, 1024);
	std::vector<void *> v;

	//����һ����
	for (int i = 0; i < 1024 / 2; ++i)
	{
		v.push_back(a.AllocMemBlock());
		//printf("%d alloc:0x%p\n", i, v.back());
	}
	a.FreeAllMemBlock();//ȫ���ͷ�

	//����ͷ�+����
	srand(1);
	void *pTemp;

	for (int i = 0; i < INT_MAX; ++i)
	{
		if (rand() % 2 == 0)//����
		{
			pTemp = a.AllocMemBlock();
			if (pTemp != nullptr)
			{
				v.push_back(pTemp);
				printf("alloc:0x%p\t", pTemp);
			}
			else
			{
				printf("alloc:fail\t\t");
			}
		}
		else//�ͷ�
		{
			if (v.size() == 0)
			{
				continue;
			}
			//���һ������Ԫ��
			int iTemp = rand() % v.size();

			pTemp = v[iTemp];
			std::swap(v[iTemp], v.back());//���ĩԪ�ؽ���
			v.pop_back();//������ɾ����

			if (a.FreeMemBlock(pTemp) == true)//�ͷ�
			{
				printf("free :0x%p\t", pTemp);
			}
			else
			{
				printf("free :fail\t\t");
			}
		}

		if (i % 5 == 0)
		{
			printf("\n");
		}
	}

	return 0;
}
