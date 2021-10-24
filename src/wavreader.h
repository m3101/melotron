#ifndef WAVREADERH
#define WAVREADERH

/*
Reads a 16bit WAV file into a memory segment.
* Returns a pointer to the segment
* Sets the "len" pointer.
* Sets the "samplerate" pointer
*/
short* readwave(char* fname, unsigned int* len, unsigned int* samplerate);

#endif