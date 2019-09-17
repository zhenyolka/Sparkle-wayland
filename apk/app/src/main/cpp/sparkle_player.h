#ifndef SPARKLE_PLAYER_H
#define SPARKLE_PLAYER_H

#include "were_object.h"
#include "were1_ring_buffer.h"
#include "were_exception.h"


#include <SLES/OpenSLES.h>

#ifdef ANDROID
#define USE_ANDROID_SIMPLE_BUFFER_QUEUE
#endif

#ifdef USE_ANDROID_SIMPLE_BUFFER_QUEUE
#include <SLES/OpenSLES_Android.h>
#endif

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


struct sparkle_audio_buffer
{
    struct were1_ring_buffer were;
    char data[65536];
};

class were_timer;

class sparkle_player : public were_object
{
public:
    ~sparkle_player();
    sparkle_player();

    void set_buffer(struct sparkle_audio_buffer *buffer) { buffer_ = buffer; }
    void start();
    void stop();

signals:
    were_signal<void ()> played;

private:
    static void callback(BufferQueueItf playerBufferqueue, void *data);
    void callback();
    void check();

private:
    SLObjectItf engine_;
    SLEngineItf engine_engine_;
    SLObjectItf output_mix_;
    SLDataSource audio_src_;
    SLDataSink audio_snk_;
    SLDataFormat_PCM pcm_;
    SLDataLocator_BufferQueue locator_buffer_queue_;
    SLDataLocator_OutputMix locator_output_mix_;
    SLObjectItf player_;
    SLPlayItf player_play_;
    BufferQueueItf player_buffer_queue_;

    SLuint32 state_;

    int playing_;

    struct sparkle_audio_buffer *buffer_;

    were_object_pointer<were_timer> check_timer_;
};

#endif // SPARKLE_PLAYER_H
