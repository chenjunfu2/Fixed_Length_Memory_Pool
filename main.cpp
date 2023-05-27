#include "Fixed_Length_Memory_Pool.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define printf
#define putchar

int main(void)
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
				printf("alloc:fail\t");
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
				printf("free :fail\t");
			}
		}

		if (i % 5 == 0)
		{
			putchar('\n');
		}
	}

	return 0;
}