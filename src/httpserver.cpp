#include "httpserver.h"

#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "phidget.h"
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
	tmp << Phidget::board()->getStatus() <<endl;
	tmp << "}" << endl;
	char * status=strdup(tmp.str().c_str());
	return MHD_create_response_from_buffer (strlen(status),(void*) status,MHD_RESPMEM_PERSISTENT);
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
	struct MHD_Response * response=NULL;
	Log::logger->log("HTTP",NOTICE) << method << " " << url << " (" << *upload_data_size << ")" << endl;
	if ((ret==MHD_NO) && (strcmp(url, "/status")==0) && (strcmp(method, "GET")==0)) {
		response=HttpServer::getStatus();
	}
	if (response!=NULL) {
		ret = MHD_queue_response(connection,MHD_HTTP_OK,response);
  		MHD_destroy_response(response);
	}
	return ret;	
}