/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "machine.H"

extern Scheduler * SYSTEM_SCHEDULER;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

extern DISK_ID disk_id;
extern unsigned int disk_size;

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
	
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::wait_until_ready(){
	//while the thread is waiting to use IO operations
	while(!is_ready()){
		SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
		SYSTEM_SCHEDULER->yield();
	}
}

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
	/* Reads 512 Bytes in the given block of the given disk drive and copies them 
	to the given buffer. No error check! Non Looping!*/
	issue_operation(READ, _block_no);

	wait_until_ready();

	/* read data from port */
	int i;
	unsigned short tmpw;
	for (i = 0; i < 256; i++) {
		tmpw = Machine::inportw(0x1F0);
		_buf[i*2]   = (unsigned char)tmpw;
		_buf[i*2+1] = (unsigned char)(tmpw >> 8);
	}
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
	/* Writes 512 Bytes from the buffer to the given block on the given disk drive. Non Looping!*/
	issue_operation(WRITE, _block_no);

	wait_until_ready();

	/* write data to port */
	int i; 
	unsigned short tmpw;
	for (i = 0; i < 256; i++) {
		tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
		Machine::outportw(0x1F0, tmpw);
	}
}
