#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "Types.h"
#include "String.h"
#include <string.h>

#define FIRST_BUCKETS_NUMBER 53

typedef struct TableHeader
{
    //use it
    i32 ElementSize;
    i32 Count;
    i32 Capacity;
    i32 Index;
    u8 Buffer[0];
} TableHeader;

i32 internal_get_prime(i32 prevPrime);
void* internal_table_reserve(void* table, i32 minCapacity, i32 elementSize);
void* internal_table_grow(const void* table, i32 elementSize);

#define table_header(b) ((TableHeader*) (((u8*)b) - sizeof(TableHeader)))
#define table_count(b) ((b != NULL) ? table_header(b)->Count : 0)
#define table_capacity(b) ((b != NULL) ? table_header(b)->Capacity : 0)
#define table_element_size(b) ((b != NULL) ? table_header(b)->ElementSize : 0)
#define table_free(b) ((b) ? memory_free(table_header(b)) : 0)
#define table_reserve(b, elementsCount) internal_table_reserve(b, elementsCount, sizeof(*b))

/*
  Base
*/
#define base_put_helper(table, key, value)				\
    (table)[(table_header((table))->Index)].Key = (key);		\
    (table)[(table_header((table))->Index)].Value = (value);		\
    ++table_header((table))->Count

#define base_hash_put(table, key, value, hashPutDelegate, defValue, keyCheckValue) \
    ({									\
	if ((table) == NULL)						\
	{								\
	    (table) = internal_table_reserve((table), FIRST_BUCKETS_NUMBER, sizeof(*table)); \
	    vassert(table_capacity(table) == FIRST_BUCKETS_NUMBER);	\
	    memset((table), defValue, table_capacity(table) * table_element_size((table))); \
	}								\
	else if (table_count((table)) >= i32(0.7 * table_capacity((table)))) \
	{								\
	    GERROR("REALLOC: count=%d capacity=%d!\n", table_count((table)), table_capacity((table))); \
	    i32 newCapacity = internal_get_prime(table_capacity((table))); \
	    __typeof__(table) newTable = internal_table_reserve((table), newCapacity, sizeof(*table)); \
	    memset((newTable), defValue, table_capacity(newTable) * table_element_size((newTable))); \
	    for (i32 i = 0; i < table_count((table)); i++)		\
	    {								\
		if ((table)[i].Key != keyCheckValue)			\
		{							\
		    hashPutDelegate((newTable), (table)[i].Key, (table)[i].Value); \
		}							\
	    }								\
	    (table) = (newTable);					\
	}								\
									\
	hashPutDelegate((table), key, value);				\
    })

/*
  String Hash Table (const char* Key)
*/

void* internal_shash_put(void* table, const char* key);
void* internal_shash_get(void* table, const char* key);
void shash_free(void* table);

#define shash_put_helper(table, key, value)				\
    (table) = internal_shash_put((table), (key));			\
    base_put_helper(table, key, value)

#define hash_put_helper(table, key, value)				\
    (table) = internal_hash_put((table), (key));			\
    base_put_helper(table, key, value)

#define base_hash_get(table, key, hashGetDelegate)			\
    ({									\
	(table) = hashGetDelegate((table), (key));			\
	((table) != NULL &&						\
	 table_header((table))->Index != -1)				\
	    ? (table)[table_header((table))->Index].Value		\
	    : ((__typeof__(table[0].Value)) { 0 });			\
    })
#define base_hash_geti(table, key, hashGetDelegate)			\
    ({									\
	(table) = hashGetDelegate((table), (key));			\
	((table) == NULL) ? -1 : table_header((table))->Index;		\
    })

#define shash_put(table, key, value) base_hash_put((table), (key), (value), shash_put_helper, '\0', NULL)
/* TODO(bies): check if table != NULL */
//hash_get(table, key)
#define shash_get(table, key) base_hash_get((table), (key), internal_shash_get)
#define shash_geti(table, key)					\
    ({								\
	base_hash_geti((table), (key), internal_shash_get);	\
    })

/*
  Int Hash Table (int Key)
*/

void* internal_hash_put(void* table, i32 key);
void* internal_hash_get(void* table, i32 key);

#define hash_put(table, key, value) base_hash_put((table), (key), (value), hash_put_helper, -1, -1)
#define hash_get(table, key) base_hash_get((table), (key), internal_hash_get)
#define hash_geti(table, key)					\
    ({								\
	base_hash_geti((table), (key), internal_hash_get);	\
    })

#endif
