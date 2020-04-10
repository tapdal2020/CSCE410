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
Console::puts("Enter Page Table Constructor\n");
	unsigned long frame = kernel_mem_pool->get_frames(1);
	long unsigned int * page_table = (long unsigned int*)(frame * PAGE_SIZE);
	unsigned long pdframe = kernel_mem_pool->get_frames(1);
   	page_directory = (long unsigned int*)(pdframe * PAGE_SIZE);
	unsigned long address = 0;
	unsigned int i;
	
	Console::puts("Map first 4MB of Memory\n");
	for(i = 0; i <1024; i++){
		page_table[i] = address | 3;
		address = address + 4096;
	}
	Console::puts("Fill first entry of page directory\n");
	page_directory[0] = (unsigned long) page_table;
	page_directory[0] = page_directory[0] | 3;
	Console::puts("Filling rest of page directory\n");
	for(i = 1; i < 1024; i++){
		//Console::puts("New Entry Created\n");
		this->page_directory[i] = 0 | 2;
	}
	page_directory[1023] = (unsigned long)page_directory;
	page_directory[1023] |= 3;
    Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
	Console::puts("Entered load function\n");
	Console::puts("Set current_page_table\n");
	current_page_table = this;
	Console::puts("Load current page table's page directory into CR3\n");
	write_cr3((unsigned long)current_page_table->page_directory);

   	Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
	write_cr0(read_cr0() | 0x80000000);
	current_page_table->paging_enabled = 1;
   	Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
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

	unsigned long new_frame = kernel_mem_pool->get_frames(1);

	unsigned long new_frame_addr = new_frame * PAGE_SIZE;
	page[p_index] = new_frame_addr | 0x3;
}

void PageTable::register_pool(VMPool * _pool){
	VMPool::pools[VMPool::numPools] = _pool;
	VMPool::numPools++;
}

void PageTable::free_page(unsigned long _page_no){
	unsigned long address = (_page_no*PAGE_SIZE);
	unsigned long i = check_address(address);
	VMPool Pool = *VMPool::pools[i];
	Pool.release(address);
}

unsigned long PageTable::check_address(unsigned long address){
	for(int i = 0; i < VMPool::numPools; i++){
		VMPool Pool = *VMPool::pools[i];
		if(Pool.is_legitimate(address)){
			return i;
		}
	}
}

