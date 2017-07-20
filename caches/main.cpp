 /*
  * Copyright (c) 2014 Mark D. Hill and David A. Wood
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are
  * met: redistributions of source code must retain the above copyright
  * notice, this list of conditions and the following disclaimer;
  * redistributions in binary form must reproduce the above copyright
  * notice, this list of conditions and the following disclaimer in the
  * documentation and/or other materials provided with the distribution;
  * neither the name of the copyright holders nor the names of its
  * contributors may be used to endorse or promote products derived from
  * this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  * Authors: Jason Power <powerjg@cs.wisc.edu>
  * Some code contributed by Joel Hestness <jthestness@gmail.com>
  * Inspired by http://www.stuffedcow.net/research/cudabmk
  *
  */


#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <cstdlib>

#define global
#include "kernel.h"
#undef global

#define REPEAT8(S) S S S S S S S S
#define REPEAT64(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S)
#define REPEAT512(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S)
#define REPEAT1024(S) REPEAT512(S) REPEAT512(S)
#define REPEAT4096(S) REPEAT1024(S) REPEAT1024(S) REPEAT1024(S) REPEAT1024(S)
#define REPEAT8192(S) REPEAT4096(S) REPEAT4096(S)

using namespace std;

void setup_global_cpu(unsigned *gbl, unsigned num_elements, unsigned stride) {
    for (unsigned i = 0; i < num_elements; i++) {
        if (i < num_elements - stride) {
            gbl[i] = i + stride;
        } else {
            gbl[i] = i % stride;
        }
    }
}

void global_reads_cpu(int warmup, 
                           int iters,
                           unsigned array_size,
                           unsigned *array,
                           unsigned block_start_offset,
                           unsigned *final_ptr,
                           unsigned thread_stride)
{
    unsigned id = 0;
    unsigned start_index = (id + block_start_offset) % array_size;
    unsigned next = array[start_index];

    // Warmup the dcache as necessary
	// unsigned prev = next;
	// while (prev < next) {
	//     prev = next;
	// 	next = array[next];
	// }

    for (int i = 0; i < iters; i++) {
        REPEAT1024(next = array[next];)
    }

    final_ptr[id] = next;
}


int main(int argc, char const *argv[])
{
	int num_iterations = 8;
    unsigned num_elements = 256;
	unsigned max_elements = 1024;
    unsigned block_start_offset = 0;
    unsigned warp_stride = 16;
    unsigned thread_stride = 1;
    unsigned max_thread_stride = -1;
    int num_threads = -1;
    int num_blocks = -1;
    int threads_per_block = -1;
    bool nice_output = false;
    bool register_optimized = false;
    bool unified = false;
    int warmup = 3;
	int use_device = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-b")) {
            if (i < argc) {
                num_blocks = atoi(argv[++i]);
            } else {
                cout << "Need to specify number of blocks to '-b'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-d")) {
            if (i < argc) {
                use_device = atoi(argv[++i]);
            } else {
                cout << "Need to specify device num to '-d'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-e")) {
            if (i < argc) {
                max_elements = atoi(argv[++i]);
            } else {
                cout << "Need to specify number of array elements to '-e'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-i")) {
            if (i < argc) {
                num_iterations = atoi(argv[++i]);
            } else {
                cout << "Need to specify number of iterations to '-i'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-n")) {
            nice_output = true;
        } else if (!strcmp(argv[i], "-o")) {
            if (i < argc) {
                block_start_offset = atoi(argv[++i]);
            } else {
                cout << "Need to specify block offset to '-o'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-p")) {
            if (i < argc) {
                threads_per_block = atoi(argv[++i]);
            } else {
                cout << "Need to specify threads per block to '-p'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-r")) {
            register_optimized = true;
        } else if (!strcmp(argv[i], "-u")) {
            unified = true;
        } else if (!strcmp(argv[i], "-s")) {
            if (i < argc) {
                thread_stride = atoi(argv[++i]);
            } else {
                cout << "Need to specify thread stride to '-s'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-m")) {
            if (i < argc) {
                max_thread_stride = atoi(argv[++i]);
            } else {
                cout << "Need to specify max thread stride to '-m'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-t")) {
            if (i < argc) {
                num_threads = atoi(argv[++i]);
            } else {
                cout << "Need to specify number of threads to '-t'\n";
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-w")) {
            if (i < argc) {
                warp_stride = atoi(argv[++i]);
            } else {
                cout << "Need to specify warp stride to '-w'\n";
                exit(-1);
            }
        }
    }

        // Setup blocks and threads
    if (num_threads < 0) {
        if (num_blocks < 0) {
            num_blocks = 1;
            if (threads_per_block < 0) {
                threads_per_block = 1;
            }
            num_threads = num_blocks * threads_per_block;
        } else {
            if (threads_per_block < 0) {
                threads_per_block = 1;
            }
            num_threads = num_blocks * threads_per_block;
        }
    } else {
        if (num_blocks < 0) {
            if (threads_per_block < 0) {
                threads_per_block = 32;
            }
            num_blocks = num_threads / threads_per_block;
            num_threads = num_blocks * threads_per_block;
        } else {
            if (threads_per_block < 0) {
                threads_per_block = num_threads / num_blocks;
                num_threads = num_blocks * threads_per_block;
            } else {
                if (num_blocks * threads_per_block != num_threads) {
                    cout << "WARNING: Your math is wrong, fixing it up\n";
                    threads_per_block = num_threads / num_blocks;
                    num_threads = num_blocks * threads_per_block;
                }
            }
        }
    }

    if (max_thread_stride == -1) {
        max_thread_stride = thread_stride;
    }

	ofstream file;
    if (nice_output) {
		file.open("data.csv");
		file << "num_iterations" << ", "
			<< "num_threads" << ", "
			<< "num_blocks" << ", "
			<< "threads_per_block" << ", "
			<< "size" << ", "
			<< "stride" << ", "
			<< "overall_kernel_time"
			<< endl;
	}

	unsigned *final_ptr = NULL;
    unsigned *global = NULL;
	final_ptr = new unsigned[num_threads];
	global = new unsigned[max_elements];

	struct timespec begin;
	struct timespec end;

    for (num_elements = 256; num_elements <= max_elements; num_elements*=2) {
    	for (thread_stride = 1; thread_stride <= num_elements; thread_stride *= 2) {
                SNK_INIT_LPARM(lparam, 1);
	    	//setup_global(global, &num_elements, &thread_stride, lparam);
	    	setup_global_cpu(global, num_elements, thread_stride);
    		int no_iterations = num_iterations;
                SNK_INIT_LPARM(lparam1, threads_per_block * num_blocks);
                lparam1->ldims[0] = threads_per_block;
    		global_reads(&warmup, &no_iterations, &num_elements, global,
    		             &block_start_offset, final_ptr, &thread_stride, lparam1);
  			clock_gettime(CLOCK_REALTIME, &begin);
    		global_reads(&warmup, &no_iterations, &num_elements, global,
    		             &block_start_offset, final_ptr, &thread_stride, lparam1);
    		// global_reads_cpu(warmup, no_iterations, num_elements, global,
    		//              block_start_offset, final_ptr, thread_stride);
    		clock_gettime(CLOCK_REALTIME, &end);
    		double overall_kernel_time = ((double)(end.tv_sec-begin.tv_sec)) + 
        							(end.tv_nsec-begin.tv_nsec)/1e9;
    		if (!nice_output) {
				cout << "Number of blocks = " << num_blocks << endl;
				cout << "Threads per block = " << threads_per_block << endl;
				cout << "Number of threads = " << num_threads << endl;
				cout << "Stride within warp (B) = " << thread_stride * sizeof(unsigned) << endl;
				cout << "Number of iterations = " << num_iterations << endl;
				cout << "Number of array elements = " << num_elements << endl;
				cout << "Array size (B) = " << num_elements * sizeof(int*) << endl;
				cout << "Total kernel time = " << overall_kernel_time << endl;
				for (int i = 0; i < num_threads; i++) {
					cout << "  " << i
						 << " ptr = " << final_ptr[i]
						 << endl;
				}
			} else {
				file << num_iterations << ", "
					 << num_threads << ", "
					 << num_blocks << ", "
					 << threads_per_block << ", "
					 << (num_elements * sizeof(unsigned)) << ", "
					 << (thread_stride * sizeof(unsigned)) << ", "
					 << overall_kernel_time
					 << endl;
			}
  	 	}
    }
    if (nice_output) {
		file.close();
	}

	return 0;
}
