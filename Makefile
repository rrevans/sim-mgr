OBJDIR := obj
TARGETS= $(OBJDIR) obj/simmgr obj/simmgrDemo obj/simpulse obj/simstatus.cgi obj/scenario obj/obscmd obj/obsmon

## -pthread (Posix Threads) is required where shared memory and/or multiple threads are used
CFLAGS=-pthread -Wall -g -ggdb -rdynamic
CXXFLAGS = -pthread -Wall -g -ggdb -rdynamic -std=c++11

## librt is required by any C/C++ where shared memory and/or multiple threads are used
LDFLAGS=-lrt

## CGIBIN must be set for the location of the cgi-bin directory on your server
CGIBIN=/var/lib/cgi-bin
BIN=/usr/local/bin

default: $(OBJDIR) obj/simstatus.cgi obj/simmgr obj/simmgrDemo obj/simpulse obj/scenario obj/obscmd obj/obsmon vpnconf

$(OBJDIR):
	mkdir -p $(OBJDIR)
	
demo: obj/simmgrDemo
	sudo cp -u obj/simmgrDemo $(BIN)
	sudo chown simmgr:simmgr $(BIN)/simmgrDemo
	sudo chmod u+s $(BIN)/simmgrDemo

vpnconf: src/vpnconf.c
	g++ -Wall -o vpnconf src/vpnconf.c

obj/obscmd: src/obscmd.c  include/obsmon.h
	g++ -Wall -o obj/obscmd src/obscmd.c $(LDFLAGS)

obj/obsmon: src/obsmon.c include/obsmon.h
	g++ -Wall -o obj/obsmon src/obsmon.c $(LDFLAGS)
	
obj/scenario: src/scenario.cpp obj/llist.o obj/sim-util.o obj/sim-parse.o obj/llist.o include/scenario.h include/simmgr.h
	g++ $(CPPFLAGS)-I/usr/include/libxml2  $(CXXFLAGS) -o obj/scenario src/scenario.cpp obj/sim-util.o obj/sim-parse.o obj/llist.o $(LDFLAGS) -lxml2
	
obj/tinyxml2.o: src/tinyxml2/tinyxml2.cpp src/tinyxml2/tinyxml2.h
	g++ $(CPPFLAGS) $(CXXFLAGS) -g -c -o obj/tinyxml2.o src/tinyxml2/tinyxml2.cpp

obj/sim-util.o: src/sim-util.c include/simmgr.h
	g++ $(CPPFLAGS) $(CXXFLAGS) -g -c -o obj/sim-util.o src/sim-util.c

obj/simpulseDemo.o: src/simpulseDemo.cpp include/simmgr.h
	g++ $(CPPFLAGS) $(CXXFLAGS) -g -c -o obj/simpulseDemo.o src/simpulseDemo.cpp
	
obj/sim-parse.o: src/sim-parse.c include/simmgr.h
	g++ $(CPPFLAGS) $(CXXFLAGS) -g -c -o obj/sim-parse.o src/sim-parse.c

obj/sim-log.o: src/sim-log.c include/simmgr.h
	g++ $(CPPFLAGS) $(CXXFLAGS) -g -c -o obj/sim-log.o src/sim-log.c

obj/llist.o: src/llist.c include/llist.h
	g++ $(CPPFLAGS) $(CXXFLAGS) -g -c -o obj/llist.o src/llist.c
	
obj/simstatus.cgi: src/simstatus.cpp obj/sim-util.o obj/sim-parse.o obj/sim-log.o include/simmgr.h
	g++ $(CPPFLAGS) $(CXXFLAGS) -o obj/simstatus.cgi src/simstatus.cpp obj/sim-util.o obj/sim-parse.o obj/sim-log.o $(LDFLAGS) -lcgicc

obj/cookie.cgi: src/cookie.cpp 
	g++ $(CPPFLAGS) $(CXXFLAGS) -o obj/cookie.cgi src/cookie.cpp -lcgicc
	
obj/simmgr: src/simmgr.cpp obj/sim-log.o obj/sim-util.o include/simmgr.h
	g++ $(CPPFLAGS) $(CXXFLAGS)  -lcgicc -o obj/simmgr src/simmgr.cpp  obj/sim-log.o obj/sim-util.o $(LDFLAGS)
	
obj/simmgrDemo: src/simmgrDemo.cpp obj/sim-log.o obj/sim-util.o obj/simpulseDemo.o include/simmgr.h
	g++ $(CPPFLAGS) $(CXXFLAGS)  -lcgicc -o obj/simmgrDemo src/simmgrDemo.cpp  obj/sim-log.o obj/sim-util.o obj/simpulseDemo.o $(LDFLAGS)
	
obj/simpulse: src/simpulse.cpp obj/sim-util.o include/simmgr.h
	g++ $(CPPFLAGS) $(CXXFLAGS)  -lcgicc -o obj/simpulse src/simpulse.cpp  obj/sim-util.o $(LDFLAGS)
	
install: check $(TARGETS) installDaemon
	sudo cp -u obj/simstatus.cgi $(CGIBIN)
	sudo chown simmgr:simmgr $(CGIBIN)/simstatus.cgi
	sudo chmod u+s $(CGIBIN)/simstatus.cgi
	sudo cp -u obj/simmgr $(BIN)
	sudo chown simmgr:simmgr $(BIN)/simmgr
	sudo chmod u+s $(BIN)/simmgr
	sudo cp -u obj/obsmon $(BIN)
	sudo cp -u obj/simmgrDemo $(BIN)
	sudo chown simmgr:simmgr $(BIN)/simmgrDemo
	sudo chmod u+s $(BIN)/simmgrDemo
	sudo cp -u obj/simpulse $(BIN)
	sudo chown simmgr:simmgr $(BIN)/simpulse
	sudo chmod u+s $(BIN)/simpulse
	sudo cp -u obj/scenario $(BIN)
	sudo chown simmgr:simmgr $(BIN)/scenario
	sudo chmod u+s $(BIN)/scenario
	sudo cp -u scripts/obs_*.sh $(BIN)
	sudo chmod 0755 $(BIN)/obs_*.sh $(BIN)/obsmon

installDaemon:
	sudo cp -u simmgr.init /etc/init.d/simmgr
	sudo update-rc.d simmgr defaults
	
removeDaemon:
	sudo update-rc.d -f simmgr remove

clean:
	rm obj/*

## Check for the installed tools we require
check:
	@if [ ! -e /usr/bin/g++ ]; then echo "GNU C++ Compiler (g++) is not found in /usr/bin\n"; exit ; fi;
	@if [ ! -e /usr/lib/libcgicc.so ]; then echo "CGICC Library libcgicc.so is not found in /usr/lib\n"; exit ; fi;
