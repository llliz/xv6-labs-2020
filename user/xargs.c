#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int i, k, l;
    int j = 0;
    int m = 0;
    char *params[32];
    char block[32];
    char buf[32];
    char *p = buf;
    for (i = 1; i < argc; i++) {
        params[j++] = argv[i];
    }
    while ((k = read(0, block, sizeof(block))) > 0) {
        for (l = 0; l < k; l++) {
            if (block[l] == '\n') {
                buf[m] = 0;
                m = 0;
                params[j++] = p;
                p = buf;
                params[j] = 0;
                j = argc - 1;
                if (fork() == 0) {
                    exec(argv[1], params);
                }
                wait(0);
            } else if (block[l] == ' ') {
                buf[m++] = 0;
                params[j++] = p;
                p = &buf[m];
            } else {
                buf[m++] = block[l];
            }
        }

    }
    exit(0);
}