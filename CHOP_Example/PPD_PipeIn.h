#include <stdio.h>
#include <stdint.h>

#define TOKEN_SIZE	(sizeof(int64_t))

class PPD_PipeIn
{

public:

     PPD_PipeIn();
    ~PPD_PipeIn();

    int		 open(const char *filename);
    int		 getSamples(double *&a);
    void	 close();

private:

    FILE	*myInput;

    void	 reset();
    int		 getToken(double &f);
    int		 getToken(int64_t &i);
    void	 addByte(char b);
    void	 receiveBytes();
    void	 processToken();
    void	 resize(int s);

    char	 myByteValues[TOKEN_SIZE];
    int		 myCount;
    int		 myEscapeFlag;
    int		 myResetFlag;
    int		 myState;
    int		 myCurrentSample;

    double	*mySamples;
    int		 mySampleSize;
    int		 mySamplesReceived;
    int		 myWaitingForReset;

};


