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
   current_page_table = new PageTable();
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable(){

   unsigned long * page_directory;
   unsigned long * page_table;
   
   page_directory = (unsigned long *) 0x9C000;
   page_table = (unsigned long *) 0x9D000;

   unsigned long address = 0;
   unsigned int i;

   //fill the first entry of the page directory
   page_directory[0] = *page_table;
   page_directory[0] = page_directory[0] | 3; //attribute set to: supervisor level, read/write, present
   for(i = 0; i < 1024; i++){
      page_table[i] = address | 3; //attribute set to: supervisor level, read/write, present
      address = address + 4096;
   }
   //map the first 4MB of memory
   for(i = 1; i < 1024; i++){
      page_directory[i] = 0 | 2; //attribute set to: supervisor level, read/write, not present
   }
   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
   write_cr3(*(current_page_table->page_directory)); //put the page directory address into CR3
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   write_cr0(read_cr0() | 0x80000000); // set the paging bit in CR0 to 1
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  assert(false);
  Console::puts("handled page fault\n");
}

