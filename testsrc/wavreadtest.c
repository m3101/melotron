#include <stdio.h>
#include "../src/wavreader.h"

int main()
{
    unsigned int len,samplerate;
    short* data = readwave("sample.wav",&len,&samplerate);
    printf("Read!\n%u bytes of data, %u samples/second\n%hd%hd%hd\n",len,samplerate,data[0],data[1],data[2]);
    return 0;
}