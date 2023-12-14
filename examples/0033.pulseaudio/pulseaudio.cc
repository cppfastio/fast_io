#include<fast_io.h>
#include<fast_io_driver/pulseaudio.h>
#include<memory>

int main()
{
/*
Referenced from
https://www.freedesktop.org/software/pulseaudio/doxygen/simple.html
*/
	pa_sample_spec ss{
	.format=PA_SAMPLE_S16NE,
	.rate = 44100,
	.channels = 2};
	::fast_io::pulseaudio_simple_file psf(::fast_io::native_interface,
		nullptr,"fooapp",PA_STREAM_PLAYBACK,nullptr,
		"MUSIC",std::addressof(ss),nullptr,nullptr);
	for(;;)	//for loop so we can hear hello world
	{
		::fast_io::io::print(psf,"Hello World\n");
	}
}
/*
need to link -lpulse-simple
*/
