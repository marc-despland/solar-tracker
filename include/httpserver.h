#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <microhttpd.h>

class HttpServer {
public:
	HttpServer(unsigned int port);
	~HttpServer();
	static int requesthandler(void * cls,
		    struct MHD_Connection * connection,
		    const char * url,
		    const char * method,
            const char * version,
		    const char * upload_data,
		    size_t * upload_data_size,
            void ** ptr);
	void start();
	void stop();
private:
	unsigned int port;
	struct MHD_Daemon * daemon;

private:
	static struct MHD_Response * getStatus();
};
#endif