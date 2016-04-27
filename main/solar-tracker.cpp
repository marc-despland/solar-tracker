#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "httpserver.h"
#include "log.h"



int main(int argc, char ** argv) {
  Log::logger->setLevel(DEBUG);
  if (argc != 2) {
    printf("%s PORT\n",
	   argv[0]);
    return 1;
  }

  HttpServer * server=new HttpServer(atoi(argv[1]));
  server->start();
  Log::logger->log("MAIN",NOTICE) << "Deamon start listening on port = " << argv[1] << endl;
  (void) getc (stdin);
  server->stop();
  return 0;
}