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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "ini.h"
#include "text_util.h"
#include "rcsid.h"

//RCSID("$Id: ini.c,v 1.5.4.1 2000/02/24 00:44:09 larocca Exp $");

/* these are passed to the destroy_hash_table function */
static void destroy_ini_section(void *);
static void destroy_ini_value(void *);

static struct ini_section *create_ini_section(char *);
static int read_ini_section(struct ini_file *, FILE *);

struct ini_file *create_ini_file(const char *fn)
{
    FILE *fp;
    struct ini_file *ini;
     
    if ((fp = fopen(fn, "r")) == NULL)
    {
	debug(DEBUG_SYSERROR, "can't open ini file %s", fn);
	goto err_out;
    }

    if ((ini = malloc(sizeof(*ini))) == NULL)
	goto err_out_close;

    if ((ini->if_sections = create_hash_table(INI_FILE_HASH_SIZE)) == NULL)
	goto err_out_free_ini;

    while (read_ini_section(ini, fp) > 0) ;

    fclose(fp);
    return ini;


 err_out_free_ini:
    free(ini);

 err_out_close:
    fclose(fp);

 err_out:
    return NULL;
}

/*
 * NOTE: read_ini_section uses a static buff for holding each line read
 * because the algorithm reads the first line of the next section in order
 * to realize this section is done.  When we get to processing that next
 * section, we will need that line.  Note a static is initialize to NULLs 
 * by default.
 */

int read_ini_section(struct ini_file *ini, FILE *fp)
{
   static char buff[BUFSIZ];
   struct ini_section *sect = NULL;
   char *p, *val;
   int got_heading = 0;
   char *eof = NULL;

   do
   {
       chop(buff);

       if (buff[0] == '#')
	   continue;

       if (buff[0] == '[' && (p = strchr(buff, ']')))
       {
	   if (!got_heading)
	   {
	       *p = '\0';
	       if ((sect = create_ini_section(buff + 1)) == NULL)
		   return -1;

	       got_heading = 1;
	       put_hash_object(ini->if_sections, sect->is_name, sect);

	       /* TODO: if we record the 'ftell' of the file stream at 
		* this point, we could add new values to this section
		* by re-openining the .ini file, opening a tmp file,
		* writing from the beginning to here, then the new value
		* then the rest of the file, then renaming the tmp file
		* and unlinking the original file
		*/

	       debug(DEBUG_STATUS, "created new section '%s'", buff + 1);
	   }
	   else
	   {
	       break;
	   }
       }
       else
       {
	   if ((p = strchr(buff, '=')) != NULL)
	   {
	       *p++ = '\0';
	       val = strdup(p);
	       upper_case(buff);
	       put_hash_object(sect->is_settings, buff, val);
	       
	       debug(DEBUG_STATUS, "added new setting %s=%s", buff, val);
	   }
       }
   } 
   while ((eof = fgets(buff, BUFSIZ, fp)));

   return (eof != NULL);
}

struct ini_section *create_ini_section(char *sectname)
{
    struct ini_section *sect;

    sect = malloc(sizeof(*sect));
    
    if (!sect)
	goto err_out;

    if ((sect->is_settings = create_hash_table(INI_SECTION_HASH_SIZE)) == NULL)
	goto err_out_free;

    strcpy(sect->is_name, sectname);
    upper_case(sect->is_name);

    return sect;

 err_out_free:
    free(sect);

 err_out:
    return NULL;
}


void destroy_ini_file(struct ini_file *ini)
{
    if (ini)
    {
	destroy_hash_table(ini->if_sections, destroy_ini_section);
	free(ini);
    }
}

static void destroy_ini_section(void *v_ini_section)
{
    struct ini_section *sect = (struct ini_section *)v_ini_section;

    destroy_hash_table(sect->is_settings, destroy_ini_value);
    free(sect);
}

static void destroy_ini_value(void *v_value)
{
    free(v_value);
}

struct ini_section *get_ini_section(struct ini_file *ini, const char *sect)
{
    struct ini_section *retval = NULL;
    char sectname[SECTION_NAME_MAX];

    strncpy(sectname, sect, SECTION_NAME_MAX - 1);
    sectname[SECTION_NAME_MAX - 1] = '\0';
    upper_case(sectname);
    
    if (ini)
	retval = (struct ini_section *)get_hash_object(ini->if_sections, sectname);

    return retval;
}

#define MAX_KEY_LEN 64

const char *get_string_setting(struct ini_section *sect, const char *s, 
			       const char *defval)
{
    const char *retval = defval;
    char keybuff[MAX_KEY_LEN];
    
    /* FIXME: need to force MAX_KEY_LEN everywhere */
    strcpy(keybuff, s);
    upper_case(keybuff);

    if (sect)
	if (!(retval = (char *)get_hash_object(sect->is_settings, keybuff)))
	    retval = defval;

    return retval;
}

int get_int_setting(struct ini_section *sect, const char *s, int defval)
{
    const char *val = get_string_setting(sect, s, NULL);

    if (val)
	return atoi(val);

    return defval;
}
