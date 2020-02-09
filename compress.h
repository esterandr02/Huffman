#ifndef COMPRESS_H
#define COMPRESS_H
#include "header.h"

/** @param frequence: */
void fill_freq_array(lli* frequence, char input_filename[50]);

/** @return returns queue */
PRIORITY_QUEUE* create_priority_queue();

/** @param frequence: */
/** @param queue: */
void fill_priority_queue(lli frequence[256], PRIORITY_QUEUE* queue);

/** @param new_node: */
/** @param queue: */
void enqueue(NODE* new_node, PRIORITY_QUEUE* queue); 

NODE* dequeue(PRIORITY_QUEUE* queue);

/** @param priority: */
/** @param caracter: */
/** @param left: */
/** @param right: */
/** @return returns */
NODE* create_node(lli priority, uchar caracter, NODE* left, NODE* right);

/** @param queue: */
/** @return returns */
NODE* create_huff_tree(PRIORITY_QUEUE* queue);

/** @param tree: */
/** @return returns */
ushort get_size_tree(NODE* tree);

/** @param tree: */
/** @param file: */
void print_tree(NODE* tree, FILE* compacted_file);

/** @param current: */
/** @return returns */
bool is_leaf(NODE* current);

// FUNCOES DE HASH

/** @return returns new_hash */
HASH* create_hash();

// FUNCOES NOVA CODIFICACAO

/** @param hash: */
/** @param tree: */
/** @param size: */
/** @param byte: */
void new_codification(HASH* hash, NODE* tree, int size, ushort byte);

/** @param tree: */
/** @param tree: */
/** @return returns */
bool is_bit_i_set(ushort byte, int i);

/** @param hash: */
/** @param frequence: */
/** @return returns */
uchar get_trash(HASH* hash, lli* frequence);

ushort create_header(HASH* hash, NODE* tree, uchar trash, ushort tree_size);

void write_header(ushort header, FILE *compact_file);

/** @param arq_compact: */
/** @param hash: */
/** @param trash_size: */
void compact_file(FILE* arq_compact, char input_filename[50], HASH* hash, uchar trash);

int compress();

#endif