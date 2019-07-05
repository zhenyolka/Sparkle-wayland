#ifndef SPARKLE_AUDIO_H
#define SPARKLE_AUDIO_H

#include "were_object_2.h"
#include "were_exception.h"

#include <SLES/OpenSLES.h>

#ifdef ANDROID
#define USE_ANDROID_SIMPLE_BUFFER_QUEUE     // change to #undef for compatibility testing
#endif

#ifdef USE_ANDROID_SIMPLE_BUFFER_QUEUE
#include <SLES/OpenSLES_Android.h>
#endif

#include <queue>
#include <memory>


#ifdef USE_ANDROID_SIMPLE_BUFFER_QUEUE
#define DATALOCATOR_BUFFERQUEUE SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE
#define IID_BUFFERQUEUE SL_IID_ANDROIDSIMPLEBUFFERQUEUE
#define BufferQueueItf SLAndroidSimpleBufferQueueItf
#define BufferQueueState SLAndroidSimpleBufferQueueState
#define INDEX index
#else
#define DATALOCATOR_BUFFERQUEUE SL_DATALOCATOR_BUFFERQUEUE
#define IID_BUFFERQUEUE SL_IID_BUFFERQUEUE
#define BufferQueueItf SLBufferQueueItf
#define BufferQueueState SLBufferQueueState
#define INDEX playIndex
#endif

#define check_result(r) do { if ((r) != SL_RESULT_SUCCESS) throw were_exception(WE_SIMPLE); } while (0)

const int period_size = 65536 / 8;

class were_unix_server;
class were_unix_socket;


class sparkle_audio_buffer
{
public:
    char data_[period_size];
};


class sparkle_audio : public were_object_2
{
public:
    ~sparkle_audio();
    sparkle_audio();

private:
    static void callback(BufferQueueItf playerBufferqueue, void *data);
    void read();
    void start();
    void stop();

private:
    SLObjectItf engineObject;
    SLEngineItf engineEngine;
    SLObjectItf outputmixObject;
    SLDataSource audiosrc;
    SLDataSink audiosnk;
    SLDataFormat_PCM pcm;
    SLDataLocator_OutputMix locator_outputmix;
    SLDataLocator_BufferQueue locator_bufferqueue;
    SLObjectItf playerObject;
    SLPlayItf playerPlay;
    BufferQueueItf playerBufferqueue;
    SLuint32 state_;

    were_object_pointer<were_unix_server> server_;
    were_object_pointer<were_unix_socket> socket_;
    uint64_t pointer_;
    std::queue< std::shared_ptr<sparkle_audio_buffer> > queue_;
};

#endif // SPARKLE_AUDIO_H
