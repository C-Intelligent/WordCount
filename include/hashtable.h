#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

/*********************************************************
 * 注意一些区别：
 * 当传入指针为结构体类型时，构造函数不另外申请空间，直接指向传入
 * 的地址，因此传入的结构体必须要使用malloc申请空间且不得在外部释放
 * 而应该在析构函数调用的时候统一进行释放。
 * key只支持数字和字符串
 * value支持所数字和字符串以及指针
 * 警告：
 * value中若包含地址，则需要显式地释放空间，哈希表的析构函数并不会
 * 释放此间接指向的空间
 * value为指针的情况有很大隐患，谨慎使用
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TYPE_CHARARR 0
#define TYPE_INT 1
#define TYPE_LUINT 2
#define TYPE_POINTER 3

typedef void* strcut_ptr;

struct hashEntry {
    void* key;
    void* value;
    int key_type;
    int value_type;
    struct hashEntry* next;
};
typedef struct hashEntry entry;

struct hashTable {
    size_t bucketcount;
    entry** bucket;
};

typedef struct hashTable table;

/**
 * 创建哈希表
 * size 为 0-31
 */
table* createHashTable(size_t size);

/**
 * 析构哈希表
 */
void freeHashTable(table* t);

/**
 * 哈希散列函数
 * 这里采用乘常数再相加 最后取模
 */
size_t hashFunction(table* t, const void* key);

/**
 * 增加表项，若不存在，则新建项目，否则在原项目基础上自增
 * （类型必须相同，否则删除原来的）若要传入指针类型的数据，则使用
 * addPointerEntry
 */
int addEntry(table* t, const void* key , const void* value, int key_type, int value_type);
int addEntryE(table* t, const entry* eptr);
int addPointerEntry(table* t, const void* key , void* value, int key_type);

/**
 * key匹配
 */
int confirmKey(const void* k1 , const void* k2, int t1, int t2);

/**
 * 更新value
 */
int addValue(void* v1 , const void* v2, int t1, int t2);

/**
 * 创建条目
 */
entry* createEntry(const void* key , const void* value, int key_type, int value_type);
entry* createPointerEntry(const void* key , void* value, int key_type);

/**
 * 查询条目 返回其地址
 */
entry* getEntry(table* t, const void* key, int key_type);

void printTable(table* t);
void printEntry(entry* eptr);
void printEntryValue(entry* eptr);
void printEntrys(table* t);
void printEnValues(table* t);

#endif