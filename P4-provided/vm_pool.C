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
	unsigned long base_address;
	unsigned long size;
	ContFramePool * frame_pool;
	PageTable * page_table;	
	VMPool * VMPool::pools[5] = {NULL,NULL,NULL,NULL,NULL};
	int VMPool::numPools = 0;

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
    base_address = _base_address;
	size = _size;
	frame_pool = _frame_pool;
	page_table = _page_table;
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
    assert(false);
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

