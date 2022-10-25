#include <stdlib.h>
#include <stdio.h>
#include <math.h>	// cos sin

#include <unistd.h>	// usleep



#include "../PPD_PipeOut.h"

int
main (int argc, char *argv[])
{
    PPD_PipeOut		 cp;
    double		 samples[2];
    float		 t = 0.0;

    if (argc != 2)
    {
	fprintf(stderr, "Usage: %s filename \n\r", argv[0]);
	return 1;
    }

    if (!cp.open(argv[1]))
    {
	fprintf(stderr, "Unable to open fifo %s \n\r", argv[1]);
	exit(1);
    }
    
    // Set the Channel Names

    cp.start_send_channel_names(2);
    cp.send_channel_name("tx");
    cp.send_channel_name("ty");
    cp.stop_send_channel_names();

    while (1)
    {
	usleep(5*10000);	// keep us from spinning too quickly

	// send a spot on a circle

	// in other situations, good idea to only send values when they've
	// changed from the previous set

	samples[0] = sinf(t);
	samples[1] = cosf(t);
	t += 0.1;


	cp.send_current_values(2, samples);
    };

}

