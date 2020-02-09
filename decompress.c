#include "decompress.h"

NODE* construct_tree(uchar pre_order_tree[], int *i) 
{
   if (pre_order_tree[*i] != '*') 
   { 
        if (pre_order_tree[*i] == '\\') *i += 1;
        
        NODE* leaf = create_node(0, pre_order_tree[*i], NULL, NULL);
        
        return leaf;
   }
   else 
   {
        NODE* tree = create_node(0, '*', NULL, NULL);

        *i += 1;
        tree->left = construct_tree(pre_order_tree, i);

        *i += 1;
        tree->right = construct_tree(pre_order_tree, i);

        return tree;
   }
}

// FUNCAO DE DESCOMPRESSAO

void descompact(FILE *compacted_file, char output_file[50], NODE *tree, lli cont_bytes, ushort trash) 
{
    uchar byte = 0;
    int limit = 0;
    NODE* current = tree; 

    printf("%s\n", output_file);

    FILE* descompacted_file = fopen(output_file, "wb");

    for (cont_bytes; cont_bytes > 0; cont_bytes--) 
    { 
        fscanf(compacted_file, "%c", &byte);
        
        if (cont_bytes == 1) 
        { 
            limit = trash; 
        }

        for (int i = 7; i >= limit; i--)
        {
            if (is_bit_i_set(byte, i)) 
            {
                current = current -> right; 
            } 
            else 
            {
                current = current -> left; 
            }
            if (is_leaf(current)) 
            { 
                fprintf(descompacted_file, "%c", *(uchar*) (current -> caracter));
                current = tree; 
            }
        }
    }
    fclose(descompacted_file);
}

int decompress()
{
    char input_file[50];
    char output_file[50];

    printf("\nDigite o nome do arquivo:\n\n");

    scanf("%s", input_file);

    int size = strlen(input_file) - 5;
    int i;
    
    for( i = 0; i < size; i++) 
    {
        output_file[i] = input_file[i];
    }
    output_file[i] = '\0';

    FILE* file = fopen(input_file, "rb");

    if(file == NULL)
    {
        printf("Empty file\n");
        return 0;
    }

    FILE* compacted_file = fopen(input_file, "rb"); 
    
    uchar byte_1, byte_2;  
    
    fscanf(compacted_file, "%c", &byte_1); 
    fscanf(compacted_file, "%c", &byte_2); 

    ushort trash = byte_1 >> 5; 
    ushort tree_size = byte_1 << 11;   
    tree_size >>= 3;            
    tree_size |= byte_2;        

    uchar pre_order_tree[tree_size];       

    for (i = 0; i < tree_size; i++) 
    { 
        fscanf(compacted_file, "%c", &pre_order_tree[i]);
    }
    
    i = 0;
    NODE* tree = construct_tree(pre_order_tree, &i);

    lli quant_bytes = 0;
    uchar byte; 

    while (fscanf(compacted_file, "%c", &byte) != EOF) 
    {
        quant_bytes++;
    }

    fseek(compacted_file, 2 + tree_size, SEEK_SET);

    descompact(compacted_file, output_file, tree, quant_bytes, trash);
    fclose(compacted_file);

    return 1;
}