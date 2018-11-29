/* Alunos: 16/0031982  Joao Pedro Mota
           16/0035481  Marcelo Araujo
           16/0016428  Paulo Victor Lopes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

/* Global definitions. */
#define PAGE_SIZE 256 // Page size, in bytes.
#define PAGE_ENTRIES 256 // Max page table entries.
#define PAGE_NUM_BITS 8 // Page number size, in bits.
#define FRAME_SIZE 256 // Frame size, in bytes.
#define FRAME_ENTRIES 256 // Number of frames in physical memory.
#define MEM_SIZE (FRAME_SIZE * FRAME_ENTRIES) // Mem size, in bytes.
#define TLB_ENTRIES 16 // Max TLB entries.

/* Global variables. */
int virtual; // Virtual address.
unsigned int page = 0; // Page number.
unsigned int shift = 0; // shift.
int physical; // Physical address.
int frame_number; // Frame number.
int value;
int page_table[PAGE_ENTRIES]; // Page table.
int tlb[TLB_ENTRIES][2]; // Translation look-aside buffer.
int tlb_front = -1; // TLB front index, queue data structure.
int tlb_back = -1; // TLB back index, queue data structure.
char memory[MEM_SIZE]; // Physical memory. Each char is 1 byte.
int mem_index = 0; // Points to beginning of first empty frame.

/* Statistics variables. */
int fault_counter = 0; // Count the page faults.
int tlb_counter = 0; // TLB hit counter.
int address_counter = 0; // Counts addresses read from file.
float fault_rate; // Fault rate.
float tlb_rate; // TLB hit rate.

/* Functions declarations. */
void initialize_page_table(int n);
void initialize_tlb(int n);
int consult_page_table(int page);
int consult_tlb(int page);
void update_tlb(int page, int frame_number);
int get_frame();

int main(int argc, char *argv[]) {
    /* File I/O variables. */
    char* in_file; // Address file name.
    //char* out_file; // Output file name.
    char* store_file = "BACKING_STORE.bin"; // Store file name.
    char* store_data; // Store file data.
    int store_fd; // Store file descriptor.
    //char line[8]; // Temp string for holding each line in in_file.
    unsigned int number = 0;
    int oldNumber = -1;
    FILE* in_ptr; // Address file pointer.
    //FILE* out_ptr; // Output file pointer.

    /* Initialize page_table, set all elements to -1. */
    initialize_page_table(-1);
    initialize_tlb(-1);

    /* Get command line arguments. */
    if (argc != 2) {
        //printf("Enter input, output, and store file names!");
        printf("Informe tambem o arquivo de enderecos virtuais");
        exit(EXIT_FAILURE);
    }
    /* Else, proceed execution. */
    else {
        /* Get the file name from argv[]. */
        in_file = argv[1];

        /* Open the address file. */
        if ((in_ptr = fopen(in_file, "r")) == NULL) {
            /* If fopen fails, print error and exit. */
            printf("Input file could not be opened.\n");

            exit(EXIT_FAILURE);
        }

        /* Open the store file. */
        /* Map the store file to memory. */
        /* Initialize the file descriptor. */

        store_fd = open(store_file, O_RDONLY);
        store_data = mmap(0, MEM_SIZE, PROT_READ, MAP_SHARED, store_fd, 0);
        /* Check that the mmap call succeeded. */
        if (store_data == MAP_FAILED) {
            close(store_fd);
            printf("Error mmapping the backing store file!");
            exit(EXIT_FAILURE);
        }

        /* Loop through the input file one line at a time. */
        while (fscanf(in_ptr, "%u", &number) && number != oldNumber) {
            page = number << 16; /* Take off the first 16 bits */
            page = page >> (16 + 8); /* Take off the last 8 and normalize */
            shift = number << (16 + 8); /* Take off the first 16 and 8 bits */
            shift = shift >> (16 + 8); /* Normalize */
            virtual = page * 256 + shift;
            /* Increment address counter. */
            address_counter++;

            /* Use page to find frame_number in TLB, if it exists. */
            frame_number = consult_tlb(page);

            /* Check frame_number returned by consult_tlb function. */
            if (frame_number != -1) {
                /* TLB lookup succeeded. */
                /* No update to TLB required. */
                physical = frame_number + shift;

                /* No store file access required... */
                /* Fetch the value directly from memory. */
                value = memory[physical];
            }
            else {
                /* TLB lookup failed. */
                /* Look for frame_number in page table instead. */
                frame_number = consult_page_table(page);

                /* Check frame number from consult_page_table. */
                if (frame_number != -1) {
                    /* No page fault. */
                    physical = frame_number + shift;

                    /* Update TLB. */
                    update_tlb(page, frame_number);

                    /* No store file access required... */
                    /* Fetch the value directly from memory. */
                    value = memory[physical];
                }
                else {
                    /* Page fault! */
                    /* When a page fault occurs, you will read in a 256-byte
                     * page from the file BACKING_STORE.bin and store it in
                     * an available page frame in the physical memory. */

                    /* Seek to the start of the page in store_ptr file. */
                    int page_address = page * PAGE_SIZE;

                    /* Check if a free frame exists. */
                    if (mem_index != -1) {
                        /* Success, a free frame exists. */
                        /* Store the page from store file into memory frame. */
                        memcpy(memory + mem_index,
                               store_data + page_address, PAGE_SIZE);

                        /* Calculate physical address of specific byte. */
                        frame_number = mem_index;
                        physical = frame_number + shift;
                        /* Fetch value. */
                        value = memory[physical];

                        /* Update page_table with correct frame number. */
                        page_table[page] = mem_index;
                        /* Update TLB. */
                        update_tlb(page, frame_number);

                        /* Increment mem_index. */
                        if (mem_index < MEM_SIZE - FRAME_SIZE) {
                            mem_index += FRAME_SIZE;
                        }
                        else {
                            /* Set mem_index to -1, indicating memory full. */
                            mem_index = -1;
                        }
                    }
                    else {
                        /* Failed, no free frame in memory exists. */
                        /* Swap! */
                    }
                }
            }
            /* Append the results to out_file. */
            printf("Endereco Logico: %d\t", number);
            printf("Quadro: %d\t", frame_number);
            printf("Pagina Virtual: %d\t", page);
            printf("Deslocamento: %d\t", shift);
            printf("Endereco Fisico: %d\t", physical);
            printf("Valor: %c\n", value);
            oldNumber = number;
        }

        /* Calculate rates. */
        fault_rate = (float) fault_counter / (float) address_counter;
        tlb_rate = (float) tlb_counter / (float) address_counter;

        /* Print the statistics to the end of the output file. */
        printf("\nNumero de enderecos traduzidos = %d\n", address_counter);
        printf("Erros de pagina = %d\n", fault_counter);
        printf("Porcentagem de erros de pagina = %.2f%%\n", fault_rate*100);
        printf("Acertos na TLB = %d\n", tlb_counter);
        printf("Porcentagem de acertos na TLB = %.2f%%\n", tlb_rate*100);

        /* Close all three files. */
        fclose(in_ptr);
        //fclose(out_ptr);
        close(store_fd);
    }

    return EXIT_SUCCESS;
}

/* Functions. */

/* Sets all page_table elements to integer n. */
void initialize_page_table(int n) {
    for (int i = 0; i < PAGE_ENTRIES; i++) {
        page_table[i] = n;
    }
}

/* Sets all TLB elements to integer n. */
void initialize_tlb(int n) {
    for (int i = 0; i < TLB_ENTRIES; i++) {
        tlb[i][0] = -1;
        tlb[i][1] = -1;
    }
}

/* Takes a page and checks for a corresponding frame number. */
int consult_page_table(int page) {
    if (page_table[page] == -1) {
        fault_counter++;
    }

    return page_table[page];
}

/* Takes a page and checks for a corresponding frame number. */
int consult_tlb(int page) {
    /* If page is found, return the corresponding frame number. */
    for (int i = 0; i < TLB_ENTRIES; i++) {
        if (tlb[i][0] == page) {
            /* TLB hit! */
            tlb_counter++;

            return tlb[i][1];
        }
    }

    /* If page doesn't exist in TLB, return -1. */
    /* TLB miss! */
    return -1;
}

void update_tlb(int page, int frame_number) {
    /* Use FIFO policy. */
    if (tlb_front == -1) {
        /* Set front and back indices. */
        tlb_front = 0;
        tlb_back = 0;

        /* Update TLB. */
        tlb[tlb_back][0] = page;
        tlb[tlb_back][1] = frame_number;
    }
    else {
        /* Use circular array to implement queue. */
        tlb_front = (tlb_front + 1) % TLB_ENTRIES;
        tlb_back = (tlb_back + 1) % TLB_ENTRIES;

        /* Insert new TLB entry in the back. */
        tlb[tlb_back][0] = page;
        tlb[tlb_back][1] = frame_number;
    }

    return;
}
