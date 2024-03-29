#include "UTest_HashTable.h"

#include "UTests/Test.h"
#define HASH_TABLE_PROFILING 1
#include <Utils/SimpleStandardLibrary.h>

/*
  String Hash Table (const char* Key)
*/

typedef struct TypeA
{
    const char* Key;
    const char* Value;
} TypeA;

typedef struct TypeB
{
    const char* Key;
    TypeA Value;
} TypeB;

typedef struct TypeC
{
    const char* Key;
    i32 Value;
} TypeC;

#define TypeA(key, value) ((TypeA) { .Key = key, .Value = value })

void
shash_put_and_get_for_strings()
{
    TypeA* table = NULL;
    shash_put(table, "Key0", "Val0");
    shash_put(table, "Key1", "Val1");
    shash_put(table, "Key2", "Val2");
    shash_put(table, "Key3", "Val3");
    shash_put(table, "Key4", "Val4");
    shash_put(table, "Key5", "Val5");
    shash_put(table, "Key6", "Val6");

    const char* val0 = shash_get(table, "Key0");
    const char* val1 = shash_get(table, "Key1");
    const char* val2 = shash_get(table, "Key2");
    const char* val3 = shash_get(table, "Key3");
    const char* val4 = shash_get(table, "Key4");
    const char* val5 = shash_get(table, "Key5");
    const char* val6 = shash_get(table, "Key6");

    Condition(val0 != NULL);
    Condition(val1 != NULL);
    Condition(val2 != NULL);
    Condition(val3 != NULL);
    Condition(val4 != NULL);
    Condition(val5 != NULL);
    Condition(val6 != NULL);

    return;

    String_Equal(val0, "Val0");
    String_Equal(val1, "Val1");
    String_Equal(val2, "Val2");
    String_Equal(val3, "Val3");
    String_Equal(val4, "Val4");
    String_Equal(val5, "Val5");
    String_Equal(val6, "Val6");
}

void
shash_put_and_get_for_value_struct()
{
    TypeB* table = NULL;
    shash_put(table, "Key0", TypeA("Key0", "Val0"));
    shash_put(table, "Key1", TypeA("Key1", "Val1"));
    shash_put(table, "Key2", TypeA("Key2", "Val2"));
    shash_put(table, "Key3", TypeA("Key3", "Val3"));

    TypeA val0 = shash_get(table, "Key0");
    TypeA val1 = shash_get(table, "Key1");
    TypeA val2 = shash_get(table, "Key2");
    TypeA val3 = shash_get(table, "Key3");

    return;

    String_Equal(val0.Key  , "Key0");
    String_Equal(val0.Value, "Val0");
    String_Equal(val1.Key  , "Key1");
    String_Equal(val1.Value, "Val1");
    String_Equal(val2.Key  , "Key2");
    String_Equal(val2.Value, "Val2");
    String_Equal(val3.Key  , "Key3");
    String_Equal(val3.Value, "Val3");
}

void
shash_put_and_get_for_base_type_value_struct()
{
    TypeC* table = NULL;
    shash_put(table, "Key0", 0);
    shash_put(table, "Key1", 2);
    shash_put(table, "Key2", 3);
    shash_put(table, "Key3", 4);

    i32 val0 = shash_get(table, "Key0");
    i32 val1 = shash_get(table, "Key1");
    i32 val2 = shash_get(table, "Key2");
    i32 val3 = shash_get(table, "Key3");

    Condition(val0 == 0);
    Condition(val1 == 2);
    Condition(val2 == 3);
    Condition(val3 == 4);
}

void
shash_realloc_test()
{
    i32 count = 52;
    TypeC* table = NULL;
    char str[10];
    memset(str, '\0', 10 * sizeof(char));
    for (i32 i = 0; i < count; i++)
    {
	sprintf(str, "%s%d", "Key", i);
	shash_put(table, str, i);
    }

    Int_Value(table_next_prime(table));
    Condition(table_capacity(table) == 97);
    Int_Value(table_count(table));
    Int_Value(table_capacity(table));

    for (i32 i = 0; i < count; i++)
    {
	sprintf(str, "%s%d", "Key", i);
	i32 value = shash_get(table, str);
	Int_Value(value);
    }
}

void
shash_same_key_test()
{
    TypeA* table = NULL;
    shash_put(table, "Key0", "Val0");
    shash_put(table, "Key0", "Val1");

    const char* val0 = shash_get(table, "Key0");
    return;
    String_Equal(val0, "Val1");
    String_Value(val0);
}

/*
  Int Hash Table (int Key)
*/

typedef struct TypeI
{
    i32 Key;
    const char* Value;
} TypeI;

typedef struct TypeDHASH_TABLE_PROFILING
{
    i32 Key;
    TypeI Value;
} TypeD;

#define TypeI(key, value) ((TypeI) { .Key = key, .Value = value })

void
hash_put_and_get_test()
{
    TypeI* table = NULL;
    hash_put(table, 0, "Val0");
    hash_put(table, 1, "Val1");
    hash_put(table, 2, "Val2");
    hash_put(table, 3, "Val3");
    hash_put(table, 4, "Val4");
    hash_put(table, 5, "Val5");
    hash_put(table, 6, "Val6");

    const char* val0 = hash_get(table, 0);
    const char* val1 = hash_get(table, 1);
    const char* val2 = hash_get(table, 2);
    const char* val3 = hash_get(table, 3);
    const char* val4 = hash_get(table, 4);
    const char* val5 = hash_get(table, 5);
    const char* val6 = hash_get(table, 6);

    Condition(val0 != NULL);
    Condition(val1 != NULL);
    Condition(val2 != NULL);
    Condition(val3 != NULL);
    Condition(val4 != NULL);
    Condition(val5 != NULL);
    Condition(val6 != NULL);

    String_Equal(val0, "Val0");
    String_Equal(val1, "Val1");
    String_Equal(val2, "Val2");
    String_Equal(val3, "Val3");
    String_Equal(val4, "Val4");
    String_Equal(val5, "Val5");
    String_Equal(val6, "Val6");
}

void
hash_put_and_get_for_value_struct_test()
{
    TypeD* table = NULL;
    hash_put(table, 0, TypeI(0, "Val0"));
    hash_put(table, 1, TypeI(1, "Val1"));
    hash_put(table, 2, TypeI(2, "Val2"));
    hash_put(table, 3, TypeI(3, "Val3"));
    hash_put(table, 4, TypeI(4, "Val4"));
    hash_put(table, 5, TypeI(5, "Val5"));
    hash_put(table, 6, TypeI(6, "Val6"));

    TypeI val0 = hash_get(table, 0);
    TypeI val1 = hash_get(table, 1);
    TypeI val2 = hash_get(table, 2);
    TypeI val3 = hash_get(table, 3);
    TypeI val4 = hash_get(table, 4);
    TypeI val5 = hash_get(table, 5);
    TypeI val6 = hash_get(table, 6);

    Condition(val0.Key == 0);
    Condition(val1.Key == 1);
    Condition(val2.Key == 2);
    Condition(val3.Key == 3);
    Condition(val4.Key == 4);
    Condition(val5.Key == 5);
    Condition(val6.Key == 6);

    String_Equal(val0.Value, "Val0");
    String_Equal(val1.Value, "Val1");
    String_Equal(val2.Value, "Val2");
    String_Equal(val3.Value, "Val3");
    String_Equal(val4.Value, "Val4");
    String_Equal(val5.Value, "Val5");
    String_Equal(val6.Value, "Val6");
}

void
hash_realloc_test()
{
    TypeI* table = NULL;
    char str[10];
    memset(str, '\0', 10 * sizeof(char));
    for (i32 i = 0; i < 100; i++)
    {
	if (i == 38)
	{
	    i32 j = 12345;
	}
	sprintf(str, "%s%d", "Val", i);
	hash_put(table, i, str);
    }

    Condition(table_capacity(table) == 193);
    Int_Value(table_count(table));
    Int_Value(table_capacity(table));
}

void
hash_same_key_test()
{
    TypeI* table = NULL;
    hash_put(table, 1, "Val0");
    hash_put(table, 1, "Val1");

    String_Equal(hash_get(table, 1), "Val1");
}

typedef struct TypeInt
{
    i32 Key;
    i32 Value;
} TypeInt;


/* We need smth like this for shash */
void
hash_crash_test()
{
    i32 count = 200000;
    TypeInt* table = NULL;

    TimeState globalState;
    profiler_start(&globalState);
    for (i32 i = 0; i < count; i++)
    {
	//TimeState localState;
	//profiler_start(&localState);
	hash_put(table, i, i);
	//profiler_end(&localState);
	//char* localTime = profiler_get_string_as_float(&localState);
	//I32_Value(i);
	//I32_Value(table_get_statistics().PutAttempt);
	//String_Value(localTime);
    }
    profiler_end(&globalState);
    char* globalTime = profiler_get_string_as_float(&globalState);
    String_Value(globalTime);

    Condition(hash_get(table, 17) == 17);

    for (i32 i = (count - 1); i >= (count*0.99); --i)
    {
	I32_Value(hash_get(table, i));
    }
}

void
hash_test()
{
    TEST(shash_put_and_get_for_strings());
    TEST(shash_put_and_get_for_value_struct());
    TEST(shash_put_and_get_for_base_type_value_struct());
    TEST(shash_realloc_test());
    TEST(shash_same_key_test());

    TEST(hash_put_and_get_test());
    TEST(hash_put_and_get_for_value_struct_test());
    TEST(hash_realloc_test());
    TEST(hash_same_key_test());

    TEST(hash_crash_test());

    //TEST(extended_hash_test());
}

void
extended_hash_test()
{
#if 0
    TypeI* table = NULL;
    extended_hash_put(table, 1, "Val0");
    extended_hash_put(table, 1, "Val1");
    extended_hash_put(table, 2, "Val2");
    extended_hash_put(table, 3, "Val3");
    extended_hash_put(table, 4, "Val4");

    const char* val0 = extended_hash_get(table, 1);
    vassert(val0);
    String_Equal(val0, "Val1");

    i32* keys = extended_hash_get_keys(table);

    i32 count = array_count(keys);
    for (i32 i = 0; i < count; i++)
    {
	Int_Value(keys[i]);
    }
#endif
}
