#include "sparkle_audio.h"
#include "were_unix_server.h"
#include "were_unix_socket.h"
#include "were1_unix_socket.h"
#include "were1_tmpfile.h"
#include "were1_ring_buffer.h"
#include <sys/stat.h> // chmod
#include "were_log.h"

#include <unistd.h> // XXX3 TMP

struct sparkle_audio_buffer
{
    struct were1_ring_buffer were;
    char data[65536];
};

sparkle_audio::~sparkle_audio()
{
    disconnect_client();
    server_.collapse();

    (*playerObject)->Destroy(playerObject);
    (*outputmixObject)->Destroy(outputmixObject);
    (*engineObject)->Destroy(engineObject);
}

sparkle_audio::sparkle_audio(const std::string &path) :
    buffer_fd_(-1), buffer_(nullptr)
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
    pcm.samplesPerSec = SL_SAMPLINGRATE_48; // XXX3 Native
    pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
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

    server_ = were_object_pointer<were_unix_server>(new were_unix_server(path));
    chmod(path.c_str(), 0666);

    were_object::connect(server_, &were_unix_server::new_connection, this_wop, [this_wop]()
    {
        this_wop->connect_client();
    });
}

void sparkle_audio::callback(BufferQueueItf playerBufferqueue, void *data)
{
    sparkle_audio *instance = reinterpret_cast<sparkle_audio *>(data);
    were_object_pointer<sparkle_audio> instance__(instance);

#if 1
    instance__->callback(); /* Unsafe */
#else
    instance__->thread()->post([instance__]()
    {
        instance__->callback();
    });
#endif
}

void sparkle_audio::callback()
{
    if (state_ != SL_PLAYSTATE_PLAYING)
        return;

    were1_ring_buffer_read_finish(&buffer_->were, playing_);

    playing_ = 0;
    check();

#if 1
    uint64_t data = 0;
    client_->send_all((char *)&data, sizeof(uint64_t));
#endif
}

void sparkle_audio::connect_client()
{
    MAKE_THIS_WOP

    if (client_)
    {
        were_log("can't accept audio connection\n");
        return;
    }

    stop();

    client_ = server_->accept();

    were_log("audio connected\n");

    were_object::connect(client_, &were_unix_socket::ready_read, this_wop, [this_wop]()
    {
        this_wop->read();
    });

    were_object::connect(client_, &were_unix_socket::disconnected, this_wop, [this_wop]()
    {
        this_wop->disconnect_client();
    });
}

void sparkle_audio::disconnect_client()
{
    if (!client_)
        return;

    stop();
    usleep(100000); /* Callback */

    if (buffer_ != nullptr)
    {
        were1_tmpfile_unmap((void **)&buffer_, sizeof(struct sparkle_audio_buffer));
        buffer_ = nullptr;
    }

    if (buffer_fd_ != -1)
    {
        close(buffer_fd_);
        buffer_fd_ = -1;
    }

    client_.collapse();

    were_log("audio disconnected\n");
}

void sparkle_audio::read()
{
    bool r;

    while (client_ && client_->bytes_available() > 0)
    {
        uint64_t code;
        r = client_->receive_all((char *)&code, sizeof(uint64_t));
        if (!r)
        {
            disconnect_client();
            return;
        }

        if (code == 0)
        {
        }
        if (code == 1)
        {
            r = client_->receive_fds(&buffer_fd_, 1);
            if (!r)
            {
                disconnect_client();
                return;
            }

            if (were1_tmpfile_map((void **)&buffer_, sizeof(struct sparkle_audio_buffer), buffer_fd_) == -1)
                throw were_exception(WE_SIMPLE);
        }
        else if (code == 2)
        {
            start();
        }
        else if (code == 3)
        {
            stop();
        }
        else
        {
            throw were_exception(WE_SIMPLE);
        }
    }
}

void sparkle_audio::start()
{
    if (state_ != SL_PLAYSTATE_PLAYING && buffer_ != nullptr)
    {
        were_log("starting player\n");

        SLresult result;
        state_ = SL_PLAYSTATE_PLAYING;
        result = (*playerPlay)->SetPlayState(playerPlay, state_);
        check_result(result);

        playing_ = 0;
        check();
    }
}

void sparkle_audio::stop()
{
    if (state_ != SL_PLAYSTATE_STOPPED)
    {
        were_log("stopping player\n");

        SLresult result;
        state_ = SL_PLAYSTATE_STOPPED;
        result = (*playerPlay)->SetPlayState(playerPlay, state_);
        check_result(result);

        result = (*playerBufferqueue)->Clear(playerBufferqueue);
        check_result(result);

        playing_ = 0;
    }
}

void sparkle_audio::check()
{
    if (state_ == SL_PLAYSTATE_PLAYING && playing_ == 0)
    {
        int n = were1_ring_buffer_bytes_available(&buffer_->were);

        if (n < 1)
            return;

        if (n > 8192)
            n = 8192;

        char *data;
        n = were1_ring_buffer_read_start(&buffer_->were, &data, n);

        SLresult result = (*playerBufferqueue)->Enqueue(playerBufferqueue, data, n);
        check_result(result);

        playing_ = n;
    }
}
