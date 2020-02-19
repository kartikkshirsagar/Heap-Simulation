#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#define SIZE 1024
typedef struct inner_node{
    char* address;
    struct inner_node* right;
}inner_node;
typedef enum{FAILURE,SUCCESS}status_code;
typedef struct outer_node{
    int size;
    struct inner_node* right;
    struct outer_node* down;
}outer_node;
typedef struct alloc_list_node{
    char *address;
    int size;
    struct free_list_node *next;
}allocnode;

allocnode* makeAllocNode(int size,char* start){
    allocnode* ptr;
    ptr=(allocnode*)malloc(sizeof(allocnode));
    ptr->address=start;
    ptr->next=NULL;
    ptr->size=size;
    return ptr;
}
outer_node* makeOuterNode(int size){
    outer_node* ptr;
    ptr=(outer_node*)malloc(sizeof(outer_node));
    ptr->down=NULL;
    ptr->right=NULL;
    ptr->size=size;
    return ptr;
}
inner_node* makeInnerNode(char* address){
    inner_node* ptr;
    ptr=(inner_node*)malloc(sizeof(inner_node));
    ptr->right=NULL;
    ptr->address=address;
    return ptr;
}

void Init(outer_node* lptr,char* memoryStart)
{
    outer_node* mainptr;inner_node* bigptr;
    bigptr=(inner_node*)malloc(sizeof(inner_node));
    mainptr=(outer_node*)malloc(sizeof(outer_node));
    mainptr->size=SIZE;
    mainptr->down=NULL;
    mainptr->right=bigptr;
    bigptr->right=NULL;
    bigptr->address=memoryStart;
}

status_code DeleteQ(char* dptr,inner_node* iptr)
{
    status_code ret_val=SUCCESS;
    inner_node* ptr;
        ptr=iptr;
        dptr=ptr->address;
        free(iptr);
    return ret_val;
}

void InsertInAlloc(allocnode** allist,char** address,int size)
{
    allocnode* nptr,*ptr=*(allist);
    nptr=makeAllocNode(size,&address);
    if(allist==NULL)
    {
        nptr->next=NULL;
        *(allist)=nptr;
    }
    else
    {
        while(ptr->next!=NULL)
        {
            ptr=ptr->next;
        }
        ptr->next=nptr;

    }
}

void deleteFreeListNode(outer_node* optr,char** adptr,int listsize){
    outer_node* ptr=optr,*prev=NULL,*ptr2;
    while(ptr->size!=listsize)
    {
        if(prev==NULL)
        {
            prev=optr;
        }
        else{
            prev=prev->down;
        }
        
        ptr=ptr->down;
    }
    DeleteQ(*(adptr),ptr->right);
    ptr=optr;
    //now checking if any outer node is all by itself (if its so then delete that outernode)
    prev=NULL;
    while(ptr!=NULL)
    {
        
        if(ptr->right==NULL)
        {
            prev->down=ptr->down;
            ptr2=ptr;
            free(ptr2);
        }
        if(prev==NULL)
        {
            prev=ptr;
        }
        else{
            prev=prev->down;
        }
        
        ptr=ptr->down;
    }

        //LEFT WORK ON THE THOUGHT PERTAINING TO DELETION OF THE OUTER NODE
        //to delete the outer node we would need the outer list pointer too


}

char* myMalloc(int size,char* memory,allocnode* allist,outer_node* freeList){
    if(size<32){
        return NULL;
    }
    int u=ceil(log2(size)),blocksize=pow(2,u),flag=0,sz;
    char* addr;
    outer_node* ptr=freeList,*prev=NULL;
    while(ptr->size<=blocksize){
        if(ptr->size==blocksize){
            flag=1;
            //delete the free list node and add it in the alloc list, return the start address
            deleteFreeListNode(freeList,&addr,blocksize);
            InsertInAlloc(&allist,addr,blocksize);
        }
        ptr=ptr->down;
        
    }
    if(flag==0){
        sz=ptr->size;
        //insert 2 (sz/2) sized nodes in free list and call the fucntion again.(careful with the address)
        addr=myMalloc(size,memory,allist,freeList);
    }
}


int main(){
    char memory[SIZE];
    outer_node *olptr;
    Init(olptr,memory);
    //myMalloc(100,&memory);
}