#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int isPrime(int number){
    for(int i = 2; i*i <= number; i++){
        if(number % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int checkers = 3;
    if(argc > 1 && sizeof(argv[1]) > 0)
        checkers = atoi(argv[1]);
    int isChecker = 0;

    // Create channels
    int check_channel = channel_create();
    int print_channel = channel_create();
    
    for (int i = 0; i < checkers; i++) {
        if(fork() == 0) {
            isChecker = 1;
            break;
        }
    }
    if (isChecker) { 
        // Checker
        int number;
        while (1) {
            if (channel_take(check_channel, &number) < 0) {
                // check channel destroyed
                exit(0);
            }

            if (isPrime(number)) {
                if (channel_put(print_channel, number) < 0) {
                    // print channel destroyed
                    exit(0);
                }
            }
        }
        exit(0);
        
    } else {
        if (fork() == 0) {
            // Printer
            int number;
            for (int i = 0; i < 100; i++) {
                channel_take(print_channel, &number);
                printf("%d\n", number);
            }
            channel_destroy(print_channel);
            channel_destroy(check_channel);
            exit(0);
            
            
        } else {
            // Generator 
            int number = 2;
            while (1) {
                if (channel_put(check_channel, number) < 0) {
                    // check channel destroyed
                    exit(0);
                }
                number++;
            }
        }
    }

    return 0;
}

