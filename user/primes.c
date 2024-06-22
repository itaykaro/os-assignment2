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
    while(1){
    int checker_pids[checkers];
    int printer_pid = 0;
    int checkerId = 0;

    // Create channels
    int check_channel = channel_create();
    int print_channel = channel_create();
    
    for (int i = 0; i < checkers; i++) {
        checker_pids[i] = fork();
        if(checker_pids[i] == 0) {
            checkerId = i+1;
            break;
        } 
    }
    if (checkerId > 0) { 
        // Checker
        int number;
        while (1) {
            if (channel_take(check_channel, &number) < 0) {
                // check channel destroyed
                break;
            }

            if (isPrime(number)) {
                if (channel_put(print_channel, number) < 0) {
                    // print channel 
                    break;
                }
            }
        }
        exit(0);
        
    } else {
        printer_pid = fork();
        if (printer_pid == 0) {
            // Printer
            int number;
            for (int i = 0; i < 100; i++) {
                channel_take(print_channel, &number);
                printf("%d\n", number);
            }
            channel_destroy(print_channel);
            printf("Printer shutdown (PID: %d)\n", getpid());
            channel_destroy(check_channel);
            exit(0);
            
            
        } else {
            // Generator 
            int number = 2;
            while (1) {
                if (channel_put(check_channel, number) < 0) {
                    // check channel destroyed
                    while (1){
                        int channel_pid = wait(0);
                        if(channel_pid > 0)
                            for (int i = 0; i < checkers; i++) {
                                if(checker_pids[i] == channel_pid)
                                    printf("Channel %d shutdown (PID: %d)\n", i, channel_pid);
                            }
                            
                        else
                            break;
                    }
                    char input[2];
                    printf("Do you want to run the primes program again? (y/n): ");
                    read(0, input, sizeof(input));
                    if (input[0] == 'y' && input[1] == '\n') {
                        break;
                    } else {
                        printf("\nGenerator shutdown (PID: %d)\n", getpid());
                        exit(0);
                    }
                    
                }
                
                number++;
            }
        }
    }
    }
    return 0;
}

