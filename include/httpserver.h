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
protected:
	unsigned int port;
	struct MHD_Daemon * daemon;


protected:
	static struct MHD_Response * getStatus();
	static struct MHD_Response * getServosStatus();
	static struct MHD_Response * startEarthRotation();
	static struct MHD_Response * stopEarthRotation();
	static struct MHD_Response * statusEarthRotation();
	static struct MHD_Response * scanForTheSun();
	static struct MHD_Response * setServoPosition(int servo, int position);
	static struct MHD_Response * setServoAngle(int servo, double angle);
};
#endif