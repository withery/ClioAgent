/*
 * clioagent/clioagent.h
 *
 * Copyright (C) 2015 - rhythmhu ( withery@icloud.com )
 *
 */

#ifndef CLIOAGENT_H
#define CLIOAGENT_H

#include <string>
#include <vector>
#include <map>

using namespace std ;

struct Command{
	string commandName ;
	string commandFormat ;
	string logDir ;
	short holdTime ;
	short uploadSwitch ;
	short cycleNeeded ; 
	short cycle ;
} ;

struct ClioAgent{
	vector<Command> cmdTable ;
	string postUrl ;
	int LoadCommand( string config ,string & errMsg ) ;
	int RunCommand( Command & command ,string & errMsg ) ;
	int Run( string config = "./clioagent.conf" ) ;
} ;
#endif /* CLIOAGENT_H */
