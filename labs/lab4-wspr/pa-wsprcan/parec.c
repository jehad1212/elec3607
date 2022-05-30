

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>

#define MXRECTIME 120                   // maximum record time in seconds
#define SRATE   12000                     // sample rate in Hz
#define MXFRAMES (MXRECTIME*SRATE)      // number of frames to read/write
#define CHANNELS 1                      // number of channels
#define BYTESPERSAMPLE 2                // 16 bit samples
#define BUFSIZE (MXFRAMES*CHANNELS*BYTESPERSAMPLE)      // MXRECTIME sec buffer
#define FNAMESIZE 30                    // size of output file name string

int 
parec(short *pabuf, int npoints) {
    /* The sample type to use */
    static const ss = 
    {
        .format =  PA_SAMPLE_S16LE,
        .rate = SRATE,
        .channels = CHANNELS
    };
    pa_simple *s = NULL;
    int ret = npoints;
    int error;

    assert(npoints < MXFRAMES);
    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, "pawsprd", PA_STREAM_RECORD, NULL, "wsprrecord", &ss, NULL, NULL, &error))) 
    {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        ret = 0;
        goto finish;
    }

    for (int i = 0; i < 1; i++) 
    {

        /* Record some data ... */
        if (pa_simple_read(s, pabuf, npoints * sizeof(pabuf[0]), &error) < 0) 
        {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            ret = 0;
            goto finish;
        }

    }

finish:

    if (s)
        pa_simple_free(s);

    return ret;
}
