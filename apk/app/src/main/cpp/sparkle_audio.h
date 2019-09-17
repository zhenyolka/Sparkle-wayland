#ifndef SPARKLE_AUDIO_H
#define SPARKLE_AUDIO_H

#include "were_object.h"
#include "sparkle_player.h"
#include <string>

class sparkle_player;

class were_unix_server;
class were_unix_socket;

class sparkle_audio : public were_object
{
public:
    ~sparkle_audio();
    sparkle_audio(const std::string &path);

private:
    void connect_client();
    void disconnect_client();
    void read();

private:
    were_object_pointer<sparkle_player> player_;
    were_object_pointer<were_unix_server> server_;
    were_object_pointer<were_unix_socket> client_;
    int buffer_fd_;
    struct sparkle_audio_buffer *buffer_;
};

#endif // SPARKLE_AUDIO_H
