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
#include <unistd.h>

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
    Config::config()->loadOptions(&options);
    Parameters * params=new Parameters(config);
    params->add("document-root", "Root folder to serve documents", true);
    params->add("longitude", "The longitude of the installation point", true);
    params->add("latitude", "The latitude of the installation point", true);
    params->add("http-port", "The port number of the http server", true);
    params->add("have-phidget", "Declare if yes or no we have a phidget 8/8/8 board connected", true);
    params->add("have-maestro", "Declare if yes or no we have a micro maestro board connected", true);

    params->add("earth-servo", "The code number of the servo associate to Earth rotation", false);
    params->add("pole-servo", "The code number of the servo to align to the celest pole", false);
    params->add("ecliptic-servo", "The code number of the servo to align ecliptic plan", false);
    params->add("reverse-earth-servo", "The code number of the servo to reverse earth rotation (mirror alignment)", false);

    params->add("input-scan", "The index on the Phidget board for the scan button", false);
    params->add("output-scan", "The index on the Phidget board for the scan LED", false);
    params->add("input-earth", "The index on the Phidget board for the earth rotation button", false);
    params->add("output-earth", "The index on the Phidget board for the earth rotation LED", false);


    if (options.get("create")->isAssign()) {
      try {
        params->create();
      } catch(CantCreateFileException &e) {
        Log::logger->log("GLOBAL", EMERGENCY) << "Can't create file " << config<< endl;
      }
      exit(0);
    }
    Servo * pole=NULL;
    Servo * ecliptic=NULL;
    Servo * reverse=NULL;
    try {
      try {
        params->parse();
        Config::config()->loadParameters(params);
       
        try {
        	if (params->get("have-maestro")->asBool()) {
           		if (Maestro::init()) {
            		if (params->get("pole-servo")->isAssign()) pole=new Servo(params->get("pole-servo")->asInt(), "pole orientation");
            		usleep(500000);
            		if (params->get("earth-servo")->isAssign()) Earth::init(params->get("earth-servo")->asInt(),params->get("longitude")->asDouble(),params->get("latitude")->asDouble());
            		usleep(500000);
            		if (params->get("ecliptic-servo")->isAssign()) ecliptic=new Servo(params->get("ecliptic-servo")->asInt(), "ecliptic");
            		usleep(500000);
            		if (params->get("reverse-earth-servo")->isAssign()) reverse=new Servo(params->get("reverse-earth-servo")->asInt(), "reverse");
          		} else {
            		Log::logger->log("MAIN",EMERGENCY) << "No Maestro controller detected" << endl;
          		}
          	}
          	if (params->get("have-phidget")->asBool()) {
              Log::logger->log("MAIN",DEBUG) << "Trying to attach phidget board" << endl;
          		Phidget::attach();
              Tracker::attachPhidget();
          	}
            Log::logger->log("MAIN",DEBUG) << "Trying to initiate http server" << endl;
          	HttpServer * server=new HttpServer(params->get("http-port")->asInt(), params->get("document-root")->asString());
            Log::logger->log("MAIN",DEBUG) << "Trying to start http server" << endl;
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
