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
#include "controller.h"
#include "regulator.h"
#include "searchalgorithm.h"
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
    params->add("input-regulator", "The index on the Phidget board for the regulation start button", false);
    params->add("output-regulator", "The index on the Phidget board for the regulation LED", false);


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
        Config::config()->loadParameters(params);
       
        try {
        	Controller::controller()->init();
          	if (params->get("have-phidget")->asBool()) {
              Log::logger->log("MAIN",DEBUG) << "Trying to attach phidget board" << endl;
          		Phidget::attach();
              Controller::attachPhidget();
              //if (params->get("earth-servo")->isAssign()) Earth::attachPhidget();
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
