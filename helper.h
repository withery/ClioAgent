/*
 * clioagent/helper.h 
 *
 * Copyright (C) 2015 - rhythmhu ( withery@icloud.com )
 *
 */

#ifndef HELPER_H
#define HELPER_H

#include <map>
#include <string>
using namespace std ;

struct AgentHelper{
	enum{
		NEWLINE ,
		GET_COMMAND ,
		GET_KEY ,
		GET_STR_VALUE ,
		GET_DEG_VALUE ,
		GET_EQUAL ,
		COMMAND_OVER ,
		KEY_OVER ,
		VALUE_OVER ,
		ERROR ,
		ANNOTATION 	
	};


	int Parse( string configFile ,map<string,map<string,string> > & res ,string & errMsg ) ;

	int GetCurrentTime( string & hour ,string & currTime ) ;

	unsigned int String2Uint( string str ,string & errMsg ) ;

	int PostData( string url ,string data ) ;

	int CheckLock( string file ,string moduleName ,string & errMsg ) ;
} ;

#endif /* PARSE_H */
