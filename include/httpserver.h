#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <microhttpd.h>
#include <string>

class HttpServer {
public:
	HttpServer(unsigned int port, std::string documentroot);
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
	static std::string documentroot;


protected:
	static struct MHD_Response * getStatus();
	static struct MHD_Response * getServosStatus();
	static struct MHD_Response * startEarthRotation();
	static struct MHD_Response * stopEarthRotation();
	static struct MHD_Response * statusEarthRotation();
	static struct MHD_Response * scanForTheSun();
	static struct MHD_Response * setServoSpeed(int servo, int speed);
	static struct MHD_Response * setServoAcceleration(int servo, int acceleration);
	static struct MHD_Response * setServoPosition(int servo, int position);
	static struct MHD_Response * setServoAngle(int servo, double angle);
	static struct MHD_Response * setServoMin(int servo, int min);
	static struct MHD_Response * setServoMax(int servo, int max);
	static struct MHD_Response * setServoNeutral(int servo, int neutral);
};
#endif