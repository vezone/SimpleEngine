#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "Types.h"
#include "String.h"
#include <string.h>

void* _base_hash_put(void* table, i32 key, i32 count, i32 capacity, i32 elementSize);
void* _base_hash_get(void* table, i32 key, i32 count, i32 capacity, i32 elementSize);
void* _base_shash_put(void* table, const char* key, i32 count, i32 capacity, i32 elementSize);
void* _base_shash_get(void* table, const char* key, i32 count, i32 capacity, i32 elementSize);

typedef struct TableHeader
{
    i32 ElementSize;
    i32 Count;
    i32 Capacity;
    i32 Index;
    i32 NextPrime;
    u8 Buffer[0];
} TableHeader;

typedef struct ExtendedTableHeader
{
    i32 ElementSize;
    i32 Count;
    i32 Capacity;
    i32 Index;
    i32 NextPrime;
    union
    {
	char** StringKeys;
	i32* IntKeys;
    };
    u8 Buffer[0];
} ExtendedTableHeader;

i32 _get_prime(TableHeader* header);
void* _table_reserve(i32 minCapacity, i32 elementSize, i32 nextPrime);
void* _table_grow(void* table, i32 elemSize, i32 isInt);

#define table_header(b) ((TableHeader*) (((u8*)b) - sizeof(TableHeader)))
#define table_count(b) ((b != NULL) ? table_header(b)->Count : 0)
#define table_capacity(b) ((b != NULL) ? table_header(b)->Capacity : 0)
#define table_element_size(b) ((b != NULL) ? table_header(b)->ElementSize : 0)
#define table_next_prime(b) ((b != NULL) ? table_header(b)->NextPrime : 0)
#define table_free(b) ((b) ? memory_free(table_header(b)) : 0)
#define table_reserve(b, elementsCount) _table_reserve(elementsCount, sizeof(*b))

#define shash_free(table) table_free((table))
/*
  Base
*/

force_inline i32
i32_comparer(i32 key)
{
    return key != -1;
}

force_inline i32
string_comparer(const char* key)
{
    return key != NULL;
}

#define base_hash_put(table, key, value, hashPutDelegate, tableHeader, HeaderType, tableGrow, tableFree, defValComparer, isInt) \
    ({									\
	HeaderType* header = tableHeader((table));			\
									\
	if ((table) == NULL)						\
	{								\
	    (table) = tableGrow((table), sizeof(*(table)), isInt);	\
	}								\
	else if (header->Count >= i32(0.7 * header->Capacity))		\
	{								\
	    i32 i;							\
	    __typeof__((table)) newTable = tableGrow((table), sizeof(*(table)), isInt); \
	    HeaderType* newHeader = tableHeader((newTable));		\
	    for (i = 0; i < header->Capacity; i++)			\
	    {								\
		if (defValComparer((table)[i].Key))			\
		{							\
		    (newTable) = hashPutDelegate((newTable), (table)[i].Key, newHeader->Count, newHeader->Capacity, newHeader->ElementSize); \
		    (newTable)[(tableHeader((newTable))->Index)].Key = ((table)[i].Key); \
		    (newTable)[(tableHeader((newTable))->Index)].Value = ((table)[i].Value); \
		}							\
	    }								\
									\
	    tableFree(table);						\
									\
	    (table) = (newTable);					\
	}								\
									\
	header = tableHeader((table));					\
	(table) = hashPutDelegate((table), (key), header->Count, header->Capacity, header->ElementSize); \
	(table)[(header->Index)].Key = (key);				\
	(table)[(header->Index)].Value = (value);			\
    })

#define base_hash_get(table, key, hashGetDelegate, tableHeader)		\
    ({								\
	(table) = hashGetDelegate((table), (key), table_count((table)), table_capacity((table)), table_element_size((table)));		\
	((table) != NULL &&					\
	 tableHeader((table))->Index != -1)			\
	    ? (table)[tableHeader((table))->Index].Value	\
	    : ((__typeof__(table[0].Value)) { 0 });		\
    })

/*
  String Hash Table (const char* Key)
*/
// base_hash_put(table, key, value, hashPutDelegate, tableHeader, HeaderType, tableGrow, tableFree, defValComparer, isInt)

#define shash_put(table, key, value)					\
    base_hash_put((table), (key), (value), _base_shash_put, table_header, TableHeader, _table_grow, table_free, string_comparer, 0)
#define shash_get(table, key)						\
    base_hash_get((table), (key), _base_shash_get, table_header)
#define shash_geti(table, key)					\
    ({								\
	(table) = _base_shash_get((table), (key), table_count((table)), table_capacity((table)), table_element_size((table))); \
	((table) == NULL) ? -1 : table_header((table))->Index;	\
    })

/*
  Int Hash Table (int Key)
*/

#define hash_put(table, key, value)					\
    base_hash_put((table), (key), (value), _base_hash_put, table_header, TableHeader, _table_grow, table_free, i32_comparer, 1)
#define hash_get(table, key)					\
    base_hash_get((table), (key), _base_hash_get, table_header)
#define hash_geti(table, key)						\
    ({									\
	(table) = _base_hash_get((table), (key), table_count((table)), table_capacity((table)), table_element_size((table))); \
	((table) == NULL) ? -1 : table_header((table))->Index;		\
    })

#define extended_table_header(b) ((ExtendedTableHeader*) (((u8*)b) - sizeof(ExtendedTableHeader)))
#define extended_table_count(b) ((b != NULL) ? extended_table_header(b)->Count : 0)
#define extended_table_capacity(b) ((b != NULL) ? extended_table_header(b)->Capacity : 0)
#define extended_table_element_size(b) ((b != NULL) ? extended_table_header(b)->ElementSize : 0)
#define extended_table_next_prime(b) ((b != NULL) ? extended_table_header(b)->NextPrime : 0)
#define extended_table_free(b) ((b) ? memory_free(extended_table_header(b)) : 0)
// TODO: complete this
#define extended_table_reserve(b, elementsCount) _extended_table_reserve(b, elementsCount, sizeof(*b), _extended_get_nearest_prime(elementCount), 1)

void* _extended_table_grow(void* table, i32 elemSize, i32 isInt);
i32 _extended_get_prime(ExtendedTableHeader* header);
void* _extended_table_reserve(const void* table, i32 elementsCount, i32 elementSize, i32 nextPrime, i32 isInt);

#define extended_hash_get_keys(table)		\
    ({						\
	extended_table_header(table)->IntKeys;	\
    })
#define extended_hash_put(table, key, value)				\
    ({									\
	ExtendedTableHeader* header = extended_table_header((table));	\
	base_hash_put((table), (key), (value), _base_hash_put, extended_table_header, ExtendedTableHeader, _extended_table_grow, extended_table_free, i32_comparer, 1); \
	i32 notExist = 1;						\
	i32 arrCount = array_count(header->IntKeys);			\
	for (i32 i = 0; i < arrCount; i++)				\
	{								\
	    if (header->IntKeys[i]==key)				\
	    {								\
		notExist = 0;						\
		break;							\
	    }								\
	}								\
									\
	if (notExist)							\
	    array_push(header->IntKeys, key);				\
    })
#define extended_hash_get(table, key)				\
    base_hash_get((table), (key), _base_hash_get, extended_table_header)
#define extended_hash_geti(table, key)					\
    ({									\
	(table) = _base_hash_get((table), (key));			\
	((table) == NULL) ? -1 : extended_table_header((table))->Index;	\
    })

#define extended_shash_get_keys(table)			\
    ({							\
	extended_table_header(table)->StringKeys;	\
    })
#define extended_shash_put(table, key, value)				\
    ({									\
	ExtendedTableHeader* header = extended_table_header((table));	\
	base_hash_put((table), (key), (value), _base_shash_put, extended_table_header, ExtendedTableHeader, _extended_table_grow, extended_table_free, string_comparer, 0); \
									\
	i32 i = 0;							\
	i32 notExist = 1;						\
	i32 arrCount = array_count(header->IntKeys);			\
									\
	for (i = 0; i < arrCount; i++)					\
	{								\
	    if (vstring_compare(header->StringKeys[i], key))		\
	    {								\
		notExist = 0;						\
		break;							\
	    }								\
	}								\
									\
	if (notExist)							\
	    array_push(header->StringKeys, key);			\
    })
#define extended_shash_get(table, key)					\
    base_hash_get((table), (key), _base_shash_get, extended_table_header)
#define extended_hash_geti(table, key)					\
    ({									\
	(table) = _base_hash_get((table), (key));			\
	((table) == NULL) ? -1 : extended_table_header((table))->Index;	\
    })

#endif
