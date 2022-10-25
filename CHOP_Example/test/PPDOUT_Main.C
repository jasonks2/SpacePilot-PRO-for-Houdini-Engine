#include <stdlib.h>
#include <stdio.h>
#include <math.h>	// cos sin

#include <unistd.h>	// usleep

#include "../PPD_PipeOut.h"



void sig(int s)
{
        spnav_close();
        exit(0);
}

int
main (int argc, char *argv[])
{
    PPD_PipeOut		 cp;
    double 		 space_samples[5]; // size for 6dof controller
    spnav_event          sev;              // handles controller states
    float		 t = 0.0;

    signal(SIGINT, sig); //spacenavd signal

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
    cp.start_send_channel_names(7);

    cp.send_channel_name("tx");
    cp.send_channel_name("ty");
    cp.send_channel_name("tz");
    cp.send_channel_name("rx");
    cp.send_channel_name("ry");
    cp.send_channel_name("rz");
    cp.send_channel_name("button");

    cp.stop_send_channel_names();



    while (1)
    {
	
	if (spnav_open() == 1)
	{
		printf("Port opened \n");
	}	

	usleep(5*10000);	
	while (spnav_wait_event(&sev))
	{

		if(sev.type == SPNAV_EVENT_MOTION)
	       	{
			space_samples[0] = sev.motion.x;
                      	space_samples[1] = sev.motion.y;
                      	space_samples[2] = sev.motion.z;
                      	space_samples[3] = sev.motion.rx;
                      	space_samples[4] = sev.motion.ry;
                        space_samples[5] = sev.motion.rz;
			space_samples[6] = -1;
                        printf("got motion event: t(%d, %d, %d) ", sev.motion.x, sev.motion.y, sev.motion.z);
                        printf("r(%d, %d, %d)\n", sev.motion.rx, sev.motion.ry, sev.motion.rz);
			cp.send_current_values(7,space_samples);
                }

	       	else 
		{        /* SPNAV_EVENT_BUTTON */
                
		space_samples[6] = sev.button.bnum;
		printf("got button %s event b(%d)\n", sev.button.press ? "press" : "release", sev.button.bnum);
		cp.send_current_values(7,space_samples);
		
		}
	}

        // in other situations, good idea to only send values when they've
        // changed from the previous set
   
    };
    //spnav_close();
}

