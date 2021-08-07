#include "HashTable.h"

#include <string.h>

#include "MemoryAllocator.h"
#include "Array.h"
#include "Math/Math.h"
#include "Logger.h"

#if 1
#define PRIME_1 117
#define PRIME_2 119
#else
#define PRIME_1 3145739
#define PRIME_2 6291469
#endif

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

static i32 g_Primes[] = {
    97, 193, 389,
    769, 1543, 3079, 6151,
    12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869,
    3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};
static i32 g_NextPrimeIndex = 0;
static i32 g_PrimesCount = ARRAY_COUNT(g_Primes);

i32
_get_prime(TableHeader* header)
{
    i32 prime = g_Primes[header->NextPrime];
    ++header->NextPrime;
    return prime;
}

void*
_table_grow(void* table, i32 elemSize, i32 isInt)
{
    TableHeader* newHeader;
    i32 capacity;
    i32 count;

    if (table == NULL)
    {
	count = 53;
	capacity = count * elemSize;

	newHeader = (TableHeader*) memory_allocate(capacity + sizeof(*newHeader));
    }
    else
    {
	TableHeader* prevHeader = table_header(table);
	count = _get_prime(prevHeader);
	capacity = count * elemSize;

	newHeader = (TableHeader*) memory_allocate(capacity + sizeof(*newHeader));
	newHeader->NextPrime = prevHeader->NextPrime;
    }

    newHeader->Count = 0;
    newHeader->Capacity = count;
    newHeader->ElementSize = elemSize;

    GWARNING("REALLOC: prevCount=%d newCapacity=%d!\n", table_count(table), newHeader->Capacity);

    if (isInt)
    {
	memset(newHeader->Buffer, -1, capacity);
    }
    else
    {
	memset(newHeader->Buffer, 0, capacity);
    }

    return newHeader->Buffer;
}

void*
_table_reserve(i32 elementsCount, i32 elementSize, i32 nextPrime)
{
    i32 capacity = elementsCount * elementSize;
    TableHeader* newHeader;
    newHeader = (TableHeader*) memory_allocate(capacity + sizeof(*newHeader));
    newHeader->Count = 0;
    newHeader->Capacity = elementsCount;
    newHeader->ElementSize = elementSize;
    newHeader->NextPrime = nextPrime;

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

void*
_shash_get(void* table, const char* key)
{
    if (table == NULL)
    {
	return NULL;
    }

    TableHeader* header = table_header(table);

    i32 i = 0;
    i32 capacity = header->Capacity;
    i32 elementSize = header->ElementSize;
    i32 length = vstring_length(key);
    i32 index = get_shash(key, capacity, i);
    InternalStringElement* elem = (InternalStringElement*) (table + index * elementSize);
    ++i;

    while (elem != NULL && elem->Key != NULL)
    {
	if (vstring_compare_length(elem->Key, key, length))
	{
	    header->Index = index;
	    return table;
	}

	++i;
	index = get_shash(elem->Key, capacity, i);
	elem = (InternalStringElement*) (table + (index) * elementSize);
    }

    header->Index = -1;

    return table;
}

/*
  Int Hash Table (int Key)
*/

force_inline i32
get_hash_old(i32 key, i32 bucketNumber)
{
    u32 ukey = (u32) key;
    ukey = ((ukey >> 16) ^ ukey) * 0x45d9f3b;
    ukey = ((ukey >> 16) ^ ukey) * 0x45d9f3b;
    ukey = (ukey >> 16) ^ ukey;
    return i32(key);
}

force_inline i32
get_hash(i32 key)
{
    u32 ukey = (u32) key;

    ukey += ~(key << 9);
    ukey ^= ((key >> 14) | (key << 18));
    ukey += (key << 4);
    ukey ^= ((key >> 10) | (key << 22));

    return i32(key);
}

void*
_base_hash_put(void* table, i32 key, i32 count, i32 capacity, i32 elementSize)
{
    vassert(table);
    vassert(capacity != 0);

    TableHeader* header = table_header(table);
    i32 i = 1;
    i32 index = get_hash(key) % capacity;
    i32 itemsKey = *((i32*)(table + index * elementSize));

    if (itemsKey == key)
    {
	goto return_label;
    }

    while (itemsKey != -1)
    {
	index = (get_hash(itemsKey) + i * get_hash(itemsKey + 1) + 1) % capacity;
	itemsKey = *((i32*)(table + index * elementSize));
	if (itemsKey == key)
	{
	    goto return_label;
	}
	++i;
    }

return_label:
    header->Index = index;
    ++header->Count;
    return table;
}

void*
_base_hash_get(void* table, i32 key, i32 count, i32 capacity, i32 elementSize)
{
    if (table == NULL)
    {
	return NULL;
    }

    i32 i = 1;
    i32 index = get_hash(key) % capacity;
    TableHeader* header = table_header(table);
    InternalIntElement* elem = (InternalIntElement*) (table + index * elementSize);

    while (elem != NULL && elem->Key != -1)
    {
	if (elem->Key == key)
	{
	    header->Index = index;
	    return table;
	}

	++i;
	index = (get_hash(elem->Key) + i * get_hash(elem->Key + 1) + 1) % capacity;
	elem = (InternalIntElement*) (table + (index) * elementSize);
    }

    header->Index = -1;

    return table;
}

void*
_base_shash_put(void* table, const char* key, i32 count, i32 capacity, i32 elementSize)
{
    vassert(table);
    vassert(capacity != 0);

    TableHeader* header = table_header(table);

    i32 i = 1;
    i32 index = -1;
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

    GERROR("Index: %d\n", index);
    header->Index = index;
    ++header->Count;

    return table;
}

void*
_base_shash_get(void* table, const char* key, i32 count, i32 capacity, i32 elementSize)
{
    if (table == NULL)
    {
	return NULL;
    }

    TableHeader* header = table_header(table);

    i32 i = 0;
    i32 length = vstring_length(key);
    i32 index = get_shash(key, capacity, i);
    InternalStringElement* elem = (InternalStringElement*) (table + index * elementSize);
    ++i;

    while (elem != NULL && elem->Key != NULL)
    {
	if (vstring_compare_length(elem->Key, key, length))
	{
	    header->Index = index;
	    return table;
	}

	++i;
	index = get_shash(elem->Key, capacity, i);
	elem = (InternalStringElement*) (table + (index) * elementSize);
    }

    header->Index = -1;

    return table;
}

/* Extended Hash Table */

i32
_extended_get_prime(ExtendedTableHeader* header)
{
    i32 prime = g_Primes[header->NextPrime];
    ++header->NextPrime;
    return prime;
}

void*
_extended_table_grow(void* table, i32 elemSize, i32 isInt)
{
    ExtendedTableHeader* newHeader;
    i32 capacity;
    i32 count;

    if (table == NULL)
    {
	count = 53;
	capacity = count * elemSize;

	newHeader = (ExtendedTableHeader*) memory_allocate(capacity + sizeof(*newHeader));
	newHeader->IntKeys = NULL;
	newHeader->StringKeys = NULL;
    }
    else
    {
	ExtendedTableHeader* prevHeader = extended_table_header(table);
	count = _extended_get_prime(prevHeader);
	capacity = count * elemSize;
	newHeader = (ExtendedTableHeader*) memory_allocate(capacity + sizeof(*newHeader));
	newHeader->NextPrime = prevHeader->NextPrime;

	GERROR("REALLOC: count=%d capacity=%d!\n", newHeader->Count, newHeader->Capacity);

	if (isInt)
	{
	    newHeader->IntKeys = array_copy(prevHeader->IntKeys);
	    newHeader->StringKeys = NULL;
	}
	else
	{
	    newHeader->IntKeys = NULL;
	    newHeader->StringKeys = array_copy(prevHeader->StringKeys);
	}
    }

    newHeader->Count = 0;
    newHeader->Capacity = count;
    newHeader->ElementSize = elemSize;

    if (isInt)
    {
	memset(newHeader->Buffer, -1, capacity);
    }
    else
    {
	memset(newHeader->Buffer, NULL, capacity);
    }

    return newHeader->Buffer;
}

void*
_extended_table_reserve(const void* table, i32 elementsCount, i32 elementSize, i32 nextPrime, i32 isInt)
{
    ExtendedTableHeader* newHeader;
    i32 capacity = elementsCount * elementSize;

    newHeader = (ExtendedTableHeader*) memory_allocate(capacity + sizeof(*newHeader));
    newHeader->Count = elementsCount;
    newHeader->Capacity = capacity;
    newHeader->ElementSize = elementSize;
    newHeader->NextPrime = nextPrime;
    newHeader->Capacity = capacity;
    newHeader->IntKeys = NULL;
    newHeader->StringKeys = NULL;

    if (table != NULL)
    {
	ExtendedTableHeader* prevHeader = extended_table_header(table);
	if (isInt)
	{
	    newHeader->IntKeys = array_copy(prevHeader->IntKeys);
	}
	else
	{
	    newHeader->StringKeys = array_copy(prevHeader->StringKeys);
	}
    }

    return newHeader->Buffer;
}
