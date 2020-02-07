#include "sparkle_player.h"
#include "were_timer.h"
#include "were_log.h"

#define UNSAFE 1


sparkle_player::~sparkle_player()
{
    check_timer_.collapse();
    (*player_)->Destroy(player_);
    (*output_mix_)->Destroy(output_mix_);
    (*engine_)->Destroy(engine_);
}

sparkle_player::sparkle_player() :
    buffer_(nullptr),
    check_timer_(new were_timer(1000))
{
    SLresult result;

    // create engine

    result = slCreateEngine(&engine_, 0, NULL, 0, NULL, NULL);
    check_result(result);
    result = (*engine_)->Realize(engine_, SL_BOOLEAN_FALSE);
    check_result(result);
    result = (*engine_)->GetInterface(engine_, SL_IID_ENGINE, &engine_engine_);
    check_result(result);

    // create output mix

    result = (*engine_engine_)->CreateOutputMix(engine_engine_, &output_mix_, 0, NULL, NULL);
    check_result(result);
    result = (*output_mix_)->Realize(output_mix_, SL_BOOLEAN_FALSE);
    check_result(result);

    // create audio player

    locator_buffer_queue_.locatorType = DATALOCATOR_BUFFERQUEUE;
    locator_buffer_queue_.numBuffers = 255;
    locator_output_mix_.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locator_output_mix_.outputMix = output_mix_;


    pcm_.formatType = SL_DATAFORMAT_PCM;
    pcm_.numChannels = 2;
    pcm_.samplesPerSec = SL_SAMPLINGRATE_48;
    pcm_.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm_.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm_.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    pcm_.endianness = SL_BYTEORDER_LITTLEENDIAN;

    audio_src_.pLocator = &locator_buffer_queue_;
    audio_src_.pFormat = &pcm_;
    audio_snk_.pLocator = &locator_output_mix_;
    audio_snk_.pFormat = NULL;


    SLInterfaceID ids[1] = {IID_BUFFERQUEUE};
    SLboolean flags[1] = {SL_BOOLEAN_TRUE};
    result = (*engine_engine_)->CreateAudioPlayer(engine_engine_, &player_, &audio_src_, &audio_snk_, 1, ids, flags);
    check_result(result);
    result = (*player_)->Realize(player_, SL_BOOLEAN_FALSE);
    check_result(result);
    result = (*player_)->GetInterface(player_, SL_IID_PLAY, &player_play_);
    check_result(result);
    result = (*player_)->GetInterface(player_, IID_BUFFERQUEUE, &player_buffer_queue_);
    check_result(result);
    result = (*player_buffer_queue_)->RegisterCallback(player_buffer_queue_, callback, this);
    check_result(result);


    auto this_wop = make_wop(this);

    were_object::connect(check_timer_, &were_timer::timeout, this_wop, [this_wop]()
    {
        this_wop->check();
    }); // XXX3 Find better solution

    state_ = 100;
    stop();
}

void sparkle_player::start()
{
    if (state_ != SL_PLAYSTATE_PLAYING)
    {
        SLresult result;

        were_log("starting player\n");

        state_ = SL_PLAYSTATE_PLAYING;

        result = (*player_play_)->SetPlayState(player_play_, state_);
        check_result(result);

        playing_ = 0;

        check();

        check_timer_->start();
    }
}

void sparkle_player::stop()
{
    if (state_ != SL_PLAYSTATE_STOPPED)
    {
        SLresult result;

        check_timer_->stop();

        were_log("stopping player\n");

        state_ = SL_PLAYSTATE_STOPPED;

        result = (*player_play_)->SetPlayState(player_play_, state_);
        check_result(result);

        result = (*player_buffer_queue_)->Clear(player_buffer_queue_);
        check_result(result);

        if (buffer_)
            were1_ring_buffer_reset(&buffer_->were);

        playing_ = 0;
    }
}

void sparkle_player::callback(BufferQueueItf playerBufferqueue, void *data)
{
    sparkle_player *instance = reinterpret_cast<sparkle_player *>(data);

    were_object_pointer<sparkle_player> instance__(instance);

#if UNSAFE
    instance__.access_UNSAFE()->callback(); /* Unsafe */
#else
    instance__.thread()->post([instance__]()
    {
        instance__->callback();
    });
#endif
}

void sparkle_player::callback()
{
    auto this_wop = make_wop(this);

    if (state_ != SL_PLAYSTATE_PLAYING)
        return;

    were1_ring_buffer_read_finish(&buffer_->were, playing_);

    playing_ = 0;

    check();

#if UNSAFE
    post([this_wop]()
    {
        were_object::emit(this_wop, &sparkle_player::played);
    });
#else
    were_object::emit(this_wop, &sparkle_player::played);
#endif
}

void sparkle_player::check()
{
    if (state_ == SL_PLAYSTATE_PLAYING && playing_ == 0 && buffer_ != nullptr)
    {
        int n = were1_ring_buffer_bytes_available(&buffer_->were);

        if (n < 1)
            return;

        if (n > 8192)
            n = 8192;

        char *data;
        n = were1_ring_buffer_read_start(&buffer_->were, &data, n);

        SLresult result = (*player_buffer_queue_)->Enqueue(player_buffer_queue_, data, n);
        check_result(result);

        playing_ = n;
    }
}
