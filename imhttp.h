#ifndef IMHTTP_H_
#define IMHTTP_H_

typedef void *ImHTTP_Socket;

typedef ssize_t (*ImHTTP_Write) (ImHTTP_Socket socket, const void *buf, size_t count);

typedef ssize_t (*ImHTTP_Read) (ImHTTP_Socket socket, const void *buf, size_t count);

typedef enum {
    IMHTTP_GET,
    IMHTTP_POST,
} ImHTTP_Method;

typedef struct {
    ImHTTP_Socket socket;
    ImHTTP_Write write;
    ImHTTP_Read read;
} ImHTTP;

void imhttp_req_begin(ImHTTP *imhttp, ImHTTP_Method method, const char *resource);
void imhttp_req_header(ImHTTP *imhttp, const char *header, const char *header_value);

void imhttp_req_header_end(ImHTTP *imhttp);

void imhttp_req_body_chunk(ImHTTP *imhttp, const char *chunk_cstr);
void imhttp_req_body_chunk_size(ImHTTP *imhttp, const char *chunk, size_t chunk_size);
void imhttp_req_end(ImHTTP *imhttp);

#endif //IMHTTP_H_




#ifdef IMHTTP_IMPLEMENTATION

void imhttp_req_begin(ImHTTP *imhttp, ImHTTP_Method method, const char *resource)
{

}
void imhttp_req_header(ImHTTP *imhttp, const char *header, const char *header_value);

void imhttp_req_header_end(ImHTTP *imhttp);

void imhttp_req_body_chunk(ImHTTP *imhttp, const char *chunk_cstr);
void imhttp_req_body_chunk_size(ImHTTP *imhttp, const char *chunk, size_t chunk_size);
void imhttp_req_end(ImHTTP *imhttp);


#endif //IMHTTP_IMPLEMENTATION
