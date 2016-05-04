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


HttpServer::HttpServer(unsigned int port) {
	this->port=port; 

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
	tmp << Phidget::board()->getStatus() <<","<<endl;
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
	int servo, position;
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
		Log::logger->log("HTTP",NOTICE) << "Matching set servo position : " << servo << endl;
		response=HttpServer::setServoPosition(servo, position);
		ret=MHD_HTTP_OK;
	}
	if ((ret==MHD_NO) && (sscanf(url, "/api/servo/%d/angle/%lf",&servo, &angle)==2) && (strcmp(method, "GET")==0)) {
		Log::logger->log("HTTP",NOTICE) << "Matching set servo position : " << servo << endl;
		response=HttpServer::setServoAngle(servo, angle);
		ret=MHD_HTTP_OK;
	}
	if (response!=NULL) {
		ret = MHD_queue_response(connection,ret,response);
  		MHD_destroy_response(response);
	}
	return ret;	
}