#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define IMHTTP_IMPLEMENTATION
#include "imhttp.h"

#define HOST "tsoding.org"
#define PORT "80"


ssize_t imhttp_write(ImHTTP_Socket socket, const void *buf, size_t count)
{
    return write((int) (int64_t) socket, buf, count);
}


ssize_t imhttp_read(ImHTTP_Socket socket, void *buf, size_t count)
{
    return read((int) (int64_t) socket, buf, count);
}


int main()
{

    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *addrs;
    if (getaddrinfo(HOST, PORT, &hints, &addrs) < 0) {
        fprintf(stderr, "Could not get address of `"HOST"` : %s\n", strerror(errno));
        exit(1);
    }
    int sd = 0;
    for (struct addrinfo *addr = addrs; addr != NULL; addr = addr->ai_next){
        sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if (sd == -1) break;
        if (connect(sd, addr->ai_addr, addr->ai_addrlen) == 0) break;
        close(sd);
        sd = -1;
    }

    freeaddrinfo(addrs);

    if(sd == -1) {
        fprintf(stderr, "Could not get address of `"HOST"` : %s\n", strerror(errno));
        exit(1);
    }
    
    // TODO: Make this static because there is no dynamic mem alloc
    ImHTTP imhttp = {
        .socket = ( void *) (int64_t) sd,
        .write = imhttp_write,
        .read = imhttp_read,
    };

    imhttp_req_begin(&imhttp, IMHTTP_GET, "/index.html");
    {
        imhttp_req_header(&imhttp, "Host", HOST);
        imhttp_req_header(&imhttp, "Foo", "Bar");
        imhttp_req_header(&imhttp, "Hello", "Worl");
        imhttp_req_headers_end(&imhttp);
        imhttp_req_body_chunk(&imhttp, "Hello Body World!\n");

    }
    imhttp_req_end(&imhttp);


    imhttp_res_begin(&imhttp);
    {

        fputs("Meta: \n", stdout);
        fwrite(imhttp.res_meta, 1, imhttp.res_meta_size, stdout);
        fputc('\n', stdout);
        fputs("Body Chunk\n", stdout);
        fwrite(imhttp.res_body_chunk, 1, imhttp.res_body_chunk_size, stdout);
    }
    imhttp_res_end(&imhttp);

    close(sd);
    return 0;
}
