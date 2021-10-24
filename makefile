main:
	@if [ -d build ]; then\
		rm -rf build;\
	fi
	@mkdir build
	g++ -o build/mel_o_tron src/main.cpp src/player.cpp src/wavreader.c -lsoundio -lm -lrtmidi -g
tests:
	@if [ -d build ]; then\
		rm -rf build;\
	fi
	@mkdir build
	gcc -o build/wavtest testsrc/wavreadtest.c src/wavreader.c -g
	g++ -o build/soundtest testsrc/soundtest.cpp src/player.cpp src/wavreader.c -lsoundio -lm -g
	g++ -o build/miditest testsrc/miditest.cpp -lrtmidi -g