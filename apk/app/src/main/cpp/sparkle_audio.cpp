#include "sparkle_audio.h"
#include "were_unix_server.h"
#include "were_unix_socket.h"
#include <sys/stat.h> // chmod


sparkle_audio::~sparkle_audio()
{
    stop();

    (*playerObject)->Destroy(playerObject);
    (*outputmixObject)->Destroy(outputmixObject);
    (*engineObject)->Destroy(engineObject);
}

sparkle_audio::sparkle_audio()
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    check_result(result);

    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    check_result(result);

    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    check_result(result);

    // create output mix
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputmixObject, 0, NULL, NULL);
    check_result(result);

    result = (*outputmixObject)->Realize(outputmixObject, SL_BOOLEAN_FALSE);
    check_result(result);

    // create audio player

    locator_bufferqueue.locatorType = DATALOCATOR_BUFFERQUEUE;
    locator_bufferqueue.numBuffers = 255;
    locator_outputmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locator_outputmix.outputMix = outputmixObject;

    pcm.formatType = SL_DATAFORMAT_PCM;
    pcm.numChannels = 2;
    pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
    pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm.containerSize = 16;
    pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

    audiosrc.pLocator = &locator_bufferqueue;
    audiosrc.pFormat = &pcm;
    audiosnk.pLocator = &locator_outputmix;
    audiosnk.pFormat = NULL;


    SLInterfaceID ids[1] = {IID_BUFFERQUEUE};
    SLboolean flags[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audiosrc, &audiosnk, 1, ids, flags);
    check_result(result);

    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    check_result(result);

    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    check_result(result);

    result = (*playerObject)->GetInterface(playerObject, IID_BUFFERQUEUE, &playerBufferqueue);
    check_result(result);

    result = (*playerBufferqueue)->RegisterCallback(playerBufferqueue, callback, this);
    check_result(result);

#if 1
    state_ = SL_PLAYSTATE_STOPPED;
    result = (*playerPlay)->SetPlayState(playerPlay, state_);
    check_result(result);
#endif


    MAKE_THIS_WOP

    server_ = were_object_pointer<were_unix_server>(new were_unix_server("/data/data/com.sion.sparkle/audio"));
    chmod("/data/data/com.sion.sparkle/audio", 0666);

    were::connect(server_, &were_unix_server::new_connection, this_wop, [this_wop]()
    {
        fprintf(stdout, "audio connection\n");

        if (!this_wop->socket_)
        {
            this_wop->socket_ = this_wop->server_->accept();

            fprintf(stdout, "audio connected\n");

            this_wop->stop();

            were::connect(this_wop->socket_, &were_unix_socket::ready_read, this_wop, [this_wop]()
            {
                this_wop->read();
            });

            were::connect(this_wop->socket_, &were_unix_socket::disconnected, this_wop, [this_wop]()
            {
                fprintf(stdout, "audio disconnected\n");
                this_wop->stop();
                this_wop->socket_.collapse();
            });
        }
        else
            fprintf(stdout, "can't accept audio connection\n");
    });
}

void sparkle_audio::callback(BufferQueueItf playerBufferqueue, void *data)
{
    sparkle_audio *instance = reinterpret_cast<sparkle_audio *>(data);

    if (instance->queue_.size() < 1)
        throw were_exception(WE_SIMPLE);

    instance->queue_.pop();

    instance->pointer_ += period_size; // XXX It will be better to save buffer size

    if (instance->socket_)
        instance->socket_->send((char *)&instance->pointer_, sizeof(uint64_t));
}

void sparkle_audio::read()
{
    uint64_t code;
    socket_->receive((char *)&code, sizeof(uint64_t));

    if (code == 1)
    {
        fprintf(stdout, "starting player\n");
        start();
    }
    else if (code == 2)
    {
        fprintf(stdout, "stopping player\n");
        stop();
    }
    else if (code == 3)
    {
        uint64_t size;
        socket_->receive((char *)&size, sizeof(uint64_t));

        if (size > period_size)
            throw were_exception(WE_SIMPLE);

        std::shared_ptr<sparkle_audio_buffer> buffer(new sparkle_audio_buffer());
        socket_->receive(buffer->data_, size);

        SLresult result = (*playerBufferqueue)->Enqueue(playerBufferqueue, buffer->data_, period_size);
        check_result(result);
        queue_.push(buffer);
    }
}

void sparkle_audio::start()
{
    if (state_ != SL_PLAYSTATE_PLAYING)
    {
        SLresult result;
        state_ = SL_PLAYSTATE_PLAYING;
        result = (*playerPlay)->SetPlayState(playerPlay, state_);
        check_result(result);

        pointer_ = 0;
    }
}

void sparkle_audio::stop()
{
    if (state_ != SL_PLAYSTATE_STOPPED)
    {
        SLresult result;
        state_ = SL_PLAYSTATE_STOPPED;
        result = (*playerPlay)->SetPlayState(playerPlay, state_);
        check_result(result);

        result = (*playerBufferqueue)->Clear(playerBufferqueue);
        check_result(result);

        queue_ = std::queue< std::shared_ptr<sparkle_audio_buffer> >();

        pointer_ = 0;
    }
}
