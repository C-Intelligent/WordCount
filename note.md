C 库函数 long int ftell(FILE *stream) 返回给定流 stream 的当前文件位置。
fseek 函数的功能更加强大，它用来设定文件的当前读写位置，从而可以实现以任意顺序访问文件的不同位置，以实现文件的随机访问。其函数的一般原型如下所示：

int fseek(FILE *fp,long offset,int from);
如果该函数执行成功，fp 将指向以 from 为基准，偏移 offset 个字节的位置，函数的返回值为 0；如果该函数执行失败（比如 offset 超过文件自身大小），则不改变 fp 指向的位置，函数的返回值为 -1，并设置 errno 的值，可以用 perror 函数来输出错误信息。

对于 fseek 函数中的参数：第一个参数 fp 为文件指针；第二个参数 offset 为偏移量，它表示要移动的字节数，整数表示正向偏移，负数表示负向偏移；第三个参数 from 表示设定从文件的哪里开始偏移，取值范围如表 1 所示。

表 1 from参数取值表 起始点 	表不符号 	数字表示
文件首 	SEEK_SET 	0
当前位置 	SEEK_CUR 	1
文件末尾 	SEEK_END 	2

由表 1 可知：

    SEEK_SET 表示从文件起始位置增加 offset 个偏移量为新的读写位置；
    SEEK_CUR 表示从目前的读写位置增加 offset 个偏移量为新的读写位置；
    SEEK_END 表示将读写位置指向文件尾后，再增加 offset 个偏移量为新的读写位置。


C语言fseek()函数：移动文件流的读写位置
头文件：
1
	
#include <stdio.h>

定义函数：
1
	
int fseek(FILE * stream, long offset, int whence);

函数说明：
fseek()用来移动文件流的读写位置.

1、参数stream 为已打开的文件指针,


2、参数offset 为根据参数whence 来移动读写位置的位移数。参数 whence 为下列其中一种:
    SEEK_SET 从距文件开头offset 位移量为新的读写位置. SEEK_CUR 以目前的读写位置往后增加offset 个位移量.
    SEEK_END 将读写位置指向文件尾后再增加offset 个位移量. 当whence 值为SEEK_CUR 或
    SEEK_END 时, 参数offset 允许负值的出现.

下列是较特别的使用方式：
1) 欲将读写位置移动到文件开头时:fseek(FILE *stream, 0, SEEK_SET);
2) 欲将读写位置移动到文件尾时:fseek(FILE *stream, 0, 0SEEK_END);

返回值：当调用成功时则返回0, 若有错误则返回-1, errno 会存放错误代码.

附加说明：fseek()不像lseek()会返回读写位置, 因此必须使用ftell()来取得目前读写的位置.

        // int new_fd = dup(_fd);
        // while (read(new_fd, &ch, 1)) printf("%c", ch);
        
        // int stat(const char * file_name, struct stat *buf);
        // 函数说明：stat()用来将参数file_name 所指的文件状态, 复制到参数buf 所指的结构中。

        // off_t lseek(int fildes, off_t offset, int whence);
        // SEEK_SET 参数offset 即为新的读写位置.
        // 想要取得目前文件位置时:lseek(int fildes, 0, SEEK_CUR);


获取文件大小
int file_size(char* filename)
{
    FILE *fp=fopen(filename,"r");
    if(!fp) return -1;
    fseek(fp,0L,SEEK_END);
    int size=ftell(fp);
    fclose(fp);
    
    return size;
}

#include <sys/stat.h>
int file_size2(char* filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;
 
    return size;
}
————————————————
版权声明：本文为CSDN博主「yutianzuijin」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/yutianzuijin/article/details/27205121