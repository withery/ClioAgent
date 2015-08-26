/*
 * clioagent/helper.h 
 *
 * Copyright (C) 2015 - rhythmhu ( withery@icloud.com )
 *
 */

#ifndef CLIOAGENT_HELPER_H
#define CLIOAGENT_HELPER_H

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


	int Parse( const string &configFile ,map<string,map<string,string> > & res ,string & errMsg ) ;

	int GetCurrentTime( string & hour ,string & currTime ) ;

	unsigned int String2Uint( const string & str ,string & errMsg ) ;

	int PostData( string url ,string data ) ;

	int CheckLock( const string & file ,const string & moduleName ,string & errMsg ) ;
} ;

#endif /* CLIOAGENT_HELPER_H */
