
int main()
{
    HTTP http = {};

    const char *url = "http://127.0.0.1:4433";
    http_begin(&http, HOST, url);
    {    
        http_req_begin(&http, HTTP_GET);
        {
            http_req_header(&http, "Auth", "Authorize %s", token);
            http_req_start_body(&http);
            http_req_body_chunk(&http, "<h1>Hi from C!</h1>");
        }
        http_req_end(&http);

        http_res_begin(&http);
        {
            if (http.status > 300){
                url = http.new_location;
                http_res_end
            }
            while (...){
                http_res_read_chunk(&http);
            }
        }
        http_res_end(&http);
    }
    http_end(&http);

    return 0;
}
