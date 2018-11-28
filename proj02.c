#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    unsigned int number = 0; /* The number we will take of the file */
    unsigned int page   = 0; /* The number of the page, bits 8-15 */
    unsigned int shift  = 0; /* The size os the shift on the page, bits 0-7 */
    int oldNumber = -1;      /* Used to compare and stop the loop */
    
    FILE *fp = fopen(argv[1], "r"); /* Open the argv passed */
    if (fp == NULL) {
        /* See if the argv passed if valid */
        printf("Falha ao ler o arquivo '%s'\n", argv[1]); /* Print a mensage 
                                                             of error */
    } else {
        while(fscanf(fp, "%u", &number) && number != oldNumber) { /* Compare 
                        if is reading the argv and stop if the value repeat */
            page = number << 16; /* Take off the fisrt 16 bits */
            page = page >> (16 + 8); /* Take off the last 8 and normalize */
            shift = number << (16 + 8); /* Take off the fisrt 16 and 8 bits */
            shift = shift >> (16 + 8); /* Normalize */
            //printf("Page: %u\t", page); /* Print to debug */
            //printf("Shift: %u\n", shift); /* Print to debug */

            oldNumber = number; /* Att the oldNumber to compare in the next 
                                   loop */
        }
        fclose(fp); /* Close the file */
    }

    return 0;
}
