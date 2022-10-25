/*
*
* COMMENTS:
*     Pipes values in from input devices and other programs.
*     Reads from a fifo and accepts commands in the
*     following formats where each value is a four byte int
*     or float:
*
*     (int)   1 - command type
*     (int)   - number of channels
*     (float) - one sample value for each channel
*
*     Other command types are ignored until a reset is detected.
*
*     An escape character (170) can be sent, followed by any
*     other character, to stop the current command parsing and
*     begin again.  If the 170-170 sequence is sent, the first
*     character is ignored and the value is sent normally.
*/


#include <stdio.h>
#include <unistd.h>
#include <endian.h>	// sleep

#include "PPD_PipeIn.h"

#define MAX_FIFO_READ           1000	// maximum bytes to read at once

// Utility methods to convert big endian values to the host system.
static inline void
convert_betoh(int64_t &host64)
{
    uint64_t ube64 = be64toh(*(uint64_t *)&host64);
    host64 = *(int64_t *)&ube64;
}

static inline void
convert_betoh(double &host_double)
{
    uint64_t ube64 = be64toh(*(uint64_t *)&host_double);
    host_double = *(double *)&ube64;
}

PPD_PipeIn::PPD_PipeIn()
{
    mySamples = 0;
    mySampleSize = 0;
    myInput = 0;
    reset();
}

PPD_PipeIn::~PPD_PipeIn()
{
    delete [] mySamples;
    close();
}

void
PPD_PipeIn::close()
{
    if (myInput)
    {
	fclose(myInput);
	myInput = 0;
    }
}

int
PPD_PipeIn::open(const char *filename)
{
    if (myInput)
	close();

    myInput =  fopen(filename, "rb");
    while (!myInput)
    {
	fprintf(stderr, "\n\rPPD_PipeIn:Awaiting writer for fifo %s \n\r", filename);
	sleep(5);	// 5 seconds
	myInput =  fopen(filename, "rb");
    }


    // always true

    if (myInput)
    {
	fprintf(stderr, "PPD_PipeIn:Found reader\n\r");
	fprintf(stderr, "----------LETS GET TO READING----------\n");
	reset();
    }

    return myInput ? 1:0;
}

int
PPD_PipeIn::getSamples(double *&a)
{
    receiveBytes();

    if (mySamplesReceived)
    {
	a = mySamples;
	mySamplesReceived = 0;
	return mySampleSize;
    }

    return 0;
}

///////////////////////////////////////////////////////////

#define ESCAPE_CHARACTER	((char)170)

enum {
    STATE_COMMAND,
    STATE_CHANNELS,
    STATE_SAMPLES
};


void
PPD_PipeIn::resize(int a)
{
    if (mySampleSize != a)
    {
	delete [] mySamples;
	mySampleSize = a;
	mySamples = new double[mySampleSize];
    }
}

void
PPD_PipeIn::reset()
{
    int		i;

    myCount = 0;
    myEscapeFlag = 0;
    myResetFlag = 0;
    myState = STATE_COMMAND;
    myCurrentSample = 0;
    mySamplesReceived = 0;
    myWaitingForReset = 0;

    for(i=0; i<TOKEN_SIZE; i++)
	myByteValues[i] = 0;
}

int
PPD_PipeIn::getToken(double &f)
{
    myCount = 0;

    if (myWaitingForReset)
    {
	if(myResetFlag)
	{
	    myResetFlag = 0;
	    myWaitingForReset = 0;
	}
	return -1;
    }

    if(myResetFlag)
    {
	myResetFlag = 0;
	myWaitingForReset = 0;
	return -1;
    }

    f = *(double *)myByteValues;
    convert_betoh(f);
    return 0;
}

int
PPD_PipeIn::getToken(int64_t &i)
{
    myCount = 0;

    if (myWaitingForReset)
    {
	if(myResetFlag)
	{
	    myResetFlag = 0;
	    myWaitingForReset = 0;
	}
	return -1;
    }

    if(myResetFlag)
    {
	myResetFlag = 0;
	return -1;
    }

    i = *(int64_t *)myByteValues;
    convert_betoh(i);
    return 0;
}

void
PPD_PipeIn::addByte(char b)
{
    // Set the flag if this is the first escape character.
    if(b == ESCAPE_CHARACTER && !myEscapeFlag)
    {
	myEscapeFlag = 1;
	return;
    }

    // Received reset sequence, set the appropriate flags.
    if(myEscapeFlag && b != ESCAPE_CHARACTER)
    {
	myEscapeFlag = 0;
	myResetFlag = 1;
	myCount = TOKEN_SIZE;
	return;
    }

    // Otherwise, add the byte to the token.
    myEscapeFlag = 0;
    if (myCount < TOKEN_SIZE)           // just in case
    {
	myByteValues[myCount] = b;
	myCount++;
    }

    return;
}


void
PPD_PipeIn::receiveBytes()
{
    int			i;
    int			a;

    // Read and process one char at a time until all the samples received
    // or many bytes have been read, in which case parsing will resume
    // at the next call

    if (myInput)
    {
	for (i=0; i<MAX_FIFO_READ; i++)
	{
	    a = fgetc(myInput);
	    if (a == EOF)
		break;

	    addByte(a);
	    if (myCount == TOKEN_SIZE)
		processToken();

	    if (mySamplesReceived)
		break;
	}
    }
}

void
PPD_PipeIn::processToken()
{
    double		sample;
    int64_t		token;

    // State machine - loop for one token

    switch(myState)
    {
	// The first token in the command determines the command type.
	case STATE_COMMAND:

	    if (getToken(token) >= 0)
	    {
		if ( token == 1 )
		    myState = STATE_CHANNELS;
		else
		{
		    fprintf(stderr,"PPD_PipeIn:Unknown command type: %d. Waiting for reset \n\r", int(token));
		    myWaitingForReset = 1;
		}
	    }
	    break;

	// Sets the number of channels read for this command. 
	case STATE_CHANNELS:

	    if (getToken(token) < 0)
	    {
		myState = STATE_COMMAND;
		break;
	    }
	    
	    myCurrentSample = 0;
	    myState = STATE_SAMPLES;
	    resize(token);

	    break;

	// Gets the sample values for each channel in this command.
	case STATE_SAMPLES:

	    if (getToken(sample) < 0)
	    {
		myState = STATE_COMMAND;
		break;
	    }

	    mySamples[myCurrentSample++] = sample;

	    if (myCurrentSample >= mySampleSize)
	    {
		myState = STATE_COMMAND;
		mySamplesReceived = 1;
	    }
	    break;
    }
}







