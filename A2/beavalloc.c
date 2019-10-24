#include <unistd.h>
#include "beavalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define MIN_BYTES 1024
#define MAX_BYTES 1024
static void *lower_mem_bound;
static void *upper_mem_bound;

void *scan(size_t size);
void *appendNode(size_t size);
void *lastAddress(int);

typedef struct node {
   size_t size;
   size_t capacity;
   int free;
   int data;
   void *prev_address;
   struct node *next;
   struct node *prev;
} Node;

void coalesceLeft(Node*, Node *);
void coalesceRight(Node*, Node *);

static void * baseAddr = 0;
static void * lastAddr = 0;

Node* head = NULL;
//looks for free block and return addr of block that is big enough
void* scan(size_t size) {
  Node* node = head;
  while(node != NULL){ //traverse through all existing nodes
    if(node->size >= size){ //if node is greater than space needed or is empty
    //  printf("node size: %d\n", node->size);
      return node; //return pointer to node with enough space
    }
    node = node->next; //move onto next node
  }
  return NULL; //return null if there is no free blocks
}

void *lastAddress(int x) {
  x = 0;
  lastAddr = head;
  while (lastAddr != NULL) {
    lastAddr = head->next;
  }
  lastAddr = sbrk(0);
  return lastAddr;

}

void *appendNode(size_t size) {//referenced: https://www.geeksforgeeks.org/doubly-linked-list/
    Node *tail = head;
    Node *new_node = NULL;
    Node *n;
    int i;
    int num = size/MAX_BYTES + 1;
    if (size > MAX_BYTES) { //account for requests larger than MAX
      for (i = 0; i < num; i++) {
        new_node = sbrk(MAX_BYTES);
        if (i == 0) {
          n = new_node; //save address of first allocated block
        }
        if(i == num - 1) {
            new_node = (void *)n;
          }
        }
      }

    else {
      new_node = sbrk(MAX_BYTES);
    }

    if (lower_mem_bound == 0)
      lower_mem_bound = new_node;
    upper_mem_bound = sbrk(0);
    new_node->data = (void *)new_node + 48;
    new_node->free = FALSE;
    new_node->prev_address = (void *)new_node;
    if(size > MAX_BYTES) {
      new_node->capacity = size;
    }
    else {
      new_node->capacity = MAX_BYTES - 48;
    }
    new_node->next = head;
    new_node->prev = NULL;

    if(baseAddr == 0) {
      baseAddr = new_node->prev_address;
    }

    new_node->next = NULL;
    new_node->size = size;


    if (head == NULL) {
          new_node->prev = NULL;
          head = new_node;
          return head->data;
    }


    while (tail->next != NULL) //traverse to end
      tail = tail->next;

    tail->next = new_node;
    new_node->prev = tail;
    return new_node->data;
}


void *beavalloc(size_t size) {
  Node *split1 = NULL;
  void *blockAddr = NULL;
  int remainder;


  blockAddr = scan(size); //get address of block that's big enough to split


  if (size <= 0) {
    errno = ENOMEM;
    return NULL;
  }
  else if (blockAddr != NULL) {
    split1 = (Node *)blockAddr; //temp block = block that's big enough
    remainder = split1->capacity - size;
    //split1->capacity = ;
    split1->free = FALSE;
    split1->prev = head;
    split1->size = size;

    return appendNode(remainder);
    //need to return node->data


  }
  else {
    return appendNode(size);
  }
}

void coalesceRight(Node* Next, Node *curr) {
  Node *temp1 = Next->next;
  Node *temp2 = Next->prev;

  //coalesce right-adjacent blocks
  curr->capacity = curr->capacity + Next->capacity + 48;
  curr->size = 0;
  /* base case */
  if (head == NULL || Next == NULL) {
    return;
  }
  /* If node to be deleted is head node */
  if (head == Next) {
    head = Next->next;
  }
  /* Change next only if node to be
  deleted is NOT the last node */
  if (Next->next != NULL) {
    temp1->prev = Next->prev;
  }
  /* Change prev only if node to be
  deleted is NOT the first node */
  if (Next->prev != NULL) {
    temp2->next = Next->next;
  }
}

void coalesceLeft(Node* Prev, Node *curr) {
  Node *temp1 = curr->next;
  Node *temp2 = curr->prev;
  //coalesce right-adjacent blocks
  Prev->capacity = Prev->capacity + curr->capacity + 48;
  Prev->size = 0;
  /* base case */
  if (head == NULL || curr == NULL) {
    return;
  }
  /* If node to be deleted is head node */
  if (head == curr) {
    head = curr->next;
  }
  /* Change next only if node to be
  deleted is NOT the last node */
  if (curr->next != NULL) {
    temp1->prev = curr->prev;
  }
  /* Change prev only if node to be
  deleted is NOT the first node */
  if (curr->prev != NULL) {
    temp2->next = curr->next;
  }
}

void beavfree(void *ptr) {
  Node *curr;
  Node *Next;
  Node *Prev;
  if(ptr == NULL) {
    return;
  }


  curr = (void *)ptr - 48;

  curr->free = TRUE;


  Next = curr->next;
  if(Next != NULL){
    if(Next->free == TRUE) {
      coalesceRight(Next, curr);
    }
  }


  Prev = curr->prev;
  if(Prev != NULL){
    if(Prev->free == TRUE) {
      coalesceLeft(Prev,curr);
    }
  }
}

void beavalloc_reset(void) {
  if(head != NULL) {
    head = baseAddr;
    brk((void *)head);
  }
  head = NULL;
  baseAddr = 0;
  lower_mem_bound = 0;
}


void beavalloc_set_verbose(uint8_t x) {
  if (x == TRUE) {
    fprintf(stderr, "Verbose Enabled\nThis is a diagnostic message\n");
  }
  else {
    return;
  }
}

void *beavcalloc(size_t nmemb, size_t size) {
  void *addr = NULL;
  if (nmemb == 0 || size == 0) {
    return NULL;
  }
  addr = beavalloc(nmemb * size);
  if (addr == NULL) {
    lower_mem_bound = 0;

    return NULL;
  }
  memset(addr, 0x0, nmemb*size);
  return addr;
}

void *beavrealloc(void *ptr, size_t size) {
  Node *temp1 = NULL;
  Node *temp2 = NULL;

  if (size == 0) {
    return NULL;
  }
  if (ptr != NULL) {
    temp1 = (Node *)ptr - 1;
    if(temp1->size >= size) {
      return ptr;
    }
    temp2 = beavalloc(size);

    if(temp2 != NULL) {
      memcpy(temp2, ptr, temp1->size);
      beavfree(ptr);
    }

  }
  else {
    temp2 = beavalloc(2 * size);


  }
  return temp2;

}

void beavalloc_dump(uint leaks_only) {
    Node *curr = NULL;
    uint i = 0;
    uint leak_count = 0;
    uint user_bytes = 0;
    uint capacity_bytes = 0;
    uint block_bytes = 0;
    uint used_blocks = 0;
    uint free_blocks = 0;

    if (leaks_only) {
        fprintf(stderr, "heap lost blocks\n");
    }
    else {
        fprintf(stderr, "heap map\n");
    }
    fprintf(stderr
            , "  %s\t%s\t%s\t%s\t%s"
            "\t%s\t%s\t%s\t%s\t%s\t%s"
            "\n"
            , "blk no  "
            , "block add "
            , "next add  "
            , "prev add  "
            , "data add  "

            , "blk off  "
            , "dat off  "
            , "capacity "
            , "size     "
            , "blk size "
            , "status   "
        );
    for (curr = head, i = 0; curr != NULL; curr = curr->next, i++) {
        if (leaks_only == FALSE || (leaks_only == TRUE && curr->free == FALSE)) {
            fprintf(stderr
                    , "  %u\t\t%9p\t%9p\t%9p\t%9p\t%u\t\t%u\t\t"
                      "%u\t\t%u\t\t%u\t\t%s\t%c\n"
                    , i
                    , curr
                    , curr->next
                    , curr->prev
                    , curr->data
                    , (unsigned) ((void *) curr - lower_mem_bound)
                    , (unsigned) ((void *) curr->data - lower_mem_bound)
                    , (unsigned) curr->capacity
                    , (unsigned) curr->size
                    , (unsigned) (curr->capacity + sizeof(Node))
                    , curr->free ? "free  " : "in use"
                    , curr->free ? '*' : ' '
                );
            user_bytes += curr->size;
            capacity_bytes += curr->capacity;
            block_bytes += curr->capacity + sizeof(Node);
            if (curr->free == FALSE && leaks_only == TRUE) {
                leak_count++;
            }
            if (curr->free == TRUE) {
                free_blocks++;
            }
            else {
                used_blocks++;
            }
        }
    }
    if (leaks_only) {
        if (leak_count == 0) {
            fprintf(stderr, "  *** No leaks found!!! That does NOT mean no leaks are possible. ***\n");
        }
        else {
            fprintf(stderr
                    , "  %s\t\t\t\t\t\t\t\t\t\t\t\t"
                      "%u\t\t%u\t\t%u\n"
                    , "Total bytes lost"
                    , capacity_bytes
                    , user_bytes
                    , block_bytes
                );
        }
    }
    else {
        fprintf(stderr
                , "  %s\t\t\t\t\t\t\t\t\t\t\t\t"
                "%u\t\t%u\t\t%u\n"
                , "Total bytes used"
                , capacity_bytes
                , user_bytes
                , block_bytes
            );
        fprintf(stderr, "  Used blocks: %u  Free blocks: %u  "
             "Min heap: %p    Max heap: %p\n"
               , used_blocks, free_blocks
               , lower_mem_bound, upper_mem_bound
            );
    }
}
