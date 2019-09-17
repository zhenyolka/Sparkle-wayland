#include <cstdio>
#include "were_thread.h"
#include "were_unix_server.h"
#include "were_unix_socket.h"

class server : public were_object
{
public:
    ~server()
    {
    }

    server()
    {
        MAKE_THIS_WOP

        server_ = were_object_pointer<were_unix_server>(new were_unix_server("/tmp/serva"));
        were_object::connect(server_, &were_unix_server::new_connection, this_wop, [this_wop]()
        {
            fprintf(stdout, "new conn\n");
            //this_wop->server_->reject();
            were_object_pointer<were_unix_socket> conn = this_wop->server_->accept();
            fprintf(stdout, "acc\n");

            were_object::connect(conn, &were_unix_socket::disconnected, this_wop, [this_wop]()
            {
                fprintf(stdout, "disconnected\n");
            });

        });
    }

private:
    were_object_pointer<were_unix_server> server_;
};

int main(int argc, char *argv[])
{
    were_object_pointer<were_thread> thread(new were_thread());

    server s;

    were_thread::current_thread()->run();

    return 0;
}
