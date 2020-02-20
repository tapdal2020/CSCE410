/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    //bitmap needs to fit inside of the frame
    assert(_n_frames <= FRAME_SIZE * 8);

    base_frame_no = _base_frame_no;
    n_frames = _n_frames;
    nFreeFrames = _n_frames;
    info_frame_no = _info_frame_no;
    
    // If _info_frame_no is zero then we keep management info in the first
    //frame, else we use the provided frame to keep management info
    if(info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE);
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }
    
    // Number of frames must be "fill" the bitmap!
    assert ((n_frames % 8 ) == 0);
    
    // Everything ok. Proceed to mark all bits in the bitmap
    for(int i=0; i*8 < n_frames; i++) {
        bitmap[i] = 0xFF;
    }
    
    // Mark the first frame as being used if it is being used
    if(_info_frame_no == 0) {
        bitmap[0] = 0b00111111;
        nFreeFrames--;
    }
    Console::puts("Frame Pool initialized\n");
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames){
    //any frames left to allocate?
    assert(nFreeFrames > 0);

    // Find a frame that is not being used and return its frame index.
    // Mark that frame as being used in the bitmap.
    unsigned int frame_no = base_frame_no;
    
    bool isSpace = false;
    while(!isSpace){
        unsigned int bitmap_index = (frame_no - base_frame_no)/4;
        unsigned char mask = 0b11000000 >> ((frame_no - base_frame_no) % 4);

        if(bitmap[bitmap_index] & mask == mask){
            isSpace = check_sequence(frame_no, _n_frames);
        }
    }
    mark_inaccessible(base_frame_no,_n_frames,frame_no);
    return (frame_no);
}

bool ContFramePool::check_sequence(unsigned long first_frame, unsigned int n_frames){

	for(int i = 0; i < n_frames; i++){
		
	    unsigned int bitmap_index = ((first_frame + i - base_frame_no) / 4);
	    unsigned char mask = 0b11000000 >> (((first_frame + i - base_frame_no) % 4)*2);
	    
	    // Is the frame being used already?
	    if((bitmap[bitmap_index] & mask) != mask){
		    return false;
	    }
	}
    return true;
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,unsigned long _n_frames, unsigned long frame_no){
     // Let's first do a range check.
    assert (frame_no >= base_frame_no);

	for(int i = 0; i < _n_frames; i++){
    
	    unsigned int bitmap_index = ((frame_no + i - _base_frame_no) / 4);
	    unsigned char mask = 0b11000000 >> ((frame_no + i - _base_frame_no) % 4)*2;
	    
	    // Update bitmap
	    bitmap[bitmap_index] ^= mask;
	    nFreeFrames--;
	}
}

void ContFramePool::release_frames(unsigned long frame_no){
/*
    //make sure that info frame is not being released.
    assert(frame_no != ContFramePool::info_frame_no);

    unsigned int bitmap_index = ((frame_no - ContFramePool::base_frame_no) /4);
    unsigned int offset = (((frame_no - ContFramePool::base_frame_no)%4)*2);

    unsigned char mask = 0b00111111 >> offset;

    //Make sure the frame is start of sequence
    assert(ContFramePool::bitmap[bitmap_index] | mask != mask);
    //modify the bitmap to change the first frame
    ContFramePool::bitmap[bitmap_index] ^= (0b11000000 >> ((frame_no - base_frame_no)%4)*2);
    //checker to see if we have reached the end of sequence
    bool eos = false;
    unsigned int i = 1;
    while(!eos){
    	bitmap_index = ((frame_no + i - ContFramePool::base_frame_no) /4);
	offset = (((frame_no + i - ContFramePool::base_frame_no)%4)*2);
    	mask = 0b01000000 >> offset;
	//checks for new beginning of sequence or unallocated frame
	if(ContFramePool::bitmap[bitmap_index] & mask != mask){
		eos = true;
	}else{
		//update bitmap
		ContFramePool::bitmap[bitmap_index] ^= (0b10000000 >> offset);
		nFreeFrames++;
	}

	i++;
 
    }*/
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
   unsigned long result = _n_frames / 32 + (_n_frames % 32 > 0 ? 1 : 0);
   return result;
}
