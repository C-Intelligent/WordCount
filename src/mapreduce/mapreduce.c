#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include "../../include/mapreduce.h"
#include "../../include/mytype.h"
#include "../../include/mthread.h"

void MR_Emit(char *key, char *value) {

}

//实验用
//每个m_out划分为num_partitions个左闭右开区间 返回索引划分
void* __ColPartition(table** t_m_out, int num_partitions) {
    size_t j = 0, table_size = t_m_out[0]->bucketcount;
    size_t left = 0, right, __off = table_size / num_partitions;
    right = __off;

    table* t_par = createHashTable(MAX_M_R_NUM_);
    lupair** list = (lupair**)malloc(sizeof(lupair*) * num_partitions);
    for (j = 0; j < num_partitions - 1; j++) {
        lupair* lp = createLuPair(left, right);
        addPointerEntry(t_par, &j, lp, TYPE_LUINT);
        left = right;
        right += __off;
    }
    right = table_size;
    lupair* lp = createLuPair(left, right);
    addPointerEntry(t_par, &j, lp, TYPE_LUINT);
    return t_par;
}

//每个m_out划分为num_partitions个左闭右开区间 返回索引划分
void* ColPartition(table** t_m_out, int num_partitions) {
    size_t j = 0, table_size = t_m_out[0]->bucketcount;
    size_t left = 0, right, __off = table_size / num_partitions;
    right = __off;

    lupair** list = (lupair**)malloc(sizeof(lupair*) * num_partitions);
    for (j = 0; j < num_partitions - 1; j++) {
        list[j] = createLuPair(left, right);
        left = right;
        right += __off;
    }
    right = table_size;
    list[num_partitions - 1] = createLuPair(left, right);
    return list;
}


//分区函数
//从key映射到0 to num_partitions - 1的数字
//将每个map产生的结果分为reduce个部分传入reduce
unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}

__mapper* create__mapper(Mapper map, entry* fb, table* t) {
    __mapper* mp = (__mapper*)malloc(sizeof(__mapper));
    mp->mapper = map;
    mp->fb = fb;
    mp->t = t;
    return mp;
}
__reducer* create__reducer(Reducer reduce, lupair* _side, 
table** t_m_out, size_t m_num, table* final) {
    __reducer* rd = (__reducer*)malloc(sizeof(__reducer));
    rd->reducer = reduce;
    rd->_side = _side;
    rd->t_m_out = t_m_out;
    rd->m_num = m_num;
    rd->final = final;
    return rd;
}


//多线程传入函数
void* t_mapper(void* ptr) {
    __mapper* p = (__mapper*)ptr;
    p->mapper(p->fb, p->t);
}
void* t_reducer(void* ptr) {
    __reducer* p = (__reducer*)ptr;
    p->reducer(p->_side, p->t_m_out, p->m_num, p->final);
}

void MR_Run(int argc, char *argv[],
Mapper map, int num_mappers,
Reducer reduce, int num_reducers,
Partition par) {
    if (argc < 2) {
        exit(0);
    }
    int i = 1;
    while (i < argc) {
        /***************************Init***************************/
        int off = 0, *fd = (int*)malloc(num_mappers * sizeof(int));
        size_t block_size, file_size, *fptr = (size_t*)malloc(num_mappers * sizeof(size_t));
        char ch, *path = argv[i++];

        struct stat statbuf;
        stat(path,&statbuf);
        file_size = (size_t)statbuf.st_size;
        block_size = file_size / num_mappers;
        printf("File name: %s  length: %lu blocksize: %lu\n", path, file_size, block_size);

        table* t_fb = createHashTable(MAX_M_R_NUM_);
        table** t_map_out = (table**)malloc(sizeof(table*) * num_mappers);
        while (off++ < num_mappers) t_map_out[off - 1] = createHashTable(MAX_W_C_NUM_);
        table* final = createHashTable(MAX_W_C_NUM_);
        printf(">> Finish init\n");

        /*****************Open file and Sharding*******************/
        fd[0] = open(path, O_RDONLY);
        fptr[0] = 0;
        off = 0;
        size_t flag = 0;
        while (++off < num_mappers) {
            fd[off] = open(path, O_RDONLY);
            if (flag + block_size < file_size)
                flag = lseek(fd[off], fptr[off - 1] + block_size, SEEK_CUR);
            else
                lseek(fd[off], fptr[off - 1], SEEK_CUR);
            
            while (flag < file_size && read(fd[off], &ch, 1) && ch != ' '){
                flag++;
            } 
            fptr[off] = (size_t)lseek(fd[off], 0, SEEK_CUR);

            fblock* fb = (fblock*)malloc(sizeof(fblock));
            fb->_fd = fd[off - 1];
            fb->off = fptr[off] - fptr[off - 1];
            addPointerEntry(t_fb, &off, fb, TYPE_INT);
        } 
        fblock* fb = (fblock*)malloc(sizeof(fblock));
        fb->_fd = fd[off - 1];
        fb->off = file_size - fptr[off - 1];
        addPointerEntry(t_fb, &off, fb, TYPE_INT);
        printf(">> Finish Sharding\n");

        /***************************Mapping**************************/
        __mapper** _map_para = (__mapper**)malloc(sizeof(__mapper*) * num_mappers);
        for (off = 1; off <= num_mappers; off++) {
            entry* en = getEntry(t_fb, &off, TYPE_INT);
            map(en, t_map_out[off -1]);
            _map_para[off - 1] = create__mapper(map, en, t_map_out[off -1]);
        }
        // excute(t_mapper, (void**)_map_para, (size_t)num_mappers);
        for (off = 0; off < num_mappers; off++) free(_map_para[off]);
        free(_map_para);
        printf(">> Finish Mapping\n");
        /*******************Close file and Partition*****************/
        for (off = 0; off < num_mappers; off++) close(fd[off]);
        // lupair** l_par = (lupair**)par(t_map_out, num_reducers);
        // table* t_par = __ColPartition(t_map_out, num_reducers);
        table* t_par = (table*)par(t_map_out, num_reducers);
        printf(">> Finish Partition\n");

        /****************************Reduce**************************/
        size_t par_off = 0;
        __reducer** _rdu_para = (__reducer**)malloc(sizeof(__reducer*) * num_reducers);
        for (par_off = 0; par_off < num_reducers; par_off++) {
            lupair* p = (lupair*)(getEntry(t_par, &par_off, TYPE_LUINT)->value);
            reduce(p, t_map_out, (size_t)num_mappers, final);
            _rdu_para[par_off] = create__reducer(reduce, p, t_map_out, (size_t)num_mappers, final);
        }
        // excute(t_reducer, (void**)_rdu_para, (size_t)num_reducers);
        for (par_off = 0; par_off < num_reducers; par_off++) free(_rdu_para[par_off]);
        free(_rdu_para);
        printf("=========================OUTPUT=========================\n");
        printEnValues(final);
        printf(">> Finish Reduce\n");

        /************************Free Resource***********************/
        freeHashTable(t_par);
        freeHashTable(t_fb);
        freeHashTable(final);
        for (off = 0; off < num_mappers; off++) freeHashTable(t_map_out[off]);
        // for (off = 0; off < num_reducers; off++) free(l_par[off]);
        // free(l_par);
        free(t_map_out);
        free(fd);
        free(fptr);
        printf(">> END\n");

        /****************************END*****************************/
    }
}

void test() {
    printf("test\n");
}