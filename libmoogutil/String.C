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
#include <string.h>
#include <ctype.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#include <iostream>
#ifdef LOCAL_RCSID
#include "rcsid.h"
#else
#include <common/rcsid.h>
#endif
#include "fastmemcpy.h"

#include "String.h"
#ifndef STRING_WITHOUT_NUMBER
#include "Number.h"
#endif



#define MAX(a,b) ( ((a) < (b)) ? (b) : (a) )

#define DATA   buffer->data
#define LEN    buffer->len
#define BUFLEN buffer->buflen

//RCSID("$Id: String.C,v 1.26.2.1 2000/02/24 00:44:09 larocca Exp $");

/* StringBuffer */
StringBuffer::StringBuffer(const char *s, int initCapacity)
{
    refCount = 1;
    len      = (s) ? strlen(s) : 0;
    buflen   = (initCapacity > len + 1) ? initCapacity : len + 1;
    data     = new char[buflen];
    
    memcpy(data, s, len);
    data[len] = 0;
}

StringBuffer::StringBuffer(StringBuffer &buff)
{
    refCount = 1;
    len      = buff.len;
    buflen   = buff.buflen;
    data     = new char[buflen];
    
    memcpy(data, buff.data, len);
    data[len] = 0;
}

StringBuffer::~StringBuffer()
{
    delete data;
}

void StringBuffer::use()
{
    refCount++;
}

void StringBuffer::unuse()
{
    if (!--refCount)
	delete this;
}

/* doesn't do anything if newSize is less than buflen */
void StringBuffer::growTo(int newSize)
{
    char *newData;

    if (newSize <= buflen)
	return;

    buflen = newSize;
    newData = new char[buflen];
    memcpy(newData, data, len);
    delete data;
    data = newData;
    data[len] = 0;
}

/* doesn't do anything if minSize is less than buflen */
void StringBuffer::grow(int minSize)
{
    // FIXME is this a sane growing algo?
    while (minSize > buflen)
	growTo(buflen * 2);
}

void StringBuffer::copyBytes(int pos, const char *src, int count)
{
    grow(pos + count + 1);
    memcpy(data + pos, src, count);
    
    if (len < pos + count)
    {
	len = pos + count;
	data[len] = 0;
    }
}

void StringBuffer::setLength(int newLen, char padChar)
{
    if (newLen < len)
    {
	if (newLen < 0)
	    return;

	len = newLen;
	data[len] = 0;
    }
    else
    {
	grow(newLen + 1);
	memset(data + len, padChar, newLen - len);
	len = newLen;
	data[len] = 0;
    }
}

/*********************/

String::String()
{
    buffer = new StringBuffer(NULL, STRING_DEFAULT_SIZE);
}

String::String(const char *s, int initCapacity /* = -1 */)
{
    buffer = new StringBuffer(s, initCapacity);
}

String::String(const String &s)
{
    buffer = s.buffer;
    buffer->use();
}

String::String(int intValue)
{
    buffer = new StringBuffer(NULL, SPRINTF_MAX_INT_WIDTH);
    sprintf("%d", intValue);
}

String::String(double doubleValue)
{
    buffer = new StringBuffer(NULL, SPRINTF_MAX_FLOAT_WIDTH);
    sprintf("%f", doubleValue);
}

#ifndef STRING_WITHOUT_NUMBER
String::String(const Number &n)
{
    /* don't use the assign function since our buffer pointer is
     * not initialized 
     */
    buffer = n.toString().buffer;
    buffer->use();
}
#endif

String::~String()
{
    buffer->unuse();
}

void String::cow()
{
    if (buffer->refCount == 1)
	return;

    buffer->unuse();
    buffer = new StringBuffer(*buffer);
}

void String::append(const String &s)
{
    cow();
    buffer->copyBytes(LEN, s.DATA, s.LEN);
}

void String::append(const char *s)
{
    append(s, strlen(s));
}


void String::append(const char *s, int slen)
{
    cow();
    buffer->copyBytes(LEN, s, slen);
}


void String::appendChar(const char c)
{
    cow();
    buffer->copyBytes(LEN, &c, 1);
}

void String::setLength(int newLen, char padChar /* = ' ' */)
{
    cow();
    buffer->setLength(newLen, padChar);
}

int String::sprintf(const char *fmt, ...)
{
    va_list ap;
    int retval;

    va_start(ap, fmt);
    retval = vsprintf(fmt, ap);
    va_end(ap);
    return retval;
}

int String::vsprintf(const char *fmt, va_list ap)
{
    va_list apSave;
    const char *pos;
    int maxlen = 0;
    int retval;

    va_copy(apSave, ap);
    cow();

    for (pos = fmt; *pos; pos++)
    {
	if (*pos != '%' || *(++pos) == '%')
	{
	    maxlen++;
	    continue;
	}

	/* parse flags for format */
	while (*pos)
	{
	    //FIXME: the ' conversion isn't supported!

	    if (*pos == '#')
	    {
		maxlen += 2;
		pos++;
	    }
	    
	    else if (*pos == '0' || *pos == '-' || *pos == ' ') 
		pos++;
	    
	    else if (*pos == '+')
	    {
		maxlen++;
		pos++;
	    }
	    
	    else
		break;
	}

	/* parse field width and precision */
	int width = 0, precision = 0;

	if (*pos == '*')
	{
	    width = va_arg(ap, int);
	    pos++;
	}
	else
	{
	    for (; *pos; pos++)
	    {
		if (!isdigit(*pos))
		    break;
		
		width = width * 10 + (*pos - '0');
	    }
	}

	if (*pos == '.')
	{
	    pos++;
	    
	    if (*pos == '*')
	    {
		precision = va_arg(ap, int);
		pos++;
	    }
	    else
	    {
		for (; *pos; pos++)
		{
		    if (!isdigit(*pos))
			break;
		    
		    precision = precision * 10 + (*pos - '0');
		}
	    }
	}

	if (*pos == 'h' || *pos == 'l' || *pos == 'L' || *pos == 'q' ||
	    *pos == 'Z')
	    pos++;

	switch(*pos)
	{
	case 'd':
	case 'i':
	case 'o':
	case 'u':
	case 'x':
	case 'X':
	case 'p':
	    maxlen += MAX(width + precision, SPRINTF_MAX_INT_WIDTH);
	    (void)va_arg(ap, int);
	    break;


	case 'e':
	case 'E':
	case 'f':
	case 'g':
	case 'G':
	    if (precision == 0)
		precision = 6;
	    
	    maxlen += MAX(width + precision,precision+SPRINTF_MAX_FLOAT_WIDTH);
	    (void)va_arg(ap, double);
	    break;
	    
	case 'c':
	    maxlen += 1;
	    (void)va_arg(ap, int);
	    break;

	case 's':
	{
	    char *s = va_arg(ap, char *);
	    maxlen += strlen(s);
	    break;
	}

	case 'n':
	    continue;
	

	default:
	    //debug(DEBUG_APPERROR, "unknown format in '%s'", fmt);
	    return -1;
	}
    }

    ap = apSave;
    va_end(apSave);

    buffer->grow(maxlen);

#if defined(SOLARIS) || defined(WIN32)
    retval = ::vsprintf(DATA, fmt, ap);
#else
    retval = vsnprintf(DATA, BUFLEN, fmt, ap);
#endif

    if (retval < 0)
    {
	//debug(DEBUG_APPERROR, "maxlen miscalculated! library bug!");
	LEN = 0;
	DATA[0] = 0;
    }
    else
	LEN = retval;

    return retval;
}

int String::readLine(std::istream *istr)
{
    int retval;
    int c;

    cow();

    LEN = 0;
    DATA[0] = 0;

    while (1)
    {
	if (istr->get(DATA + LEN, BUFLEN - LEN).eof())
	{
	    retval = 0;
	    break;
	}
	
	/* is the next character a newline? if so, consume and return
	 * otherwise, grow the buffer and try again until we do get one
	 */

	if ((c = istr->peek()) == '\n' || c == EOF)
	{
	    istr->get();
	    LEN += strlen(DATA + LEN);
	    appendChar('\n');
	    retval = 1;
	    break;
	}

	LEN = BUFLEN - 1;
	buffer->grow(BUFLEN + 1);
    }

    return retval;
}

/*
int String::write(int fd)
{
    return writen(fd, DATA, LEN);
}
*/

int String::write(std::ostream *ostr) const
{
    return ostr->write(DATA, LEN).eof();
}

int String::write(std::ostream *ostr, int start, int howMany) const
{
    return ostr->write(DATA + start, howMany).eof();
}

String &String::chop()
{
    rtrim('\n');
    rtrim('\r');
    return *this;
}

String &String::trim(const char which /* = ' ' */)
{
    ltrim(which);
    rtrim(which);
    return *this;
}

String &String::rtrim(const char which /* = ' ' */)
{
    int len = LEN;

    while (len > 0 && DATA[len - 1] == which)
	len--;

    if (len != LEN)
    {
	cow();
	LEN = len;
	DATA[LEN] = 0;
    }

    return *this;
}

String &String::ltrim(const char which  /* = ' ' */)
{
    char *p = DATA;
    char *eod = DATA + LEN;
    int len = LEN;

    while (p < eod && *p == which)
	p++, len--;
    
    /* we always have an extra NULL, make sure to copy this as well */
    if (p != DATA)
    {
	cow();
	LEN = len;
	memmove(DATA, p, LEN + 1);
    }

    return *this;
}

String &String::upper()
{
    cow();

    for (char *p = DATA; *p; p++)
	*p = toupper(*p);

    return *this;
}

String &String::lower()
{
    cow();

    for (char *p = DATA; *p; p++)
	*p = tolower(*p);

    return *this;
}

int String::find(const char which, int startPos /* = 0 */) const
{
    char *pos;

    if ((startPos < LEN) && (pos = ::strchr(DATA + startPos, which)))
	return pos - DATA;

    return -1;
}

int String::find(const char *s, int startPos /* = 0 */) const
{
    char *pos;

    if ((startPos < LEN) && (pos = ::strstr(DATA + startPos, s)))
	return pos - DATA;

    return -1;
}

int String::replace(const String &orig, const String &repl)
{
    return replaceInternal(orig.DATA, orig.LEN, repl.DATA, repl.LEN);
}

int String::replace(const String &orig, const char *repl)
{
    return replaceInternal(orig.DATA, orig.LEN, repl, strlen(repl));
}

int String::replace(const char *orig, const String &repl)
{
    return replaceInternal(orig, strlen(orig), repl.DATA, repl.LEN);
}

int String::replace(const char *orig, const char *repl)
{
    return replaceInternal(orig, strlen(orig), repl, strlen(repl));
}

int String::replaceInternal(const char * s1, int l1, const char * s2, int l2)
{
    int pos = find(s1);

    fprintf(stderr, "**********warning this may be broken!\n");

    if (pos >= 0)
    {
	cow();

	buffer->grow(LEN + (l2 - l1));

	memmove(DATA + pos + l2, 
		DATA + pos + l1,
		LEN - (pos + l1));
	LEN += l2 - l1;
	buffer->copyBytes(pos, s2, l2);
	DATA[LEN] = 0;

	return 1;
    }
    return -1;
}

String String::substr(int begin, int amt /* = -1 */) const
{
    if (amt == -1)
	amt = LEN - begin;

    if (begin < 0 || begin >= LEN)
	return "";

    if (begin + amt > LEN)
	amt = LEN - begin;

    String r(NULL, amt + 1);
    r.buffer->copyBytes(0, buffer->data + begin, amt);
    return r;
}

String String::left(int amt) const
{
    return substr(0, amt);
}

String String::right(int amt) const
{
    return substr(LEN - amt, amt);
}

int String::compareTo(const String &s) const
{
    //FIXME: will this test help in the long run?
    if (s.buffer == buffer)
	return 0;

    return compareTo(s.DATA);
}

int String::compareTo(const char *s) const
{
    return strcmp(DATA, s);
}

int String::intValue(int def) const 
{
    char *ptr;
    int retval;

    retval = strtol(DATA, &ptr, 0);
    if (ptr == DATA)
	retval = def;

    return retval;
}

/* operators and operator overloads */

String::operator const char *() const
{
    return DATA;
}

char String::operator[](int pos) const
{
    if (pos >= 0 || pos < LEN)
	return DATA[pos];

    return -1;
}

const String &String::assign( const char *s, int size )
{
  cow();
  
  LEN = 0;
  
  if(s)
      buffer->copyBytes( 0, s, size );
  else
      DATA[0] = 0;
  
  return *this;
}

const String &String::operator = (const char *s)
{
  return( assign( s, s ? strlen( s ) : 0 ) );
}

const String &String::operator = (const String &s)
{
    return assign(s);
}

const String &String::assign(const String &s)
{
    s.buffer->use();
    buffer->unuse();
    buffer = s.buffer;
    
    return *this;
}

const String &String::operator += (const char *s)
{
    append(s);
    return *this;
}

const String &String::operator += (const String &s)
{
    append(s);
    return *this;
}

String operator + (const String &s1, const String &s2)
{
    String r(NULL, s1.LEN + s2.LEN + 1);
    
    r.append(s1);
    r.append(s2);

    return r;
}

String operator + (const String &s1, const char *s2)
{
    int sl = strlen(s2);
    String r(NULL, s1.LEN + sl + 1);
    r.append(s1);
    r.append(s2, sl);
    return r;
}

String operator + (const char *s1, const String &s2)
{
    int sl = strlen(s1);
    String r(NULL, s2.LEN + sl + 1);

    r.append(s1, sl);
    r.append(s2);

    return r;
}

String operator + (const String &s1, int intValue)
{
    String numString(intValue);
    String r(NULL, s1.LEN + numString.LEN);
    r.append(s1);
    r.append(numString);
    return r;
}

String operator + (const String &s1, double doubleValue)
{
    String numString(doubleValue);
    String r(NULL, s1.LEN + numString.LEN + 1);
    r.append(s1);
    r.append(numString);
    return r;
}

int operator == (const String &s1, const String &s2)
{
    return (s1.compareTo(s2) == 0);
}

int operator == (const String &s1, const char *s2)
{
    return (s1.compareTo(s2) == 0);
}

int operator == (const char *s1, const String &s2)
{
    return (s2.compareTo(s1) == 0);
}

int operator == (const String &s1, const int s2)
{
    return (s1.compareTo((String)s2) == 0);
}

int operator == (const int s1, const String &s2)
{
    return (s2.compareTo((String)s1) == 0);
}

int operator != (const String &s1, const String &s2)
{
    return (s1.compareTo(s2) != 0);
}

int operator != (const String &s1, const char *s2)
{
    return (s1.compareTo(s2) != 0);
}

int operator != (const char *s1, const String &s2)
{
    return (s2.compareTo(s1) != 0);
}
