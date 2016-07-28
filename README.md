# Villain's Utility Library
TODO: Write this stuff.
TODO: Mention dual license (public domain or MIT)

## Libraries
The various single-file libraries. The following table describes them, and how "fit for production" they are, as well as which other single-file libraries they rely on. Most support custom allocators if relevant.

For more complete descriptions and instructions, see the comments at the top of each file.

| File | Description | FFP\* | Dependencies | Notes |
|------|-------------|-------|--------------|-------|
| vul_astar.h | Generic A\* implementation                                                          | &#9734; | vul_priority_heap, vul_queue, vul_stack, vul_stable_array, vul_types |
| vul_audio.h | OS-agnostic audio wrapper around PulseAudio, ALSA, OSS, DSound and CoreAudio        | &#9888;   |   | WIP |
| vul_benchmark.h  | Simple function benchmarking                                                   | &#9734; | vul_types, vul_timer, vul_sort |   |
| vul_cl.h | OpenCL wrappers and helper functions                                                   | &#9872; | vul_stable_array |   |
| vul_cmath.h | C vector and matrix math library                                                    | &#9734; |  | Needs tests, but used a lot |
| vul_csp.h | Constraint satisfaction problem solver using GAC                                      | &#9734; | vul_astar |  |
| vul_distributions.h | Contains a halton series generator                                          | &#9872; | vul_types | Intended to contain more distributions as the need arises |
| vul_file.h | System-agnosting mmap and file change monitoring + some stb.h file-related functions | &#9872; | vul_string, vul_types | Has seen some use, but has no tests |
| vul_gl.h | Some minor GL helper functions (error printing and glu-matrix-likes)                   | &#9734; | vul_types | |
| vul_hash_map.h | Generic C hash-map. Buckets are linked lists (which is bad)                      | &#9872; | vul_types, vul_linked_list | Has seen plenty of use and is stable, but slow (and usage can be annoying. Rewrite coming at some point!) |
| vul_linear_solvers.h | Linear system solvers for dense, square matrices                           | &#9872; | | QR decomposition is broken. Non-square and sparse are useful, and on the TODO list. |
| vul_linked_list.h | Non-intrusive linked list                                                     | &#9734; | vul_types | |
| vul_noise.h | Various noise functions                                                             | &#9872; | | Currently generates gaussian and worley noise only |
| vul_priority_heap.h | Generic fibonacci heap                                                      | &#9734; | vul_types | Needs tests |
| vul_queue.h | Generic queue (linked list of fixed-size arrays)                                    | &#9734; | vul_types, vul_linked_list | |
| vul_raycast.h | Triangle-soup ray-caster using SSE                                                | &#9888; |  | WIP (BVH version is incomplete, both untested) |
| vul_resizable_array.h | Stretchy buffer. Re-allocates on resize                                   | &#9734; | vul_types  |  |
| vul_rngs.h | A number of PRNG implementations                                                     | &#9734; | vul_types | The PCG32 function implementation is Apache 2.0 licenced. See comment in source |
| vul_shapes.h | Arbitraty precision 3D shape construction functions                                | &#9888; | vul_types | WIP, only supports spheres (by tetrahedron subdivision) |
| vul_skip_list.h | Skip-list implementation. Not indexable                                         | &#9888; | vul_types | WIP/Broken |
| vul_socket.h | OS-agnostic TCP sockets                                                            | &#9872; |  | Needs more testing on windows and OS X, but has been used for game jams |
| vul_sort.h | Sorting for vul_resizable_array. Insertion-, shell-, quick- and a variation of Timsort (using shellsort for runs) | &#9734; | vul_resizable_array | |
| vul_stable_array.h | Generic resizable array that does not reallocate on resize                   | &#9734; | vul_types | |
| vul_stack.h | Generic stack                                                                       | &#9734; | vul_stable_array | Pointers to elements are stable |
| vul_string.h | Some useful string functions (UTF8 & search/pattern matching (KMP))                | &#9872; | | UTF-8 functions are due stb.h |
| vul_timer.h | OS-agnostic timer & sleep function                                                  | &#9734; | vul_types |  |
| vul_types.h | Defines standard types in my favourable form                                        | &#9734; | | Useful if you like the form, horrible otherwise. This is an anti-pattern I should get rid of in the above (and is on the TODO-list) |

\* FFP (Fit-for-production) is a 3-tier system: 

- &#9734; means this either has extensive tests or has been used to a degree that it should be low on bugs.
- &#9872; means it either has tests but has not been used, or has been used some, but not extensively, and should
probably be extensively tested before it is used in production.
- &#9888; indicates it has neither been tested nor used, and is probably a WIP.

## Math
A C++ math library. Here, all files are expected to be present for use. 

TODO: More complete description
NOTE: vul_linear.hpp is broken. The library shares the problems and limitations of 
vul_linear_solvers.h, and it shows worse stability for the solvers that *do* work than the C equivalent.
Consider it a WIP, and avoid for now.

## Collission
Contains a file filled with a variation of primitive collission checks (direct, n^2 tests). Not intended for use 
as a library, but useful as a copy-paste resource for when you don't need a full CSO sollution.
