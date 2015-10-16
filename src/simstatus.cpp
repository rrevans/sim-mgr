/*
 * simstatus.cpp
 *
 * Open the SHM segment and provide status/control operations.
 *
 * Copyright 2015 Terence Kelleher. All rights reserved.
 *
 */
 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>  
#include <string>  
#include <cstdlib>
#include <sstream>
#include <utility>

#include "../include/simmgr.h" 

#include <cgicc/CgiDefs.h> 
#include <cgicc/Cgicc.h> 
#include <cgicc/HTTPHTMLHeader.h> 
#include <cgicc/HTMLClasses.h>  


using namespace std;
using namespace cgicc;

struct paramCmds
{
	char cmd[16];
};

void makejson(ostream & output, string key, string content)
{
    output << "\"" << key << "\":\"" << content << "\"";
}

std::vector<std::string> explode(std::string const & s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(token );
    }

    return result;
}

struct paramCmds paramCmds[] =
{
	{ "check", },
	{ "uptime", },
	{ "date", },
	{ "",  }
};
int debug = 0;

int
main( int argc, const char* argv[] )
{
    char buffer[256];
	char cmd[32];
    //struct tm* tm_info;
	//unsigned int val;
	//int index;
	int i;
	int set_count = 0;
	int sts;
	char *cp;
	std::string key;
	std::string value;
	std::string command;
	std::vector<std::string> v;
	
	// Cgicc formData;
	const_form_iterator iter;
	Cgicc formData;
	
	cout << "Content-Type: application/json\r\n\r\n";
	cout << "{\n";

	sts = initSHM(OPEN_ACCESS );
	if ( sts < 0 )
	{
		makejson(cout, "error", "initSHM failed" );
		cout << "\n}\n";
		return ( 0 );
	}
	

	i = 0;
	sprintf(cmd, "none" );
	try 
	{
		Cgicc cgi;
		
		// Parse the submitted GET/POST elements
		for( iter = cgi.getElements().begin(); iter != cgi.getElements().end(); ++iter )
		{
			key = iter->getName();
			value = iter->getValue();
			if ( i > 0 )
			{
				cout << ",\n";
			}
			i++;
			if ( key.compare("check" ) == 0 )
			{
				makejson(cout, "check", "check is ok" );
			}
			else if ( key.compare("uptime" ) == 0 )
			{
				cp = do_command_read("/usr/bin/uptime", buffer, sizeof(buffer)-1 );
				if ( cp == NULL )
				{
					makejson(cout, "uptime", "no data");
				}
				else
				{
					makejson(cout, "uptime", buffer);
				}
			}
			else if ( key.compare("date" ) == 0 )
			{
				get_date(buffer );
				makejson(cout, "date", buffer );
			}
			else if ( key.compare("ip" ) == 0 )
			{
				makejson(cout, "ip_addr", simmgr_shm->server.ip_addr );
			}
			else if ( key.compare("host" ) == 0 )
			{
				makejson(cout, "hostname", simmgr_shm->server.name );
			}
			else if ( key.compare("time" ) == 0 )
			{
				makejson(cout, "time", simmgr_shm->server.server_time );
			}
			else if ( key.compare("status" ) == 0 )
			{
				// The meat of the task - Return the content of the SHM
				cout << " \"scenario\" : {\n";
				makejson(cout, "active", simmgr_shm->status.scenario.active );
				cout << ",\n";
				makejson(cout, "start", simmgr_shm->status.scenario.start );
				cout << ",\n";
				makejson(cout, "start_msec", itoa(simmgr_shm->status.scenario.start_msec, buffer, 10) );
				cout << "\n},\n";
				
				cout << " \"cardiac\" : {\n";
				makejson(cout, "rhythm", simmgr_shm->status.cardiac.rhythm	);
				cout << ",\n";
				makejson(cout, "rate", itoa(simmgr_shm->status.cardiac.rate, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "pwave", simmgr_shm->status.cardiac.pwave );
				cout << ",\n";
				makejson(cout, "pr_interval", itoa(simmgr_shm->status.cardiac.pr_interval, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "qrs_interval", itoa(simmgr_shm->status.cardiac.qrs_interval, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "bps_sys", itoa(simmgr_shm->status.cardiac.bps_sys, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "bps_dia", itoa(simmgr_shm->status.cardiac.bps_dia, buffer, 10 ) );
				cout << "\n},\n";
				
				cout << " \"respiration\" : {\n";
				makejson(cout, "left_sound_in", simmgr_shm->status.respiration.left_sound_in );
				cout << ",\n";
				makejson(cout, "left_sound_out", simmgr_shm->status.respiration.left_sound_out );
				cout << ",\n";
				makejson(cout, "left_sound_back", simmgr_shm->status.respiration.left_sound_back );
				cout << ",\n";
				makejson(cout, "right_sound_in", simmgr_shm->status.respiration.right_sound_in );
				cout << ",\n";
				makejson(cout, "right_sound_out", simmgr_shm->status.respiration.right_sound_out );
				cout << ",\n";
				makejson(cout, "right_sound_back", simmgr_shm->status.respiration.right_sound_back );
				cout << ",\n";
				makejson(cout, "inhalation_duration", itoa(simmgr_shm->status.respiration.inhalation_duration, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "exhalation_duration", itoa(simmgr_shm->status.respiration.exhalation_duration, buffer, 10 ) );
				cout << "\n},\n";
				
				cout << " \"auscultation\" : {\n";
				makejson(cout, "side", itoa(simmgr_shm->status.auscultation.side, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "row", itoa(simmgr_shm->status.auscultation.row, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "col", itoa(simmgr_shm->status.auscultation.col, buffer, 10 ) );
				cout << "\n},\n";
				
				cout << " \"pulse\" : {\n";
				makejson(cout, "position", itoa(simmgr_shm->status.pulse.position, buffer, 10 ) );
				cout << "\n},\n";

				cout << " \"cpr\" : {\n";
				makejson(cout, "last", itoa(simmgr_shm->status.cpr.last, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "compression", itoa(simmgr_shm->status.cpr.compression, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "release", itoa(simmgr_shm->status.cpr.release, buffer, 10 ) );
				cout << "\n},\n";
				
				cout << " \"defibrillation\" : {\n";
				makejson(cout, "last", itoa(simmgr_shm->status.defibrillation.last, buffer, 10 ) );
				cout << ",\n";
				makejson(cout, "energy", itoa(simmgr_shm->status.defibrillation.energy, buffer, 10 ) );
				cout << "\n}\n";
				
			}
			else if ( key.compare(0, 4, "set:" ) == 0 )
			{
				set_count++;
				// set command: Split to segments to contruct the reference
				v = explode(key, ':');
				cout << " \"set_" << set_count << "\" : {\n    ";
				makejson(cout, "class", v[1] );
				cout << ",\n    ";
				makejson(cout, "param", v[2] );
				cout << ",\n    ";
				makejson(cout, "value", value );
				cout << ",\n    ";
				sts = 0;
				
				if ( v[1].compare("cardiac" ) == 0 )
				{
					if ( v[2].compare("rhythm" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.cardiac.rhythm, "%s", value.c_str() );
					}
					else if ( v[2].compare("pwave" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.cardiac.pwave, "%s", value.c_str() );
					}
					else if ( v[2].compare("rate" ) == 0 )
					{
						simmgr_shm->instructor.cardiac.rate = atoi(value.c_str() );
					}
					else if ( v[2].compare("pr_interval" ) == 0 )
					{
						simmgr_shm->instructor.cardiac.pr_interval = atoi(value.c_str() );
					}
					else if ( v[2].compare("qrs_interval" ) == 0 )
					{
						simmgr_shm->instructor.cardiac.qrs_interval = atoi(value.c_str() );
					}
					else if ( v[2].compare("bps_sys" ) == 0 )
					{
						simmgr_shm->instructor.cardiac.bps_sys = atoi(value.c_str() );
					}
					else if ( v[2].compare("bps_dia" ) == 0 )
					{
						simmgr_shm->instructor.cardiac.bps_dia = atoi(value.c_str() );
					}
					else
					{
						sts = 1;
					}
				}
				else if ( v[1].compare("scenario" ) == 0 )
				{
					if ( v[2].compare("active" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.scenario.active, "%s", value.c_str() );
					}
					else
					{
						sts = 1;
					}
				}
				else if ( v[1].compare("respiration" ) == 0 )
				{
					if ( v[2].compare("left_sound_in" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.respiration.left_sound_in, "%s", value.c_str() );
					}
					else if ( v[2].compare("left_sound_out" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.respiration.left_sound_out, "%s", value.c_str() );
					}
					else if ( v[2].compare("left_sound_back" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.respiration.left_sound_back, "%s", value.c_str() );
					}
					else if ( v[2].compare("right_sound_in" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.respiration.right_sound_in, "%s", value.c_str() );
					}
					else if ( v[2].compare("right_sound_out" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.respiration.right_sound_out, "%s", value.c_str() );
					}
					else if ( v[2].compare("right_sound_back" ) == 0 )
					{
						sprintf(simmgr_shm->instructor.respiration.right_sound_back, "%s", value.c_str() );
					}
					else if ( v[2].compare("inhalation_duration" ) == 0 )
					{
						simmgr_shm->instructor.respiration.inhalation_duration = atoi(value.c_str() );
					}
					else if ( v[2].compare("exhalation_duration" ) == 0 )
					{
						simmgr_shm->instructor.respiration.exhalation_duration = atoi(value.c_str() );
					}
					else if ( v[2].compare("rate" ) == 0 )
					{
						simmgr_shm->instructor.respiration.rate = atoi(value.c_str() );
					}
					else
					{
						sts = 1;
					}
				}
				else
				{
					sts = 2;
				}
				if ( sts == 1 )
				{
					makejson(cout, "status", "invalid param" );
				}
				else if ( sts == 2 )
				{
					makejson(cout, "status", "invalid class" );
				}
				else
				{
					makejson(cout, "status", "ok" );
				}
				cout << "\n    }";
			}
			else
			{
				makejson(cout, "Invalid Command", cmd );
			}
		}		
	}
	catch(exception& e) 
	{
		// Caught a standard library exception
		makejson(cout, "status", "Fail");
		cout << "\n}\n";
		return ( 0 );
	}
	
	cout << "\n}\n";
	return ( 0 );
}