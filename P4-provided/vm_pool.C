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
<<<<<<< HEAD

unsigned long base_address;
unsigned long size;
ContFramePool * frame_pool;
PageTable * page_table;
static unsigned long * regions[512];
int allocations;
=======
	unsigned long base_address;
	unsigned long size;
	ContFramePool * frame_pool;
	PageTable * page_table;	
	VMPool * VMPool::pools[5] = {NULL,NULL,NULL,NULL,NULL};
	int VMPool::numPools = 0;
>>>>>>> fa0c733fe79dac999a03ed275d788f2270550f58

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
<<<<<<< HEAD
   	base_address = _base_address;
   	size = _size;
   	frame_pool = _frame_pool;
   	page_table = _page_table;
   	allocations = 0;
   	
   	regions = (unsigned long*) base_address;
   	base_address+=Machine::PAGE_SIZE;
   	
=======
    base_address = _base_address;
	size = _size;
	frame_pool = _frame_pool;
	page_table = _page_table;
>>>>>>> fa0c733fe79dac999a03ed275d788f2270550f58
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
	
    Console::puts("Allocated region of memory.\n");
}

void VMPool::release(unsigned long _start_address) {
   assert(false);
   Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
    if(_address < base_address && base_address + size > _address){
		return true;
	}else{
		return false;
	}
    Console::puts("Checked whether address is part of an allocated region.\n");
}

