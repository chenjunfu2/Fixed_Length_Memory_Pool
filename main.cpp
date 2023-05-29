#include "Fixed_Length_Memory_Pool.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

//#define printf
//0x7fffffff//数组上限
//0x80000000//映像上限

#define BLOCK_SIZE 12
#define BLOCK_NUM 134217728

#define CLOCKTOSEC(start,end) ((long double)((end) - (start)) / (long double)CLOCKS_PER_SEC)

void *pArr[BLOCK_NUM];

//性能测试
int main(void)
{
	printf("info:\n    block size=%ldB\n     block num=%ldc\n      mem size=%.4lfMB\n", BLOCK_SIZE, BLOCK_NUM, (long double)BLOCK_SIZE * (long double)BLOCK_NUM / (long double)1024 / (long double)1024);

	clock_t my;
	{
		clock_t start = clock();
		FixLen_MemPool<void, false> a(BLOCK_SIZE, BLOCK_NUM);
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			pArr[i] = a.AllocMemBlock();
		}
		clock_t alloc = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			a.FreeMemBlock(pArr[i]);
		}
		a.~FixLen_MemPool<void, false>();
		clock_t end = clock();

		printf("my:\n    alloc=%.4lfs\n     free=%.4lfs\n      all=%.4lfs\n\n", CLOCKTOSEC(start, alloc), CLOCKTOSEC(alloc, end), CLOCKTOSEC(start, end));
		my = end - start;
	}

	clock_t my_lazy;
	{
		clock_t start = clock();
		FixLen_MemPool<void, true> a(BLOCK_SIZE, BLOCK_NUM);
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			pArr[i] = a.AllocMemBlock();
		}
		clock_t alloc = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			a.FreeMemBlock(pArr[i]);
		}
		a.~FixLen_MemPool<void, true>();
		clock_t end = clock();

		printf("my_lazy:\n    alloc=%.4lfs\n     free=%.4lfs\n      all=%.4lfs\n\n", CLOCKTOSEC(start, alloc), CLOCKTOSEC(alloc, end), CLOCKTOSEC(start, end));
		my_lazy = end - start;
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

	clock_t m;
	{
		clock_t start = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			pArr[i] = malloc(BLOCK_SIZE);
		}
		clock_t alloc = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			free(pArr[i]);
		}
		clock_t end = clock();

		printf("malloc:\n    alloc=%.4lfs\n     free=%.4lfs\n      all=%.4lfs\n\n", CLOCKTOSEC(start, alloc), CLOCKTOSEC(alloc, end), CLOCKTOSEC(start, end));
		m = end - start;
	}

	printf("ratio:\n");
	printf("       new/my     =%.6lfr\n    malloc/my     =%.6lfr\n", (long double)n / (long double)my, (long double)m / (long double)my);
	printf("       new/my_lazy=%.6lfr\n    malloc/my_lazy=%.6lfr\n", (long double)n / (long double)my_lazy, (long double)m / (long double)my_lazy);
	printf("    malloc/new    =%.6lfr\n        my/my_lazy=%.6lfr\n", (long double)m / (long double)n, (long double)my / (long double)my_lazy);

	return 0;
}

#include <limits.h>

//随机分配释放测试
int maind(void)
{
	//FixLen_MemPool<void> a(10, 1024);
	FixLen_MemPool<void, true> a(10, 1024);
	std::vector<void *> v;

	//分配一部分
	for (int i = 0; i < 1024 / 2; ++i)
	{
		v.push_back(a.AllocMemBlock());
		//printf("%d alloc:0x%p\n", i, v.back());
	}
	a.FreeAllMemBlock();//全部释放

	//随机释放+分配
	srand(1);
	void *pTemp;

	for (int i = 0; i < INT_MAX; ++i)
	{
		if (rand() % 2 == 0)//分配
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
		else//释放
		{
			if (v.size() == 0)
			{
				continue;
			}
			//随机一个幸运元素
			int iTemp = rand() % v.size();

			pTemp = v[iTemp];
			std::swap(v[iTemp], v.back());//与队末元素交换
			v.pop_back();//弹出（删除）

			if (a.FreeMemBlock(pTemp) == true)//释放
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


#include <iostream>
#include <chrono>
#include "Fixed_Length_Memory_Pool.hpp"
using namespace std;

struct Ctest
{
	int a;
	int b;
	int c;
};

#define SIZE sizeof(Ctest)
#define MAX_CYCLE 134217728

void *arr[MAX_CYCLE];

int maine(void)
{
	using Micro = std::chrono::duration<double, micro>;
#define Now() chrono::system_clock::now();

	auto start = Now();
	auto end = Now();

	start = Now();
	{
		for (int i = 0; i < MAX_CYCLE; i++)
			arr[i] = new char[SIZE];
		for (int i = 0; i < MAX_CYCLE; ++i)
			delete[] arr[i];
	}
	end = Now();
	std::cout << Micro(end - start) << std::endl;

	start = Now();
	{
		for (int i = 0; i < MAX_CYCLE; i++)
			arr[i] = new Ctest;
		for (int i = 0; i < MAX_CYCLE; ++i)
			delete arr[i];
	}
	end = Now();
	std::cout << Micro(end - start) << std::endl;

	start = Now();
	{
		FixLen_MemPool<void, false> mem_pool(SIZE, MAX_CYCLE);
		for (int i = 0; i < MAX_CYCLE; ++i)
			arr[i] = mem_pool.AllocMemBlock();
		for (int i = 0; i < MAX_CYCLE; ++i)
			mem_pool.FreeMemBlock(arr[i]);
	}
	end = Now();
	std::cout << Micro(end - start) << std::endl;

	start = Now();
	{
		FixLen_MemPool<void, true> mem_pool(SIZE, MAX_CYCLE);
		for (int i = 0; i < MAX_CYCLE; ++i)
			arr[i] = mem_pool.AllocMemBlock();
		for (int i = 0; i < MAX_CYCLE; ++i)
			mem_pool.FreeMemBlock(arr[i]);
	}
	end = Now();
	std::cout << Micro(end - start) << std::endl;


	return 0;
}
