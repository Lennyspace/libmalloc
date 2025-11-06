#include "big_nmap.h"

void add_bignmap(struct list_big_nmap *list_b, struct big_nmap *node) {
  node->next = list_b->head;
  list_b->head = node;
}

void remove_bignmap(struct list_big_nmap *list_b, struct big_nmap *node) {

  if (list_b->head == node) {
    list_b->head = node->next;
    return;
  }

  struct big_nmap *cur = list_b->head;
  while (cur && cur->next != node) {
    cur = cur->next;
  }
  if (cur) {
    cur->next = node->next;
  }
}
void init_big_nmap(struct big_nmap *big_nmap, size_t taille_mmap,
                   unsigned char *page_base) {
  big_nmap->next = NULL;
  big_nmap->size_page = taille_mmap;
  big_nmap->data_adress = page_base + sizeof(struct big_nmap);
}
