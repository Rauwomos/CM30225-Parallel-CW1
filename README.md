# CM30225-Parallel-CW1
3rd Year Parallel Coursework 1

To compile the files run the follow commands:
  ```shell
  gcc -Wall -Werror -Wextra -Wconversion -Wpedantic -O1 -lrt -lpthread -mavx -std=gnu11 multi-avx.c -o multi

  gcc -Wall -Werror -Wextra -Wconversion -Wpedantic -O1 -lrt -mavx -std=gnu11 single-avx.c -o single
  ```

To run the programs you must pass in arguments for the values of each edge, the size of the array, the precision to work to and if needed the number of threads to use:
  ```shell
  ./single -s 1000 -u 1 -d 3 -l 4 -r 2 -p 0.00001
  ./multi -s 1000 -u 1 -d 3 -l 4 -r 2 -p 0.00001 -t 2
  ```
  ```shell
  -u give value for top edge
  -d give value for bottom edge
  -r give value for right edge
  -l give value for left edge
  -p give precision to work to
  -s give size of the array. e.g 100 gives 100x100 array
  -t give number of threads to use
  ```