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
/**
 * Copyright (c) UltraMaster Group, LLC. All Rights Reserved.
 * $Revision: 1.5.4.1 $$Date: 2000/02/24 00:44:04 $
 */
#include "Settings.h"

//#include <pwd.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <util/debug.h>

Settings::Settings()
{

    String iniFileName = "juno6.ini";


    iniFile = create_ini_file( iniFileName );
}

Settings::~Settings()
{
    destroy_ini_file( iniFile );
}

int 
Settings::getInt( const char* sectionName, const char* itemName )
{
    struct ini_section* section = get_ini_section( iniFile, sectionName );
    return( get_int_setting( section, itemName, 0 ) );
}

const char*
Settings::getString( const char* sectionName, const char* itemName )
{
    struct ini_section* section = get_ini_section( iniFile, sectionName );
    return( get_string_setting( section, itemName, "" ) );
}
