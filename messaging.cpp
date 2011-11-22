/*
 * messaging.cpp
 *
 *  Created on: Apr 24, 2010
 *      Author: senem
 */

#include "messaging.h"

/* helper functions that are needed for logging */
char * getCurrentTime()
{
	memset(time_string, 0, TIME_STRING_SIZE);

	time_t rawtime;
	time (&rawtime);
	struct tm * timeinfo = localtime (&rawtime);

	strftime (time_string, TIME_STRING_SIZE, "%c", timeinfo);

	return time_string;
}

char * createMessageForLogFile(const char * current_time, const int iD, const char * message)
{
	memset(log_message, 0, LOG_MESSAGE_SIZE);

	sprintf(log_message, "%s - ID %d: %s \n", current_time, iD, message);

	return log_message;
}
