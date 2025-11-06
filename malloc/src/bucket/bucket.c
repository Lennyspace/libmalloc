#include "bucket.h"
#include <stddef.h>
#include <stdio.h>

void init_list_list_bucket(struct list_bucket list_gen[7]) {
  size_t p = 16;
  for (int i = 0; i < 7; i++) {
    list_gen[i].size_block = p;
    p = p * 2;
    list_gen[i].head = NULL;
  }
}

void init_bucket(struct bucket *bucket, unsigned char *page_base,
                 size_t block_size) {
  bucket->block_size = block_size;

  bucket->nb_block = (4096 - sizeof(struct bucket)) / block_size;
  bucket->nb_block_free = (4096 - sizeof(struct bucket)) / block_size;
  for (size_t i = 0; i < (bucket->nb_block + 7) / 8; i++) {
    bucket->flags[i] = 0; // free
  }
  bucket->data_start = page_base + sizeof(struct bucket);

  bucket->next = NULL;
  bucket->prev = NULL;
}

void list_bucket_append(struct list_bucket *list_b, struct bucket *bucket) {
  if (list_b->head == NULL) {
    list_b->head = bucket;
    bucket->prev = NULL;
    bucket->next = NULL;

    return;
  }
  struct bucket *cur = list_b->head;
  while (cur->next) {
    cur = cur->next;
  }
  cur->next = bucket;
  bucket->prev = cur;
}

void list_bucket_remove(
    struct list_bucket *list_b,
    size_t index) { // ne ke free pas/ on  considere index valide
  if (list_b->head->next == NULL) {
    list_b->head = NULL;
    return;
  }
  struct bucket *cur = list_b->head;
  size_t i = 0;
  while (i < index) {
    cur = cur->next;
    i++;
  }
  if (cur->next == NULL) {
    cur->prev->next = NULL;
    return;
  }
  if (cur->prev == NULL) {
    cur->next->prev = NULL;
    list_b->head = cur->next;
    return;
  }
  cur->next->prev = cur->prev;
  cur->prev->next = cur->next;
}

int find_free_block(struct bucket *bucket) {
  for (size_t i = 0; i < bucket->nb_block; i++) {
    size_t bytes_i = i / 8;
    size_t bit_index = i % 8;
    if ((bucket->flags[bytes_i] & (1 << bit_index)) == 0) { // est free
      return i;
    }
  }
  return -1;
}

void set_block_used(struct bucket *bucket, int bloc_index) {
  size_t bytes_i = bloc_index / 8;
  size_t bit_index = bloc_index % 8;
  bucket->flags[bytes_i] = (bucket->flags[bytes_i] | (1 << bit_index));
  bucket->nb_block_free--;
}

void set_block_free(struct bucket *bucket, int bloc_index) {
  size_t bytes_i = bloc_index / 8;
  size_t bit_index = bloc_index % 8;
  bucket->flags[bytes_i] = (bucket->flags[bytes_i] & (~(1 << bit_index)));
  bucket->nb_block_free++;
}
struct bucket *get_bucket_not_full(struct list_bucket *bucket_list) {
  struct bucket *bucket_cur = bucket_list->head;
  while (bucket_cur && bucket_cur->nb_block_free == 0) {
    bucket_cur = bucket_cur->next;
  }
  return bucket_cur;
}
