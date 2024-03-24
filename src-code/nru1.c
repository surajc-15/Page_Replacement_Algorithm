#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // Include for O_CREAT constant
#include <semaphore.h> // Include semaphore header
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
sem_t *sem; // declare semaphore variable
const char *sem_name = "/my_semaphore"; // name of the semaphore
// int framenumber = 0;

struct pageTable
{
    int framenumber;
    int pagenumber;
       int referencebit;
    int modifiedbit;
    struct pageTable *next;
};

struct pageTable *pthead = NULL;  // head pointer to page table
struct Page *physicalMemory[100]; // array storing the page memory address

void resetbit()
{
    struct pageTable *ptr = pthead;
    while (ptr != NULL)
    {
        ptr->referencebit=0;
        // ptr->modifiedbit=0;
        ptr = ptr->next;
    }
}
int checkpagehit(int pagenumber)
{
    struct pageTable *ptr = pthead;
    while (ptr != NULL)
    {
        if (ptr->pagenumber == pagenumber)
        {    
            
             ptr->referencebit=1;
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
int NRU ()
{
  struct pageTable *table =pthead ;
  unsigned int size = pagetablecount;
  unsigned int i;
  
  // class 0
  for (i = 0; i < size; i ++)
    {
      if (!table->referencebit && !table->modifiedbit)
        {
            
          return table->framenumber;
        }
        table=table->next;
    }
table =pthead ;
  // class 1
  for (i = 0; i < size; i ++)
    {
      if (!table->referencebit && table->modifiedbit)
        {
          return table->framenumber;
        }
         table=table->next;
    }
table =pthead ;
  // class 2
  for (i = 0; i < size; i ++)
    {
      if (table->referencebit && !table->modifiedbit)
        {
          return table->framenumber;
        }
         table=table->next;
    }
table =pthead ;
  // if we reach here, - all pages are class 3
  // just returning the first accessible one
  return table->framenumber;
}

void insert_into_table(struct Page *page,int framenumber)
{  
     int index;
     if(checkpagehit(page->pagenumber)!=0){ 
   
    if(getfreeframe(physicalMemory, framenumber)==-1){
         index=NRU();
         resetbit();
         printf("\npage | %d  |is replaced  page| %d |\n",page->pagenumber,physicalMemory[index]->pagenumber);
         deletepaget( physicalMemory[index]->pagenumber);
         
       }
    else{ 
        index = getfreeframe(physicalMemory, framenumber);
        };
    struct pageTable *newentry = (struct pageTable *)malloc(sizeof(struct pageTable));
    newentry->referencebit=1;
    newentry->modifiedbit=0;
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

void modifybit(int pagenumber)
{
    struct pageTable *temp = pthead;
    while (temp != NULL)
    {
        if (temp->pagenumber == pagenumber)
        {
            temp->modifiedbit = 1;
            break; // exit loop once modification is done
        }
        temp = temp->next;
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
            printf("|  %d  |\t\t|   %d   |\n", temp->pagenumber, temp->framenumber);
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
    int pagenumber, choice, framenumber;
    int storagesize, nopages = 0;

    // Create a named semaphore
    sem = sem_open(sem_name, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("Semaphore initialization failed");
        return 1;
    }

    printf("Enter the quantity of storage you have (in MB): ");
    scanf("%d", &storagesize);
    if (storagesize < 4)
    {
        printf("incefficent memory.\n");
        return 0;
    }
    framenumber = storagesize / 4;
    printf("Number of frames: %d\n", framenumber);

    while (1)
    {
        printf("\n1. Refer a new page number");
        printf("\n2. Modify a page");
        printf("\n3. Print the page table");
        printf("\n4. number of page faults and  page fault ratio");
        printf("\n5. Print the pages mapped  in frames[0-%d]", framenumber);
        printf("\n6. number of pagehit and  page hit ratio");
        printf("\nexit 0\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
             sem_wait(sem);
             printf("\nProcess is locked\n");
            printf("Please enter the page number: ");
            scanf("%d", &pagenumber);
            struct Page *newpage = createPage(pagenumber);
            insert_into_table(newpage, framenumber);
            printf("Page %d entered successfully into page table.\n", pagenumber);
            nopages++;
           printf("\nProcess is unlocked\n");
        sem_post(sem);
            break;
        case 2:
            sem_wait(sem);
        printf("\nProcess is locked\n");
            printf("Please enter the page number: ");
            scanf("%d", &pagenumber);
            if (checkpagehit(pagenumber) != 0)
            {
                printf("The page you want is not present.\n");
                struct Page *newpage = createPage(pagenumber);
                insert_into_table(newpage, framenumber);
                modifybit(pagenumber);
                printf("Page entry successful into page table.\n");
                nopages++;
            }
            else
            {
                modifybit(pagenumber);
            }
             printf("\nProcess is unlocked\n");
        sem_post(sem);
            break;
        case 3:
            printf("===============================\n");
            printpagetable();
            break;
        case 4:
            printf("Number of page fault is %d\n", pagefault);
            printf("page fault ratio %d%% \n", (pagefault * 100) / nopages);
            break;

        case 5:
            printframe(framenumber);
            break;
        case 6:
            printf("Number of page hit is %d\n", (nopages - pagefault));
            printf("page hit ratio %d%% \n", ((nopages - pagefault) * 100) / nopages);
            break;

        case 0:
           
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
        }

      
    }

   // sem_close(sem); // close semaphore
    //sem_unlink(sem_name); // unlink semaphore
    return 0;
}

