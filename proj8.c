/*
Basics of program needed to complete assignment. 
This example processes the first logical address stored in addresses.txt.
It translates the address to a physical address and looks up that physical address in BACKING_STORE.bin
The output is identical to the first line of correct.txt
usage: ./a.out BACKING_STORE.bin addresses.txt
BACKING_STORE.bin and addresses.txt must bin the same directory as the executable

Your task is to complete the program for all virtual adresses in addresses.txt.  Think of a loop reading addresses.txt 
as a memory bus passsing logical addresses to a program.   

See Figure 8.34 on p. 433.   The page number/offset block is what comes from addresses.txt.   
Here are the conditions that must be accounted for:

1. logical addreess is in TLB
2. logical address in not in TLB but is in memory
3. logical address is neither in TLB nor in memory



while(there are more logical addresses)
{
 extract the page number and offset from the logical address. 
 Search the TLB for the page number.
 If the page number is in the TLB, add the offset to the frame number found in the TLB. This is the physical address. 
   Use it to extract data from memory.
   Print out according to instructions in the text. 
 If the page number is not in the TLB, use the page number as an offset into the page table.
 If the page number is in the page table, put the page number and the frame number in the TLB. Add the offset to the
   page frame number found in the page table.  This is the physical address.  Use it to extract data from memory. 
 If the page number is not in the page table, bring the page in from BACKING_STORE.bin. You do this by translating the 
   address, just as I do in the example below. Add it to memory.  Update the page table and TLB.   
   Print out according to instructions in the text. 
}

This program simulates demand paging.  This means that memory is initially empty.  The program will page fault until
memory is full.  Since memory is the same size as BACKING_STORE, you do not have to handle page replacement. TLB is 
another story.  If you access a page that is not in TLB and TLB is full, you must evict an entry in the TLB.  
This means you need an eviction strategy.  Use LRU.  Include an integer in the TLB that will store the most recent 
address reference.  For example, there are 1000 addresses stored in addresses.txt.  If you get a TLB  hit 
(or store an entry in the TBL) on 77th counting from 0, store 77 in the TLB struct for that page.  If you need to evict
a page from the TLB, overwrite the TLB entry with the lowest associated number. 
*/

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define PAGES 256
#define PAGE_MASK 255
#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMORY_SIZE PAGES * PAGE_SIZE 


/*
********Important Types, Data Structures and Variables************************ 
Structs
tlb: struct representing an entry in translation lookaside buffer 

Storage
backing: pointer to memory mapped secondary storage
main_memory: array of integers simulating RAM

Tables
tlb: array of type tlb representing the translation lookaside buffer
pagetable: array of signed chars simulating the page table

Counters
numPageRefs: number of page table references
numPageFaults: counts number of page faults
numTLBRefs: number of TLB tries
numTLBHits: number of TLB references that resulted in a hit

Addresses 
logical_address: address read from addresses.txt, simulating the address bus 
physical_page: page frame number
physical_address: bytes from the 0th byte of RAM, i.e., the actual physical address 
logical_page: page table number
offset: displacement within page table/frame 

Output
Virtual address: logical_address, in Addresses, above
Physical address: physical_address in Addresses, above 
value: value stored in main_memory at physical_page displacement plus offset
*/

struct tlbentry 
{
    unsigned char page_number;
    unsigned char frame_number;
    int tlb_ref;
};
typedef struct tlbentry tlbentry;  



int main(int argc, char *argv[])
{ 
    int pagetable[PAGES];
    tlbentry tlb[TLB_SIZE];
    signed char main_memory[MEMORY_SIZE];
    signed char *backing;
    int logical_address;
    int offset;
    int logical_page;
    int physical_page;
    int physical_address;
    int numPageFaults = 0;
    int numPageRefs = 0;
    int numTLBRefs = 0;
    int numTLBHits = 0;

    //open simulation of secondary storage     
    const char *backing_filename = argv[1]; 					//BACKING_STORE.bin 
    int backing_fd = open(backing_filename, O_RDONLY);
    backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0); 	//backing can be accessed as an array  

    //open simulation of address bus and read the first line 
    FILE *ifp = fopen(argv[2],"r"); 						//addresses.txt 

    while(fscanf(ifp,"%d", &logical_address) != EOF){

        /*
        If the page number is not in the TLB, use the page number as an offset into the page table.
        If the page number is in the page table, put the page number and the frame number in the TLB. Add the offset to the
        page frame number found in the page table.  This is the physical address.  Use it to extract data from memory. 
        If the page number is not in the page table, bring the page in from BACKING_STORE.bin. You do this by translating the 
        address, just as I do in the example below. Add it to memory.  Update the page table and TLB.   
        Print out according to instructions in the text. 
        */

        //extract low order 8 bits from the logical_address. 
        offset = logical_address & OFFSET_MASK;

        //extract bits 8 through 15. This is the page number gotten by shifting right 8 bits 
        logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK;

        //When you write the program, the physical page will increment by 1 for each copy
        //from simulated backing store to main memory 
        physical_page = 0; 

        //copy from secondary storage to simulated RAM. The address on secondary storage
        //is an offset into backing, computed by multiplying the logical
        //page number by 256 and adding the offset 
        memcpy(main_memory + physical_page * PAGE_SIZE, 
                backing + logical_page * PAGE_SIZE, PAGE_SIZE);

        //Shift the physical page left 8 bits and or with the offset
        //This has the effect of adding the offset to the physical_page
        physical_address = (physical_page << OFFSET_BITS) | offset;

        //extract the value stored at offset bytes within the page
        signed char value = main_memory[physical_page * PAGE_SIZE + offset];
            
        printf("Virtual address: %d Physical address: %d Value: %d\n", 
                logical_address, physical_address, value);
    }
    return 0;
}
