#ifndef DESCOMPRESS_H
#define DESCOMPRESS_H
#include "header.h"

/** @param str: a pointer to the */
/** @param i: a pointer to the */
/** @return returns */

bool is_bit_i_set(ushort byte, int i);

NODE* create_node(lli priority, uchar caracter, NODE* left, NODE* right);

bool is_leaf(NODE* current);

NODE* construct_tree(uchar pre_order_tree[], int *i); 

void descompact(FILE *compacted_file, char output_filename[256], NODE *tree, lli cont_bytes, ushort trash);

int decompress();

#endif