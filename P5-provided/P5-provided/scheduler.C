/*
 File: scheduler.C
 
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

#include "scheduler.H"
#include "thread.H"
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
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/


Scheduler::Scheduler() {
  	Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  	//get the next thread going to the CPU
	if(rl.head !=NULL){
		Thread * next = rl.pop();
		//resume(Thread::CurrentThread());
		//send CPU control to the next thread
		Thread::dispatch_to(next);
	}else{
		return;
	}
}

void Scheduler::resume(Thread * _thread) {
 	rl.add_node(_thread);
}

void Scheduler::add(Thread * _thread) {
  	rl.add_node(_thread);
}

void Scheduler::terminate(Thread * _thread) {
	if(rl.head !=NULL){	
		Thread * next = rl.pop();
		Thread::dispatch_to(next);
	}else{
		Console::puts("No More Threads!\n");
	}
}	
