#include "kernel/types.h"
#include "user/user.h"

#define MAXNUM 36 


void
prime(int read_p, int write_p, char nums[])
{
    read(read_p, nums, MAXNUM);
    int target = 0;
    for (int i = 0; i < MAXNUM; i++) {
        if (nums[i] == '0') {
            target = i;
            break;
        }
    }
    // no primes left
    if (target == 0) {
        exit(0);
    }
    printf("prime %d\n", target);
    nums[target] = '1';
    for (int i = target; i < MAXNUM; i++) {
        if ((i & 1) == 0) {
            nums[i] = '1';
        }
    }

    // fork
    if (fork() == 0) {
        prime(read_p, write_p, nums);
        wait(0);
    } else {
        write(write_p, nums, MAXNUM);
        wait(0);
    }
}


int
main(int argc, char *argv[])
{
    // 1 => prime
    char nums[MAXNUM];
    memset(nums,'0',sizeof(nums));
    int p[2];
    pipe(p);
    if (fork() == 0) {
        prime(p[0], p[1], nums);
        wait(0);
    } else {
        nums[0] = '1';
        nums[1] = '1';
        write(p[1], nums, MAXNUM);
        wait(0);
    }
    exit(0);
}