#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;

void PageTable::init_paging(ContFramePool * _kernel_mem_pool,ContFramePool * _process_mem_pool,const unsigned long _shared_size){
   kernel_mem_pool = _kernel_mem_pool;
   process_mem_pool = _process_mem_pool;
   shared_size = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable(){

	unsigned long frame = kernel_mem_pool->get_frames(1);
	long unsigned int * page_table = (long unsigned int*)(frame * PAGE_SIZE);
	unsigned long pdframe = kernel_mem_pool->get_frames(1);
   	page_directory = (long unsigned int*)(pdframe * PAGE_SIZE);
	unsigned long address = 0;
	unsigned int i;
	
	
	for(i = 0; i <1024; i++){
		page_table[i] = address | 3;
		address = address + 4096;
	}
	
	page_directory[0] = (unsigned long) page_table;
	page_directory[0] = page_directory[0] | 3;
	for(i = 1; i < 1024; i++){
		//Console::puts("New Entry Created\n");
		this->page_directory[i] = 0 | 2;
	}
    Console::puts("Constructed Page Table object\n");
}

void PageTable::load()
{
	current_page_table = this;
	write_cr3((unsigned long)current_page_table->page_directory);

   	Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
	write_cr0(read_cr0() | 0x80000000);
	current_page_table->paging_enabled = 1;
   	Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r){

	Machine::enable_interrupts();
	unsigned long address = read_cr2();
	unsigned long p_num = address >> 12;
	unsigned long p_index = address >> 22;

	if((current_page_table->page_directory[p_index] & 0x1)!=0x1){
		unsigned long * page = (unsigned long *) (kernel_mem_pool->get_frames(1)*PAGE_SIZE);
		for(int i = 0; i < PAGE_SIZE; i++){
			page[i]=0x2;
		}
	current_page_table->page_directory[p_index] = (unsigned long)page;
	current_page_table->page_directory[p_index] |= 0x3;
	
	}

unsigned long * page = (unsigned long *)(current_page_table->page_directory[p_index] & 0xFFFFF000);

p_index = (p_num) & (0x000003FF);

unsigned long new_frame = process_mem_pool->get_frames(1);

unsigned long new_frame_addr = new_frame * PAGE_SIZE;
page[p_index] = new_frame_addr | 0x3;

}

