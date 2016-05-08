#include "httpserver.h"

#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "phidget.h"
#include "maestro.h"
#include "servo.h"
#include "earth.h"
#include "tracker.h"
#include <sstream>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>


std::string HttpServer::documentroot="./";

HttpServer::HttpServer(unsigned int port, std::string documentroot) {
	this->port=port; 
	HttpServer::documentroot=documentroot;

}

HttpServer::~HttpServer() {
	
}

void HttpServer::start() {
	char * page=strdup("<HTML><BODY>DEFAULT</BODY></HTML>");
	this->daemon= MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
		       this->port,
		       NULL,
		       NULL,
		       &HttpServer::requesthandler,
		       page,
		       MHD_OPTION_END);
}

void HttpServer::stop() {
	MHD_stop_daemon(this->daemon);
}


struct MHD_Response * HttpServer::getStatus() {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"status\": \"ok\"," << endl;
	tmp << Phidget::getStatus() <<","<<endl;
	tmp << Maestro::getStatus() <<endl;
	tmp << "}" << endl;
	char * status=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(status),(void*) status,MHD_RESPMEM_PERSISTENT);
}


struct MHD_Response * HttpServer::getServosStatus() {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"servos\": [" << endl;
	for(std::map<int,Servo *>::iterator iter = Servo::servos.begin(); iter != Servo::servos.end(); ++iter) {
		int k =  iter->first;
		Servo * servo=iter->second;
		if (iter !=Servo::servos.begin()) tmp << ","<<endl;
		tmp << "\t\t{" << endl;
		tmp << "\t\t\t\"code\": "<< k<<"," << endl;
		tmp << "\t\t\t\"name\": \""<< servo->getName()<<"\"," << endl;
		tmp << "\t\t\t\"position\": "<< servo->getPosition()<<"," << endl;
		tmp << "\t\t\t\"angle\": " << servo->getAngle()<<"," << endl;
		tmp << "\t\t\t\"min\": " << servo->getMin()<<"," << endl;
		tmp << "\t\t\t\"max\": " << servo->getMax() << endl;
		tmp << "\t\t}";
	}
	tmp << endl << "\t]" << endl;
	tmp << "}" << endl;
	char * status=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(status),(void*) status,MHD_RESPMEM_PERSISTENT);
}


struct MHD_Response * HttpServer::setServoPosition(int num, int position) {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"servo\" : \"ok\""<<endl;
	tmp << "}"<< endl;
	Servo * servo=Servo::servos[num];
	if (servo!=NULL) {
		servo->setPosition(position);
	}
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}

struct MHD_Response * HttpServer::setServoSpeed(int num, int speed) {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"servo\" : \"ok\""<<endl;
	tmp << "}"<< endl;
	Servo * servo=Servo::servos[num];
	if (servo!=NULL) {
		servo->setSpeed(speed);
	}
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}
struct MHD_Response * HttpServer::setServoAcceleration(int num, int acceleration) {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"servo\" : \"ok\""<<endl;
	tmp << "}"<< endl;
	Servo * servo=Servo::servos[num];
	if (servo!=NULL) {
		servo->setAcceleration(acceleration);
	}
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}
struct MHD_Response * HttpServer::setServoAngle(int num, double angle) {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"servo\" : \"ok\""<<endl;
	tmp << "}"<< endl;
	Servo * servo=Servo::servos[num];
	if (servo!=NULL) {
		servo->setAngle(angle);
	}
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}
struct MHD_Response * HttpServer::setServoMin(int num, int min) {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"servo\" : \"ok\""<<endl;
	tmp << "}"<< endl;
	Servo * servo=Servo::servos[num];
	if (servo!=NULL) {
		servo->setMin(min);
	}
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}

struct MHD_Response * HttpServer::setServoMax(int num, int max) {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"servo\" : \"ok\""<<endl;
	tmp << "}"<< endl;
	Servo * servo=Servo::servos[num];
	if (servo!=NULL) {
		servo->setMax(max);
	}
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}

struct MHD_Response * HttpServer::setServoNeutral(int num, int neutral) {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"servo\" : \"ok\""<<endl;
	tmp << "}"<< endl;
	Servo * servo=Servo::servos[num];
	if (servo!=NULL) {
		servo->setNeutral(neutral);
	}
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}


struct MHD_Response * HttpServer::startEarthRotation() {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"earth\" : \"start\""<<endl;
	tmp << "}"<< endl;
	Earth::start();
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}


struct MHD_Response * HttpServer::stopEarthRotation() {
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"earth\" : \"stop\""<<endl;
	tmp << "}"<< endl;
	Earth::stop();
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}

struct MHD_Response * HttpServer::statusEarthRotation() {
	stringstream tmp;
	tmp << "{" << endl;
	if (Earth::running()) {
		tmp << "\t\"rotation\" : \"started\","<<endl;
	} else {
		tmp << "\t\"rotation\" : \"stop\","<<endl;
	}
	tmp << "\t\"angle\" : "<< Earth::getRealAngle()<<","<<endl;
	tmp << "\t\"theorical\" : "<< Earth::getTheoricalAngle()<<endl;
	tmp << "}"<< endl;
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}

struct MHD_Response * HttpServer::scanForTheSun() {
	Tracker * tracker=new Tracker();
	tracker->scan();
	stringstream tmp;
	tmp << "{" << endl;
	tmp << "\t\"left\" : {"<<endl;
	tmp << "\t\t\"lux\": "<< tracker->maxleft <<","<<endl;
	tmp << "\t\t\"angle\": "<< tracker->angleleft <<endl;
	tmp << "\t},";
	tmp << endl;
	tmp << "\t\"right\" : {"<<endl;
	tmp << "\t\t\"lux\": "<< tracker->maxright <<","<<endl;
	tmp << "\t\t\"angle\": "<< tracker->angleright <<endl;
	tmp << "\t},";
	tmp << endl;
	tmp << "\t\"total\" : {"<<endl;
	tmp << "\t\t\"lux\": "<< tracker->maxlux <<","<<endl;
	tmp << "\t\t\"angle\": "<< tracker->anglelux <<endl;
	tmp << "\t},";
	tmp << endl;
	tmp << "\t\"situation\" : {"<<endl;
	tmp << "\t\t\"angle\" : "<< Earth::getRealAngle()<<","<<endl;
	tmp << "\t\t\"theorical\" : "<< Earth::getTheoricalAngle()<<endl;
	tmp << "\t}" << endl;
	tmp << "}"<< endl;
	char * response=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(response),(void*) response,MHD_RESPMEM_PERSISTENT);
}


int HttpServer::requesthandler(void * cls,
		    struct MHD_Connection * connection,
		    const char * url,
		    const char * method,
            const char * version,
		    const char * upload_data,
		    size_t * upload_data_size,
            void ** ptr) {
	int ret=MHD_NO;
	int servo, position, speed, acceleration, min, max, neutral;
	double angle;
	struct MHD_Response * response=NULL;
	Log::logger->log("HTTP",NOTICE) << method << " " << url << " (" << *upload_data_size << ")" << endl;
	if ((ret==MHD_NO) && (strcmp(url, "/api/status")==0) && (strcmp(method, "GET")==0)) {
		response=HttpServer::getStatus();
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (strcmp(url, "/api/servos/status")==0) && (strcmp(method, "GET")==0)) {
		response=HttpServer::getServosStatus();
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (strcmp(url, "/api/earthrotate/start")==0) && (strcmp(method, "GET")==0)) {
		response=HttpServer::startEarthRotation();
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (strcmp(url, "/api/earthrotate/stop")==0) && (strcmp(method, "GET")==0)) {
		response=HttpServer::stopEarthRotation();
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (strcmp(url, "/api/earthrotate/status")==0) && (strcmp(method, "GET")==0)) {
		response=HttpServer::statusEarthRotation();
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (strcmp(url, "/api/tracker/scan")==0) && (strcmp(method, "GET")==0)) {
		response=HttpServer::scanForTheSun();
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (sscanf(url, "/api/servo/%d/position/%d",&servo, &position)==2) && (strcmp(method, "GET")==0)) {
		Log::logger->log("HTTP",NOTICE) << "Matching set servo position : " << servo << " position: " << position<< endl;
		response=HttpServer::setServoPosition(servo, position);
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (sscanf(url, "/api/servo/%d/angle/%lf",&servo, &angle)==2) && (strcmp(method, "GET")==0)) {
		Log::logger->log("HTTP",NOTICE) << "Matching set servo angle : " << servo << " angle: " << angle << endl;
		response=HttpServer::setServoAngle(servo, angle);
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (sscanf(url, "/api/servo/%d/speed/%d",&servo, &speed)==2) && (strcmp(method, "GET")==0)) {
		Log::logger->log("HTTP",NOTICE) << "Matching set servo speed : " << servo << " speed: " << speed<< endl;
		response=HttpServer::setServoSpeed(servo, speed);
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (sscanf(url, "/api/servo/%d/acceleration/%d",&servo, &acceleration)==2) && (strcmp(method, "GET")==0)) {
		Log::logger->log("HTTP",NOTICE) << "Matching set servo acceleration : " << servo << " acceleration: " << acceleration<< endl;
		response=HttpServer::setServoAcceleration(servo, acceleration);
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (sscanf(url, "/api/servo/%d/min/%d",&servo, &min)==2) && (strcmp(method, "GET")==0)) {
		Log::logger->log("HTTP",NOTICE) << "Matching set servo min : " << servo << " min: " << min<< endl;
		response=HttpServer::setServoMin(servo, min);
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (sscanf(url, "/api/servo/%d/max/%d",&servo, &max)==2) && (strcmp(method, "GET")==0)) {
		Log::logger->log("HTTP",NOTICE) << "Matching set servo max : " << servo << " max: " << max<< endl;
		response=HttpServer::setServoMax(servo, max);
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (sscanf(url, "/api/servo/%d/neutral/%d",&servo, &neutral)==2) && (strcmp(method, "GET")==0)) {
		Log::logger->log("HTTP",NOTICE) << "Matching set servo neutral : " << servo << " neutral: " << neutral<< endl;
		response=HttpServer::setServoNeutral(servo, neutral);
		ret=MHD_HTTP_OK;
	}
	if (ret==MHD_NO) {
		Log::logger->log("HTTP",NOTICE) << url << "not a defined in the api" << endl;
		int fd=-1;
		string filename=HttpServer::documentroot+url;
		if ((fd = open(filename.c_str(), O_RDONLY)) == -1) {
			Log::logger->log("HTTP",NOTICE) << "File " << filename << " not found on this server" << endl;
			char * data = strdup("<html><body><p>404 Not Found</p></body></html>");
			response = MHD_create_response_from_buffer(strlen(data), data,MHD_RESPMEM_PERSISTENT);
			ret=404;
		} else {
			Log::logger->log("HTTP",NOTICE) << filename.c_str() << " existed" << endl;
			struct stat infos;
			fstat(fd, &infos);
			response=MHD_create_response_from_fd (infos.st_size, fd);
			ret=MHD_HTTP_OK;
		}
	}
	if (response!=NULL) {
		ret = MHD_queue_response(connection,ret,response);
  		MHD_destroy_response(response);
	}
	return ret;	
}
