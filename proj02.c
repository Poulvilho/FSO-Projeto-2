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
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
      printf("falha ao ler o arquivo \"%s\"\n",argv[1]);
      return 0;
    }
    while(1) {
        //printf("scan = %d\n", fscanf(fp, "%u", &number));
        if (fscanf(fp, "%u", &number) != 1) {
          break;
        }
        page = number << 16;
        page = page >> (16 + 8);
        shift = number << (16 + 8);
        shift = shift >> (16 + 8);
        //printf("page=%d\n", number);

        //i++;
        //if(i > 5) break;
        //printf("page=%d, shift=%d\n",page,shift); // para debug
    }
    fclose(fp);
    return 0;
}
