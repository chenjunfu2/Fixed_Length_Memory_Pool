#include "Fixed_Length_Memory_Pool.hpp"
#include "Automatic_Expand_Fixed_Length_Memory_Pool.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

//#define printf
//0x7fffffff//��������
//0x80000000//ӳ������

#define BLOCK_SIZE 8
//#define BLOCK_NUM 134217728
#define BLOCK_NUM 67108864
//#define BLOCK_NUM 32768

#define CLOCKTOSEC(start,end) ((long double)((end) - (start)) / (long double)CLOCKS_PER_SEC)


void *pArr[BLOCK_NUM];

//���ܲ���
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

//��������ͷŲ���
int test_2(void)
{
	FixLen_MemPool<void> a(10, 1024);
	std::vector<void *> v;

	//����һ����
	for (int i = 0; i < 1024 / 2; ++i)
	{
		v.push_back(a.AllocMemBlock());
		//printf("%d alloc:0x%p\n", i, v.back());
	}
	a.Reset();//ȫ���ͷ�

	//����ͷ�+����
	srand(2);
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
				printf("alloc:fail\t\t\t");
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
using my = FixLen_MemPool<void>;
using my_auto = AutoExpand_FixLen_MemPool<my>;

#ifdef MY
#define INIT(s)	my a(s, BLOCK_NUM)
#define ALLOC(s)	a.AllocMemBlock()
#define FREE(p)		a.FreeMemBlock(p)
#define UNIN()		a.~my();
/*
time:
	 init :0.1040s
	 alloc:18.8580s
	 freem:9.4230s
	 unin :0.0380s
	 all  :28.4230s
*/
#elif defined MYAUTO
#define INIT(s)	my_auto a(s, 1)//����͵�1��ʼ���䣬���޲���
#define ALLOC(s)	a.AllocMemBlock()
#define FREE(p)		a.FreeMemBlock(p)
#define UNIN()		a.~my_auto();
/*
time:
	 init :0.0000s
	 alloc:19.2120s
	 freem:20.8840s
	 unin :0.0450s
	 all  :40.1410s
*/
#elif defined NEW
#define INIT(s)	//do nothing
#define ALLOC(s)	new char[s]
#define FREE(p)		delete[](char*)p
#define UNIN()		//do nothing
/*
time:
	 init :0.0000s
	 alloc:44.6310s
	 freem:49.4090s
	 unin :0.0000s
	 all  :94.0400s
*/
#elif defined MALLOC//
#define INIT(s)	//do nothing
#define ALLOC(s)	malloc(s)
#define FREE(p)		free(p)
#define UNIN()		//do nothing
/*
time:
	 init :0.0000s
	 alloc:41.8040s
	 freem:47.4000s
	 unin :0.0000s
	 all  :89.2040s
*/
#endif // MY

#undef CLOCKTOSEC
#define CLOCKTOSEC(val) (((long double)(val)) / (long double)CLOCKS_PER_SEC)
//#define memeset

//���ܲ���
int test_3(void)
{
	clock_t init = 0, alloc = 0, freem = 0, unin = 0, all = 0;
	clock_t temp = 0;

	{
		//���μ�ʱ
		init = clock();
		INIT(BLOCK_SIZE);
		init = clock() - init;

		printf("init:ok\n\n");

		//��ȫ��������
		//һ��ִ��16��
		for (int j = 0; j < 16; ++j)
		{
			printf("for:%d\n", j);
			//��ʼ��������ӣ����ɹ̶�α��������У�
			srand(j);
			printf("  srand:ok\n");

			//ѭ����ʱ
			temp = clock();
			for (int i = 0; i < BLOCK_NUM; ++i)
			{
				pArr[i] = ALLOC(BLOCK_SIZE);
			}
			temp = clock() - temp;
			alloc += temp;//�ۼƺ�ʱ

			printf("  alloc:ok\n");

			for (int i = BLOCK_NUM - 1; i >= 0; --i)
			{
				int f = rand() % (i + 1);//��ѡһ���������Ԫ���ͷ�
				//��ѡ����Ԫ����ĩβ����
				std::swap(pArr[f], pArr[i]);
				memset(pArr[i], 0xCD, BLOCK_SIZE);//��ȫ��д���ԣ�ȷ����ȫ�ȶ�
			}

			printf("  rand:ok\n");

			//ѭ����ʱ
			temp = clock();
			for (int i = 0; i < BLOCK_NUM; ++i)
			{
				//�ͷ�ĩβԪ��
				FREE(pArr[i]);
			}
			temp = clock() - temp;
			freem += temp;//�ۼƺ�ʱ

			printf("  free:ok\n");
		}

#ifdef MYAUTO
		putchar('\n');
		printf("use:%zu\n", a.GetMemBlockUse());//��ȡʹ����
		printf("all:%zu\n", a.GetMemBlockNum());//��ȡ���ڴ����

		printf("pool num:%zu\n", a.GetPoolNum());//��ȡ���ڴ����
		printf("free pool:%zu\n", a.GetFreePoolNum());//��ȡ�����ڴ����
		printf("full pool:%zu\n", a.GetFullPoolNum());//��ȡ���ڴ����

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

		//���μ�ʱ
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
#define SIZE BLOCK_SIZE//�ڴ浥Ԫ��С
#define NUM 65536*2//�ڴ��������

#define MEDIUM 10240 //�������ֵ
#define OFFSET 5120 //�����ƫ��

#define MIN 512 //�������Сֵ
#define MAX INT_MAX/2//��������ֵ

clock_t new_test(void **memstack, const long *num, std::mt19937 &gen)
{
	size_t top = 0;
	clock_t temp = 0, add = 0;

	for (long j = 0; j < NUM; ++j)
	{
		std::shuffle(memstack, memstack + top, gen);//�������
		long n = num[j];

		if (n > 0)
		{
			temp = clock();//��ʱ��ʼ
			for (long i = 0; i < n; ++i)
			{
				memstack[top++] = new char[SIZE];
			}
			add += clock() - temp;//��ʱ�ۼ�
		}
		else if (n < 0)
		{
			temp = clock();//��ʱ��ʼ
			for (long i = 0; i > n; --i)
			{
				delete[](char *)memstack[--top];
			}
			add += clock() - temp;//��ʱ�ۼ�
		}
	}

	std::shuffle(memstack, memstack + top, gen);//�������

	temp = clock();//��ʱ��ʼ
	while (top > 0)
	{
		delete[](char *)memstack[--top];
	}
	add += clock() - temp;//��ʱ�ۼ�

	return add;
}

clock_t my_test(void **memstack, const long *num, std::mt19937 &gen)
{
	size_t top = 0;
	clock_t temp = 0, add = 0;

	temp = clock();//��ʱ��ʼ
	my a(SIZE, BLOCK_NUM);
	add += clock() - temp;//��ʱ�ۼ�

	for (long j = 0; j < NUM; ++j)
	{
		std::shuffle(memstack, memstack + top, gen);//�������
		long n = num[j];

		if (n > 0)
		{
			temp = clock();//��ʱ��ʼ
			for (long i = 0; i < n; ++i)
			{
				memstack[top++] = a.AllocMemBlock();
				if (memstack[top - 1] == NULL)
				{
					return 0;
				}
			}
			add += clock() - temp;//��ʱ�ۼ�
		}
		else if (n < 0)
		{
			temp = clock();//��ʱ��ʼ
			for (long i = 0; i > n; --i)
			{
				a.FreeMemBlock(memstack[--top]);
			}
			add += clock() - temp;//��ʱ�ۼ�
		}
	}

	std::shuffle(memstack, memstack + top, gen);//�������

	temp = clock();//��ʱ��ʼ
	while (top > 0)
	{
		a.FreeMemBlock(memstack[--top]);
	}
	a.~my();
	add += clock() - temp;//��ʱ�ۼ�

	return add;
}

clock_t myauto_test(void **memstack, const long *num, std::mt19937 &gen)
{
	size_t top = 0;
	clock_t temp = 0, add = 0;

	temp = clock();//��ʱ��ʼ
	my_auto a(SIZE, 1);
	add += clock() - temp;//��ʱ�ۼ�

	for (long j = 0; j < NUM; ++j)
	{
		std::shuffle(memstack, memstack + top, gen);//�������
		long n = num[j];

		if (n > 0)
		{
			temp = clock();//��ʱ��ʼ
			for (long i = 0; i < n; ++i)
			{
				memstack[top++] = a.AllocMemBlock();
			}
			add += clock() - temp;//��ʱ�ۼ�
		}
		else if (n < 0)
		{
			temp = clock();//��ʱ��ʼ
			for (long i = 0; i > n; --i)
			{
				a.FreeMemBlock(memstack[--top]);
			}
			add += clock() - temp;//��ʱ�ۼ�
		}
	}

	std::shuffle(memstack, memstack + top, gen);//�������

	temp = clock();//��ʱ��ʼ
	while (top > 0)
	{
		a.FreeMemBlock(memstack[--top]);
	}
	a.~my_auto();
	add += clock() - temp;//��ʱ�ۼ�

	return add;
}

int test_4(void)
{
	std::mt19937 gen{std::random_device{}()};
	std::normal_distribution<> d{MEDIUM,OFFSET};

	long *num = new long[NUM];
	void **memstack = new void *[MAX];

	for (size_t n = 0; n < NUM; ++n)//�ó��������
	{
		num[n] = std::max((long)MIN, (long)std::round(d(gen)));//���x�������ޣ�����x�ض���MIN
		num[n] = std::min((long)MAX, num[n]);//����������ޣ�����x�ض���MAX
	}

	for (size_t i = NUM - 1; i >= 1; i--)//�����ֵ
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