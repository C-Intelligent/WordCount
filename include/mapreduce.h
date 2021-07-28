#ifndef __mapreduce_h__
#define __mapreduce_h__
/**************************************************************
 * author:CaoZhi
 * 注：本项目为适应引入的哈希表，重定义各函数构件，但是不改变总的思想
 * 由于key已经进行了哈希映射，故抛弃实验手册中给的Partitioner，改为直接
 * 拆分map产生的哈希表的各列传入reduce处理，故此一并抛弃getter函数
 * MAX_M_R_NUM_: mapper和reducer的哈希表散列限制 (<=2^MAX_M_R_NUM_)
 * MAX_W_C_NUM_: 单词表散列限制(<=2^MAX_W_C_NUM_)
 *************************************************************/
#include "hashtable.h"
#include "mytype.h"
#define MAX_M_R_NUM_ 4
#define MAX_W_C_NUM_ 16

typedef struct file_block {
    int _fd;
    size_t off;
} fblock;

// Different function pointer types used by MR
// typedef void (*Mapper)(char *file_name);
// typedef void (*Reducer)(char *key, Getter get_func, int partition_number);

typedef char *(*Getter)(char *key, int partition_number);
// Input: fb (file block) Output: Hashtable list<key, value>
typedef void (*Mapper)(entry* fb, table* t);
typedef void (*Reducer)(lupair* _side, table** t_m_out, size_t m_num, table* final);
typedef unsigned long (*Partitioner)(char *key, int num_partitions);
typedef void* (*Partition)(table** t_m_out, int num_partitions);

// External functions: these are what you must define
void MR_Emit(char *key, char *value);
unsigned long MR_DefaultHashPartition(char *key, int num_partitions);
void* ColPartition(table** t_m_out, int num_partitions);
void* __ColPartition(table** t_m_out, int num_partitions);

typedef struct _para_for_thread_Mapper {
    Mapper mapper;
    entry* fb;
    table* t;
} __mapper;
typedef struct _para_for_thread_Reducer {
    Reducer reducer;
    lupair* _side;
    table** t_m_out;
    size_t m_num;
    table* final;
} __reducer;

__mapper* create__mapper(Mapper map, entry* fb, table* t);
__reducer* create__reducer(Reducer reduce, lupair* _side, 
table** t_m_out, size_t m_num, table* final);

typedef void* (*_mapper)(void* ptr);
typedef void* (*_reducer)(void* ptr);

void* t_mapper(void* ptr);
void* t_reducer(void* ptr);


void MR_Run(int argc, char *argv[],
    Mapper map, int num_mappers,
    Reducer reduce, int num_reducers,
    Partition par);

void test();

#endif // __mapreduce_h__