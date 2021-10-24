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

#include <stdio.h>
#include "player.hpp"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <rtmidi/RtMidi.h>
#include <math.h>

Player* p;
unsigned char ref = 64;
Sample*keys[108-21];

#define RATIO 1.0594630943592953d

void play( double deltatime, std::vector< unsigned char > *message, void *userData )
{
    unsigned int nBytes = message->size();
    unsigned char action = message->at(0);
    unsigned char key = message->at(1);
    printf("%hhu %hhu\n",action,key);
    double delta;
    if(action==144)//press
    {
        delta=key;
        delta-=ref;
        printf("%.2lf\n",delta);
        if(key>64)
            keys[key-21] = new Sample("E-Mu-Proteus-2-Flute-C5.wav",440*pow(RATIO,delta),440,1);
        else
            keys[key-21] = new Sample("sample.wav",440*pow(RATIO,delta),440*RATIO,1);
        q_add(&p->q,keys[key-21]);
    }
    else if(action==128)//release
    {   
        keys[key-21]->stop=1;
    }
}

int main()
{
    for(int i=0;i<108-21;i++)
        keys[i]=0;

    p = new Player();
    
    RtMidiIn *midiin = new RtMidiIn();

    midiin->openPort(1);
    midiin->setCallback(&play);
    midiin->ignoreTypes( false, false, false );

    fcntl(fileno(stdin),O_NONBLOCK);
    printf("Type anything to stop\n");
    fflush(stdin);
    for(;;)
    {
        fgetc(stdin);
        if(errno!=EAGAIN)
        break;
    }
    delete midiin;
    return 0;
}