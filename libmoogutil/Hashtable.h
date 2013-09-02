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
 * Template class to implement a c++ hashtable similar to java.util.Hashtable.
 *
 * @author Karl LaRocca
 * @created Tue Sep 29 12:13:53 1998
 * @version $Revision: 1.5.4.1 $$Date: 2000/02/24 00:44:09 $
 */
#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <string.h>    /* for memset */
#include <sys/types.h> /* for size_t */
#include "ctltypes.h"
#include "fastmemcpy.h"

#define HASH_CONST 37

typedef enum
{
  KT_CHAR = 0,
  KT_INT
} Keytype;


template <class T> class Hashtable;
template <class T> class HashtableEntry;

/******************************************************************************
 * HashtableIterator CLASS DECLARATION
 *****************************************************************************/
template <class T>
class HashtableIterator
{
 protected:
  size_t              capacity;
  HashtableEntry<T>** table;
  int                 idx;
  HashtableEntry<T>*  current;
  
  HashtableEntry<T>*  next();

 public:
                      HashtableIterator( HashtableEntry<T>** _table, 
					 size_t capacity );
  virtual            ~HashtableIterator(){}
  void                reset();
  int                 hasMoreElements();
};

template <class T>
class HashtableKeyIterator : public HashtableIterator<T>
{
 public:
                      HashtableKeyIterator( HashtableEntry<T>** _table, 
					    size_t _capacity ) : 
			HashtableIterator<T>( _table, _capacity ){}
  void*               getNextKey();
};

template <class T>
class HashtableValueIterator : public HashtableIterator<T>
{
 public:
                      HashtableValueIterator( HashtableEntry<T>** _table, 
					      size_t _capacity ) : 
			HashtableIterator<T>( _table, _capacity ){}
  T*                  getNextValue();
};

/******************************************************************************
 * HashtableEntry CLASS DECLARATION
 *****************************************************************************/
template <class T>
class HashtableEntry
{
 private:
  friend class Hashtable<T>;  
  friend class HashtableIterator<T>;
  friend class HashtableKeyIterator<T>;
  friend class HashtableValueIterator<T>;

  Keytype keytype;
  void* key;

  T*                 value;
  HashtableEntry<T>* next;

                     HashtableEntry( Keytype keytype, const void* key, 
				     T* _value, HashtableEntry<T>* _next );
		     ~HashtableEntry();
  int                matches( Keytype keytype, const void* key );
};

/******************************************************************************
 * Hashtable CLASS DECLARATION
 *****************************************************************************/
template <class T>
class Hashtable 
{
 private:
  int                        count;
  size_t                     capacity;
  HashtableEntry<T>**        table;
  unsigned int               getHash( const char* c_key );
  unsigned int               getHash( int         i_key );
  float                      loadFactor;
  int                        threshold;
  DeleteFlag                 deleteFlag;
  T*                         put( unsigned int hash, Keytype keytype, 
				  const void* key, T* value );
  T*                         get( unsigned int hash, Keytype keytype, 
				  const void* key );
  T*                         remove( unsigned int hash, Keytype keytype, 
				     const void* key );

 public:
                             Hashtable( size_t _capacity = 101, 
					float loadFactor = .75 );
                            ~Hashtable();
  T*                         put( const char* c_key, T* value );
  T*                         put( int         i_key, T* value );
  T*                         get( const char* c_key );
  T*                         get( int         i_key );
  T*                         remove( const char* c_key );
  T*                         remove( int         i_key );
  void                       rehash( size_t capacity );
  int                        getCount();
  size_t                     getCapacity();
  int                        isEmpty();
  void                       clear();
  int                        contains( T* value );
  int                        containsKey( const char* c_key );
  int                        containsKey( int         i_key );
  void                       setDeleteFlag( DeleteFlag flag );

  HashtableKeyIterator<T>*   getKeys();
  HashtableValueIterator<T>* getValues();
};

/******************************************************************************
 * HashtableEntry FUNCTION DEFINITIONS
 *****************************************************************************/
template <class T>
HashtableEntry<T>::HashtableEntry( Keytype _keytype, 
				   const void* _key, 
				   T* _value, 
				   HashtableEntry<T>* _next )
{
  keytype = _keytype;
  switch( keytype )
  {
    case KT_CHAR:
      key = ( void * )strdup( ( char* )_key );
      break;

    case KT_INT:
      key = new int();
      memcpy( key, _key, sizeof( int ) );
      break;

    default:
      key = NULL;
  }

  value = _value;
  next  = _next;
}

template <class T>
HashtableEntry<T>::~HashtableEntry()
{
  delete( key );
}

template <class T>
int HashtableEntry<T>::matches( Keytype _keytype, const void* _key )
{
  if ( keytype == _keytype &&
       ( ( keytype == KT_CHAR && strcmp( (char *)key, (char*)_key ) == 0 ) ||
	 ( keytype == KT_INT && *( int * )( key ) == *( int * )_key ) ) )
  {
    return( 1 );
  }

  return( 0 );
}
/******************************************************************************
 * HashtableIterator FUNCTION DEFINITIONS
 *****************************************************************************/
template <class T>
HashtableIterator<T>::HashtableIterator( HashtableEntry<T>** _table, 
					 size_t _capacity )
{
  table    = _table;
  capacity = _capacity;
  reset();
}

template <class T>
void HashtableIterator<T>::reset()
{
  idx     = capacity;
  current = NULL;
}

template <class T>
int HashtableIterator<T>::hasMoreElements()
{
  if ( current != NULL )
  {
    return( 1 );
  }

  while( idx-- > 0 )
  {
    if ( ( current = table[ idx ] ) != NULL )
    {
      return( 1 );
    }
  }

  return( 0 );
}

template <class T>
inline HashtableEntry<T>* HashtableIterator<T>::next()
{
  while( current == NULL && idx-- > 0 )
  {
    current = table[ idx ];
  }

  HashtableEntry<T>* tmp = current;

  if ( current != NULL )
  {
    current = current->next;
  }

  return( tmp );
}

template <class T>
void* HashtableKeyIterator<T>::getNextKey()
{
  HashtableEntry<T>* tmp = HashtableIterator<T>::next();
  return( tmp ? tmp->key : NULL );
}

template <class T>
T* HashtableValueIterator<T>::getNextValue()
{
  HashtableEntry<T>* tmp = HashtableIterator<T>::next();
  return( tmp ? tmp->value : NULL );
}

/******************************************************************************
 * Hashtable FUNCTION DEFINITIONS
 *****************************************************************************/
template <class T> 
Hashtable<T>::Hashtable( size_t _capacity, float _loadFactor )
{
  count      = 0;
  capacity   = _capacity;
  loadFactor = _loadFactor;
  threshold  = ( int )( capacity * loadFactor );
  table      = new HashtableEntry<T>*[ capacity ];
  memset( table, 0, sizeof( HashtableEntry<T>* ) * capacity );
  deleteFlag = DF_CONTAINER;
}

template <class T> 
Hashtable<T>::~Hashtable()
{
  clear();
  delete( table );
}

template <class T> 
inline unsigned int Hashtable<T>::getHash( const char* c_key )
{
  register unsigned int hash = 0;

  while( *c_key )
  {
    hash = ( hash * HASH_CONST + *c_key++ ) % capacity; 
  }

  return( hash );
}

template <class T> 
inline unsigned int Hashtable<T>::getHash( int i_key )
{
  return( ( unsigned int )( i_key % capacity ) );
}

template <class T> 
inline T* Hashtable<T>::put( unsigned int hash, Keytype keytype, 
			       const void* key, T* value )
{
  HashtableEntry<T>* entry;

  for( entry = table[ hash ]; entry; entry = entry->next )
  {
    if ( entry->matches( keytype, key ) )
    {
      T* retval = entry->value;

      if ( deleteFlag == DF_CONTAINER )
      {
	delete( retval );
	retval = NULL;
      }

      entry->value = value;
      return( retval );
    }
  }

  // This is a new entry
  entry = new HashtableEntry<T>( keytype, key, value, table[ hash ] );
  table[ hash ] = entry;
  count++;

  if ( count >= threshold )
  {
    rehash( capacity * 2 + 1 );
  }

  return( NULL );
}

template <class T> 
T* Hashtable<T>::put( const char* c_key, T* value )
{
  return( put( getHash( c_key ), KT_CHAR, c_key, value ) );
}

template <class T> 
T* Hashtable<T>::put( int i_key, T* value )
{
  return( put( getHash( i_key ), KT_INT, &i_key, value ) );
}

template <class T> 
inline T* 
Hashtable<T>::get( unsigned int hash, Keytype keytype, const void* key )
{
  HashtableEntry<T>* entry = table[ hash ];

  while( entry )
  {
    if ( entry->matches( keytype, key ) )
    {
      return( entry->value );
    }

    entry = entry->next;
  }

  return( NULL );
}

template <class T> 
T* 
Hashtable<T>::get( const char* c_key )
{
  return( get( getHash( c_key ), KT_CHAR, ( void * )c_key ) );
}

template <class T> 
T* Hashtable<T>::get( int i_key )
{
  return( get( getHash( i_key ), KT_INT, ( void * )&i_key ) );
}

template <class T> 
inline T*
Hashtable<T>::remove( unsigned int hash, Keytype keytype, const void* key )
{
  T* retval = NULL;
  HashtableEntry<T>* entry = table[ hash ];
  HashtableEntry<T>* prev  = NULL;

  while( entry )
  {
    if ( entry->matches( keytype, key ) )
    {
      if ( prev == NULL )
      {
	table[ hash ] = entry->next;
      }

      else
      {
	prev->next = entry->next;
      }

      retval = entry->value;

      if ( deleteFlag == DF_CONTAINER )
      {
	delete( retval );
	retval = NULL;
      }

      delete( entry );

      count--;

      break;
    }

    prev  = entry;
    entry = entry->next;
  }

  return( retval ); // FIXME: no way to know if remove worked if DF_CONTAINER
}

template <class T> 
T* Hashtable<T>::remove( const char* c_key )
{
  return( remove( getHash( c_key ), KT_CHAR, ( void * )c_key ) );
}

template <class T> 
T* Hashtable<T>::remove( int i_key )
{
  return( remove( getHash( i_key ), KT_INT, ( void * )&i_key ) );
}

template <class T> 
void Hashtable<T>::clear()
{
  HashtableEntry<T>* entry;
  HashtableEntry<T>* tmp;

  for( int i = capacity; i-- > 0; )
  {
    entry = table[ i ];
    
    while( entry )
    {
      tmp = entry->next;

      if ( deleteFlag == DF_CONTAINER )
      {
	delete( entry->value );
      }

      delete( entry );
      entry = tmp;
    }
  }

  memset( table, 0, sizeof( HashtableEntry<T>* ) * capacity );
}

template <class T> 
void Hashtable<T>::rehash( size_t _capacity )
{
  if ( _capacity == 0 )
  {
    return;
  }

  int oldCapacity = capacity;
  capacity = _capacity;
  threshold = ( int )( capacity * loadFactor );

  HashtableEntry<T>** newTable = new HashtableEntry<T>*[ capacity ];
  memset( newTable, 0, sizeof( HashtableEntry<T>* ) * capacity );

  for ( int i = oldCapacity ; i-- > 0 ; ) 
  {
    HashtableEntry<T>* entry = table[ i ];
    while( entry )
    {
      HashtableEntry<T>* tmp = entry;
      entry = entry->next;

      int hash;

      if ( tmp->keytype == KT_CHAR )
      {
	hash = getHash( ( char * )( tmp->key ) );
      }

      else
      {
	hash = getHash( *( int * )( tmp->key ) );
      }

      tmp->next = newTable[ hash ];
      newTable[ hash ] = tmp;
    }
  }

  delete( table );
  table = newTable;
}

template <class T> 
int Hashtable<T>::getCount()
{
  return( count );
}

template <class T> 
size_t Hashtable<T>::getCapacity()
{
  return( capacity );
}

template <class T> 
int Hashtable<T>::isEmpty()
{
  return( ( count == 0 ) ? 1 : 0 );
}

template <class T> 
HashtableKeyIterator<T>* Hashtable<T>::getKeys()
{
  return( new HashtableKeyIterator<T>( table, capacity ) );
}

template <class T> 
HashtableValueIterator<T>* Hashtable<T>::getValues()
{
  return( new HashtableValueIterator<T>( table, capacity ) );
}

template <class T> 
int Hashtable<T>::contains( T* value )
{
  int retval = 0;

  HashtableIterator<T>* i = getElements();

  while( i->hasMoreElements() )
  {
    if ( i->next()->value == value )
    {
      retval = 1;
      break;
    }
  }

  delete( i );

  return( retval );
}

template <class T> 
int Hashtable<T>::containsKey( const char* c_key )
{
  return( get( c_key ) == NULL ? 0 : 1 );
}

template <class T> 
int Hashtable<T>::containsKey( int i_key )
{
  return( get( i_key ) == NULL ? 0 : 1 );
}

/**
 * Does this container delete values for us?
 */
template <class T> 
void Hashtable<T>::setDeleteFlag( DeleteFlag _deleteFlag )
{
  deleteFlag = _deleteFlag;
}

#endif /* _HASHTABLE_H */
