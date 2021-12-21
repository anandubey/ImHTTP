#ifndef IMHTTP_H_
#define IMHTTP_H_
#include <assert.h>

typedef void *ImHTTP_Socket;

typedef ssize_t (*ImHTTP_Write) (ImHTTP_Socket socket, const void *buf, size_t count);

typedef ssize_t (*ImHTTP_Read) (ImHTTP_Socket socket, void *buf, size_t count);

typedef enum {
    IMHTTP_GET,
    IMHTTP_POST,
} ImHTTP_Method;

#define IMHTTP_RESPONSE_META_CAPACITY 1024
#define IMHTTP_RESPONSE_BODY_CHUNK_CAPACITY 1024

typedef struct {
    ImHTTP_Socket socket;
    ImHTTP_Write write;
    ImHTTP_Read read;

    char response_meta[IMHTTP_RESPONSE_META_CAPACITY];
    size_t response_meta_size;

    char response_body_chunk[IMHTTP_RESPONSE_BODY_CHUNK_CAPACITY];
    size_t response_body_chunk_size;
} ImHTTP;

void imhttp_req_begin(ImHTTP *imhttp, ImHTTP_Method method, const char *resource);
void imhttp_req_header(ImHTTP *imhttp, const char *header_name, const char *header_value);

void imhttp_req_header_end(ImHTTP *imhttp);

void imhttp_req_body_chunk(ImHTTP *imhttp, const char *chunk_cstr);
void imhttp_req_body_chunk_size(ImHTTP *imhttp, const char *chunk, size_t chunk_size);
void imhttp_req_end(ImHTTP *imhttp);

#endif //IMHTTP_H_




#ifdef IMHTTP_IMPLEMENTATION

static const char *imhttp_method_as_cstr(ImHTTP_Method method)
{
    switch (method){
        case IMHTTP_GET:
            return "GET";
            break;
        case IMHTTP_POST:
            return "POST";
            break;
        default:
            assert(0 && "imhttp_method_as_cstr: unreachable\n");
    }
}

static void imhttp_write_cstr(ImHTTP *imhttp, const char *cstr)
{
    size_t cstr_size = strlen(cstr);

    // TODO: imhttp_write_cstr does not handle ImHTTP_Write errors
    imhttp->write(imhttp->socket, cstr, cstr_size);
}


void imhttp_req_begin(ImHTTP *imhttp, ImHTTP_Method method, const char *resource)
{

    imhttp_write_cstr(imhttp, imhttp_method_as_cstr(method));
    imhttp_write_cstr(imhttp, " ");
    imhttp_write_cstr(imhttp, resource);
    imhttp_write_cstr(imhttp, " HTTP/1.0\r\n");
}

void imhttp_req_header(ImHTTP *imhttp, const char *header_name, const char *header_value)
{

    imhttp_write_cstr(imhttp, header_name);
    imhttp_write_cstr(imhttp, ": ");
    imhttp_write_cstr(imhttp, header_value);
    imhttp_write_cstr(imhttp, "\r\n");
}

void imhttp_req_header_end(ImHTTP *imhttp)
{
    imhttp_write_cstr(imhttp, "\r\n");
}

void imhttp_req_body_chunk(ImHTTP *imhttp, const char *chunk_cstr)
{

    imhttp_write_cstr(imhttp, chunk_cstr);
}

void imhttp_req_body_chunk_size(ImHTTP *imhttp, const char *chunk, size_t chunk_size)
{

    imhttp->write(imhttp->socket, chunk, chunk_size);
}

void imhttp_req_end(ImHTTP *imhttp)
{
    (void) imhttp;

}

#endif //IMHTTP_IMPLEMENTATION
