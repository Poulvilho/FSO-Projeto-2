#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    unsigned int number = 0, shift = 0, page = 0, i = 0; //Retirar o i para rodar todo o arquivo
    //FILE *fp = fopen(argv[1], "r"); // Retirar o comentário para ler o arquivo
    while(/*fscanf(fp, "%u", &number)*/i<3) { //Retirar o i para rodar todo o arquivo
        page = number << 16;
        page = page >> (16 + 8);
        shift = number << (16 + 8);
        shift = shift >> (16 + 8);
        i++; // Retirar o i para rodar todo o arquivo
    }
    return 0;
}
