/*
==================================================
HEADERS
==================================================
*/
#include "mem.h"

/*
==================================================
INTERNAL FUNCTION PROTOTYPES
==================================================
*/
static long mem_read_word_impl(mem_t *self, unsigned long addr);
static void mem_dump_impl(mem_t *self, unsigned long addr, size_t bytes);
static void mem_destroy_impl(mem_t **self_ptr);

/*
==================================================
CONSTRUCTOR & DESTRUCTOR
==================================================
*/
mem_t *mem_create(pid_t pid) {
    OUTPUT_D_MSG("mem_create : Attempting to create a new memory accessor...");
    
    // ~ Create a new memory accessor
    mem_t *new = NULL;
    
    // ~ Allocate memory + error check
    new = (mem_t *)calloc(1, sizeof(mem_t));
    if (new == NULL) {
        perror("\n[ERROR]:mem_create");
        return NULL;
    }
    
    // ~ Initialize fields with default values
    new->pid = pid;
    new->last_addr = 0;
    
    // ~ Wire up function pointers
    new->read_word = mem_read_word_impl;
    new->dump = mem_dump_impl;
    new->destroy = mem_destroy_impl;
    
    OUTPUT_D_MSG("mem_create : Successfully created memory accessor!");
    return new;
}

static void mem_destroy_impl(mem_t **self_ptr) {
    mem_t *self = *self_ptr;
    
    OUTPUT_D_MSG("mem_destroy : Memory accessor being destroyed...");
    
    // ~ No owned sub-allocations to free
    
    // ~ Deallocate this entire memory block
    free(self);
    *self_ptr = NULL;
    
    OUTPUT_D_MSG("mem_destroy : Memory accessor destroyed successfully!");
}

/*
==================================================
METHOD IMPLEMENTATIONS
==================================================
*/
static long mem_read_word_impl(mem_t *self, unsigned long addr) {
    if (self == NULL) {
        perror("\n[ERROR]:mem_read_word : self is NULL");
        return -1;
    }
    OUTPUT_D_MSG("mem_read_word : Attempting to read word from tracee...");
    
    // ~ Clear errno before the ptrace call to distinguish real errors from valid -1 data
    errno = 0;
    
    // ~ Call PTRACE_PEEKDATA to read one word (8 bytes on x86_64) from tracee's memory
    // ~ This is unusual because ptrace returns data directly, not through the data parameter
    long word = ptrace(PTRACE_PEEKDATA, self->pid, addr, NULL);
    
    // ~ Check for real errors: ptrace returns -1 AND sets errno
    if (word == -1 && errno != 0) {
        perror("\n[ERROR]:mem_read_word : PTRACE_PEEKDATA failed");
        return -1;
    }
    
    // ~ Store the last address for convenience
    self->last_addr = addr;
    
    OUTPUT_D_MSG("mem_read_word : Successfully read word from tracee!");
    return word;
}

static void mem_dump_impl(mem_t *self, unsigned long addr, size_t bytes) {
    if (self == NULL) {
        perror("\n[ERROR]:mem_dump : self is NULL");
        return;
    }
    OUTPUT_D_MSG("mem_dump : Attempting to dump memory from tracee...");
    
    // ~ Display the memory range we're about to dump
    printf("\n=== Memory Dump: 0x%016lx (%zu bytes) ===\n", addr, bytes);
    
    // ~ Calculate how many words we need to read
    // ~ Each word is sizeof(long) bytes (8 bytes on x86_64)
    size_t word_size = sizeof(long);
    size_t words_to_read = (bytes + word_size - 1) / word_size;
    
    // ~ Read and display memory word by word
    for (size_t i = 0; i < words_to_read; i++) {
        unsigned long current_addr = addr + (i * sizeof(long));
        
        // ~ Read one word from the tracee
        long word = self->read_word(self, current_addr);
        if (word == -1) {
            printf("  [Failed to read at address 0x%016lx]\n", current_addr);
            break;
        }
        
        // ~ Print the address followed by the word as hex bytes
        printf("0x%016lx: ", current_addr);
        
        // ~ Extract and print each byte from the word
        unsigned char *word_bytes = (unsigned char *)&word;
        for (size_t j = 0; j < word_size && (i * word_size + j) < bytes; j++) {
            printf("%02x ", word_bytes[j]);
        }
        
        // ~ If this is the last word and we don't have a full word, pad the output
        if (i == words_to_read - 1 && bytes % word_size != 0) {
            for (size_t j = bytes % word_size; j < word_size; j++) {
                printf("   ");  // ~ Three spaces for each missing byte
            }
        }
        
        // ~ Print the ASCII representation (if printable)
        printf(" |");
        for (size_t j = 0; j < word_size && (i * word_size + j) < bytes; j++) {
            unsigned char c = word_bytes[j];
            if (c >= 32 && c <= 126) {  // ~ Printable ASCII range
                printf("%c", c);
            } else {
                printf(".");
            }
        }
        printf("|\n");
    }
    
    printf("=================================\n");
    
    // ~ Store the last address for convenience
    self->last_addr = addr + bytes;
    
    OUTPUT_D_MSG("mem_dump : Successfully dumped memory from tracee!");
}