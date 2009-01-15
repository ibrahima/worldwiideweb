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

#ifndef SYSLOG_H
#define SYSLOG_H

#ifdef __cplusplus
extern "C" {
#endif

// Opaque data structure (used for thread-safety)
typedef struct syslog_instance_t syslog_instance_t;

// Priority constants
enum {
	SYSLOG_PRI_KERNEL = 0,
	SYSLOG_PRI_USER = 1,
	SYSLOG_PRI_MAIL = 2,
	SYSLOG_PRI_SYSTEM = 3,
	SYSLOG_PRI_SECURITY1 = 4,
	SYSLOG_PRI_INTERNAL = 5,
	SYSLOG_PRI_LP = 6,
	SYSLOG_PRI_NETNEWS = 7,
	SYSLOG_PRI_UUCP = 8,
	SYSLOG_PRI_CLOCK1 = 9,
	SYSLOG_PRI_SECURITY2 = 10,
	SYSLOG_PRI_FTP = 11,
	SYSLOG_PRI_NTP = 12,
	SYSLOG_PRI_LOGAUDIT = 13,
	SYSLOG_PRI_LOGALERT = 14,
	SYSLOG_PRI_CLOCK2 = 15,
	SYSLOG_PRI_LOCAL0 = 16,
	SYSLOG_PRI_LOCAL1 = 17,
	SYSLOG_PRI_LOCAL2 = 18,
	SYSLOG_PRI_LOCAL3 = 19,
	SYSLOG_PRI_LOCAL4 = 20,
	SYSLOG_PRI_LOCAL5 = 21,
	SYSLOG_PRI_LOCAL6 = 22,
	SYSLOG_PRI_LOCAL7 = 23
};

// Severity constants
enum {
	SYSLOG_SEV_EMERGENCY = 0,
	SYSLOG_SEV_ALERT = 1,
	SYSLOG_SEV_CRITICAL = 2,
	SYSLOG_SEV_ERROR = 3,
	SYSLOG_SEV_WARNING = 4,
	SYSLOG_SEV_NOTICE = 5,
	SYSLOG_SEV_INFORMATIONAL = 6,
	SYSLOG_SEV_DEBUG = 7,
};

// Error constants
enum {
	SYSLOG_ERR_NONE = 0,
	SYSLOG_ERR_INVDESTADDR,
	SYSLOG_ERR_CONNECT,
	SYSLOG_ERR_SEND
};

// Retrieves the error code (see error constants) or a string describing the error, respectively
unsigned Syslog_GetError(syslog_instance_t *instance);
const char *Syslog_GetErrorMessage(syslog_instance_t *instance);

// Starts the syslog client and sets the own hostname to myhost
syslog_instance_t *Syslog_Start(const char *myhost);
// Sets the destination, i.e. the IP (dotted format; "a.b.c.d") and port of the remote host
int Syslog_SetDestination(syslog_instance_t *instance, const char *hostname, unsigned port);
// Sends a message (optionally a formatted message using a printf like format string)
int Syslog_Send(syslog_instance_t *instance, unsigned priority, unsigned severity, const char *message);
int Syslog_SendFormat(syslog_instance_t *instance, unsigned priority, unsigned severity, const char *fmt, ...);
// Stops the syslog client and releases resources
void Syslog_End(syslog_instance_t *instance);

#ifdef __cplusplus
}
#endif

#endif
