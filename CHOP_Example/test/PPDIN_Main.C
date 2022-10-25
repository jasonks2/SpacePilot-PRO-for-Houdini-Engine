#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>	// usleep

#include "../PPD_PipeIn.h"

int
main (int argc, char *argv[])
{
    PPD_PipeIn		 cp;
    double		*samples;
    int			 i, len;
    
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
    
    while (1)
    {
	len = cp.getSamples(samples);
	if (len)
	{
	    fprintf(stderr, "Received %d samples \n\r", len);
	    for (i=0; i<len; i++)
		fprintf(stderr, "%g\n\r", samples[i]);
	}
	else
	    usleep(5*10000);		// keep us from spinning too quickly

    };
}


