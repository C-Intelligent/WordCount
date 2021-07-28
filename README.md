# Project Description
author: Cao-Intelligent
date: 2021/7/28
email: 1842306143@qq.com
github: git@github.com:C-Intelligent/WordCount.git

---
## About Project
Word count statistics based on Mapreduce model and c. CMake is used for creating project.

## Install and Run
- apt-get install cmake (if you already have it, pass, version limited: 3.5+)
- bash start.sh

## File directory
```
.
├── bin    //Executable program
│   └── WordCount
├── CMakeLists.txt
├── include
│   ├── hashtable.h
│   ├── mapreduce.h
│   ├── mthread.h
│   └── mytype.h
├── lib     //Shared Libraries
│   ├── libhash.so
│   ├── libmapreduce.so
│   ├── libmthread.so
│   └── libmytype.so
├── note.md
├── README.md
├── src
│   ├── CMakeLists.txt
│   ├── hash
│   │   ├── CMakeLists.txt
│   │   ├── hashtable.c
│   │   ├── img
│   │   │   └── 1.png
│   │   ├── main
│   │   ├── main.c_
│   │   ├── README.md
│   │   ├── src
│   │   │   └── temp.c
│   │   └── start.sh
│   ├── mapreduce
│   │   ├── CMakeLists.txt
│   │   └── mapreduce.c
│   ├── mthread
│   │   ├── CMakeLists.txt
│   │   ├── main.c_
│   │   └── mthread.c
│   ├── type
│   │   ├── CMakeLists.txt
│   │   └── mytype.c
│   └── wordcount.c  //Main Function
├── start.sh
└── WordCount_test      //Test files
    ├── file0
    ├── file1
    ├── file2
    ├── file3
    ├── file3.out   //Output file
    └── file4

11 directories, 36 files
```