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
#include "player.hpp"
#include "wavreader.h"
#include <soundio/soundio.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14
#define TPI 6.28

/*SAMPLE*/

Sample::Sample(unsigned int samplerate, double frequency, double basefrequency,unsigned int length,double volume,short* data)
{
    this->csample=0;
    this->data=data;
    this->samplerate=samplerate;
    this->frequency=frequency;
    this->basefrequency=basefrequency;
    this->volume=volume;
    this->length=length;
    this->stop=0;
}
Sample::Sample(char* fname, double frequency, double basefrequency,double volume)
{
    this->csample=0;
    this->frequency=frequency;
    this->basefrequency=basefrequency;
    this->stop=0;
    this->volume=volume;
    this->data = readwave(fname,&this->length,&this->samplerate);
}
Sample::~Sample()
{
    free(this->data);
}

/*QUEUE*/
void q_add(queue**q,Sample*s)
{
    queue *c=*q;
    if(c)
    {
        while(c->next!=NULL)
            c=c->next;
        c->next=(queue*)malloc(sizeof(queue));
        c=c->next;
        c->next=NULL;
        c->sample=s;
    }
    else
    {
        *q=(queue*)malloc(sizeof(queue));
        (*q)->next=NULL;
        (*q)->sample=s;
    }
}
unsigned char q_len(queue*q)
{
    unsigned char l=0;
    while(q)
    {
        l+=1;
        q=q->next;
    }
    return l;
}

/*PLAYER*/

static void _generate_stream(struct SoundIoOutStream *outstream,
        int frame_count_min, int frame_count_max)
{
    //printf("%d\n%d\n########\n",frame_count_max,frame_count_min);
    const struct SoundIoChannelLayout *layout = &outstream->layout;
    float float_sample_rate = outstream->sample_rate;
    float seconds_per_frame = 1.0f / float_sample_rate;
    struct SoundIoChannelArea *areas;
    int err;
    
    double cutoff=((Player*)outstream->userdata)->player_parameters.cutoff;
    double strength=((Player*)outstream->userdata)->player_parameters.strength;
    double curviness=((Player*)outstream->userdata)->player_parameters.curviness;
    double sat0=log10(curviness);

    if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count_max))) {
        fprintf(stderr, "%s\n", soundio_strerror(err));
        exit(1);
    }
    //printf("%.2lf\n",((Player*)outstream->userdata)->currentwow);
    for(unsigned int frame=0;frame<frame_count_max;frame++)
    {
        short sample=0;

        /*For each sample*/
        queue*prev=NULL;
        queue*q = ((Player*)outstream->userdata)->q;
        unsigned char l = q_len(q);

        double acc=0;
        unsigned int pos;
        double length;
        double slow = 1+(l-1)/((Player*)outstream->userdata)->player_parameters.motor_factor;

        double deltawow = ((rand()%1000)/500-1)*.01;
        ((Player*)outstream->userdata)->currentdwow =deltawow*((Player*)outstream->userdata)->player_parameters.wow+(1-((Player*)outstream->userdata)->currentwow)*(1-((Player*)outstream->userdata)->player_parameters.wow);
        ((Player*)outstream->userdata)->currentwow+=((Player*)outstream->userdata)->currentdwow;
        ((Player*)outstream->userdata)->currentwow = ((Player*)outstream->userdata)->currentwow>1.1?1.1:((Player*)outstream->userdata)->currentwow;
        ((Player*)outstream->userdata)->currentwow = ((Player*)outstream->userdata)->currentwow<.9?.9:((Player*)outstream->userdata)->currentwow;

        ((Player*)outstream->userdata)->time+=seconds_per_frame;
        ((Player*)outstream->userdata)->time=fmod(((Player*)outstream->userdata)->time,1/((Player*)outstream->userdata)->player_parameters.flutter);

        double flutter = (1+sin(TPI*((Player*)outstream->userdata)->time*((Player*)outstream->userdata)->player_parameters.flutter))/2;
        double intermediate;
        while(q!=0)
        {
            if(q==0)
                break;
            if (q->sample->stop)
            {
                if(q->sample->volume>0.01)
                {
                    q->sample->volume-=.0005;
                    q->sample->volume=q->sample->volume<0.01?0:q->sample->volume;
                }
                else
                {
                    if(!prev)
                    {
                        if(!q->next)
                        {
                            delete ((Player*)outstream->userdata)->q;
                            ((Player*)outstream->userdata)->q=NULL;
                            break;
                        }
                        else
                        {
                            ((Player*)outstream->userdata)->q=q->next;
                            delete q;
                            q=((Player*)outstream->userdata)->q;
                        }
                    }
                    else
                    {
                        if(!q->next)
                            prev->next=NULL;
                        else
                            prev->next=q->next;
                        delete q;
                        q=NULL;
                    }
                    continue;
                }
            }
            double interval = 2*(q->sample->samplerate/float_sample_rate);
            interval/= q->sample->basefrequency/q->sample->frequency;
            interval/= slow;
            interval*=((Player*)outstream->userdata)->player_parameters.flutterfactor*flutter+(1-flutter);
            interval*=((Player*)outstream->userdata)->currentwow;
            pos = q->sample->csample;
            intermediate=(q->sample->volume*.5*q->sample->data[pos]/128.0);
            if (((rand()%100000)/100000.0)<((Player*)outstream->userdata)->player_parameters.dirtnoise/float_sample_rate)
                intermediate+=128*((rand()%100)/100)-128/2;
            //acc+=intermediate;
            ///*
            if(intermediate>0)
                acc+=128*(strength*cutoff*(log10(intermediate+curviness)-sat0)+(1-strength)*intermediate);
            else
                acc+=-128*(strength*cutoff*(log10(-intermediate+curviness)-sat0)+(1-strength)*-intermediate);
            //*/
            q->sample->csample+=interval;
            length=q->sample->length/2;
            q->sample->csample = fmod(q->sample->csample,length);
            prev = q;
            q = q->next;
        }
        
        sample = l>0?acc:0;

        for (int channel = 0; channel < layout->channel_count; channel += 1) {
            short *ptr = (short*)(areas[channel].ptr + areas[channel].step * frame);
            *ptr = sample;
        }
    }
    soundio_outstream_end_write(outstream);
}

Player::Player()
{
    int err;
    this->player_parameters.motor_factor=120;
    this->player_parameters.dirtnoise=0.0000005;
    this->player_parameters.flutter=2;
    this->player_parameters.wow=.7;
    this->player_parameters.flutterfactor=.985;
    this->player_parameters.strength=0.7;
    this->player_parameters.curviness=2;
    this->player_parameters.cutoff=3.3;
    this->currentwow=0;
    this->currentdwow=0;
    this->time=0;
    this->soundio = soundio_create();
    if ((err = soundio_connect(this->soundio))) {
        fprintf(stderr, "error connecting: %s\n", soundio_strerror(err));
        exit(1);
    }
    soundio_flush_events(this->soundio);
    int default_out_device_index = soundio_default_output_device_index(this->soundio);
    this->device = soundio_get_output_device(this->soundio, default_out_device_index);
    this->outstream = soundio_outstream_create(this->device);
    outstream->userdata=this;
    outstream->format = SoundIoFormatS16LE;
    outstream->write_callback = _generate_stream;
    outstream->sample_rate = 44100;
    outstream->software_latency=0.1;
    soundio_outstream_open(this->outstream);
    if ((err = soundio_outstream_start(this->outstream))) {
        fprintf(stderr, "unable to start device: %s\n", soundio_strerror(err));
        exit(1);
    }
}

void Player::wait_sound_events()
{
    soundio_wait_events(this->soundio);
}

Player::~Player()
{
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
}