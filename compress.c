#include "compress.h"

// ARRAY DE FREQUENCIA

void fill_freq_array(lli* frequence, char input_file[50])
{
    FILE* file = fopen(input_file, "rb");
    uchar caracter;

    while (fscanf(file, "%c", &caracter) != EOF)
    {
        frequence[caracter] += 1;
    }
    fclose(file);
}
// FILA DE PRIORIDADE

PRIORITY_QUEUE* create_priority_queue()
{
    PRIORITY_QUEUE* queue = (PRIORITY_QUEUE*) malloc(sizeof(PRIORITY_QUEUE));

    queue -> head = NULL; 
    queue -> size = 0;

    return queue;
};

void fill_priority_queue(lli frequence[256], PRIORITY_QUEUE* queue)
{
    for (int i = 0; i < 256; i++)
    {
        if (frequence[i] != 0)
        {
            NODE* new_node = create_node(frequence[i], i, NULL, NULL);
            enqueue(new_node, queue);
        }
    }
}

void enqueue(NODE* new_node, PRIORITY_QUEUE* queue)
{
    if (queue -> head == NULL || new_node -> priority <= queue -> head -> priority)
    {
        new_node -> next = queue -> head;
        queue -> head = new_node;
    } 
    else
    {
        NODE* current = queue -> head;

        while (current -> next != NULL && current-> next-> priority < new_node -> priority)
        {
            current = current->next;
        }
        new_node -> next = current -> next;
        current -> next = new_node;
    }
    queue -> size += 1;
}

NODE* dequeue(PRIORITY_QUEUE* queue)
{
    if (queue -> head == NULL)
    {
        printf("QUEUE UNDERFLOW\n");
        return NULL;
    }
    queue -> size -= 1;
    NODE* first_node = queue -> head;
    queue -> head = queue -> head -> next;
    first_node -> next = NULL;
    
    return first_node;
}

// TREE E NODE

NODE* create_node(lli priority, uchar caracter, NODE* left, NODE* right)
{
    NODE* new_node = (NODE*) malloc(sizeof(NODE));
    
    uchar* aux = (uchar*) malloc(sizeof(uchar)); 
    *aux = caracter;

    new_node -> caracter = aux; 
    new_node -> priority = priority;
    new_node -> left = left;
    new_node -> right = right;
    new_node -> next = NULL;

    return new_node;
}

NODE* create_huff_tree(PRIORITY_QUEUE* queue)
{
    while (queue -> size > 1)
    {
        NODE* left_node = dequeue(queue);
        NODE* right_node = dequeue(queue);
        
        lli sum = left_node -> priority + right_node -> priority;

        NODE* tree_node = create_node(sum, '*', left_node, right_node);

        enqueue(tree_node, queue);
    }
    return queue -> head;
}

ushort get_tree_size(NODE* tree)
{
    if (is_leaf(tree))
    {
        if ( *((uchar*) (tree -> caracter)) == '*' || *((uchar*) (tree -> caracter)) == '\\')
        {
            return 2;
        } 
        else
        {
            return 1;
        }
    }
    return 1 + get_tree_size(tree -> left) + get_tree_size(tree -> right);
}

void print_tree(NODE* tree, FILE* compacted_file)
{
    if (tree != NULL)
    {
        if (is_leaf(tree))
        {
            if ( (*(uchar*)(tree -> caracter)) == '*' || *((uchar*)(tree -> caracter)) == '\\')
            {
                fputc('\\', compacted_file);
            }
        }
        fputc( *(uchar*) (tree -> caracter), compacted_file);
        print_tree(tree -> left, compacted_file);
        print_tree(tree -> right, compacted_file);
    }
}

bool is_leaf(NODE* current)
{
    if (current -> left == NULL && current -> right == NULL) return true;
    else return false;
}

// HASH

HASH* create_hash()
{
    HASH* new_hash = (HASH*) malloc(sizeof(HASH));

    for (int i = 0; i < 256; i++)
    {
        new_hash -> array[i] = NULL;
    }
    return new_hash;
}

// NOVA CODIFICACAO

void new_codification(HASH* hash, NODE* tree, int size, ushort byte)
{
    if (is_leaf(tree))
    {    
        int index = *((uchar*) (tree -> caracter)); 
        
        ushort* aux = (ushort*) malloc(sizeof(ushort)); 
        *aux = byte;        

        ELEMENT* element = (ELEMENT*) malloc(sizeof(ELEMENT)); 
        element -> code = aux;
        element -> size = size;

        hash -> array[index] = element;
        
        return;
    }
    byte <<= 1;
    new_codification(hash, tree -> left, size + 1, byte);

    byte++; 
    new_codification(hash, tree -> right, size + 1, byte);
}

bool is_bit_i_set(ushort byte, int i)
{
    ushort temp = 1 << i;
    return temp & byte;
}

// LIXO, TAMANHO DA ÁRVORE E CABEÇALHO

uchar get_trash(HASH* hash, lli frequence[256]) 
{
    lli bits_amount = 0; 
    uchar trash = 0; 

    for (int i = 0; i < 256; i++)
    {
        if (frequence[i] > 0)
        {
            bits_amount += hash -> array[i] -> size * frequence[i];
        }
    }

    if (bits_amount % 8 != 0)
    {
        trash = 8 - (bits_amount % 8);
    }
    return trash;
}

ushort create_header(HASH* hash, NODE* tree, uchar trash, ushort tree_size) 
{
    ushort header = 0; 
    header |= trash;  
    header <<= 13;   
    header = header | tree_size;  

    return header;
}

void write_header(ushort header, FILE* compacted_file)
{
    uchar byte_1 = header >> 8; 
    uchar byte_2 = header; 

    fputc(byte_1, compacted_file);
    fputc(byte_2, compacted_file);
}

// COMPACTAÇÃO

void compact_file(FILE* compacted_file, char input_file[50], HASH* hash, uchar trash)
{
    FILE* origin_file = fopen(input_file, "rb"); 
    int size;
    int quant_bits;      
    ushort code;         
    uchar caracter;      
    uchar compress_byte; 
    
    quant_bits = 0, compress_byte = 0, size = 0;  

    while (fscanf(origin_file, "%c", &caracter) != EOF)
    { 
        code = *(ushort*) (hash -> array[caracter] -> code);     
        size = hash -> array[caracter] -> size - 1;

        for (size; size >= 0; size--)
        { 
            if (is_bit_i_set(code, size))
            {
                compress_byte += 1; 
            }

            quant_bits += 1; 
            
            if (quant_bits == 8)
            { 
                fprintf(compacted_file, "%c", compress_byte); 

                quant_bits = 0;    
                compress_byte = 0; 
            }
            compress_byte <<= 1; 
        }
    }
    compress_byte >>= 1;          
    compress_byte <<= trash;

    fprintf(compacted_file, "%c", compress_byte); 
    fclose(origin_file);
}

int compress()
{
    char input_file[50]; 
    char output_file[50];

    printf("\nDigite o nome do arquivo:\n\n");

    scanf("%s", input_file);
    
    strcpy(output_file, input_file);
    strcat(output_file, ".huff");

    FILE* file = fopen(input_file, "rb");

    if(file == NULL)
    {
        printf("Empty file\n");
        return 0;
    }
                
    lli frequence[256] = {0};
    fill_freq_array(frequence, input_file);

    PRIORITY_QUEUE* queue = create_priority_queue();
    fill_priority_queue(frequence, queue);

    NODE* tree = create_huff_tree(queue);
                
    HASH* hash = create_hash();
    new_codification(hash, tree, 0, 0); 

    FILE* compacted_file = fopen(output_file, "wb"); 
                
    uchar trash = get_trash(hash, frequence); // trash so ocupa 3 bits, por isso so alocamos 1 byte para guardar o lixo.
    ushort tree_size = get_tree_size(tree);

    ushort header = create_header(hash, tree, trash, tree_size);
    write_header(header, compacted_file); // escrever o cabeçalho no arquivo compactado.
                    
    print_tree(tree, compacted_file);
    compact_file(compacted_file, input_file, hash, trash);

    fclose(compacted_file);

    return 1;
}