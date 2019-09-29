#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "Debug.h"
#include "symbol.h"

/** @file symbol.c
 *  @brief You will modify this file and implement the symbol.h interface
 *  @details Your implementation of the functions defined in symbol.h.
 *  You may add other functions if you find it helpful. Added functions
 *  should be declared <b>static</b> to indicate they are only used
 *  within this file. The reference implementation added approximately
 *  110 lines of code to this file. This count includes lines containing
 *  only a single closing bracket (}).
 * <p>
 * @author <b>Your name</b> goes here
 */

/** size of LC3 memory */
#define LC3_MEMORY_SIZE  (1 << 16)

/** Provide prototype for () */
char *strdup(const char *s);

/** defines data structure used to store nodes in hash table */
typedef struct node {
  struct node* next;     /**< linked list of symbols at same index */
  int          hash;     /**< hash value - makes searching faster  */
  symbol_t     symbol;   /**< the data the user is interested in   */
} node_t;

/** defines the data structure for the hash table */
struct sym_table {
  int      capacity;    /**< length of hast_table array                  */
  int      size;        /**< number of symbols (may exceed capacity)     */
  node_t** hash_table;  /**< array of head of linked list for this index */
  char**   addr_table;  /**< look up symbols by addr (optional)          */
};

/** djb hash - found at http://www.cse.yorku.ca/~oz/hash.html
 * tolower() call to make case insensitive.
 */

static int symbol_hash (const char* name) {
  unsigned char* str  = (unsigned char*) name;
  unsigned long  hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + tolower(c); /* hash * 33 + c */

  c = hash & 0x7FFFFFFF; /* keep 31 bits - avoid negative values */

  return c;
}

/** @todo implement this function */
sym_table_t* symbol_init (int capacity) {
  sym_table_t* x = calloc(1, sizeof(sym_table_t));
  x->capacity = capacity;
  x->size = 0;
  x->addr_table = (char**)calloc(LC3_MEMORY_SIZE, sizeof(char*));
  x->hash_table = (node_t**)calloc(capacity, sizeof(node_t*));
  return x;
}

/** @todo implement this function */
void symbol_term (sym_table_t* symTab) {
 symbol_reset(symTab);
 free(symTab->hash_table);
 free(symTab->addr_table);
 free(symTab);
}	

/** @todo implement this function */
void symbol_reset(sym_table_t* symTab) {

 int cap = symTab->capacity - 1;
 for (int i =0; i<cap; i++)
 {   
   node_t* head = symTab->hash_table[i];
   while(head!=NULL)
   {
     node_t* ref = head;
     head = head->next;
     free(ref->symbol.name);
     free(ref);
   }
 }
 for(int i = 0; i < ((2^16)-1); i++)
 {
   char* current = symTab->addr_table[i];
   free(current);
 }
 /*node_t* x;
 
 for (int i = 0; i < symTab->capacity; i++)
 {
     x = symTab->hash_table[i];
     
     while(x != NULL)
     {
       node_t* ref = x;
       x = x->next;
       free(ref->symbol.name);
       
       free(ref);
     }
   symTab->hash_table[i] = NULL;
  }
 for (int i = 0; i < 65536; i++)
 {
   symTab->addr_table[i] = NULL;
 }
  free(x);
  symTab->size = 0;*/
   
     
}

/** @todo implement this function */
int symbol_add (sym_table_t* symTab, const char* name, int addr) {
  int index = 0;
  int hash = 0;
  
  node_t * thisOne = symbol_search(symTab, name, &hash, &index);
  if (thisOne == NULL)
  {
    node_t* head = symTab->hash_table[index];
    
    node_t* newNode = malloc(sizeof(node_t));

    newNode->next = head;
    newNode->hash = hash;

    symbol_t* symbol = malloc(sizeof(symbol_t));
    symbol->addr = addr;
    symbol->name = strdup(name);
   
    newNode->symbol = *symbol;
  
    symTab->hash_table[index] = newNode;
    symTab->addr_table[addr]= newNode->symbol.name;

    symTab->size += 1;
    free(symbol);
    return 1;
  } 
  return 0;
}

/** @todo implement this function */
struct node* symbol_search (sym_table_t* symTab, const char* name, int* hash, int* index) {
  *hash = symbol_hash(name);
  *index = ((*hash) % symTab->capacity); 

  node_t* newNode = symTab->hash_table[*index];
  
  while(newNode != NULL)
  {
    if(strcasecmp(newNode->symbol.name, name) == 0)
    {
      return newNode;
    }
   newNode = newNode->next;
  }
 return NULL;
}

/** @todo implement this function */
symbol_t* symbol_find_by_name (sym_table_t* symTab, const char* name) {
  int hash = 0;
  int index = 0;
  node_t* newNode = symbol_search(symTab, name, &hash, &index);
  if(newNode == NULL)
  {
    return NULL;
  }
  else
  {
    return &(newNode->symbol);
  }
  return NULL;
}

/** @todo implement this function */
char* symbol_find_by_addr (sym_table_t* symTab, int addr) {
  if (symTab->addr_table[addr] == 0)
  {
    return NULL;
  }
  return symTab->addr_table[addr];
}

/** @todo implement this function */
void symbol_iterate (sym_table_t* symTab, iterate_fnc_t fnc, void* data) {
 for (int i = 0; i < symTab->capacity;i++)
 {
   node_t* thisOne = symTab->hash_table[i];
   while(thisOne != NULL)
   {
     (*fnc)(&thisOne->symbol, data);
     thisOne = thisOne->next;
   }
 }
}

/** @todo implement this function */
int symbol_size (sym_table_t* symTab) {
  return symTab->size;
}

/** @todo implement this function */
int compare_names (const void* vp1, const void* vp2) {
  symbol_t* sym1 = *((symbol_t**) vp1);
  symbol_t* sym2 = *((symbol_t**) vp2); // study qsort to understand this

  //printf("%s\n",sym2->name);
  //fflush(stdin);
  
  /*if (j > x)
    return 1;
  else if (x > j)
    return -1;
  else */
    return (sym1->name - sym2->name);
}

/** @todo implement this function */
int compare_addresses (const void* vp1, const void* vp2) {
  symbol_t* sym1 = *((symbol_t**) vp1);
  symbol_t* sym2 = *((symbol_t**) vp2);
  int final = 0;
  if (sym1->addr > sym2->addr)
  {
    final = 1;
  }
  if (sym1->addr < sym2->addr)
  {
    final = -1;
  }
  if (final == 0)
  {
    final = compare_names(vp1, vp2);
  }
  return final;
}

/** @todo implement this function */
symbol_t** symbol_order (sym_table_t* symTab, int order) 
{
  
  // will call qsort with either compare_names or compare_addresses
  symbol_t** x = calloc(symTab->size, sizeof(symbol_t));
  int current = 0;
  for (int i = 0; i < (symTab->capacity); i++)
  {
     node_t* thisOne = symTab->hash_table[i];
     while(thisOne != NULL)
     {
      x[current] = &(thisOne->symbol);
      current++;
      thisOne = thisOne->next;
     }
  }

  if (order == 0)
  {
    //symbol_iterate(symTab, symbol_hash, 0);
  }
  else if (order == 1)
  {
    printf("%s\n",x[0]->name);
    fflush(stdin);
    qsort(x, symTab->size, sizeof(symbol_t), compare_names);
  }
  else 
  {
    //qsort(x, symTab->size, sizeof(symbol_t), compare_addresses);
  }

  return x;
  
}

