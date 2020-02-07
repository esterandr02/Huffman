#include "header.h"
#include "compress.h"

// FUNCOES FILA DE PRIORIDADE

PRIORITY_QUEUE* create_priority_queue()
{
    PRIORITY_QUEUE* queue = (PRIORITY_QUEUE*) malloc(sizeof(PRIORITY_QUEUE));

    queue -> head = NULL; 
    queue -> size = 0;

    return queue;
};

void fill_priority_queue(lli frequence[], PRIORITY_QUEUE* queue)
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

void print_queue(NODE* current)
{
    printf("*** QUEUE ***\n\n");
    while (current != NULL)
    {

        printf("caracter: %c %lld\n", *((uchar*)(current -> caracter)), current -> priority);
        current = current->next;
    }
    printf("\n");
}

// FUNCOES NODE 

NODE* create_node(lli priority, uchar caracter, NODE* left, NODE* right)
{
    NODE* new_node = (NODE*) malloc(sizeof(NODE));
    // parte do cast do caracter *void
    uchar* aux = (uchar*) malloc(sizeof(uchar)); // aloco para nao ser uma variavel temporaria na funcao.
    *aux = caracter;
    new_node -> caracter = aux; // representacao do caracter na ASCII.

    new_node -> priority = priority;
    new_node -> left = left;
    new_node -> right = right;
    new_node -> next = NULL;

    return new_node;
}

// FUNCOES ARRAY DE FREQUENCIA

void fill_freq_array(lli* frequence)
{
    FILE* file = fopen("meg.mp4", "rb");
    uchar caracter;

    while (fscanf(file, "%c", &caracter) != EOF)
    {
        frequence[caracter] += 1;
    }
    fclose(file);
}

// FUNCOES ARVORE DE HUFFMAN

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

void print_tree(NODE* current)
{
    if (current == NULL) return;

    printf("caracter: %c freq: %lld\n", *((uchar*) (current -> caracter)), current -> priority);
    print_tree(current -> left);
    print_tree(current -> right);
}

void get_pre_order_tree(NODE* tree, FILE* file)
{
    if (tree != NULL)
    {
        if (is_leaf(tree))
        { // se cheguei numa folha
        
            if ( (*(uchar*)(tree -> caracter)) == '*' || *((uchar*)(tree -> caracter)) == '\\')
            {
                fputc('\\', file);
            }
        }
        fputc(*((uchar*)(tree -> caracter)), file);
        get_pre_order_tree(tree -> left, file);
        get_pre_order_tree(tree -> right, file);
    }
}

bool is_leaf(NODE* current)
{
    if (current -> left == NULL && current -> right == NULL) return true;
    else return false;
}

// FUNCOES DE HASH

HASH* create_hash()
{
    HASH* new_hash = (HASH*) malloc(sizeof(HASH));

    for (int i = 0; i < 256; i++)
    {
        new_hash -> array[i] = NULL;
    }
    return new_hash;
}

void print_hash(HASH* hash)
{
    ushort byte;
    int size;

    for (int i = 0; i < 256; i++)
    {
        if (hash -> array[i] != NULL)
        {
            byte = *(ushort*) (hash -> array[i] -> code);
            size = hash -> array[i] -> size;

            printf("new_code: %u size: %d\n", byte, size);
        }
    }
}

// NOVA CODIFICACAO

bool is_bit_i_set(ushort byte, int i)
{
    ushort temp = 1 << i;
    return temp & byte;
}

void new_codification(HASH* hash, NODE* tree, int size, ushort byte)
{
    if (is_leaf(tree))
    { // se cheguei numa folha (achei uma caracter).
        // crio um elemento.
        ELEMENT* element = (ELEMENT*) malloc(sizeof(ELEMENT)); 
        int index = *((uchar*) (tree -> caracter)); // pego a representação inteira do caracter (indice na hash).
        // parte do cast
        ushort* aux = (ushort*) malloc(sizeof(ushort)); // aloco uma posicao na memoria que ira salvar meu byte.
        *aux = byte;        

        element -> code = aux;
        element -> size = size;
        hash -> array[index] = element;
        
        return;
    }
    byte <<= 1;
    new_codification(hash, tree -> left, size + 1, byte);

    byte++; // poe 1 no primeiro bit do byte.
    new_codification(hash, tree -> right, size + 1, byte);
}

// CALCULAR LIXO, TAMANHO DA ÁRVORE E MONTAR O CABEÇALHO

void write_header(ushort header, FILE* compact_file)
{
    uchar byte_1 = header >> 8; // pega so o primeiro byte da header(que tem 2 bytes)
    uchar byte_2 = header; // nao precisa shiftar, como o byte 2 so tem 1 byte, ele pega o segundo byte da header.

    fputc(byte_1, compact_file);
    fputc(byte_2, compact_file);
}

ushort create_file_header(HASH* hash, lli frequence[], NODE* tree, uchar trash, ushort size_tree) 
{
    ushort header = 0; // zera os 16 bits (2 bytes): 00000000 00000000
    header |= trash;  // agora ficou assim: 000000 00000101 (ultimo byte eh o lixo em binario)
    header <<= 13;   //  para trazer o lixo pras 3 primeiras posicoes do primeiro byte: 10100000 00000000
    header = header | size_tree;  // inserir o restante da arvore (11 exemplo) nos 2 bits: 1010 0000 0000 1011 (header pronta!)            

    // printf("trash %u\n", trash);
    // printf("header %u\n", header);

    return header;
}

ushort get_size_tree(NODE* tree)
{
    if (is_leaf(tree))
    {
        if ( *((uchar*) (tree -> caracter)) == '*' || *((uchar*)(tree -> caracter)) == '\\')
        {
            return 2;
        } 
        else
        {
            return 1;
        }
    }
    return 1 + get_size_tree(tree -> left) + get_size_tree(tree -> right);
}

uchar get_trash(HASH* hash, lli* frequence) 
{
    lli sum = 0; 

    for (int i = 0; i < 256; i++)
    {
        if (hash -> array[i] != NULL)
        {
            sum += hash -> array[i] -> size * frequence[i];
        }
    }
    uchar trash = 0; // tamanho do lixo
    lli bits = 0; // quantia de bits usados
    lli bytes = sum / 8; // quantia de bytes alocados

    if (sum % 8 != 0)
    {
        bytes++;
        bits = bytes * 8; // quantidade de bits alocados(NAO É O TOTAL DE BITS USADOS TA!)
        trash = bits - sum;
    }
}

// COMPACTAR ARQUIVO TESTE

// executar um caso pequeno para entender a funcao, ela eh facil, so entender com calma :D

void compact_file(FILE* compacted_file, HASH* hash, uchar trash_size)
{
    FILE* read_file = fopen("meg.mp4", "rb"); // abro e leio o arquivo.
    int size;
    int quant_bits;      // verifica se o byte ja esta completo com 8 bits.
    ushort code;         // salva a codificaçao do caracter lido (salvo na hash).
    uchar caracter;      // pega a leitura do caracter.
    uchar compress_byte; // byte com a compressao.
    
    quant_bits = 0, compress_byte = 0, size = 0;  // inicializo tudo com zero.

    while (fscanf(read_file, "%c", &caracter) != EOF)
    { // lemos o arquivo ate o EOF.

        code = *(ushort*) (hash -> array[caracter] -> code);     // pega a nova codificaçao (na hash) do caracter lido.
        size = hash -> array[caracter] -> size - 1; // pega o tamanho dessa codificaçao.

        for (size; size >= 0; size--)
        { 
            if (is_bit_i_set(code, size))
            {
                compress_byte += 1; // poe 1 no bit mais a direita.
            }
            
            quant_bits += 1; // soma 1 a quantia de bits.
            
            if (quant_bits == 8)
            { // renovar o byte (atingiu o limite da quantidade de bits).

                fprintf(compacted_file, "%c", compress_byte); // printa o byte no arquivo.

                quant_bits = 0;    // zera a quantia de bits.
                compress_byte = 0; // zera o byte (0000 0000).
            }
            compress_byte <<= 1; // shifta para frente o bit mais a dieita para nao perde-lo.
        }
    }
    compress_byte >>= 1;          // ajusta o ultimo bit dp ultimo byte, pois ele contem o ultimo bit errado.
    compress_byte <<= trash_size; //  shiftei o tamanho do lixo no ultimo byte do arquivo codificado.

    fprintf(compacted_file, "%c", compress_byte); // printa o ultimo byte no arquivo.
    fclose(read_file);
}