#ifndef SPARKLE_AUDIO_H
#define SPARKLE_AUDIO_H

#include "were.h"
#include "sparkle_player.h"
#include <string>

class sparkle_player;

class were_unix_server;
class were_unix_socket;

class sparkle_audio : virtual public were_object
{
public:
    ~sparkle_audio() override;
    explicit sparkle_audio(const std::string &path);

private:
    void connect_client();
    void disconnect_client(were_pointer<were_unix_socket> client);
    void read(were_pointer<were_unix_socket> client);

private:
    were_pointer<sparkle_player> player_;
    were_pointer<were_unix_server> server_;
    int buffer_fd_;
    struct sparkle_audio_buffer *buffer_;
    bool connected_;
};

#endif // SPARKLE_AUDIO_H
