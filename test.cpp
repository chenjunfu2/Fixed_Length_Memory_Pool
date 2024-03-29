#include "Fixed_Length_Memory_Pool.hpp"
#include "Automatic_Expand_Fixed_Length_Memory_Pool.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

//#define printf
//0x7fffffff//数组上限
//0x80000000//映像上限

#define BLOCK_SIZE 8
//#define BLOCK_NUM 134217728
#define BLOCK_NUM 67108864
//#define BLOCK_NUM 32768

#define CLOCKTOSEC(start,end) ((long double)((end) - (start)) / (long double)CLOCKS_PER_SEC)


void *pArr[BLOCK_NUM];

//性能测试
int test_0(void)
{
	printf("info:\n    block size=%ldByte\n     block num=%ld\n      mem size=%.4lfMB\n", BLOCK_SIZE, BLOCK_NUM, (long double)BLOCK_SIZE * (long double)BLOCK_NUM / (long double)1024 / (long double)1024);
	clock_t start, alloc, end;

	start = clock();
	{
		FixLen_MemPool<void> a(BLOCK_SIZE, BLOCK_NUM);
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			pArr[i] = a.AllocMemBlock();
		}
		alloc = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			a.FreeMemBlock(pArr[i]);
		}
	}
	end = clock();
	clock_t my = end - start;
	printf("my:\n    alloc=%.4lfs\n     free=%.4lfs\n      all=%.4lfs\n\n", CLOCKTOSEC(start, alloc), CLOCKTOSEC(alloc, end), CLOCKTOSEC(start, end));


	start = clock();
	{
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			pArr[i] = new char[BLOCK_SIZE];
		}
		alloc = clock();
		for (int i = 0; i < BLOCK_NUM; ++i)
		{
			delete[](char *)pArr[i];
		}
	}
	end = clock();
	clock_t n = end - start;
	printf("new:\n    alloc=%.4lfs\n     free=%.4lfs\n      all=%.4lfs\n\n", CLOCKTOSEC(start, alloc), CLOCKTOSEC(alloc, end), CLOCKTOSEC(start, end));


	printf("ratio:\n");
	printf("    new/my=%.6lf\n\n", (long double)n / (long double)my);
	return 0;
}


#include <iostream>
#include <chrono>
using namespace std;

struct Ctest
{
	int a;
	int b;
	int c;
};

#define SIZE sizeof(Ctest)
#define MAX_CYCLE 134217728/256

void *arr[MAX_CYCLE];

int test_1(void)
{
	using Micro = std::chrono::duration<double, micro>;
#define Now() chrono::system_clock::now();

	auto start = Now();
	auto end = Now();

	start = Now();
	{
		FixLen_MemPool<void> mem_pool(SIZE, MAX_CYCLE);
		for (int i = 0; i < MAX_CYCLE; ++i)
			arr[i] = mem_pool.AllocMemBlock();
		for (int i = 0; i < MAX_CYCLE; ++i)
			mem_pool.FreeMemBlock(arr[i]);
	}
	end = Now();
	std::cout << Micro(end - start) << std::endl;

	start = Now();
	{
		for (int i = 0; i < MAX_CYCLE; i++)
			arr[i] = new char[SIZE];
		for (int i = 0; i < MAX_CYCLE; ++i)
			delete[](char *)arr[i];
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

	return 0;
}

#include <limits.h>

//随机分配释放测试
int test_2(void)
{
	FixLen_MemPool<void> a(10, 1024);
	std::vector<void *> v;

	//分配一部分
	for (int i = 0; i < 1024 / 2; ++i)
	{
		v.push_back(a.AllocMemBlock());
		//printf("%d alloc:0x%p\n", i, v.back());
	}
	a.Reset();//全部释放

	//随机释放+分配
	srand(2);
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
				printf("alloc:fail\t\t\t");
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
				printf("free :fail\t\t\t");
			}
		}

		if (i % 5 == 0)
		{
			printf("\n");
		}
	}

	return 0;
}


#define MY
using my = FixLen_MemPool<void, 4, false>;
using my_auto = AutoExpand_FixLen_MemPool<my>;

#ifdef MY
#define INIT(s)	my a(s, BLOCK_NUM)
#define ALLOC(s)	a.AllocMemBlock()
#define FREE(p)		a.FreeMemBlock(p)
#define UNIN()		a.~my();
/*
time:
	 init :0.1060s
	 alloc:7.2090s
	 freem:9.3240s
	 unin :0.0520s
	 all  :16.6910s
*/
#elif defined MYAUTO
#define INIT(s)	my_auto a(s, 1)//从最低的1开始分配，极限测试
#define ALLOC(s)	a.AllocMemBlock()
#define FREE(p)		a.FreeMemBlock(p)
#define UNIN()		a.~my_auto();
/*
time:
	 init :0.0000s
	 alloc:8.7560s
	 freem:14.2750s
	 unin :0.0570s
	 all  :23.0880s
*/
#elif defined NEW
#define INIT(s)	//do nothing
#define ALLOC(s)	new char[s]
#define FREE(p)		delete[](char*)p
#define UNIN()		//do nothing
/*
time:
	 init :0.0000s
	 alloc:40.1790s
	 freem:41.8710s
	 unin :0.0000s
	 all  :82.0500s
*/
#elif defined MALLOC//
#define INIT(s)	//do nothing
#define ALLOC(s)	malloc(s)
#define FREE(p)		free(p)
#define UNIN()		//do nothing
/*
time:

*/
#endif // MY

#undef CLOCKTOSEC
#define CLOCKTOSEC(val) (((long double)(val)) / (long double)CLOCKS_PER_SEC)
//#define memeset

//性能测试
int test_3(void)
{
	clock_t init = 0, alloc = 0, freem = 0, unin = 0, all = 0;
	clock_t temp = 0;

	{
		//单次计时
		init = clock();
		INIT(BLOCK_SIZE);
		init = clock() - init;

		printf("init:ok\n\n");

		//先全部请求完
		//一共执行16次
		for (int j = 0; j < 16; ++j)
		{
			printf("for:%d\n", j);
			//初始化随机种子（生成固定伪随机数序列）
			srand(j);
			printf("  srand:ok\n");

			//循环计时
			temp = clock();
			for (int i = 0; i < BLOCK_NUM; ++i)
			{
				pArr[i] = ALLOC(BLOCK_SIZE);
			}
			temp = clock() - temp;
			alloc += temp;//累计耗时

			printf("  alloc:ok\n");

			for (int i = BLOCK_NUM - 1; i >= 0; --i)
			{
				int f = rand() % (i + 1);//挑选一个随机幸运元素释放
				//挑选到的元素与末尾交换
				std::swap(pArr[f], pArr[i]);
				memset(pArr[i], 0xCD, BLOCK_SIZE);//完全覆写测试，确保安全稳定
			}

			printf("  rand:ok\n");

			//循环计时
			temp = clock();
			for (int i = 0; i < BLOCK_NUM; ++i)
			{
				//释放末尾元素
				FREE(pArr[i]);
			}
			temp = clock() - temp;
			freem += temp;//累计耗时

			printf("  free:ok\n");
		}

#ifdef MYAUTO
		putchar('\n');
		printf("use:%zu\n", a.GetMemBlockUse());//获取使用数
		printf("all:%zu\n", a.GetMemBlockNum());//获取总内存块数

		printf("pool num:%zu\n", a.GetPoolNum());//获取总内存池数
		printf("free pool:%zu\n", a.GetFreePoolNum());//获取空闲内存池数
		printf("full pool:%zu\n", a.GetFullPoolNum());//获取满内存池数

		int iCur = 0;
		auto printEvery = [&](const my &c) -> bool
		{
			printf("Cur:%d ", iCur++);
			printf("use:%zu,all:%zu\n", c.GetMemBlockUse(), c.GetMemBlockNum());

			return true;
		};
		a.TraverseEligibleMemPool(printEvery);
		putchar('\n');
#endif // MYAUTO

		//单次计时
		unin = clock();
		UNIN();
		unin = clock() - unin;

		printf("\nunin:ok\n\n");
	}

	all = init + alloc + freem + unin;

	printf(
		"time:\n"\
		"     init :%.4lfs\n"\
		"     alloc:%.4lfs\n"\
		"     freem:%.4lfs\n"\
		"     unin :%.4lfs\n"\
		"     all  :%.4lfs\n",
		CLOCKTOSEC(init),
		CLOCKTOSEC(alloc),
		CLOCKTOSEC(freem),
		CLOCKTOSEC(unin),
		CLOCKTOSEC(all)
	);

	return 0;
}

#include <algorithm>
#include <random>

#undef SIZE
#define SIZE BLOCK_SIZE//内存单元大小
#define NUM 65536*2//内存操作次数

#define MEDIUM 10240 //随机数均值
#define OFFSET 5120 //随机数偏移

#define MIN 512 //随机数最小值
#define MAX INT_MAX/2//随机数最大值

clock_t new_test(void **memstack, const long *num, std::mt19937 &gen)
{
	size_t top = 0;
	clock_t temp = 0, add = 0;

	for (long j = 0; j < NUM; ++j)
	{
		std::shuffle(memstack, memstack + top, gen);//随机打乱
		long n = num[j];

		if (n > 0)
		{
			temp = clock();//计时开始
			for (long i = 0; i < n; ++i)
			{
				memstack[top++] = new char[SIZE];
			}
			add += clock() - temp;//计时累计
		}
		else if (n < 0)
		{
			temp = clock();//计时开始
			for (long i = 0; i > n; --i)
			{
				delete[](char *)memstack[--top];
			}
			add += clock() - temp;//计时累计
		}
	}

	std::shuffle(memstack, memstack + top, gen);//随机打乱

	temp = clock();//计时开始
	while (top > 0)
	{
		delete[](char *)memstack[--top];
	}
	add += clock() - temp;//计时累计

	return add;
}

clock_t my_test(void **memstack, const long *num, std::mt19937 &gen)
{
	size_t top = 0;
	clock_t temp = 0, add = 0;

	temp = clock();//计时开始
	my a(SIZE, BLOCK_NUM);
	add += clock() - temp;//计时累计

	for (long j = 0; j < NUM; ++j)
	{
		std::shuffle(memstack, memstack + top, gen);//随机打乱
		long n = num[j];

		if (n > 0)
		{
			temp = clock();//计时开始
			for (long i = 0; i < n; ++i)
			{
				memstack[top++] = a.AllocMemBlock();
				if (memstack[top - 1] == NULL)
				{
					return 0;
				}
			}
			add += clock() - temp;//计时累计
		}
		else if (n < 0)
		{
			temp = clock();//计时开始
			for (long i = 0; i > n; --i)
			{
				a.FreeMemBlock(memstack[--top]);
			}
			add += clock() - temp;//计时累计
		}
	}

	std::shuffle(memstack, memstack + top, gen);//随机打乱

	temp = clock();//计时开始
	while (top > 0)
	{
		a.FreeMemBlock(memstack[--top]);
	}
	a.~my();
	add += clock() - temp;//计时累计

	return add;
}

clock_t myauto_test(void **memstack, const long *num, std::mt19937 &gen)
{
	size_t top = 0;
	clock_t temp = 0, add = 0;

	temp = clock();//计时开始
	my_auto a(SIZE, 1);
	add += clock() - temp;//计时累计

	for (long j = 0; j < NUM; ++j)
	{
		std::shuffle(memstack, memstack + top, gen);//随机打乱
		long n = num[j];

		if (n > 0)
		{
			temp = clock();//计时开始
			for (long i = 0; i < n; ++i)
			{
				memstack[top++] = a.AllocMemBlock();
			}
			add += clock() - temp;//计时累计
		}
		else if (n < 0)
		{
			temp = clock();//计时开始
			for (long i = 0; i > n; --i)
			{
				a.FreeMemBlock(memstack[--top]);
			}
			add += clock() - temp;//计时累计
		}
	}

	std::shuffle(memstack, memstack + top, gen);//随机打乱

	temp = clock();//计时开始
	while (top > 0)
	{
		a.FreeMemBlock(memstack[--top]);
	}
	a.~my_auto();
	add += clock() - temp;//计时累计

	return add;
}

int test_4(void)
{
	std::mt19937 gen{std::random_device{}()};
	std::normal_distribution<> d{MEDIUM,OFFSET};

	long *num = new long[NUM];
	void **memstack = new void *[MAX];

	for (size_t n = 0; n < NUM; ++n)//得出随机数列
	{
		num[n] = std::max((long)MIN, (long)std::round(d(gen)));//如果x低于下限，则令x截断至MIN
		num[n] = std::min((long)MAX, num[n]);//如果高于上限，则令x截断至MAX
	}

	for (size_t i = NUM - 1; i >= 1; i--)//计算差值
	{
		num[i] -= num[i - 1];
	}

	clock_t cnew = new_test(memstack, num, gen);
	puts("new:ok");
	clock_t cmy = my_test(memstack, num, gen);
	puts("my:ok");
	clock_t cmyauto = myauto_test(memstack, num, gen);
	puts("myauto:ok\n");
	
	printf(
		"new   :%.6lfs\n"\
		"my    :%.6lfs\n"\
		"myauto:%.6lfs\n",
		CLOCKTOSEC(cnew),
		CLOCKTOSEC(cmy),
		CLOCKTOSEC(cmyauto));

	delete[] memstack;
	delete[] num;

	return 0;
}


#define TEST_COUNT (UINT_MAX/8)//67108864
#define RANDOM_SEED 0

using safe_test = FixLen_MemPool<unsigned int, 4, true>;//bLessMemExpend: true:8711.9mb false:10759.9mb
using safe_test_autoExp = AutoExpand_FixLen_MemPool<safe_test>;

//内存池数据校验测试
int test_5(void)
{
	safe_test m1(sizeof(unsigned int), TEST_COUNT);
	//safe_test_autoExp m1(sizeof(unsigned int), 1);
	unsigned int **arr = new unsigned int*[TEST_COUNT];


	srand(RANDOM_SEED);//固定种子，固定序列
	for (long i = 0; i < TEST_COUNT; ++i)
	{
		arr[i] = m1.AllocMemBlock();
		*arr[i] = rand();
	}

	printf("fill ok!\n");

	srand(RANDOM_SEED);//重置
	for (long i = 0; i < TEST_COUNT; ++i)
	{
		unsigned int r = rand();
		if (*arr[i] != r)
		{
			printf("error: *arr[%d]=%d, rand=%d\n", i, *arr[i], r);
			break;
		}
	}

	delete[] arr;
	printf("test end.\n");

	return 0;
}