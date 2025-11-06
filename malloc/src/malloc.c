#include "big_nmap/big_nmap.h"
#include "bucket/bucket.h"
#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
static struct {
  int init;
  struct list_bucket list_bucket[7]; // 16/32/64/128/256/512/1024/
  struct list_big_nmap list_big_nmap;
  //
  // 1024 =>page entiere pas besoin de bucket
} Global;

size_t indice_tableau_puissance2(size_t size) {
  size_t i = 0;
  size_t p = 16;
  while (size > p) {
    i++;
    p = p * 2;
  }
  return i;
}

void init_global(void) {
  if (Global.init == 1)
    return;
  else {
    init_list_list_bucket(Global.list_bucket);
    Global.list_big_nmap.head = NULL;
    Global.init = 1;
  }
}
static size_t multiple_de_4096(size_t size) {
  size_t taille = 4096;
  while (size > taille) {
    taille = taille + 4096;
  }
  return taille;
}
__attribute__((visibility("default"))) void *malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }
  if (size > 1024) {
    size = size + sizeof(struct big_nmap);
    size_t taille_mmap = multiple_de_4096(size);
    void *page = mmap(NULL, taille_mmap, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) {
      return NULL;
    }
    struct big_nmap *big_nmap = (struct big_nmap *)page;
    init_big_nmap(big_nmap, taille_mmap, page);
    add_bignmap(&Global.list_big_nmap, big_nmap);
    return big_nmap->data_adress;
  }
  init_global();
  size_t taille_tab = indice_tableau_puissance2(size);

  struct list_bucket *list_bucket = &Global.list_bucket[taille_tab];

  struct bucket *bucket_not_full = get_bucket_not_full(list_bucket);
  if (bucket_not_full == NULL) {
    void *page = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) {
      return NULL;
    }
    struct bucket *new_bucket = (struct bucket *)page;
    size_t block_size =
        16 << taille_tab; // 16* 2 puissance indice tab (decalage)
    init_bucket(new_bucket, page, block_size);
    list_bucket_append(list_bucket, new_bucket);
    set_block_used(new_bucket, 0);
    return new_bucket->data_start;
  }
  int block_index_free = find_free_block(bucket_not_full);
  set_block_used(bucket_not_full, block_index_free);
  return bucket_not_full->data_start +
         (block_index_free * bucket_not_full->block_size);
}

static struct bucket *find_bucket_for_ptr(void *ptr) {
  unsigned char *my_ptr = ptr;
  for (int i = 0; i < 7; i++) {
    struct bucket *cur = Global.list_bucket[i].head;
    while (cur) {
      unsigned char *start_data = cur->data_start;
      unsigned char *end_data = start_data + (cur->nb_block * cur->block_size);
      if (my_ptr >= start_data && my_ptr < end_data) {
        return cur;
      }
      cur = cur->next;
    }
  }
  return NULL;
}

static void free_block_in_bucket(struct bucket *bucket, void *ptr) {
  unsigned char *my_ptr = ptr;
  unsigned char *start_data = bucket->data_start;

  size_t diff = my_ptr - start_data;
  size_t block_i = diff / bucket->block_size;
  set_block_free(bucket, block_i);
}
static void remove_and_unmap_bucket(struct bucket *bucket) {
  size_t list_index = 0;
  size_t size = 16;

  for (int i = 0; i < 7; i++) {
    if (size == bucket->block_size) {
      list_index = i;
      break;
    }
    size *= 2;
  }

  struct list_bucket *list_b = &Global.list_bucket[list_index];
  list_bucket_remove(list_b, list_index);
  munmap(bucket, 4096);
}

static struct big_nmap *find_big_allocation(void *ptr) {
  struct big_nmap *cur = Global.list_big_nmap.head;
  while (cur) {
    if (cur->data_adress == ptr) {
      return cur;
    }
    cur = cur->next;
  }
  return NULL;
}
static void free_big_nmap(struct big_nmap *big_nmap) {
  remove_bignmap(&Global.list_big_nmap, big_nmap);
  unsigned char *data_ad = big_nmap->data_adress;
  void *pointeur_start = data_ad - sizeof(struct big_nmap);
  munmap(pointeur_start, big_nmap->size_page);
}

__attribute__((visibility("default"))) void free(void *ptr) {
  if (ptr == NULL) {
    return;
  }
  init_global();
  struct big_nmap *big_nmap = find_big_allocation(ptr);
  if (big_nmap != NULL) {
    free_big_nmap(big_nmap);
    return;
  }
  struct bucket *bucket = find_bucket_for_ptr(ptr);
  free_block_in_bucket(bucket, ptr);
  if (bucket->nb_block_free == bucket->nb_block) {
    remove_and_unmap_bucket(bucket);
  }
  return;
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size) {
  if (ptr == NULL) {
    return NULL;
  }
  if (size == 0) {
    free(ptr);
    return NULL;
  }
  void *new_ptr = malloc(size);
  if (new_ptr == NULL) {
    return NULL;
  }
  size_t cur_size = 0;
  struct big_nmap *big_nmap = find_big_allocation(ptr);
  if (big_nmap != NULL) {
    cur_size = big_nmap->size_page - sizeof(struct big_nmap);
  } else {
    struct bucket *bucket = find_bucket_for_ptr(ptr);
    cur_size = bucket->block_size;
  }
  if (cur_size > size) {
    cur_size = size;
  }
  memcpy(new_ptr, ptr, cur_size);
  free(ptr);
  return new_ptr;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size) {
  size_t total_size = 0;
  if (__builtin_mul_overflow(nmemb, size, &total_size)) {
    return NULL;
  }
  if (nmemb == 0 || size == 0) {
    return NULL;
  }
  void *ptr = malloc(size);
  if (ptr == NULL) {
    return NULL;
  }
  memset(ptr, 0, total_size);
  return ptr;
}
