/*
Syslog client v0.2

Copyright (c) 2009 Marc Thrun (marc.thrun@nexos-it.de)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <network.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "syslog.h"

struct syslog_instance_t {
	const char *myhost;
	const char *hostname;
	unsigned port;
	s32 socket;
	unsigned error;
};

static const char *Syslog_errormessages[] = {
	"no error",
	"invalid destination address",
	"error calling net_connect on socket",
	"unable to send packet"
};

const char *Syslog_GetErrorMessage(syslog_instance_t *instance)
{
	return Syslog_errormessages[instance->error];
}

unsigned Syslog_GetError(syslog_instance_t *instance)
{
	return instance->error;
}

syslog_instance_t *Syslog_Start(const char *myhost)
{
	// Create new instance
	syslog_instance_t *instance = malloc(sizeof *instance);
	
	if( instance )
	{
		// Clear instance structure
		memset(instance, 0, sizeof *instance);
		
		// Save my hostname
		instance->myhost = myhost;
		
		// Create socket
		instance->socket = net_socket(AF_INET, SOCK_DGRAM, 0);
		if( instance->socket == INVALID_SOCKET )
		{
			free(instance);
			return 0;
		}

		instance->error = SYSLOG_ERR_NONE;
	}
	
	return instance;
}

int Syslog_SetDestination(syslog_instance_t *instance, const char *hostname, unsigned port)
{
	s32 res;

	// Connect socket
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof saddr);
	saddr.sin_len = sizeof saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	if( inet_aton(hostname, &saddr.sin_addr) == 0 )
	{
		instance->error = SYSLOG_ERR_INVDESTADDR;
		return FALSE;
	}
	
	if( (res = net_connect(instance->socket, (struct sockaddr*)&saddr, sizeof saddr)) < 0 )
	{
		instance->error = SYSLOG_ERR_CONNECT;
		return FALSE;
	}
	
	instance->hostname = hostname;
	instance->port = port;

	return TRUE;
}
int Syslog_Send(syslog_instance_t *instance, unsigned priority, unsigned severity, const char *message)
{
	// Array with abbreviated month names
	const char *monthabbr[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	
	s32 result;

	// Get current time
	time_t t;
	struct tm * ts;
	time(&t);
	ts = localtime(&t);

	// Build packet data
	char packet[1024];
	char pri[3+1];
	char timestamp[15+1];
	char myhost[32+1];
	
    snprintf(pri, sizeof pri, "%d", (priority % 125) * 8 + (severity & 7));
    snprintf(myhost, sizeof myhost, "%s", instance->myhost);
    snprintf(timestamp, sizeof timestamp, "%s %2d %02d:%02d:%02d", monthabbr[ts->tm_mon], 
			 ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec);
    snprintf(packet, sizeof packet, "<%s>%s %s %s", pri, timestamp, myhost, message);
	
	// Send packet
	result = net_send(instance->socket, packet, strlen(packet), 0);
	if( result < 0 )
	{
		instance->error = SYSLOG_ERR_SEND;
		return FALSE;
	}
	
	return TRUE;
}
int Syslog_SendFormat(syslog_instance_t *instance, unsigned priority, unsigned severity, const char *fmt, ...)
{
	char message[950];
	va_list args;
	va_start(args, fmt);
	
	vsnprintf(message, sizeof message, fmt, args);
	
	va_end(args);
	
	return Syslog_Send(instance, priority, severity, message);
}
void Syslog_End(syslog_instance_t *instance)
{
	net_close(instance->socket);
	free(instance);
}
