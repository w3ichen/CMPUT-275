/*
  Name: Weichen Qiu
  ID: 1578205

  Weekly Assignment #4: Dynamic Hashing
  CMPUT 275
*/
#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "linked_list.h"
#include <cassert>

// forward declaration of HashTable class
template <typename T>
class HashTable;


template <typename T>
class HashTableIterator {
public:
  // constructor, need to know a pointer to the hash table we are iterating over
  // this initialize this iterator to the first entry of the table (if any)
  HashTableIterator(const HashTable<T>* hashTable);

  // no need for a destructor, we don't allocate dynamic memory here

  // signals if we are at the end
  bool atEnd() const;

  // post increment operator for the iterator, look it up on the
  // page on operator overloading linked from the lecture slides to see
  // why we use this specific syntax
  HashTableIterator<T> operator++(int);

  // get the item the iterator is looking at
  const T& operator*() const;

private:
  // the bucket we are currently examining, we are at the end of iteration
  // if and only if bucket == the number of buckets held by *tablePtr
  unsigned int bucket;

  // the table itself that we are iterating over
  const HashTable<T>* tablePtr;

  // the node in the list we are examining
  ListNode<T>* node;

  // advance the iterator
  void advance();
};

template <typename T>
HashTableIterator<T>::HashTableIterator(const HashTable<T>* hashTable) {
  bucket = 0;
  tablePtr = hashTable;
  node = tablePtr->table[0].getFirst();
  if (node == NULL) {
    // if the first bucket was empty, advance to the next item in the table
    advance();
  }
}

template <typename T>
bool HashTableIterator<T>::atEnd() const {
  return bucket == tablePtr->tableSize;
}

template <typename T>
HashTableIterator<T> HashTableIterator<T>::operator++(int) {
  HashTableIterator<T> copy = *this; // create a copy
  advance();
  return copy;
}

template <typename T>
const T& HashTableIterator<T>::operator*() const {
  assert(!atEnd());
  return node->item;
}

template <typename T>
void HashTableIterator<T>::advance() {
  assert(!atEnd());
  node = node->next;
  // if we advanced past the end of the list in this bucket,
  // then scan for the next nonempty bucket
  while (node == NULL && bucket < tablePtr->tableSize) {
    ++bucket;
    node = tablePtr->table[bucket].getFirst();
  }
}

/*
  A hash table for storing items. It is assumed the type T of the item
  being stored has a hash method, eg. you can call item.hash(), which
  returns an unsigned integer.

  Also assumes the != operator is implemented for the item being stored,
  so we could check if two items are different.

  If you just want store integers int for the key, wrap it up in a struct
  with a .hash() method and both == and != operator.
*/

template <typename T>
class HashTable {
  friend class HashTableIterator<T>; // now we can access the private variables of HashTable<T>

public:

  // creates an empty hash table with the given number of buckets.
  // default parameter is set to 10
  HashTable(unsigned int tableSize = 10);

  ~HashTable();

  // Check if the item already appears in the table.
  bool contains(const T& item) const;

  // Insert the item, do nothing if it is already in the table.
  // Returns true iff the insertion was successful (i.e. the item was not there).
  bool insert(const T& item);

  // Removes the item after checking, via assert, that the item was in the table.
  void remove(const T& item);

  unsigned int size() const;


private:
  LinkedList<T> *table; // start of the array of linked lists (buckets)
  unsigned int numItems; // # of items in the table
  unsigned int tableSize; // # of buckets

  // Computes the hash table bucket that the item maps into
  // by calling it's .hash() method.
  unsigned int getBucket(const T& item) const;

  // dynamically extend length of hash table if numItems > tableSize
  void extendHash();
  // dynamically shrink hash table if numItes < tableSize/4
  void shrinkHash();
};

template <typename T>
HashTable<T>::HashTable(unsigned int tableSize) {
  // make sure there is at least one bucket
  assert(tableSize > 0);

  // calls the constructor for each linked list
  // so each is initialized properly as an empty list
  table = new LinkedList<T>[tableSize];

  // we are not storing anything
  numItems = 0;
  this->tableSize = tableSize;
}

template <typename T>
HashTable<T>::~HashTable() {
  // this will call the destructor for each linked
  // list before actually deleting this table from
  // the heap
  delete[] table;
}

template <typename T>
bool HashTable<T>::contains(const T& item) const {
  unsigned int bucket = getBucket(item);

  return table[bucket].find(item) != NULL;
}


template <typename T>
void HashTable<T>::extendHash(){
  // create a table double the size
  this->tableSize = tableSize*2;
  // create new list to temporarily store extended new list
  LinkedList<T> *new_table = new LinkedList<T>[tableSize];
  // create node variable to store current node
  ListNode<T> *node;

  // copy items to the new list
  for (unsigned int i=0; i< tableSize/2;i++){
    // iterate through all the buckets in the old list
    // get first item in linked list of new bucket
    node = this->table[i].getFirst();

    while (node != NULL){
      // if the node is not empty
      unsigned int new_bucket = getBucket(node->item);

      // assign new bucket in new table as the node
      new_table[new_bucket].insertFront(node->item);

      // go to next item in linked list
      node = node->next;
    }
  }
  // delete the old table
  delete[] table;
  // new table now becomes table
  this->table = new_table; 
}

template <typename T>
bool HashTable<T>::insert(const T& item) {
  // if the item is here, return false
  if (contains(item)) {
    return false;
  }
  else {
    ++numItems;
    if (numItems > tableSize){
      // if numofItems exceeds num of buckets, need to extend array
      extendHash();
    }

    // otherwise, insert it into the front of the list
    // in this bucket and return true
    unsigned int bucket = getBucket(item);
    table[bucket].insertFront(item);

    return true;
  }
}

template <typename T>
void HashTable<T>::shrinkHash(){

  // create a table that iss 1/2 size
  this->tableSize = max(tableSize/2,(unsigned int) 10);
  // temporary linked list to store the shrinked table
  LinkedList<T> *new_table = new LinkedList<T>[tableSize];
  // new node to store current node
  ListNode<T> *node;

  // copy items to the new list
  for (unsigned int i=0; i< tableSize*2;i++){
    // iterate through all the buckets in the old list
    // get first item in linked list of new bucket
    node = this->table[i].getFirst();

    // if the node is not empty
    while (node != NULL){
      // get the new bucket in the shortened hash table
      unsigned int new_bucket = getBucket(node->item);

      // assign new bucket in new table as the node
      new_table[new_bucket].insertFront(node->item);
      // go to next item in linked list
      node = node->next;
    }
  }
  // delete the old table
  delete[] table;
  // new table now becomes table
  this->table = new_table;
}

template <typename T>
void HashTable<T>::remove(const T& item) {
  --numItems;
  if (numItems < tableSize && tableSize > 10){
  //   // reclaim memory by shrinkng hash table
        shrinkHash();
  }

  unsigned int bucket = getBucket(item);

  ListNode<T>* link = table[bucket].find(item);

  // make sure the item was in the list
  assert(link != NULL);

  table[bucket].removeNode(link);
}

template <typename T>
unsigned int HashTable<T>::size() const {
  return numItems;
}


template <typename T>
unsigned int HashTable<T>::getBucket(const T& item) const {
  return item.hash() % tableSize;
}


#endif