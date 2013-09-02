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
#ifndef _COMMON_INI_H
#define _COMMON_INI_H

#define SECTION_NAME_MAX 64

/* size of hash table that holds the sections */
#define INI_FILE_HASH_SIZE    13

/* size of hash table that holds the settings */
#define INI_SECTION_HASH_SIZE 23

#include "hash.h"

struct ini_file
{
    struct hash_table *if_sections;
};

struct ini_section
{
    char is_name[SECTION_NAME_MAX];
    struct hash_table *is_settings;
};

#ifdef __cplusplus
extern "C" {
#endif

/* ini_file functons */
struct ini_file *create_ini_file(const char *);
void destroy_ini_file(struct ini_file *);
struct ini_section *get_ini_section(struct ini_file *, const char *);

/* ini_section functions */
const char *get_string_setting(struct ini_section *, const char *, const char *);
int get_int_setting(struct ini_section *, const char *, int);

#ifdef __cplusplus
}
#endif

#endif /*  _COMMON_INI_H  */
