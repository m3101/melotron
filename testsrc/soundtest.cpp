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
#include "../src/player.hpp"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    Player* p = new Player();
    Sample* flute = new Sample("mel.wav",440,440,1);
    Sample* flute2 = new Sample("mel.wav",110*3,440,1);
    Sample* flute3 = new Sample("mel.wav",220,440,1);
    q_add(&p->q,flute);
    fcntl(fileno(stdin),O_NONBLOCK);
    unsigned char count=0;
    if(pid_t child = fork()) /*Main thread*/
    {
        printf("Type anything to stop\n");
        fflush(stdin);
        for(;;)
        {
            fgetc(stdin);
            if(errno!=EAGAIN)
            {
                switch (count)
                {
                case 0:
                    q_add(&p->q,flute2);
                    break;
                case 1:
                    q_add(&p->q,flute3);
                    break;
                case 2:
                    p->q->next->next->sample->stop=1;
                    break;
                }
                count++;
                printf("%hhu\ne",count);
                if(count>3)
                    break;
            }
        }
        kill(child,SIGKILL);
    }
    else /*Audio thread*/
    {
        for(;;)
        {
            //p->wait_sound_events();
        }
    }
}