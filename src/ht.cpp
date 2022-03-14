#include <stdio.h>
#include <stdlib.h>
#include "rng/splitmix64.h"
#include <chrono>

//amount of times to loop for average latency
#define LATENCY_ROUNDS 64

#define DISPLAYTIME true

//windows doesn't include _V2 so use ifdef
#ifdef __unix__ 
typedef std::chrono::_V2::steady_clock chronoclock; 
#elif defined(_WIN32) || defined(WIN32)
typedef std::chrono::steady_clock chronoclock; 
#endif

typedef chronoclock::time_point time_point;

void printHeadsTails(size_t target) {
    size_t ht = 0;
#if DISPLAYTIME
    //time start and stop for flipping    
    time_point start, stop, dt;

    start = chronoclock::now();
#endif
    
    for(size_t i = 0; i < target; i++) {
        ht += splitmix64() & 1;
    }
#if DISPLAYTIME
    stop = chronoclock::now();

    // latency calculator doesnt work fully on windows so just skip can probably be redone later
#ifdef __unix__
    size_t latency = 0;
    for(int i = 0; i < LATENCY_ROUNDS; i++) {
        time_point t_0 = chronoclock::now();
        time_point t_1 = chronoclock::now();
        latency += std::chrono::duration_cast<std::chrono::nanoseconds>(t_1 - t_0).count(); 
    }
    
    latency /= LATENCY_ROUNDS;
    auto totalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() - latency;
#else
    auto totalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
#endif
#endif

    printf("Flips: %zu\n", target);
#if DISPLAYTIME
    printf("Exec Time: \n%zu nanoseconds\n%f milliseconds\n%f seconds\n\n", 
    totalTime, 
    (float)totalTime / 1e+6,
    (float)totalTime / 1e+9);
#endif
    
    printf("Tails: %zu\nHeads: %zu\n\n", target - ht, ht);

    printf("%s Wins!\n", target - ht > ht ? "Tails" : "Heads");
}

// create function for windows
char* getlinestdin(void) {
    char* line = (char*)malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL)
        return NULL;

    for (;;) {
        c = fgetc(stdin);
        if (c == EOF)
            break;

        if (--len == 0) {
            len = lenmax;
            char* linen = (char*)realloc(linep, lenmax *= 2);

            if (linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}

int main(int argc, char** kwargs) {
    srand(time(NULL)); // initialize random seed 
    splitmix64_seed((uint64_t(rand()) << 32) | rand());
    splitmix64_seed(splitmix64() * ((uint64_t(rand()) << 32) | rand()) / 4);

    //loop until user gives input not a number
    printf("Enter Number: ");
  
    for (char* line; line = getlinestdin(); ) {
        size_t target;

        if (sscanf(line, "%zu", &target) == 1) {
            printf("\n");
            printHeadsTails(target);
            printf("\nEnter Number: ");
        }
        else {
            return 0;
        }
    }

    return 0;
}
