
#define REPEAT8(S) S S S S S S S S
#define REPEAT64(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S) REPEAT8(S)
#define REPEAT512(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S) REPEAT64(S)
#define REPEAT1024(S) REPEAT512(S) REPEAT512(S)
#define REPEAT4096(S) REPEAT1024(S) REPEAT1024(S) REPEAT1024(S) REPEAT1024(S)
#define REPEAT8192(S) REPEAT4096(S) REPEAT4096(S)

__kernel void setup_global(global unsigned *gbl, global unsigned *ne, global unsigned *s) {
    unsigned num_elements = *ne;
    unsigned stride = *s;
    for (unsigned i = 0; i < num_elements; i++) {
        if (i < num_elements - stride) {
            gbl[i] = i + stride;
        } else {
            gbl[i] = i % stride;
        }
    }
}

/*
__kernel void global_reads_opt(int iters, unsigned array_size, __global unsigned long *array, __global unsigned long *final_ptr, unsigned thread_stride) {
    unsigned id = get_group_size(1) * get_group_id(1) + get_global_id(0);
    unsigned start_index = (thread_stride * id) % array_size;
    int *ptr = array[start_index];

    barrier(CLK_LOCAL_MEM_FENCE);

    for (int i = 0; i < iters; i++) {
        REPEAT8192(ptr = *(int**)ptr;)
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    final_ptr[id] = ptr;
}
*/

__kernel void global_reads(global int *w, 
                           global int *its,
                           global unsigned *as,
                           global unsigned *array,
                           global unsigned *bso,
                           global unsigned *final_ptr,
                           global unsigned *ts)
{
    int warmup = *w;
    int iters = *its;
    int array_size = *as;
    unsigned block_start_offset = *bso;
    unsigned thread_stride = *ts;
    unsigned id = get_local_size(1) * get_group_id(1) + get_global_id(0);
    unsigned start_index = (id + block_start_offset) % array_size;
    unsigned next = array[start_index];

    // Warmup the dcache as necessary
	// unsigned prev = next;
	// while (prev < next) {
	//     prev = next;
	// 	next = array[next];
	// }

    barrier(CLK_LOCAL_MEM_FENCE);

    for (int i = 0; i < iters; i++) {
        REPEAT1024(next = array[next];)
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    final_ptr[id] = next;
}
