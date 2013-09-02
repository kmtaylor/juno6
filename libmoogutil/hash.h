/*
 * Copyright(c) 2000 UltraMaster Group
 *
 * License to use UltraMaster Juno-6 is provided free of charge subject to the 
 * following restrictions:
 *
 * 1.) This license is for your personal use only.
 *
 * 2.) No portion of this software may be redistributed by you to any other 
 *     person in any form. 
 *
 * 3.) You may not sell UltraMaster Juno-6 to any person.
 *
 * 4.) UltraMaster Juno-6 is provided without any express or implied warranty. 
 *     In no event shall UltraMaster Group be held liable for any damages 
 *     arising from the use of UltraMaster Juno-6.
 */
#ifndef _COMMON_HASH_H
#define _COMMON_HASH_H

#include "list.h"

struct hash_entry
{
    char              *he_key;
    void              *he_obj;
    struct list_head   he_list;
};

struct hash_table
{
    int                ht_size;
    struct list_head  *ht_lists;
};

#ifdef __cplusplus
extern "C" {
#endif

struct hash_table *create_hash_table(unsigned int sz);
void destroy_hash_table(struct hash_table *tbl, void (*delete_obj)(void *));
void *put_hash_object(struct hash_table *tbl, const char *key, void *obj);
void *get_hash_object(struct hash_table *tbl, const char *key);
void *remove_hash_object(struct hash_table *tbl, const char *key);

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_HASH_H */
