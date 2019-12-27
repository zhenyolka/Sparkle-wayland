#include "sparkle_audio.h"
#include "sparkle_player.h"
#include "were_unix_server.h"
#include "were_unix_socket.h"
#include "were1_unix_socket.h"
#include "were1_tmpfile.h"
#include <sys/stat.h> // chmod
#include "were_log.h"
#include <unistd.h> // XXX3 TMP



sparkle_audio::~sparkle_audio()
{
    server_.collapse();
    player_.collapse();
}

sparkle_audio::sparkle_audio(const std::string &path) :
    player_(new sparkle_player()),
    server_(new were_unix_server(path)),
    buffer_fd_(-1), buffer_(nullptr), connected_(false)
{
    MAKE_THIS_WOP

    chmod(path.c_str(), 0666);

    were_object::connect(server_, &were_unix_server::new_connection, this_wop, [this_wop]()
    {
        this_wop->connect_client();
    });
}

void sparkle_audio::connect_client()
{
    MAKE_THIS_WOP

    if (connected_)
    {
        server_->reject();
        return;
    }

    player_->stop();

    were_object_pointer<were_unix_socket> client = server_->accept();

    were_object::connect(client, &were_unix_socket::ready_read, this_wop, [this_wop, client]()
    {
        this_wop->read(client);
    });

    were_object::connect(client, &were_unix_socket::disconnected, this_wop, [this_wop, client]()
    {
        this_wop->disconnect_client(client);
    });

    were_object::connect(player_, &sparkle_player::played, client, [client]()
    {
        uint64_t data = 0;
        client->send_all((char *)&data, sizeof(uint64_t));
    });

    connected_ = true;

    were_log("audio connected\n");
}

void sparkle_audio::disconnect_client(were_object_pointer<were_unix_socket> client)
{
    if (!connected_)
        return;

    player_->stop();

    usleep(100000); /* Callback */

    if (buffer_ != nullptr)
    {
        player_->set_buffer(nullptr);

        were1_tmpfile_unmap((void **)&buffer_, sizeof(struct sparkle_audio_buffer));
        buffer_ = nullptr;
    }

    if (buffer_fd_ != -1)
    {
        close(buffer_fd_);
        buffer_fd_ = -1;
    }

    client.collapse();

    connected_ = false;

    were_log("audio disconnected\n");
}

void sparkle_audio::read(were_object_pointer<were_unix_socket> client)
{
    bool r;

    while (client->bytes_available() > 0)
    {
        uint64_t code;
        r = client->receive_all((char *)&code, sizeof(uint64_t));
        if (!r)
        {
            disconnect_client(client);
            return;
        }

        if (code == 0)
        {
        }
        if (code == 1)
        {
            r = client->receive_fds(&buffer_fd_, 1);
            if (!r)
            {
                disconnect_client(client);
                return;
            }

            if (were1_tmpfile_map((void **)&buffer_, sizeof(struct sparkle_audio_buffer), buffer_fd_) == -1)
                throw were_exception(WE_SIMPLE);

            player_->set_buffer(buffer_);
        }
        else if (code == 2)
        {
            player_->start();
        }
        else if (code == 3)
        {
            player_->stop();
        }
        else
        {
            throw were_exception(WE_SIMPLE);
        }
    }
}
