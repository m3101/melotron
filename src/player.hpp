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

#ifndef PLAYERH
#define PLAYERH

#include <soundio/soundio.h>

/*
A sample
*/
class Sample
{
    private:
    public:
    unsigned int samplerate;
    double frequency;
    double basefrequency;
    double csample;
    double volume;
    unsigned int length;
    short* data;
    /*Flags the sample for removal*/
    unsigned char stop;
    Sample(unsigned int samplerate, double frequency, double basefrequency,unsigned int length,double volume,short* data);
    Sample(char* fname, double frequency,double basefrequency,double volume);
    ~Sample();
};

/*
A simple forward-linked list containing
the current playing sounds
*/
typedef struct queue{
    struct queue* next;
    Sample* sample;
}queue;

/*
Adds an element to the queue
*/
void q_add(queue** q, Sample* s);
/*
Returns the length of the queue
*/
unsigned char q_len(queue* q);

/*
Used internally for generating the final audio stream.
This is the core of the whole program.
*/
static void _generate_stream(struct SoundIoOutStream *outstream,
        int frame_count_min, int frame_count_max);

/*
Player parameters
*/
struct player_parameters
{
    /*
    Motor slowing factor
    Higher means less slowing
    */
    double motor_factor;
    /*
    Flutter frequency
    (generally around 2hz)
    */
    double flutter;
    /*
    The factor to be multiplied to the speed
    at maximum flutter
    */
    double flutterfactor;
    /*
    Wow randomness and range
    */
    double wow;
    /*
    Dirt chance
    */
    double dirtnoise;
    /*
    Saturation parameters
    */
    double cutoff,strength,curviness;
};

/*
A sound player
*/
class Player
{
    private:
    struct SoundIo* soundio;
    struct SoundIoDevice *device;
    struct SoundIoOutStream *outstream;

    public:
    double currentwow;
    double currentdwow;
    double time;
    struct player_parameters player_parameters;
    queue* q;
    /*
    Processes audio events.
    Should be called in a loop in its own thread.
    */
    void wait_sound_events();
    Player();
    ~Player();
};
#endif