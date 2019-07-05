#include <cstdio>
#include "were_debug.h"
#include "were_unix_server.h"
#include "were_unix_socket.h"
#include "were_timer.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>



int main(int argc, char *argv[])
{
    were_object_pointer<were_thread> thread(new were_thread());
    were_object_pointer<were_debug> debug(new were_debug());
    were_object_pointer<were_unix_server> server(new were_unix_server("/tmp/sparkle-audio"));
    were_object_pointer<were_timer> timer(new were_timer(1000));
    were_object_pointer<were_unix_socket> socket;


    uint64_t pointer = 0;
    uint64_t in_buffer = 0;

    were::connect(timer, &were_timer::timeout, timer, [&socket, &pointer, &in_buffer]()
    {
        if (socket)
        {
            pointer += (44100 * 4);
            if (pointer > in_buffer)
                pointer = in_buffer;
            fprintf(stdout, "Pointer %lu/%lu\n", pointer, in_buffer);
            socket->send((char *)&pointer, sizeof(uint64_t));
        }
    });

    timer->start();

    were::connect(server, &were_unix_server::new_connection, server, [&server, &socket, &timer, &pointer, &in_buffer]()
    {
        fprintf(stdout, "new connection\n");
        socket = server->accept();
        fprintf(stdout, "accepted\n");


        were::connect(socket, &were_unix_socket::ready_read, socket, [&server, &socket, &timer, &pointer, &in_buffer]()
        {
            uint64_t code;
            socket->receive((char *)&code, sizeof(uint64_t));

            if (code == 1)
            {
                fprintf(stdout, "Start\n");
                pointer = 0;
                timer->start();
            }
            else if (code == 2)
            {
                fprintf(stdout, "Stop\n");
                timer->stop();
                in_buffer = 0;
                pointer = 0;
            }
            else if (code == 3)
            {
                uint64_t size;
                socket->receive((char *)&size, sizeof(uint64_t));
                fprintf(stdout, "Data %lu\n", size);

                char *buffer = new char[size];
                socket->receive(buffer, size);
                delete[] buffer;

                in_buffer += size;
            }

        });

    });



    thread->run();

    return 0;
}
