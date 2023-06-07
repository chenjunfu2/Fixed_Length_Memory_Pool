#include "Fixed_Length_Memory_Pool.hpp"

#include <stdio.h>
#include <string.h>

//ʹ��ʾ��
int Example_Fixed_Length_Memory_Pool(void)
{
	struct Link_Node//ʾ������ڵ�
	{
		int iData;
		Link_Node *pNext;
	};

	struct Test//ʾ���ڵ�
	{
		char c;
	};

	//���춨���ڴ��

	//����ʱ����void* ע��ʹ��voidʱ�޷�Ĭ�ϴ���sizeof(void)��������Ҫ�ֶ������ڴ���С
	FixLen_MemPool<void> pPool(16);

	//����ʱ����long*
	FixLen_MemPool<long> longPool;

	//����ʱ����Link_Node*
	FixLen_MemPool<Link_Node> linkPool;

	//����ʱ����Test* ����sizeof(Test)==1���ڴ�û�ж���ᵼ�º��������ٶȱ����������ֶ�����ָ����С��ǿ�ƶ���
	//��ģ����ָ���������ʼ�����ڴ����ʼ��ַ��Ҳ���ǵ�һ��������ڴ�飩���뵽8�ֽڱ߽�
	size_t szAlignedSize = FixLen_MemPool<Test>::Aligned(sizeof(Test), 8);//ʹ���˾�̬����Aligned�����ڴ����
	FixLen_MemPool<Test, true, 8> testPool(szAlignedSize);//����ʵ�ʷ�����ڴ���8�ֽڣ�ʹ��1�ֽڣ�ʣ��7�ֽ����ڶ��룬ͬʱģ��ָ����һ���ڴ�����ʼ��ַ�Ƕ���8�ֽڱ߽��

	//ʹ�ö����ڴ��

	puts("void:");
	//void*
	void *pTemp = pPool.AllocMemBlock();//����һ���ڴ��
	size_t szpFixSize = pPool.GetMemBlockFixSize();//��ȡ����ʱָ���Ķ������С�����ֽ�Ϊ��λ
	memset(pTemp, 0xCF, szpFixSize);//ʹ���ڴ��...
	pPool.FreeMemBlock(pTemp);//�ͷ��ڴ��

	size_t szpUse = pPool.GetMemBlockUse();
	printf("use:%zu\n", szpUse);//Ԥ������Ӧ�ô�ӡ��use:0

	puts("\nlong:");
	//long*
	long *lArr[16];
	for (int i = 0; i < 16; ++i)
	{
		lArr[i] = longPool.AllocMemBlock();//����һЩ�ڴ��
	}

	size_t szlUse = longPool.GetMemBlockUse();//��ȡʹ�����
	printf("use:%zu\n", szlUse);//Ԥ������Ӧ�ô�ӡ��use:16

	puts("\nLink_Node:");
	//Link_Node*
	Link_Node *pHead = NULL;

	//���뺯��
	auto InsertHead = [&](int iData) -> void
	{
		Link_Node *pNewNode = linkPool.AllocMemBlockConstructor(Link_Node(iData, pHead));//�����ڴ沢����

		pHead = pNewNode;//��������

		printf("insert head:%d\n", iData);
	};

	//�Ƴ�����
	auto RemoveHead = [&](void) -> int
	{
		if (pHead == NULL)
		{
			return -1;
		}

		Link_Node *pRemoveNode = pHead;
		pHead = pHead->pNext;//�Ƴ�����
		int rData = pRemoveNode->iData;//����data

		linkPool.FreeMemBlockDestructor(pRemoveNode);//�ͷ��ڴ沢����

		printf("remove head:%d\n", rData);
		return rData;
	};

	//����Ĳ�����Ƴ�
	puts("Insert:");
	InsertHead(16);
	for (int i = 0; i < 9; ++i)
	{
		InsertHead(i);
	}

	puts("Remove:");
	RemoveHead();

	puts("Insert:");
	InsertHead(32);

	puts("RemoveAll:");
	while (RemoveHead() != -1)
	{
		continue;
	}

	puts("\nTest:");
	//����ʱ����Test*
	for (int i = 0; i < testPool.GetMemBlockNum(); ++i)//ȫ��������
	{
		testPool.AllocMemBlock();//��ѽ��й¶�ˣ������ֶ���������������ѷ���ָ��
	}

	const char *cp = testPool.GetMemBlockUse() == testPool.GetMemBlockNum() ? "true" : "false";
	printf("use all:%s\n", cp);//Ԥ������Ӧ�ô�ӡ��use all:true

	testPool.Reset();//ֱ�����������࣬�ص���ʼ״̬

	cp = testPool.GetMemBlockUse() == testPool.GetMemBlockNum() ? "true" : "false";
	printf("now\nuse all:%s\n", cp);//Ԥ������Ӧ�ô�ӡ��use all:false

	puts("\nsize_t:");
	//����ʱ����size_t*
	//�ƶ�����
	auto MoveFunc = [](void)
	{
		//����ʱ����size_t* �����ֶ�ָ���˳�ʼ�ڴ�����Ϊ2048������Ĭ�ϵ�1024
		return FixLen_MemPool<size_t>(sizeof(size_t), 2048);
	};

	auto sizePool = MoveFunc();
	sizePool.AllocMemBlock();//��ѽ��й¶�ˣ�й¶��ָ������������ᱻ���գ����赣��

	size_t szBlockNum = sizePool.GetMemBlockNum();
	printf("block num:%zu\n", szBlockNum);//Ԥ�ƴ�ӡ��block num:2048

	return 0;
}


#include "Automatic_Expand_Fixed_Length_Memory_Pool.hpp"

int Example_Automatic_Expand_Fixed_Length_Memory_Pool(void)
{
	//�Զ��������ʼ��������ʹ�÷�ʽ�붨������һ�£�����ģ��Ͳ���������Ա����
	using Pool = FixLen_MemPool<size_t>;
	
	//ģ���һ��������Ҫ����һ��FixLen_MemPool���ͣ���������ģ���һ������ָ�����ڴ��
	//ģ�����Pool����ÿ�δ������ڴ����ΪPool��2����ÿ�����ݶ��Ե�ǰ��С����2�����ݣ�4����ÿ�����ݺ�Ĵ�С�����뵽4�ֽڱ߽���
	AutoExpand_FixLen_MemPool<Pool, 2, 4> sizeAutoPool(sizeof(size_t), 8);//ָ������ĵ�һ���ڴ�ذ���8���ڴ��

	auto printInfo = [&](const char *p) -> void
	{
		printf("%s\n", p);
		printf("use:%zu\n", sizeAutoPool.GetMemBlockUse());//��ȡʹ����
		printf("all:%zu\n", sizeAutoPool.GetMemBlockNum());//��ȡ���ڴ����

		printf("pool num:%zu\n", sizeAutoPool.GetPoolNum());//��ȡ���ڴ����
		printf("free pool:%zu\n", sizeAutoPool.GetFreePoolNum());//��ȡ�����ڴ����
		printf("full pool:%zu\n", sizeAutoPool.GetFullPoolNum());//��ȡ���ڴ����
	};

	int iCur = 0;
	auto printEvery = [&](const Pool &c) -> bool
	{
		printf("Cur:%d ", iCur++);
		printf("use:%zu,all:%zu\n", c.GetMemBlockUse(), c.GetMemBlockNum());

		return true;
	};

	printInfo("Init");
	sizeAutoPool.TraverseEligibleMemPool(printEvery);
	iCur = 0;
	putchar('\n');

	for (int i = 0; i < 1023; ++i)
	{
		sizeAutoPool.AllocMemBlock();//��ѽ��й¶�ˣ�����ʹ��ν���ֶ��������й������������ڴ��
	}

	printInfo("Alloc");
	sizeAutoPool.TraverseEligibleMemPool(printEvery);
	iCur = 0;
	putchar('\n');

	sizeAutoPool.Capacity();//����ģ��Ԥ���ֶ�����һ��
	
	printInfo("Capacity");
	sizeAutoPool.TraverseEligibleMemPool(printEvery);
	iCur = 0;
	putchar('\n');
	
	auto ResetUse = [&](const Pool &c) -> bool
	{
		return c.GetMemBlockUse() != 0;//����
	};

	sizeAutoPool.ResetEligibleMemPool(ResetUse);
	printInfo("ResetAll");
	sizeAutoPool.TraverseEligibleMemPool(printEvery);
	iCur = 0;
	putchar('\n');

	auto clearFunc = [](const Pool &c) -> bool//�����ڴ�صĺ���
	{
		if (c.GetMemBlockUse() == 0)//ֻҪûʹ�ã�������
		{
			return true;//ɾ��
		}
		return false;
	};

	sizeAutoPool.RemoveEligibleMemPool(clearFunc);
	printInfo("RemoveAll");
	sizeAutoPool.TraverseEligibleMemPool(printEvery);
	iCur = 0;
	putchar('\n');

	//��Ϊ�����ֶ������������ڴ�أ�ʹ����ָ��س�ʼ״̬�������ֶ������һ���ڴ��
	//Ҳ���Բ����䣬��������Alloc����ʱ���Զ��½���һ��
	sizeAutoPool.AddFirstMemPool(1);
	printInfo("AddNew");
	sizeAutoPool.TraverseEligibleMemPool(printEvery);
	iCur = 0;
	putchar('\n');
	
	return 0;
}