#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int cd = channel_create();
    if (cd < 0) {
        printf("Failed to create channel\n");
        exit(1);
    } else {
        printf("CD: %d\n", cd);
    }
    if (fork() == 0) {
        printf("CD after fork: %d\n", cd);
        if (channel_put(cd, 42) < 0) {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        channel_put(cd, 43); // Sleeps until cleared
        channel_destroy(cd);
    } else {
        int data;
        if (channel_take(cd, &data) < 0) { // 42
            printf("Failed to take data from channel\n");
            exit(1);
        }
        int result;
        result = channel_take(cd, &data); // 43
        // printf("Returned with code %d. Took %d from cd %d\n", result, data, cd);
        result = channel_take(cd, &data); // Sleep until child destroys channel
        // printf("Returned with code %d. Took %d from cd %d\n", result, data, cd);
    }
    return 0;
}