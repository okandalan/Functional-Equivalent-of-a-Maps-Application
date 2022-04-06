#include "HashTable.h"

#include <cassert>
#include <iostream>
#include <iomanip>

const int KeyedHashTable::PRIME_LIST[PRIME_TABLE_COUNT] =
{
     2,    3,   5,   7,  11,  13,  17,  19,
     23,  29,  31,  37,  41,  43,  47,  53,
     59,  61,  67,  71,  73,  79,  83,  89,
     97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223,
    227, 229, 233, 239, 241, 251, 257, 263,
    269, 271, 277, 281, 283, 293, 307, 311,
    313, 317, 331, 337, 347, 349, 353, 359,
    367, 373, 379, 383, 389, 397, 401, 409,
    419, 421, 431, 433, 439, 443, 449, 457,
    461, 463, 467, 479, 487, 491, 499, 503,
    509, 521, 523, 541
};

int KeyedHashTable::Hash(const std::string& key) const
{
    int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash += key[i] * PRIME_LIST[i];
    }

    return hash % tableSize;
}

void KeyedHashTable::ReHash()
{
     
    int oldSize = tableSize;
    tableSize = FindNearestLargerPrime(tableSize * 2);
    
    HashData* tmp_table = table;
    table = new HashData[tableSize];
    for (int i = 0; i < oldSize; i++) {
        if (tmp_table[i].key != "") {
            int index = Hash(tmp_table[i].key);
            table[index] = tmp_table[i];
        }
    }
    delete [] tmp_table;
    
}

int KeyedHashTable::FindNearestLargerPrime(int requestedCapacity)
{
    for (int i = 0; i < PRIME_TABLE_COUNT; i++) {
        if (requestedCapacity < PRIME_LIST[i]) {
            return PRIME_LIST[i];
        }
    }
    return 0;
}

KeyedHashTable::KeyedHashTable()
{
    tableSize = 2;
    table = new HashData[2];
    occupiedElementCount = 0;
}

KeyedHashTable::KeyedHashTable(int requestedCapacity)
{
    tableSize = FindNearestLargerPrime(requestedCapacity);
    table = new HashData[tableSize];
    occupiedElementCount = 0;
}

KeyedHashTable::KeyedHashTable(const KeyedHashTable& other)
{
    tableSize = other.tableSize;
    table = new HashData[tableSize];
    for (int i = 0; i < tableSize; i++) {
        table[i] = other.table[i];
    }
    occupiedElementCount = other.occupiedElementCount;
}

KeyedHashTable& KeyedHashTable::operator=(const KeyedHashTable& other)
{
    if (this != &other) {
        delete [] table;
        tableSize = other.tableSize;
        table = new HashData[tableSize];
        for (int i = 0; i < tableSize; i++) {
            table[i] = other.table[i];
        }
        occupiedElementCount = other.occupiedElementCount;        
    }
    return *this;
}

KeyedHashTable::~KeyedHashTable()
{
    tableSize = 0;
    occupiedElementCount = 0;
    delete [] table;
}

bool KeyedHashTable::Insert(const std::string& key,
                            const std::vector<int>& intArray)
{
    int index = Hash(key);
    HashData new_data;
    new_data.key = key;
    new_data.intArray = intArray;

    if (table[index].key == "" || table[index].key == "__DELETED__") {
        table[index] = new_data;
        occupiedElementCount++;
    }

    else if (table[index].key == key) {
        return false;
    }    

    else {
        for (int i = 1; ; i++) {
            index = (index + 2*i - 1) % tableSize;
            if (table[index].key == "" || table[index].key == "__DELETED__") {
                table[index] = new_data;
                occupiedElementCount++;
                break;
            }

            if (table[index].key == key) {
                return false;
            }
        }
    }


    if (occupiedElementCount * EXPAND_THRESHOLD >= tableSize) {
        ReHash();
    }

    return true;

}

bool KeyedHashTable::Remove(const std::string& key)
{
    int index = Hash(key);
    if (table[index].key == "") {
        return false;
    }

    if (table[index].key == key) {
        table[index].key = "__DELETED__";
        table[index].intArray.clear();
        occupiedElementCount--;
        return true;
    }

    for (int i = 1; ; i++) {
        index = (index + 2*i - 1) % tableSize;
        if (table[index].key == "") {
            return false;
        }

        if (table[index].key == key) {
            table[index].key = "__DELETED__";
            table[index].intArray.clear();
            occupiedElementCount--;
            return true;
        }
    }         
    
}

void KeyedHashTable::ClearTable()
{
   for (int i = 0; i < tableSize; i++) {
       table[i].key = "";
       table[i].intArray.clear();
   }
   occupiedElementCount = 0;
}

bool KeyedHashTable::Find(std::vector<int>& valueOut,
                          const std::string& key) const
{
    int index = Hash(key);
    if (table[index].key == "") {
        return false;
    }

    if (table[index].key == key) {
        valueOut = table[index].intArray;
        return true;
    }

    for (int i = 1; ; i++) {
        index = (index + 2*i - 1) % tableSize;
        if (table[index].key == "") {
            return false;
        }

        if (table[index].key == key) {
            valueOut = table[index].intArray;
            return true;
        }
    } 
}

void KeyedHashTable::Print() const
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    std::cout << "HT:";
    if(occupiedElementCount == 0)
    {
        std::cout << " Empty";
    }
    std::cout << "\n";
    for(int i = 0; i < tableSize; i++)
    {
        if(table[i].key == "") continue;

        std::cout << "[" << std::setw(3) << i << "] ";
        std::cout << "[" << table[i].key << "] ";
        std::cout << "[";
        for(size_t j = 0; j < table[i].intArray.size(); j++)
        {
            std::cout << table[i].intArray[j];
            if((j + 1) != table[i].intArray.size())
                std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout.flush();
}