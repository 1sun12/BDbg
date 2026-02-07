#ifndef DEBUG_H
#define DEBUG_H

// ~ Exit with an error message and return EXIT_FAILURE
#define exit_with_error(msg) do {perror(msg); return(EXIT_FAILURE);} while(0)

// ~ Null-check on self at the start of methods (void return functions only)
#define does_exist(obj) do {if(obj == NULL){perror("\n[ERROR]:does_exist"); return;}} while(0)

// ~ Toggle debug output (set to 1 for verbose, 0 for quiet)
#define OUTPUT_DEBUG 0

// ~ Print a debug message if OUTPUT_DEBUG is enabled
#define OUTPUT_D_MSG(msg) do {if(OUTPUT_DEBUG) {printf("\n[INFO]:%s\n", msg);}} while(0)

#endif