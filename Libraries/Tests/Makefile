FLAGS= -g -O0 -DVUL_LINUX -I../ -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=address

all: test image

test_astar:
	gcc $(FLAGS) test_astar.c -o test_astar -lm

test_blas:
	gcc $(FLAGS) test_blas.c -o test_blas -lm

test_cl:
	gcc $(FLAGS) test_cl.c -o test_cl -lm -lOpenCL

test_csp:
	gcc $(FLAGS) test_csp.c -o test_csp -lm

test_file:
	gcc $(FLAGS) test_file.c -o test_file -lm

test_gl:
	gcc $(FLAGS) test_gl.c -o test_gl -lm -lGL

test_noise:
	gcc $(FLAGS) test_noise.c -o test_noise -lm

test_prioheap:
	gcc $(FLAGS) test_priority_queues.c -o test_priority_queues -lm


tests: test_astar test_blas test_cl test_csp test_file test_gl test_noise test_prioheap

run: tests
	./test_astar
	./test_blas
#	./test_cl TODO: Fix the CL-issues we're having on this machine, and actually test this
	./test_csp
	./test_file
	./test_gl
	./test_noise
	./test_priority_queues

image:
	clang -I ../../../../Libs/stb_git -I ../ -DVUL_LINUX -O2 -g -o svd_image svd_image_example.c -lm

clean:
	rm test_astar
	rm test_blas
	rm test_cl
	rm test_csp
	rm test_file
	rm test_gl
	rm test_noise
	rm test_prioheap
	rm image
