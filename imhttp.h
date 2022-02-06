#ifndef IMHTTP_H_
#define IMHTTP_H_
#include <assert.h>
#include <stddef.h>

#include "sv.h"

typedef void *ImHTTP_Socket;

typedef ssize_t (*ImHTTP_Write) (ImHTTP_Socket socket, const void *buf, size_t count);

typedef ssize_t (*ImHTTP_Read) (ImHTTP_Socket socket, void *buf, size_t count);

typedef enum {
    IMHTTP_GET,
    IMHTTP_POST,
} ImHTTP_Method;

#define IMHTTP_RES_META_CAPACITY (8 * 1024)
#define IMHTTP_RES_BODY_CHUNK_CAPACITY IMHTTP_RES_META_CAPACITY

static_assert(IMHTTP_RES_META_CAPACITY <= IMHTTP_RES_BODY_CHUNK_CAPACITY,
              "If we overshoot and read a part of the body into meta data buffer, we want that \"tail\" to actually fit into res_body");


typedef struct {
    ImHTTP_Socket socket;
    ImHTTP_Write write;
    ImHTTP_Read read;

    char res_meta[IMHTTP_RES_META_CAPACITY];
    size_t res_meta_size;

    char res_body_chunk[IMHTTP_RES_BODY_CHUNK_CAPACITY];
    size_t res_body_chunk_size;
} ImHTTP;

void imhttp_req_begin(ImHTTP *imhttp, ImHTTP_Method method, const char *resource);
void imhttp_req_header(ImHTTP *imhttp, const char *header_name, const char *header_value);

void imhttp_req_headers_end(ImHTTP *imhttp);

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

void imhttp_req_headers_end(ImHTTP *imhttp)
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

static void imhttp_res_springback_meta(ImHTTP *imhttp)
{
    String_View sv = {
        .count = imhttp->res_meta_size,
        .data = imhttp->res_meta
    };

    while (sv.count > 0){
        String_View line = sv_chop_by_delim(&sv, '\n');

        if(sv_eq(line, SV("\r"))) {
            memcpy(imhttp->res_body_chunk, sv.data, sv.count);
            imhttp->res_body_chunk_size = sv.count;
            assert(imhttp->res_meta < sv.data);
            imhttp->res_meta_size = sv.data - imhttp->res_meta;
            return;
        }
    }

    assert(0 && "IMHTTP_RES_META_CAPACITY is too small!");
}

void imhttp_res_begin(ImHTTP *imhttp)
{
    ssize_t n = imhttp->read(imhttp->socket, imhttp->res_meta, IMHTTP_RES_META_CAPACITY);
    // TODO: imhttp_res_begin does not handle read error
    assert(n > 0);
    imhttp->res_meta_size = n;

    imhttp_res_springback_meta(imhttp);
}

void imhttp_res_end(ImHTTP *imhttp)
{
    (void) imhttp;
}


#endif //IMHTTP_IMPLEMENTATION
