#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    unsigned int number = 0, shift = 0, page = 0;
    int i = 0; //Retirar o i para rodar todo o arquivo
    if (argc == 1) {
      printf("meu irmao, rode o programa assim:\n");
      printf("gcc proj02.c && ./a.out addresses.txt\n\n");
      return 0;
    }
    //FILE *fp = fopen(argv[1], "r"); // Retirar o coment�rio para ler o arquivo
    while(/*fscanf(fp, "%u", &number)*/i<3) { //Retirar o i para rodar todo o arquivo
        //fscanf(fp, "%u", &number);
        page = number << 16;
        page = page >> (16 + 8);
        shift = number << (16 + 8);
        shift = shift >> (16 + 8);
        i++; // Retirar o i para rodar todo o arquivo
        //printf("page=%d, shift=%d\n",page,shift); // para debug
    }
    //fclose(fp);
    return 0;
}
