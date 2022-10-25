#include <stdio.h>
#include <signal.h>
#include <X11/Xlib.h>
#include "spnav.h"

class PPD_PipeOut
{

public:

     PPD_PipeOut();
    ~PPD_PipeOut();

    int	open(const char *filename);
    void close();
    static void sig(int s);
    
    void send_current_values(int num, double *samples);
    void send_upload(int length, int rate, int start, int channels);

    void start_send_channel_names(int n);
    void send_channel_name(const char *name);
    void stop_send_channel_names();

private:

    void write_byte(char b);
    void write_values(const char *p, int size);
    void send_reset();

    FILE	*myOutput;

};


