/*
    Mel-O-Tron, a mellotron simulator
    Copyright (C) 2021 Amélia O. F. da S.
    <a.mellifluous.one@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
short* readwave(char* fname,unsigned int* len, unsigned int* samplerate)
{
    FILE* f = fopen(fname,"r");
    unsigned char buffer[4];
    if(!f)
        return NULL;
    fseek(f,0x08,SEEK_SET);
    fread(buffer,4,1,f);
    /*Not a wave file*/
    if(strcmp((char*)buffer,"WAVE"))
        return NULL;
    fseek(f,12,SEEK_CUR);
    fread(samplerate,1,4,f);
    fseek(f,6,SEEK_CUR);
    fread(buffer,2,1,f);
    /*Not 16bit*/
    if(*((unsigned short*)buffer)!=16)
        return NULL;
    /*Finds data section*/
    for(;;)
    {
        fread(buffer,4,1,f);
        if(!strcmp((char*)buffer,"data"))
            break;
        fread(buffer,4,1,f);
        fseek(f,*((unsigned int*)buffer),SEEK_CUR);
    }
    fread(len,4,1,f);
    short* data = (short*)malloc(*len);
    fread(data,*len,1,f);
    return data;
}