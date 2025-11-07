#ifndef BIG_NMAP_H
#define BIG_NMAP_H
#include <stddef.h>
struct list_big_nmap
{
    struct big_nmap *head;
};

struct big_nmap
{
    struct big_nmap *next;
    unsigned char *data_adress; // pointeur retourner a l utilisateur
    size_t size_page; // taille map en octets (multiple de 4096)
    size_t size_demande;
};

void remove_bignmap(struct list_big_nmap *list_b, struct big_nmap *);
void add_bignmap(struct list_big_nmap *list_b, struct big_nmap *);
void init_big_nmap(struct big_nmap *big_nmap, size_t taille_mmap,
                   unsigned char *page_base);

#endif // ! BIG_NMAP_H
