#include <cstdio>
#include "were_debug.h"
#include "were_unix_server.h"
#include "were_unix_socket.h"
#include "were_timer.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>



class reader : public were_object_2
{
public:
    ~reader()
    {
    }
    reader()
    {
        MAKE_THIS_WOP

        server_ = were_object_pointer<were_unix_server>(new were_unix_server("/tmp/sparkle-audio"));
        timer_ = were_object_pointer<were_timer>(new were_timer(500));

        were::connect(timer_, &were_timer::timeout, this_wop, [this_wop]()
        {
            if (this_wop->socket_)
            {
                this_wop->pointer_ += (44100 * 4) / 2;
                if (this_wop->pointer_ > this_wop->in_buffer_)
                    this_wop->pointer_ = this_wop->in_buffer_;
                fprintf(stdout, "Pointer %lu/%lu\n", this_wop->pointer_, this_wop->in_buffer_);
                this_wop->socket_->send((char *)&this_wop->pointer_, sizeof(uint64_t));
            }
        });

        were::connect(server_, &were_unix_server::new_connection, this_wop, [this_wop]()
        {
            fprintf(stdout, "new connection\n");

            if (!this_wop->socket_)
            {
                this_wop->socket_ = this_wop->server_->accept();
                fprintf(stdout, "accepted\n");


                were::connect(this_wop->socket_, &were_unix_socket::ready_read, this_wop, [this_wop]()
                {
                    uint64_t code;
                    this_wop->socket_->receive((char *)&code, sizeof(uint64_t));

                    if (code == 1)
                    {
                        fprintf(stdout, "Start\n");
                        this_wop->pointer_ = 0;
                        this_wop->timer_->start();
                    }
                    else if (code == 2)
                    {
                        fprintf(stdout, "Stop\n");
                        this_wop->timer_->stop();
                        this_wop->in_buffer_ = 0;
                        this_wop->pointer_ = 0;
                    }
                    else if (code == 3)
                    {
                        uint64_t size;
                        this_wop->socket_->receive((char *)&size, sizeof(uint64_t));
                        fprintf(stdout, "Data %lu\n", size);

                        char *buffer = new char[size];
                        this_wop->socket_->receive(buffer, size);
                        delete[] buffer;

                        this_wop->in_buffer_ += size;
                    }

                });

                were::connect(this_wop->socket_, &were_unix_socket::disconnected, this_wop, [this_wop]()
                {
                    fprintf(stdout, "disconnecting\n");
                    this_wop->socket_.collapse();
                });

            }
        });

    }

private:
    were_object_pointer<were_unix_server> server_;
    were_object_pointer<were_unix_socket> socket_;
    were_object_pointer<were_timer> timer_;

    uint64_t pointer_;
    uint64_t in_buffer_;
};


int main(int argc, char *argv[])
{
    were_object_pointer<were_thread> thread(new were_thread());
    were_object_pointer<were_debug> debug(new were_debug());

    were_object_pointer<reader> r(new reader());



    thread->run();

    return 0;
}
