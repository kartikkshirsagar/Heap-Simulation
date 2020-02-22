//Implement pqueues and functions insert and delete
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define P 10
#define SIZE 1024
#define THR 32
typedef enum{FALSE,TRUE}Boolean;

typedef enum{FAILURE,SUCCESS}status_code;

typedef struct node_type{
    char* address;
    int size;
    struct node_type* next;
}node;

typedef struct queue_type{
    node* lptr;
}List;

typedef struct pqueue_type{
    List list[P]; 
}FL;

node* MakeNode(int size,char* addr){
    node* nptr;
    nptr=(node*)malloc(sizeof(node));
    nptr->size=size;
    nptr->address=addr;
    nptr->next=NULL;
    return nptr;
}

void InitList(List* list)
{
    list->lptr=NULL;
}
Boolean IsListEmpty(List* qptr)
{
    Boolean ret_val;
    if(qptr->lptr==NULL)
    {
        ret_val=TRUE;
    }
    else{
        ret_val=FALSE;
    }
    return ret_val;
}
status_code InsertList(List* listptr,int size,char* addr){
    //INSERT AT START
    status_code sc=SUCCESS;
    node *ptr;
    node* nptr=MakeNode(size,addr);
    if(nptr==NULL)
    {
        sc=FAILURE;
    }
    else{
        if(IsListEmpty(listptr)==TRUE)
        {
            listptr->lptr=nptr;
            nptr->next=NULL;
        }
        else{
            nptr->next=listptr->lptr;
            listptr->lptr=nptr;
        }
    }
    return sc;

}

node* InsertAlloc(node** lpptr,char* addr,int size)
{
    node* nptr;
    nptr=MakeNode(size,addr);
    if(*(lpptr)==NULL)
    {
        nptr->next=NULL;
        *(lpptr)=nptr;
    }
    else{
        nptr->next=*(lpptr);
        *(lpptr)=nptr;
    }
    return *(lpptr);
}

status_code DeleteList(char** dptr,List* listptr)
{
    status_code ret_val=SUCCESS;
    node* ptr;
    if(IsListEmpty(listptr)==TRUE){
        ret_val=FAILURE;
    }
    else{
        ptr=listptr->lptr;
        *dptr=(ptr->address);
        listptr->lptr=(listptr->lptr)->next;
        free(ptr);
    }
    return ret_val;
}

status_code DeleteAllocAddr(int* size,char** adptr,node** lpptr,char* address)
{
    node* lptr=*lpptr,*prev=NULL;status_code sc=SUCCESS;
    while(lptr!=NULL)
    {
        if(lptr->address==address)
        {
            if(prev!=NULL)
            {
                prev->next=lptr->next;
                *adptr=lptr->address;
                *size = lptr->size;
                free(lptr);
            }
            else{
                *adptr=lptr->address;
                *size = lptr->size;
                *lpptr=lptr->next;
                free(lptr);
            }
        }
        if(prev==NULL)
        {
            prev=lptr;
        }
        else{
            prev=prev->next;
        }
        
        lptr=lptr->next;
    }
    return sc;   
}

void InsertFL(char* addr,FL* fl,int size){
    InsertList(&(fl->list[(int)ceil(log2(size))]),size,addr);
}

void DeleteFL(char** adptr,FL* fl,char* tobedeleted){
    int flag=0;
    node* ptr,*prev=NULL,*dptr;List* q;
    for(int i=0;i<P && flag==0;i++)
    {
        if(IsListEmpty(&(fl->list[i]))==FALSE)
        {  
            q=&(fl->list[i]);
            ptr=q->lptr;
            while(ptr!=NULL && flag==0)
            {
                if(tobedeleted==ptr->address)
                {
                    *(adptr)=ptr->address;
                    if(prev==ptr)
                    {
                        free(ptr);
                        q->lptr=NULL;
                    }
                    else if(prev==NULL){
                        dptr=ptr;
                        q->lptr=q->lptr->next;
                        free(dptr);
                    }
                    else{
                        prev->next=ptr->next;
                        free(ptr);
                        }
                    flag=1;
                    }
                    if(prev==NULL)
                    {
                        prev=ptr;
                    }
                    else{
                        prev=prev->next;
                    }
                ptr=ptr->next;
            }
        }
    }
}

void InitFL(FL* fl,char* memaddr,node** alloclistptr)
{
    List* q;
    for (int i = 0; i < P; i++)
    {
        InitList(&(fl->list[i]));
    }
    q=&(fl->list[P-1]);
    q->lptr=MakeNode(SIZE,memaddr);
    *(alloclistptr)=NULL;

}

node* MergeFreeNodes(node* node1,node* node2,node* prev,List* listptr)
{
    node* nptr=(node*)malloc(sizeof(node));
    if(prev==NULL)
    {
        if(node1->address<node2->address)
        {
            nptr->address = node1->address;
        }
        else{
            nptr->address = node2->address;
        }
        
        nptr->size= (node1->size)*2;
        listptr->lptr=node2->next;
    }
    else{
          if(node1->address<node2->address)
            {
            nptr->address = node1->address;
            }
        else{
            nptr->address = node2->address;
        }
         nptr->size= (node1->size)*2;
         prev->next=node2->next;
    }
    free(node1);
    free(node2);
    return nptr;
}

void CheckMerge(FL* fl){
    int i=0;List* listptr;node* ptr,*prev=NULL,*nptr;int flag=0;
    for(i=0;i<P && flag==0;i++)
    {
        listptr=&(fl->list[i]);
        if(IsListEmpty(listptr)==FALSE)
        {
            if(listptr->lptr->next!=NULL){
                ptr=listptr->lptr;
                while(ptr->next!=NULL)
                {
                    if((ptr->address - (ptr->size))==ptr->next->address ||(ptr->next->address - (ptr->size))==ptr->address)
                    {
                        nptr=MergeFreeNodes(ptr,ptr->next,prev,listptr);
                        nptr->next=fl->list[i+1].lptr;
                        fl->list[i+1].lptr=nptr;

                        flag=1;
                    }
                    if(prev==NULL)
                    {
                        prev=ptr;
                    }
                    else
                    {
                        prev=prev->next;
                    }
                    ptr=ptr->next;
                }

            }
        }
    }
    if(flag==1)
    {
        CheckMerge(fl);
    }
}

void Free(char* tobefreed,FL* fl,node** alloclistptr){
    int i;status_code sc;char* addr;int flag=0;int size;
    List* listptr;node* ptr;
    int index;
        sc=DeleteAllocAddr(&size,&addr,alloclistptr,tobefreed);
        index=log2(size)-1;
        listptr=&(fl->list[index]);
        InsertList(listptr,size,addr);
        CheckMerge(fl);
}







char* myMalloc(int size,char* memory,FL* fl,node** alloclistptr){
    if(size>SIZE || size==0)
    {
        return NULL;
    }
    int u=ceil(log2(size)),blocksize=pow(2,u),flag=0,sz;
    if(u<=0)
    {
        u=1;
    }
    if(blocksize==1)
    {
        blocksize=2;
    }
    char* addr=NULL;int i;
    int tempsize;
    List* listptr;
    /*if((size/THR - 1)<0) //SIZE<32 then allocate 32 only.
    {
       listptr=&(fl->list[0]);    
    }
    else{*/
        listptr=&(fl->list[u-1]);
    //}
    
    if(IsListEmpty(listptr)==FALSE)
    {
        //delete the free list node and add it in the alloc list, return the start address
        
        InsertAlloc(alloclistptr,listptr->lptr->address,blocksize);
        DeleteList(&addr,listptr);
        return addr;
    }
    else
    {
        i=u-1;
        while(i<P && flag==0)
        {
            if(IsListEmpty(&(fl->list[i]))==FALSE)
            {
                tempsize=(fl->list[i]).lptr->size;
                flag=1;
                //insert 2 (sz/2) sized nodes in free list and call the fucntion again.(careful with the address)
                DeleteList(&addr,&(fl->list[i]));
                InsertList(&(fl->list[i-1]),(tempsize)/2,addr);
                InsertList(&(fl->list[i-1]),(tempsize)/2,(addr + (tempsize/2)));
            }
            i++;
        }
        if(flag==0 && i==P)
        {
            printf("---------------------------------------\n");
            printf("Cannot Satisfy the request\n");
            printf("---------------------------------------\n");
            return NULL;
            
        }
        addr=myMalloc(size,memory,fl,alloclistptr);
        
    }
            
}

void printDetails(FL* fl,node *alloclistptr)
{
    int i;int count=0;node* ptr;
    for(i=0;i<P;i++)
    {
        count=0;
        printf("\nList of size %d has ----->",(int)pow(2,i+1));
        if(IsListEmpty(&(fl->list[i]))==FALSE)
        {
            ptr=fl->list[i].lptr;
            while(ptr!=NULL)
            {
                printf("%d(%p) ",ptr->size,ptr->address);
                ptr=ptr->next;
            }   
        }
        

    }

    printf("\nThe Alloc list has ----");
    ptr=alloclistptr;
    while(ptr!=NULL)
    {
        printf("%d(%p) ",ptr->size,ptr->address);
        ptr=ptr->next;
    }
    printf("\n");
}








int main(){
    node* alloclistptr;
    char* address;
    alloclistptr=(node*)malloc(sizeof(node));
    char memory[SIZE];
    FL *fl;
    fl=(FL*)malloc(sizeof(FL));
    InitFL(fl,memory,&(alloclistptr));
    //List* q=&(fl->list[P-1]);
    address=myMalloc(500,memory,fl,&alloclistptr);
    if(address==NULL)
    {
        printf("Sorry cannot allocate, too big request \n");
    }
    else{
       // printDetails(fl,alloclistptr);
    //printf("%d",alloclistptr->size)
    //Free(address,fl,&alloclistptr);
    printDetails(fl,alloclistptr);
    //printDetails(fl,alloclistptr);
    char* address1=myMalloc(60,memory,fl,&alloclistptr);
    printDetails(fl,alloclistptr);
    Free(address1,fl,&alloclistptr);
    //char* address2=myMalloc(50,memory,fl,&alloclistptr);
    //printDetails(fl,alloclistptr);
    //char* address3=myMalloc(2,memory,fl,&alloclistptr);
    //printDetails(fl,alloclistptr);
    //char* address4=myMalloc(32,memory,fl,&alloclistptr);
    //printDetails(fl,alloclistptr);
    Free(address,fl,&alloclistptr);
    //printDetails(fl,alloclistptr);
    //Free(address,fl,&alloclistptr);
    //printDetails(fl,alloclistptr);
    //Free(address1,fl,&alloclistptr);
    
    //Free(address4,fl,&alloclistptr);
   // printDetails(fl,alloclistptr);
  //  Free(address2,fl,&alloclistptr);
   // printDetails(fl,alloclistptr);
    }

/*printf("Which operation do you want to do?(Current heap size is %d)\n1.MALLOC \n2.FREE",SIZE);
int ans;int size;
scanf("%d",&ans);
if(ans==1)
{
    printf("Please enter the size for malloc\n");
    scanf("%d",&size);
    myMalloc(size,memory,fl,&alloclistptr);
    printf("Here are the details:\n");
    printDetails(fl,alloclistptr);
}
else{
    printf("Please enter the size to free\n");
    scanf("%d",&size);
}*/




}
    
