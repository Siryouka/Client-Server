#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include "../reader/text_reader.h"

#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class MyShm{

	public:
		// read a str
		void readStr(std::string &overwrite_str);
		// write a str (in entirety)
		void writeStr(const std::string &str);

		// read an int from shared int
		void readInt(int &overwrite_int);

		// write an int from shared int
		void writeInt(const int &n);

		int fd_;
		// path of shared mem
		// typically in /dev/shm
		char* shmpath_;

		struct shmbuf * shmp_;
};



// This is a function used accross all classes to help catch errors.
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
						} while (0)

struct shmbuf {
	sem_t  sem1_;            /* POSIX unnamed semaphore */
	sem_t  sem2_;            /* POSIX unnamed semaphore */

	// shared int for writing
	int shared_int_;

	// number of bytes for msg sizes
	size_t cnt_;   
	char buf_[4096];   

    const int BUF_SIZE_ {4096};

};


#endif