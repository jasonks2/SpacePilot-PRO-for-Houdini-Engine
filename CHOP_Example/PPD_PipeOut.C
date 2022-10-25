#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <endian.h>

#include "PPD_PipeOut.h"

#include <signal.h>
#include <X11/Xlib.h>
#include "spnav.h"

#define ESCAPE_CHARACTER        170
#define TOKEN_SIZE		(sizeof(int64_t))

// Utility methods to convert host system values to big endian.
static inline void
convert_htobe(int64_t &host64)
{
    uint64_t ube64 = htobe64(*(uint64_t *)&host64);
    host64 = *(int64_t *)&ube64;
}

static inline void
convert_htobe(double &host_double)
{
    uint64_t ube64 = htobe64(*(uint64_t *)&host_double);
    host_double = *(double *)&ube64;
}

PPD_PipeOut::PPD_PipeOut()
{
    myOutput = 0;
}

PPD_PipeOut::~PPD_PipeOut()
{
    close();
}

void
PPD_PipeOut::close()
{
    if (myOutput)
    {
	fclose(myOutput);
	myOutput = 0;
    }
}

int
PPD_PipeOut::open(const char *filename)
{
    mode_t               prev_mask;
    if (myOutput)
	close();

    prev_mask = ::umask(0);
    mkfifo (filename, 0666);
    ::umask(prev_mask);

    fprintf(stderr, "\n\rAwaiting reader for fifo %s \n\r", filename);
    myOutput =  fopen(filename, "wb");
    
	if (myOutput)
	fprintf(stderr, "Found reader\n\r");

    return myOutput ? 1:0;
}

void
PPD_PipeOut::write_byte(char b)
{
    if ((unsigned char)b == ESCAPE_CHARACTER)
	fputc(ESCAPE_CHARACTER, myOutput);  /* prepend escape character */
    fputc(b, myOutput);
}

void
PPD_PipeOut::write_values(const char *p, int size)
{
    int          i;

    if (myOutput)
    {
	assert( (size % TOKEN_SIZE) == 0 );
	for(i=0; i<size; i++)
	    write_byte(p[i]);
    }
}
  
void
PPD_PipeOut::send_reset()
{
    if (myOutput)
    {
	fputc(ESCAPE_CHARACTER, myOutput);
	fputc(0, myOutput);
	fputc(ESCAPE_CHARACTER, myOutput);
	fputc(0, myOutput);
	fputc(ESCAPE_CHARACTER, myOutput);
	fputc(0, myOutput);
	fputc(ESCAPE_CHARACTER, myOutput);
	fputc(0, myOutput);
	fflush(myOutput);
    }
}

void
PPD_PipeOut::send_current_values(int num, double *samples)
{
    int64_t	token;
    int		j;

    send_reset();		/* just to be safe */

    /* Command Type */
    token = 1;
    convert_htobe(token);
    write_values((char *)&token, sizeof(token));

    /* Number of Channels */
    token = num;
    convert_htobe(token);
    write_values((char *)&token, sizeof(token));

    /* Sample Values */
    for(j=0; j<num; j++)
    {
	double	 sample = samples[j];
	convert_htobe(sample);
	write_values((char *)&sample, sizeof(sample));
    }

    send_reset();		/* just to be safe */

    if (myOutput)
	fflush(myOutput);
}

void
PPD_PipeOut::send_upload(int length, int rate, int start, int channels)
{
    double	sample;
    int64_t	token;
    int64_t	length64 = length;
    int64_t	rate64 = rate;
    int64_t	start64 = start;
    int64_t	channels64 = channels;
    int		i, j;

    send_reset();		/* just to be safe */

    /* Command Type 2 */
    token = 2;
    convert_htobe(token);
    write_values((char *)&token, sizeof(token));

    /* Track Length */
    convert_htobe(length64);
    write_values((char *)&length64, sizeof(length64));

    /* Sample Rate */
    convert_htobe(rate64);
    write_values((char *)&rate64, sizeof(rate64));

    /* Start Index */
    convert_htobe(start64);
    write_values((char *)&start64, sizeof(start64));

    /* Number of Channels */
    write_values((char *)&channels64, sizeof(channels64));

    /* Samples */
    for (i=0; i<length; i++)
    {
	for (j=0; j<channels; j++)
	{
	    sample = i/10;
	    convert_htobe(sample);
	    write_values((char *)&sample, sizeof(sample));
	}
    }

    send_reset();		/* just to be safe */

    if (myOutput)
	fflush(myOutput);
}

void
PPD_PipeOut::start_send_channel_names(int n)
{
    int64_t	 token;

    send_reset();		/* just to be safe */

    /* Command Type 3 */
    token = 3;
    convert_htobe(token);
    write_values((char *)&token, sizeof(token));

    /* Number of Names */
    token = n;
    convert_htobe(token);
    write_values((char *)&token, sizeof(token));
}

void
PPD_PipeOut::send_channel_name(const char *name)
{
    int64_t	 token, num_tokens;

    /* Name Length */
    num_tokens = (int64_t) ceilf((double)strlen(name)/sizeof(token));
    token = num_tokens;
    convert_htobe(token);
    write_values((char *)&token, sizeof(token));

    /* Channel Name */
    write_values(name, num_tokens*sizeof(token));
}

void
PPD_PipeOut::stop_send_channel_names()
{
   send_reset();             /* just to be safe */
           
   if (myOutput)
       fflush(myOutput);

}

