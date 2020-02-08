#include "header.h"
#include "compress.h"
#include "decompress.h"

int main() 
{
    printf("\n====================================\n");
    printf("\n\tHUFFMAN\n");
    printf("\n====================================\n");

    while(1)
    {
        int choice, i;
        printf("\nEscolha uma opção:\n\n1 >> Compress\n\n2 >> Descompress\n\n3 >> Exit\n\n");
    
        scanf("%d", &choice);


        if (choice == 1) 
        {
            if (compress()) printf("\nArquivo Compactado com sucesso!\n");
            else printf("Arquivo inexistente, tente novamente!\n");
        }
        else if (choice == 2) 
        {
            if (decompress()) printf("\nArquivo Descompactado com sucesso!\n");
            else printf("Arquivo inexistente, tente novamente!\n");
        }
        else if (choice == 3) return 0;

        else printf("\nOperação Inválida, tente novamente!\n");

    }

    return 0;
}
