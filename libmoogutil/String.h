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
#ifndef _CBT_STRING_H
#define _CBT_STRING_H

#define STRING_DEFAULT_SIZE     32
#define SPRINTF_MAX_INT_WIDTH   32
#define SPRINTF_MAX_FLOAT_WIDTH 32

#include <stdio.h>
#include <stdarg.h>
#include <iostream>

#ifndef STRING_WITHOUT_NUMBER
class Number;
#endif

class StringBuffer
{
 public:
    int   refCount;
    char *data;
    int   len;
    int   buflen;

    StringBuffer(const char *, int);
    StringBuffer(StringBuffer &);
    ~StringBuffer();

    void use();
    void unuse();
    void growTo(int);
    void grow(int);
    void setLength(int, char padChar);
    void copyBytes(int, const char *, int);
};


class String
{
 protected:
    StringBuffer *buffer;
    
    /* copy on write */
    void    cow();
    int     replaceInternal(const char *, int, const char *, int);

 public:
    String();
    String(const char *s, int = -1);
    String(const String &);

    String(int);
    String(double);
#ifndef STRING_WITHOUT_NUMBER
    String(const Number &);
#endif

    ~String();

    const String &assign( const String &);
    const String &assign( const char *, int );

    void    append(const String &);
    void    append(const char *);
    void    append(const char *, int);
    void    appendChar(const char);
    
    int     getLength() const { return buffer->len; }
    void    setLength(int, char padChar = ' ');

    int     sprintf(const char *, ...);
    int     vsprintf(const char *, va_list);

    int     readLine(std::istream *);
    int     write(std::ostream *) const;
    int     write(std::ostream *, int, int) const;

    String &chop();
    String &trim(const char = ' ');
    String &rtrim(const char = ' ');
    String &ltrim(const char = ' ');
    String &upper();
    String &lower();
    int     find(const char, int = 0) const;
    int     find(const char *, int = 0) const;
    int     replace(const String &, const String &);
    int     replace(const String &, const char *);
    int     replace(const char *, const String &);
    int     replace(const char *, const char *);


    String  substr(int, int = -1) const;
    String  left(int) const;
    String  right(int) const;

    int     compareTo(const String &s) const;
    int     compareTo(const char *s) const;

    int     intValue(int) const ;

    /* operator overloads */

    operator const char * () const;
    char operator [] (int) const;

    const String &operator = (const char *);
    const String &operator = (const String &);

    const String &operator += (const char *);
    const String &operator += (const String &);

    friend String operator + (const String &, const String &);
    friend String operator + (const String &, const char *);
    friend String operator + (const char *,   const String &);
    friend String operator + (const String &, int);
    friend String operator + (const String &, double);

    friend int operator == (const String &, const String &);
    friend int operator == (const String &, const char *);
    friend int operator == (const char *, const String &);
    friend int operator == (const String &, const int);
    friend int operator == (const int, const String &);

    friend int operator != (const String &, const String &);
    friend int operator != (const String &, const char *);
    friend int operator != (const char *, const String &);
};

#endif /* _CBT_STRING_H */




