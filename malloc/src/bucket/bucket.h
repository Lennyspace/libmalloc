#ifndef BUCKET_H
#define BUCKET_H

#include <stddef.h>

struct list_bucket
{
    size_t size_block;
    struct bucket *head;
};

struct bucket
{
    struct bucket *next; // next bucket si faut 2 page par exemple
    struct bucket *prev;
    size_t block_size;
    size_t nb_block;
    size_t nb_block_free;

    unsigned char
        flags[32]; // 4096/16 256 case -> 256bits ->32octer ->32 unsigned char
    unsigned char *data_start;
};
void list_bucket_remove(struct list_bucket *list_b, struct bucket *bucket);

void list_bucket_append(struct list_bucket *list_b, struct bucket *bucket);
void init_bucket(struct bucket *bucket, unsigned char *page_base,
                 size_t block_size);
void init_list_list_bucket(struct list_bucket list_gen[7]);

int find_free_block(struct bucket *bucket);
void set_block_used(struct bucket *bucket, int bloc_index);
void set_block_free(struct bucket *bucket, int bloc_index);
struct bucket *get_bucket_not_full(struct list_bucket *bucket_list);
#endif // ! BUCKET_H
