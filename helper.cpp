/*
 * clioagent/helper.cpp 
 *
 * Copyright (C) 2015 - rhythmhu ( withery@icloud.com )
 *
 */

#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "helper.h"

using namespace std ;

int AgentHelper::Parse( const string &configFile ,map<string,map<string,string> > &res ,string & errMsg )
{	
	FILE *config = fopen( configFile.c_str() ,"r" ) ;
	if( config == NULL ){
		errMsg += "can't find config\n" ;
		return -1 ;
	}

	int line = 1 ;
	const int buffer_size = 256 ;
	char c = 0 ,error[buffer_size] ;
	string key ,value ,command ; 
	int status = NEWLINE ;

	while( ( c = fgetc(config) ) != EOF )
	{
		if( c == '\n' ) line ++ ;
		if( status == NEWLINE )
		{
			value = key = "" ;
			if( c == ' ' || c == '\n' || c == '\t' ) 
				continue ;

			if( c == '[' )
			{
				command = "" ;
				status = GET_COMMAND ;
			} 
			else if( c == '#' )
			{
				status = ANNOTATION ;
			}
			else
			{
				status = GET_KEY ;
				key += c ;
			}
		} 
		else if( status == GET_COMMAND )
		{
			if( c == ']' )
			{
				status = COMMAND_OVER ;
			}
			else
			{
				command += c ;
			}
		} 
		else if( status == COMMAND_OVER )
		{
			if( c == '\n' )
			{
				status = NEWLINE ;
			} 
			else if( c == '#' )
			{
				status = ANNOTATION ;

			}
			else if( c != ' ' && c != '\t' )
			{
				status = ERROR ;
				snprintf( error ,buffer_size ,"line %d Presentation Error\n" ,line - (c=='\n') ) ;
				errMsg += error ;
			}
		} 
		else if( status == GET_KEY )
		{
			if( c == ' ' || c == '\t' )
			{
				status = GET_EQUAL ;	
			} 
			else if( c == '=' )
			{
				status = KEY_OVER ;
			} 
			else
			{
				key += c ;
			}
		} 
		else if( status == GET_EQUAL )
		{
			if( c == '=' )
			{
				status = KEY_OVER ;
			}
			else if( c != ' ' && c != '\t' )
			{
				status = ERROR ;
				snprintf( error ,buffer_size ,"line %d Presentation Error\n" ,line - (c=='\n') ) ;
				errMsg += error ;
			}
		} 
		else if( status == KEY_OVER )
		{
			if( c != ' ' && c != '\t' ) 
			{
				if( c >= '0' && c <= '9' ) 
				{
					status = GET_DEG_VALUE ;
					value += c ;
				}
				else if( c == '"' )
				{
					status = GET_STR_VALUE ;
				}
				else
				{
					status = ERROR ;
					snprintf( error ,buffer_size ,"line %d Presentation Error\n" ,line - (c=='\n') ) ;
					errMsg += error ;
				}
			}
		} 
		else if( status == GET_STR_VALUE )
		{
			if( c == '"' )
			{
				status = VALUE_OVER ;
				res[command][key] = value ;
			} 
			else if( c == '\n' )
			{
				status = ERROR ;	
				snprintf( error ,buffer_size ,"line %d Presentation Error\n" ,line - (c=='\n') ) ;
				errMsg += error ;
			} 
			else 
				value += c ;
		} 
		else if( status == GET_DEG_VALUE )
		{
			if( c >= '0' && c <= '9' )
			{
				value += c ;
			} 
			else if( c == ' ' || c == '\t' )
			{
				status = VALUE_OVER ;
				res[command][key] = value ;
				status = NEWLINE ;
			} 
			else if( c == '\n' )
			{
				res[command][key] = value ;
				status = NEWLINE ;
			} 
			else if( c == '#' )
			{
				res[command][key] = value ;
				status = ANNOTATION ;
			}
			else
			{	
				status = ERROR ;
				snprintf( error ,buffer_size ,"line %d Presentation Error\n" ,line - (c=='\n') ) ;
				errMsg += error ;
			}
		} 
		else if( status == VALUE_OVER )
		{
			if( c == '\n' )
			{
				status = NEWLINE ;
			}
			else if( c == '#' )
			{
				status = ANNOTATION ;
			}	
			else if( c != ' ' && c != '\t' )
			{
				status = ERROR ;
				snprintf( error ,buffer_size ,"line %d Presentation Error\n" ,line - (c=='\n') ) ;
				errMsg += error ;
			}
		}
		else if( status == ANNOTATION || status == ERROR )
		{
			if( c == '\n' )
			{
				status = NEWLINE ;
			}
		}
	}
	if( errMsg != "" )
	{
		return -1 ;
	}
	return 0 ;
}

int AgentHelper::GetCurrentTime( string &hour ,string &currTime ){

	time_t timer = time(NULL) ;
	struct tm *tblock ;
	tblock = localtime(&timer) ;

	char buffer[1024] ;

	hour = currTime = "" ;

	snprintf( buffer ,1024 ,"%d%02d%02d%d" ,
			tblock->tm_year+1900 ,
			tblock->tm_mon+1 ,
			tblock->tm_mday ,
			tblock->tm_hour) ;
	hour = buffer ;

	snprintf( buffer ,1024 ,"%02d:%02d" ,
			tblock->tm_min ,
			tblock->tm_sec ) ;
	currTime = buffer ;

	return 0 ;
}

unsigned int AgentHelper::String2Uint( const string & str ,string & errMsg ){
	unsigned int ret = 0 ;
	for( int i = 0 ;i < str.size() ;i ++ ){
		if( str[i] >= '0' && str[i] <= '9' ){
			ret = ret * 10 + str[i] - '0' ;	
		}
		else{
			errMsg += "\"" + str + "\" : String2Uint Fail!\n" ;
			return 0 ;
		}
	}
	return ret ;
}

int AgentHelper::CheckLock( const string & file ,const string & moduleName ,string &errMsg ){
	int fd = fileno(fopen( file.c_str() ,"w+" )) ;
	if( fd >= 0 ){
		flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_whence = SEEK_SET;
		lock.l_len = 0;

		if( fcntl( fd, F_SETLK, &lock ) < 0 ){
			errMsg += moduleName + " is already running!" ;
			return -1 ;
		}
		return 0 ;
	}
	return 1 ;
}
