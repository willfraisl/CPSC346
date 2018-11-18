/*
Class: CPSC 346-02
Team Member 1: Will Fraisl
Team Member 2: N/A 
GU Username of project lead: wfraisl
Pgm Name: proj8.c
Pgm Desc: simulates virtual memory using page table
Usage: ./a.out BACKING_STORE.bin addresses.txt
Due Date: 11/20/18
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

int main(int argc, char *argv[])
{ 
    int pagetable[PAGES];
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
    int i;
    int found;
    int address_count = 0;

    //open simulation of secondary storage     
    const char *backing_filename = argv[1]; 					//BACKING_STORE.bin 
    int backing_fd = open(backing_filename, O_RDONLY);
    backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0); 	//backing can be accessed as an array  

    //open simulation of address bus and read the first line 
    FILE *ifp = fopen(argv[2],"r"); 						//addresses.txt 

    for(i=0; i<256; i++){
        pagetable[i] = 0;
    }

    physical_page = 0; 

    while(fscanf(ifp,"%d", &logical_address) != EOF){
        numPageRefs++;

        //extract low order 8 bits from the logical_address. 
        offset = logical_address & OFFSET_MASK;

        //extract bits 8 through 15. This is the page number gotten by shifting right 8 bits 
        logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK;
        
        found = 0;

        //search through pagetable for logical_address
        for(i=0; i<=physical_page; i++){
            if(pagetable[i] == logical_page){
                found = 1;
                physical_address = (i << OFFSET_BITS) | offset;
                break;
            }
        }

        if(found == 0){
            numPageFaults++;
            //copy from secondary storage to simulated RAM. The address on secondary storage
            //is an offset into backing, computed by multiplying the logical
            //page number by 256 and adding the offset 
            physical_page++;
            memcpy(main_memory + physical_page * PAGE_SIZE, 
                    backing + logical_page * PAGE_SIZE, PAGE_SIZE);

            //Shift the physical page left 8 bits and or with the offset
            //This has the effect of adding the offset to the physical_page
            physical_address = (physical_page << OFFSET_BITS) | offset;
            
            pagetable[physical_page] = logical_page;
        }

        //extract the value stored at offset bytes within the page
        signed char value = main_memory[physical_page * PAGE_SIZE + offset];
            
        printf("Virtual address: %d Physical address: %d Value: %d\n", 
                logical_address, physical_address, value);
    }
    printf("Number of Translated Addresses = %d\n", numPageRefs);
    printf("Page Faults = %d\n", numPageFaults);
    printf("Page Fault Rate = %f\n", (float)numPageFaults/numPageRefs);
    return 0;
}
