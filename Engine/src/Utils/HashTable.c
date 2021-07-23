#include "HashTable.h"

#include <string.h>

#include "MemoryAllocator.h"
#include "Array.h"
#include "Math/Math.h"

#define PRIME_1 117
#define PRIME_2 119

typedef struct InternalStringElement
{
    const char* Key;
    void* Data;
} InternalStringElement;

typedef struct InternalIntElement
{
    i32 Key;
    void* Data;
} InternalIntElement;

force_inline i32
internal_is_prime(i32 x)
{
    if (x < 2 || (x % 2) == 0)
    {
	return 0;
    }
    else if (x == 3)
    {
	return 1;
    }

    for (i32 i = 3; i <= floor(sqrt(x)); i += 2)
    {
	if ((x % i) == 0)
	{
	    return 0;
	}
    }

    return 1;
}

i32
internal_get_prime(i32 prevPrime)
{
    prevPrime = 1.5 * prevPrime;
    while (internal_is_prime(prevPrime) != 1)
    {
	++prevPrime;
    }
    return prevPrime;
}

void*
internal_table_reserve(void* array, i32 elementsCount, i32 elementSize)
{
    i32 capacity = elementsCount * elementSize;
    TableHeader* newHeader;
    newHeader = (TableHeader*) memory_allocate(capacity + sizeof(*newHeader));
    newHeader->Count = 0;
    newHeader->Capacity = elementsCount;
    newHeader->ElementSize = elementSize;

    return newHeader->Buffer;
}

force_inline i32
shash(const char* key, i32 prime, i32 bucketNumber)
{
    vassert(bucketNumber != 0);
    i32 shash = 0;
    i32 keyLength = vstring_length(key);
    for (i32 i = 0; i < keyLength; i++)
    {
	shash += i32_pow(prime, (keyLength - (i + 1))) * key[i];
	shash %= bucketNumber;
    }

    return shash;
}

force_inline i32
get_shash(const char* key, i32 bucketNumber, i32 attempt)
{
    i32 hashA = shash(key, PRIME_1, bucketNumber);
    i32 hashB = shash(key, PRIME_2, bucketNumber);
    return (hashA + (attempt * (hashB + 1))) % bucketNumber;
}

#include "Utils/Logger.h"
void*
internal_shash_put(void* table, const char* key)
{
    vassert(table_capacity(table) != 0);

    i32 i = 1;
    i32 index = -1;
    i32 count = table_count(table);
    i32 capacity = table_capacity(table);
    i32 elementSize = table_element_size(table);
    i32 keyLength = vstring_length(key);

    const char* itemsKey = key;
    index = get_shash(itemsKey, capacity, 0);
    itemsKey = (const char*) (table + index * elementSize);

    while (*itemsKey != '\0' && vstring_compare_length(itemsKey, key, keyLength))
    {
	index = get_shash(itemsKey, capacity, i);
	itemsKey = (const char*) (table + index * elementSize);
	++i;
    }

    table_header(table)->Index = index;
    return table;
}

void*
internal_shash_get(void* table, const char* key)
{
    if (table == NULL)
    {
	return NULL;
    }

    i32 i = 0;
    i32 capacity = table_capacity(table);
    i32 elementSize = table_element_size(table);
    i32 length = vstring_length(key);
    i32 index = get_shash(key, capacity, i);
    InternalStringElement* elem = (InternalStringElement*) (table + index * elementSize);
    ++i;

    while (elem != NULL && elem->Key != NULL)
    {
	if (vstring_compare_length(elem->Key, key, length))
	{
	    table_header(table)->Index = index;
	    return table;
	}

	++i;
	index = get_shash(elem->Key, capacity, i);
	elem = (InternalStringElement*) (table + (index) * elementSize);
    }

    table_header(table)->Index = -1;

    return table;
}

/*
  Int Hash Table (int Key)
*/

force_inline i32
get_hash(i32 key, i32 bucketNumber)
{
    u32 ukey = (u32) key;
    ukey = ((ukey >> 16) ^ ukey) * 0x45d9f3b;
    ukey = ((ukey >> 16) ^ ukey) * 0x45d9f3b;
    ukey = (ukey >> 16) ^ ukey;
    key = (i32) (ukey % bucketNumber);
    return key;
}

void*
internal_hash_put(void* table, i32 key)
{
    vassert(table_capacity(table) != 0);

    i32 i = 1;
    i32 index = -1;
    i32 count = table_count(table);
    i32 capacity = table_capacity(table);
    i32 elementSize = table_element_size(table);

    index = get_hash(key, capacity);
    i32 itemsKey = *((i32*)(table + index * elementSize));
    if (itemsKey == key)
    {
	goto return_label;
    }

    while (itemsKey != -1)
    {
	index = (get_hash(itemsKey, capacity) + i * get_hash(itemsKey + 1, capacity) + 1) % capacity;
	itemsKey = *((i32*)(table + index * elementSize));
	if (itemsKey == key)
	{
	    goto return_label;
	}
	++i;
    }

return_label:
    table_header(table)->Index = index;
    return table;
}

void*
internal_hash_get(void* table, i32 key)
{
    if (table == NULL)
    {
	return NULL;
    }

    i32 i = 1;
    i32 capacity = table_capacity(table);
    i32 elementSize = table_element_size(table);
    i32 index = get_hash(key, capacity);
    InternalIntElement* elem = (InternalIntElement*) (table + index * elementSize);

    while (elem != NULL && elem->Key != -1)
    {
	if (elem->Key == key)
	{
	    table_header(table)->Index = index;
	    return table;
	}

	++i;
	index = get_hash(elem->Key, capacity);
	elem = (InternalIntElement*) (table + (index) * elementSize);
    }

    table_header(table)->Index = -1;

    return table;
}

void
shash_free(void* table)
{
}
