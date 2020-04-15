/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "page_table.H" 
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "paging_low.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/


unsigned long base_address;
unsigned long size;
ContFramePool * frame_pool;
PageTable * page_table;
static unsigned long * regions[512];
int allocations;

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
   	base_address = _base_address;
   	size = _size;
   	frame_pool = _frame_pool;
   	page_table = _page_table;
   	allocations = 0;
   	
   	page_table->PageTable::register_pool(this);
   	
   	base_address+=Machine::PAGE_SIZE;
   	
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
	int count = 0;
	for(int i = 0; i < 512; i++){
		if(count == _size){
			unsigned long address = (read_cr3() << 10);
			unsigned long val = (1 + (_size << 1));
			for(int j = 0; j < _size; j++){
				regions[i - count + j] = (unsigned long *)(address + val);
			}
			Console::puts("Allocated region of memory.\n");
			return address >> 10;
		}else if(count < _size){
			if(regions[i] != NULL){
				count++;
			}else{
				count = 0;
			}
		}
	}
}

void VMPool::release(unsigned long _start_address) {
   	for(int i = 0; i < 512; i++){
   		if((unsigned long)regions[i] >> 10 == _start_address){
   			unsigned long size = ((unsigned long)regions[i] & 1023) >> 1; //get the size value alone
   			for(int j = 0; j < size; j++){
				regions[i+j] == NULL;
			}
		Console::puts("Released region of memory.\n");
		return;
   		}	
   	}
}

bool VMPool::is_legitimate(unsigned long _address) {
    if(_address > base_address && base_address + size > _address){
		return true;
	}else{
		return false;
	}
    Console::puts("Checked whether address is part of an allocated region.\n");
}

