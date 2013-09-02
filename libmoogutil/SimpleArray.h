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
#ifndef _SIMPLEARRAY_H
#define _SIMPLEARRAY_H

#include <string.h>
#include <stdio.h>
#include "fastmemcpy.h"
#define ASSERT_LOC(x)/* {\
   if (loc >= size || loc < 0)\
   {\
       fprintf(stderr, "SimpleArray loc out of bounds %d\n", loc);\
       return x;\
   }\
}\*/
   

template <class T> class SimpleArray
{
    int size;
    int capacity;
    T **elements;
    
    void grow(int);
 public:
    SimpleArray(int initialCapacity = 16);
    ~SimpleArray();
    
    int appendElement(T *);
    T *removeElement(T *);
    T *removeElement(int);
    int insertElement(int, T *);
    int getSize() {return size;}
    T& operator[](int);
    void clear();
#ifdef DEBUG
    void print();
#endif
};

template <class T> SimpleArray<T>::SimpleArray(int initialCapacity)
{
    size = 0; 
    capacity = initialCapacity;
    elements = new T*[capacity];
}

template <class T> SimpleArray<T>::~SimpleArray()
{
    for (int i = 0; i < size; i++)
    {
	delete elements[i];
    }
    delete elements;
}

template <class T> void SimpleArray<T>::grow(int amount)
{
    T **newData = new T*[capacity + amount];
    memcpy(newData, elements, sizeof(T*) * capacity);
    delete elements;
    elements = newData;
    capacity += amount;
}

template <class T> int SimpleArray<T>::appendElement(T *elem)
{
    if (size == capacity)
	grow(capacity);
    elements[size] = elem;
    return size++;
}

template <class T> int SimpleArray<T>::insertElement(int loc, T *elem)
{
    ASSERT_LOC(-1);
    if (size == capacity)
	grow(capacity);
    memmove(elements + loc + 1, elements + loc, (size - loc) * sizeof(T*));
    elements[loc] = elem;
    size++;
    return loc;
}

template <class T> T *SimpleArray<T>::removeElement(T *elem)
{
    T* retval = NULL;
    for (int i = 0; i < size; i++)
    {
	if (elements[i] == elem)
	{
	    retval = removeElement(i);
	    break;
	}
    }
    return retval;
}

template <class T> T *SimpleArray<T>::removeElement(int loc)
{
    ASSERT_LOC(NULL);
    T* retval;
    if (loc == size - 1)
    {
	retval =  elements[size];
    }
    else
    {
	retval = elements[loc];
	memmove(elements + loc, elements + loc + 1, (size-loc-1) * sizeof(T*));
    }
    size--;
    return retval;
}

template <class T> T& SimpleArray<T>::operator[](int loc)
{
    ASSERT_LOC(*((T*)NULL));
    return *elements[loc];
}


template <class T> void SimpleArray<T>::clear()
{
    for (int i = 0; i < size; i++)
    {
        delete elements[i];
    }
    size = 0;
}

#ifdef DEBUG
template <class T> void SimpleArray<T>::print()
{
    for (int i = 0; i < size; i++)
	elements[i]->print();
}
#endif

#endif /* _SIMPLEARRAY_H */
