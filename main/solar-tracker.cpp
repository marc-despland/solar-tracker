#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "httpserver.h"
#include "log.h"
#include "phidget.h"
#include "earth.h"
#include "options.h"
#include "parameters.h"
#include "tracker.h"

int main(int argc, char ** argv) {
  Log::logger->setLevel(DEBUG);
  string config="config.cfg";
  Options options(argv[0], "1.0.0", "Earth Tracker");
  try {
    options.add('f', "config", "The configuration file to parse", true, false);
    options.add('c', "create", "Generate a sample config file", false, false);
  } catch(ExistingOptionException &e ) {
  }
  try {
    options.parse(argc, argv);
    if (options.get("config")->isAssign()) {
      config=options.get("config")->asString();
    }
    Parameters * params=new Parameters(config);
    params->add("document-root", "Root folder to serve documents", true);
    params->add("http-port", "The port number of the http server", true);
    params->add("earth-servo", "The number of the servo associate to Earth rotation", true);
    params->add("longitude", "The longitude of the installation point", true);
    params->add("latitude", "The latitude of the installation point", true);


    if (options.get("create")->isAssign()) {
      try {
        params->create();
      } catch(CantCreateFileException &e) {
        Log::logger->log("GLOBAL", EMERGENCY) << "Can't create file " << config<< endl;
      }
      exit(0);
    }
    try {
      try {
        params->parse();
        try {
          if (Maestro::init()) {
            Earth::init(params->get("earth-servo")->asInt(),params->get("longitude")->asDouble(),params->get("latitude")->asDouble());
          } else {
            Log::logger->log("MAIN",EMERGENCY) << "No Maestro controller detetcted" << endl;
          }
          Phidget::attach();
          HttpServer * server=new HttpServer(params->get("http-port")->asInt());
          server->start();
          Log::logger->log("MAIN",NOTICE) << "Deamon start listening on port = " << params->get("http-port")->asInt() << endl;
          (void) getc (stdin);
          Earth::stop();
          server->stop();
        } catch (UnknownParameterNameException &e) {
          Log::logger->log("GLOBAL", EMERGENCY) << "Not defined parameter " << endl;
        }
      }catch(FileNotFoundException &e) {
        Log::logger->log("GLOBAL", EMERGENCY) << "Can't open file " << config<< endl;
      }catch(InvalidConfigFileException &e) {
        Log::logger->log("GLOBAL", EMERGENCY) << "Invalid configuration file " << config<< endl;
      }
    }catch(FileNotFoundException &e) {
      Log::logger->log("GLOBAL", EMERGENCY) << "Can't open file  " << config<< endl;
    }
  } catch (OptionsStopException &e) {
  } catch (UnknownOptionException &e) {
    Log::logger->log("MAIN",NOTICE) << "Unknown option requested" << endl;
  }
  return 0;
}
