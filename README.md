# Mel-O-Tron

A Mellotron simulator.

---

## Requirements

Requires `libsoundio` and `RtMidi`.

Install the packages with `sudo apt install libsoundio-dev librtmidi-dev`.

## Description

Simulates five artifacts commonly associated with the Mellotron sampler:

* Capstan Flutter - The slight, repetitive and higher frequency speed/frequency variation coming from the angular velocity of the driving mechanism of the tape
* Wow - Slow speed/frequency variations caused by irregular motion of the tape.
* Tape Saturation - Reduction of the response for high intensity signals.
* Noise - From dirt and imperfections on the tape
* Motor drag - Playing more than one note at once puts extra stress on the motors and makes them slow down slightly, detuning the notes.

## Examples

See `strawberry.mp3` and `callipeimprov.mp3`.