#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "../include/mapreduce.h"
#include "../include/hashtable.h"
#include "../include/mytype.h"

#define READ_BUF 1024
#define BUF_SIZE READ_BUF + 128

void MyMap(entry* fb, table* t) {
    int fd = ((fblock*)(fb->value))->_fd;
    size_t count = 1, off = ((fblock*)(fb->value))->off;
    char *token, *buf;

    while (off > READ_BUF) {
        buf = (char*)malloc(BUF_SIZE);
        size_t _ch_off = read(fd, buf, READ_BUF);
        while (read(fd, &buf[_ch_off], 1) && buf[_ch_off++] != ' ');
        off -= _ch_off;
        buf[_ch_off] = '\0';
        while ((token = strsep(&buf, " \t\n\r,.!;:\"\'?_--[]{}()+*/#")) != NULL) {
            if (strlen(token) > 0) {
                // printf("%lu--->%s|\n",off, token);
                addEntry(t, token, &count, TYPE_CHARARR, TYPE_LUINT);
            }
        }
        free(buf);
    }

    buf = (char*)malloc(BUF_SIZE);
    size_t _ch_off = read(fd, buf, off);
    buf[_ch_off] = '\0';
    while ((token = strsep(&buf, " \t\n\r,.!;:\"\'")) != NULL) {
        if (strlen(token) > 0) {
            // printf("--->%s|\n", token);
            addEntry(t, token, &count, TYPE_CHARARR, TYPE_LUINT);
        }
    }

    // printTable(t);
    free(buf);
}

void Map(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);
    char *line = NULL;
    size_t size = 0;
    //getline:读取到下一个空白字符时，它将停止读取
    //ssize_t getline(char **lineptr, size_t *n, FILE *stream); 
    // lineptr：指向存放该行字符的指针，如果是NULL，则有系统帮助malloc，请在使用完成后free释放。 
    // n：如果是由系统malloc的指针，请填0
    while (getline(&line, &size, fp) != -1) {
        char *token, *dummy = line;
        while ((token = strsep(&dummy, " \t\n\r")) != NULL) {
            // Each token is then emitted using the MR_Emit()
            //接受键（单词本身）和值(count) (从众多mapper中获取键值对)
            //需要设计数据结构储存它
            MR_Emit(token, "1");
        }
    }
    free(line);
    fclose(fp);
}

void Reduce(char *key, Getter get_next, int partition_number) {
    int count = 0;
    char *value;
    while ((value = get_next(key, partition_number)) != NULL)
        count++;
    printf("%s %d\n", key, count);
}

void MyReduce(lupair* _side, table** t_m_out, size_t m_num, table* final) {
    size_t i = 0, j = 0;
    entry* eptr;
    for (i = _side->_first; i < _side->_second; i++) {
        for (j = 0; j < m_num; j++) {
            eptr = t_m_out[j]->bucket[i];
            while (eptr != NULL) {
                addEntryE(final, eptr);
                eptr = eptr->next;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    MR_Run(argc, argv, MyMap, 10, MyReduce, 10, __ColPartition);
}