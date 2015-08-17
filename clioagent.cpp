/*
 * clioagent/clioagent.cpp
 *
 * Copyright (C) 2015 - rhythmhu ( withery@icloud.com )
 *
 */

#include <string>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>

#include "helper.h"
#include "clioagent.h"

using namespace std ;

#define GET(attr) ( itor->second.find(attr)==itor->second.end() ? cmdMap["meta"][attr] : itor->second[attr] )

int ClioAgent::RunCommand( Command & command ,string &errMsg ){
	AgentHelper helper ;
	FILE *logFd = NULL ;
	FILE *commandFd = NULL ;
	char logFile[1024] ,ioBuffer[1024] ,rmOldLog[1024] ,urlBuffer[1024] ;
	string hour ,currTime ; 
	helper.GetCurrentTime(hour,currTime) ;
	snprintf( logFile ,1024 ,"%s/%s_%s" ,
			command.logDir.c_str() ,
			command.commandName.c_str() ,
			hour.c_str() ) ;
	snprintf( rmOldLog ,1024 ,"find %s -cmin +%d -type f -name \"%s_*\" -exec rm -f {} \\;" ,
			command.logDir.c_str() ,
			command.holdTime * 60 ,
			command.commandName.c_str() ) ;
	system( rmOldLog ) ;

	if( ( logFd = fopen( logFile ,"a" ) ) == NULL ){
		printf( "open %s fail\n" ,logFile ) ;
		return -1 ;
	}
	do{
		if( helper.CheckLock( command.logDir+"/"+command.commandName+".lk" ,command.commandName ,errMsg ) ){
			puts( errMsg.c_str() ) ;
			return 0 ;
		}
		bool runFail = true ;
		if( ( commandFd = popen( command.commandFormat.c_str() ,"r" ) ) == NULL ){
			return -1 ;
		}
		while( fgets( ioBuffer ,sizeof(ioBuffer) ,commandFd ) != NULL ){
			runFail = false ;
			string mhour ;
			helper.GetCurrentTime(mhour,currTime) ;
			if( mhour != hour ){
				hour = mhour ;
				fclose(logFd) ;
				snprintf( logFile ,1024 ,"%s/%s_%s" ,
						command.logDir.c_str() ,
						command.commandName.c_str() ,
						hour.c_str() ) ;
				logFd = fopen( logFile ,"a" ) ;

				snprintf( rmOldLog ,1024 ,"find %s -cmin +%d -type f -name \"%s_*\" -exec rm -f {} \\;" ,
						command.logDir.c_str() ,
						command.holdTime * 60 ,
						command.commandName.c_str() ) ;
				system( rmOldLog ) ;
			}
			fprintf( logFd ,"%s  %s" ,currTime.c_str() ,ioBuffer ) ;
			fflush( logFd ) ;
		}
		pclose( commandFd ) ;
		if( runFail ){
			printf( "run \"%s\" fail\n" ,command.commandFormat.c_str() ) ;
			return -1 ;
		}
		if( command.cycleNeeded ){
			fprintf( logFd ,"\n" ) ;
			sleep( command.cycle ) ;
		}
	}while(command.cycleNeeded) ;	
	return 0 ;
}	

int ClioAgent::LoadCommand( string config ,string & errMsg ){
	typedef map<string,map<string,string> > ParseRes ;
	AgentHelper helper ;
	ParseRes cmdMap ;
	helper.Parse( config ,cmdMap ,errMsg ) ;
	if( cmdMap.find( "meta" ) == cmdMap.end() ){
		errMsg += "not find meta\n" ;
	}
	else{
		if( cmdMap["meta"].find( "logDir" ) == cmdMap["meta"].end() )
			errMsg += "not find meta-logDir\n" ;
		if( cmdMap["meta"].find( "uploadSwitch" ) == cmdMap["meta"].end() )
			errMsg += "not find meta-uploadSwitch\n" ;
		if( cmdMap["meta"].find( "cycleNeeded" ) == cmdMap["meta"].end() )
			errMsg += "not find meta-cycleNeeded\n" ;
		if( cmdMap["meta"].find( "cycle" ) == cmdMap["meta"].end() )
			errMsg += "not find meta-cycle\n" ;
		if( cmdMap["meta"].find( "holdTime" ) == cmdMap["meta"].end() )
			errMsg += "not find meta-holdTime\n" ;
	}
	for( ParseRes::iterator itor = cmdMap.begin() ;itor != cmdMap.end() ;itor ++ ){
		if( itor->first == "meta" || itor->first == "general" ) continue ;
		if( itor->second.find("commandFormat") == itor->second.end() ){
			errMsg += "not find " + itor->first + "-commandFormat\n" ; 
		}
	}

	if( errMsg != "" ) return -1 ;
	for( ParseRes::iterator itor = cmdMap.begin() ;itor != cmdMap.end() ;itor ++ ){
		if( itor->first == "meta" || itor->first == "general" ) continue ;	
		Command command ;
		command.commandName = itor->first ; 
		command.commandFormat = itor->second["commandFormat"] ;
		command.logDir = GET("logDir") ; 
		command.uploadSwitch = helper.String2Uint(GET("uploadSwitch") ,errMsg ) ;
		command.cycleNeeded = helper.String2Uint(GET("cycleNeeded") ,errMsg ) ;
		command.cycle = helper.String2Uint(GET("cycle") ,errMsg ) ;
		command.holdTime = helper.String2Uint(GET("holdTime") ,errMsg ) ;
		cmdTable.push_back(command) ;
	}
	if( errMsg == "" ) return 0 ;
	return -1 ;
}

int ClioAgent::Run( string config ){
	string errMsg ;
	LoadCommand( config ,errMsg ) ;
	if( errMsg != "" )
	{
		puts( errMsg.c_str() ) ;
		return -1 ;
	}
	for( int i = 0 ;i < cmdTable.size() ;i ++ ){
		if( fork() == 0){
			RunCommand( cmdTable[i] ,errMsg ) ;
			exit(0) ;
		}
	}
}

int main( int argc ,char **argv ){
	ClioAgent().Run( argc > 1 ? argv[1] : "./ClioAgent.conf" ) ;
}
