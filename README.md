# ParallelGraph

## infrastructure
bitset
atomic<size_t>
fiber
run_multi


Libraries have been installed in:
   /home/cjh/project/LazyGraph_v0/deps/local/lib

If you ever happen to want to link against installed libraries
in a given directory, LIBDIR, you must either use libtool, and
specify the full pathname of the library, or use the `-LLIBDIR'
flag during linking and do at least one of the following:
   - add LIBDIR to the `LD_LIBRARY_PATH' environment variable
     during execution
   - add LIBDIR to the `LD_RUN_PATH' environment variable
     during linking
   - use the `-Wl,-rpath -Wl,LIBDIR' linker flag
   - have your system administrator add LIBDIR to `/etc/ld.so.conf'

See any operating system documentation about shared libraries for
more information, such as the ld(1) and ld.so(8) manual pages.

### atomic
powergraph的atomic是用
__sync_add_and_fetch
为什么double版的 atomic inc
是这样的
```cpp
//int type
T inc(const T val) { return __sync_add_and_fetch(&value, val);  }
//float type
T inc(const T val) { 
    T prev_value;
    T new_value;
    do {
    prev_value = value;
    new_value = prev_value + val;
    } while(!atomic_compare_and_swap(value, prev_value, new_value));
    return new_value; 
}
template <>
inline bool atomic_compare_and_swap(volatile float& a, 
                                    float oldval, 
                                    float newval) {
volatile uint32_t* a_ptr = reinterpret_cast<volatile uint32_t*>(&a);
const uint32_t* oldval_ptr = reinterpret_cast<const uint32_t*>(&oldval);
const uint32_t* newval_ptr = reinterpret_cast<const uint32_t*>(&newval);
return __sync_bool_compare_and_swap(a_ptr, *oldval_ptr, *newval_ptr);
};
```