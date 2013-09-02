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
#ifndef _COMMON_LIST_H
#define _COMMON_LIST_H

/*
 * Stolen from linux-2.1.131
 * All comments from the original source unless otherwise noted
 * Added: the CLEAR_LIST_NODE macro
 */

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct list_head {
        struct list_head *next, *prev;
};

#define LIST_HEAD(name) \
        struct list_head name = { &name, &name }

#define INIT_LIST_HEAD(ptr)  do { \
        (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#define CLEAR_LIST_NODE(ptr) do { \
        (ptr)->next = NULL;  (ptr)->prev = NULL; \
} while (0)

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline__ void __list_add(struct list_head *li,
        struct list_head * prev,
        struct list_head * next)
{
        next->prev = li;
        li->next = next;
        li->prev = prev;
        prev->next = li;
}

/*
 * Insert a new entry after the specified head..
 */
static __inline__ void list_add(struct list_head *li, struct list_head *head)
{
        __list_add(li, head, head->next);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline__ void __list_del(struct list_head * prev,
                                  struct list_head * next)
{
        next->prev = prev;
        prev->next = next;
}

static __inline__ void list_del(struct list_head *entry)
{
        __list_del(entry->prev, entry->next);
}

static __inline__ int list_empty(struct list_head *head)
{
        return head->next == head;
}

/*
 * Splice in "list" into "head"
 */
static __inline__ void list_splice(struct list_head *list, struct list_head *head)
{
        struct list_head *first = list->next;

        if (first != list) {
                struct list_head *last = list->prev;
                struct list_head *at = head->next;

                first->prev = head;
                head->next = first;

                last->next = at;
                at->prev = last;
        }
}

#define list_entry(ptr, type, member) \
        ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#endif /* _COMMON_LIST_H */
