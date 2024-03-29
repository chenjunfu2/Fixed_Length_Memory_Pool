# 定长内存快速分配器  
  
## 介绍：  
**Fixed_Length_Memory_Pool**：  
一个用于分配定长内存的内存池，不可扩容  
分配耗时：固定O(1)  
释放耗时：固定O(1)  
  
**Automatic_Expand_Fixed_Length_Memory_Pool**：  
与上面相比，它多了扩容机制，是一个用于管理上面内存池的管理器  
分配耗时：最差O(log n)（保证n<64）（m次分配内O(1)，m次分配时O(log n)，m\*=扩容倍数，m起始值为用户指定，依此循环，扩容次数为n）  
释放耗时：固定O(log n)（保证n<64）（这里的n与上面相同）  
  
## 场景：  
这两个内存池都是在大量定长内存场景下优化分配和释放速度，如**链表、队列节点**，**树、图节点**，**哈希节点**等的分配与释放  
  
## 优化：  
这两个内存池都是通过减少对系统分配释放函数的调用，并使用简单的调度算法和检查代码，来减少内存分配释放的开销  
  
## 原理：  
**Fixed_Length_Memory_Pool**：  
这个内存池在初始化时按照指定的单个块大小和总块数来预分配内存并按固定大小切分，  
分配时从栈中弹出返回给用户，释放时检查并回收入栈，在其生命周期内，管理的内存块大小不变  
  
**Automatic_Expand_Fixed_Length_Memory_Pool**：  
这个自动扩容内存池依赖并管理上面的定长内存池，使其组成内存池阵列，通过用户设置的预分配项来构造第一个内存池，  
分配时通过向其管理的内存池阵列中的空闲池，请求内存返回给用户，并在池内存用尽后自动构造一个新池扩容，  
释放时通过二分查找在内存池阵列中找到对应释放指针的地址所属内存池，并交由其检查指针合法性后回收，  
在其生命周期内，管理的总内存池数会变化   
