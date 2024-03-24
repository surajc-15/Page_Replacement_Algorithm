#include <stdio.h>
#include <stdlib.h>

#define FRAMESIZE 4 // 4MB page and frame size
#define PAGESIZE 4
int pagetablecount = 0; 
int pagefault=0;
int checkpagehit(int pagenumber);
void deletepaget(int pagenumber);
struct Page
{
    int pagenumber;
    struct Page *next;
};

struct Page *pagehead = NULL;

// int framenumber = 0;

struct pageTable
{
    int framenumber;
    int pagenumber;
    int frequency;
    struct pageTable *next;
};

struct pageTable *pthead = NULL;  // head pointer to page table
struct Page *physicalMemory[100]; // array storing the page memory address

int checkpagehit(int pagenumber)
{
    struct pageTable *ptr = pthead;
    while (ptr != NULL)
    {
        if (ptr->pagenumber == pagenumber)
        {
             ptr->frequency++;
            return 0;
        }
        ptr = ptr->next;
    }
    pagefault++;
    return -1;
}

struct Page *createPage(int pagenumber)
{
    struct Page *newpage = (struct Page *)malloc(sizeof(struct Page));// its newly created
    newpage->pagenumber = pagenumber;
    newpage->next = NULL;

    // add it to the pagelist
    if (pagehead == NULL)
    {
        pagehead = newpage;
    }
    else
    {
        struct Page *temp = pagehead;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newpage;
    }
    
    return newpage;
}

int getfreeframe(struct Page *physicalMemory[], int framenumber)
{
    for (int index = 0; index < framenumber; index++)
    {
        if (physicalMemory[index] == NULL)
        {
            return index;
        }
    }
    return -1; // no free frame available in memory
}
int LFU(){
    struct pageTable *table =pthead ;
     unsigned int size = pagetablecount;
     int i,pos, minimum=table->frequency;
     pos =table->framenumber;
     for (i = 0; i < pagetablecount; i++) {
        if (table->frequency < minimum) {
            minimum = table->frequency;
            pos =table->framenumber;
        }
        table = table->next;
    }
    return pos;
}
void insert_into_tablel(struct Page *page,int framenumber)
{  
     int index;
     if(checkpagehit(page->pagenumber)!=0){
   
    if(getfreeframe(physicalMemory, framenumber)==-1){
         index=LFU();
         printf("\npage %d is replaced  page %d\n",page->pagenumber,physicalMemory[index]->pagenumber);
         deletepaget( physicalMemory[index]->pagenumber);
         
       }
    else{ 
        index = getfreeframe(physicalMemory, framenumber);
        };
    struct pageTable *newentry = (struct pageTable *)malloc(sizeof(struct pageTable));
    newentry->frequency=0;
    newentry->framenumber = index;
    newentry->pagenumber = page->pagenumber;
    physicalMemory[index] = page; // inserting page data in physical memory
    newentry->next = NULL;
    if (pthead == NULL)
    {
        pthead = newentry;
    }
    else
    {
        struct pageTable *ptr = pthead;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        // insert new entry into page table
        ptr->next = newentry;
    }
    pagetablecount++;
    }
}
void deletePage(int pagenumber)
{
    struct Page *current = pagehead;
    struct Page *prev = NULL;

    if (current == NULL)
    {
        return;
    }

    while (current != NULL)
    {
        if (current->pagenumber == pagenumber)
        {
            if (prev != NULL)
            {
                prev->next = current->next;
            }
            else
            {
                pagehead = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}
void deletepaget(int pagenumber){
    struct pageTable *current = pthead;
    struct pageTable *prev = NULL;

    if (current == NULL)
    {
        return;
    }

    while (current != NULL)
    {
        if (current->pagenumber == pagenumber)
        {
            if (prev != NULL)
            {
                prev->next = current->next;
            }
            else
            {
                pthead = current->next;
            }
            free(current);
            pagetablecount--;
            return;
        }
        prev = current;
        current = current->next;
    }
    
}
void printpagetable()
{
    struct pageTable *temp = pthead;

    if (temp == NULL)
    {
        printf("Empty table\n");
    }
    else
    {
        printf("Page table entry is\n");
        printf("Page Number\tFrame Number\n");
        while (temp != NULL)
        {
            printf("%d\t\t%d\n", temp->pagenumber, temp->framenumber);
            temp = temp->next;
        }
    }
}
void printframe(int framenumber){
    int i=0;
    for(i=0;i<framenumber;i++){
        printf("%d ",physicalMemory[i]->pagenumber);
    }
}

int main()
{
    int pagenumber, choice,framenumber;
    int storagesize,nopages=0;
    printf("Enter the quantity of storage you have (in MB): ");
    scanf("%d", &storagesize);
    if (storagesize < 4)
    {
        printf("Memory is too low for storing the data. Please have additional storage.\n");
        return 0;
    }
    framenumber = storagesize / 4;
    printf("Number of frames: %d\n", framenumber);
    // printpagetable(); 
    while (1)
    {
        printf("\n1. Refer a  page number");
        printf("\n2. Print the page table");
        printf("\n3. number of page faults and page fault ratio");
        printf("\n4. Print the pages mapped  in frames[0-%d]",framenumber);
         printf("\n5. number of pagehit and  page hit ratio");
        printf("\nexit 0\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            printf("Please enter the page number: ");
            scanf("%d", &pagenumber);
            struct Page *newpage = createPage(pagenumber);
            insert_into_tablel(newpage,framenumber);
            printf("Page %d entred successful into page table.\n", pagenumber);
            nopages++;
            break;
        case 2:
            printf("===============================\n");
            printpagetable();
            break;
        case 3:
            printf("Number of page fault is %d",pagefault);
            printf("page fault ratio %d%% \n",(pagefault*100)/nopages);
            break;
        case 4:
            printframe(framenumber);
            break;
        case 5:
         printf("Number of page hit is %d\n",(nopages-pagefault));
            printf("page fault ratio %d%% \n",((nopages-pagefault)*100)/nopages);
            break;
        case 0:
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
  
    return 0;
}