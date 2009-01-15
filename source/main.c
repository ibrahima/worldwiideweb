#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <network.h>
#include <string.h>
#include <fat.h>
#include <mxml.h>
#include "syslog.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
static volatile u8 _reset = 0;
void init();
void reset();
char *defaulturl = "example.com/index.html";
syslog_instance_t *syslog;

int displayInetFile(char *url);
struct httpresponse{
	float version;
	int response_code;
	char *text;
	char *date;
	char *modified;
	char *server;
	size_t content_length;
	char *content_type;
	char *charset;

};
/*
 * Downloads and displays a file from the internet.
 * Returns 0 on success, -1 on error.
 */

void printResponse(struct httpresponse response);
int displayInetFile(char *url){
	struct httpresponse response;
	char *filepath = strchr(url, '/');
	char *hostname = strndup(url, filepath - url);
	printf("hostname is %s and filepath is %s\n", hostname, filepath);
	struct hostent *host = net_gethostbyname(hostname);/*gets the host information by domain name*/
	struct sockaddr_in server;
	s32 socket = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);/*creates a socket*/
	if(host == NULL){
		printf("Oh crap, can't find %s\n", hostname);
		return -1;
	}
	memset(&server, 0, sizeof(server));/*clears out the sockaddr_in structure, just saw this in an example and not sure if it's needed*/
	server.sin_family = AF_INET;/*sets the socket type family thing to IPv4*/
	server.sin_port = htons(80);/*uses port 80 for normal HTTP*/
	memcpy(&server.sin_addr, host->h_addr_list[0], host->h_length);/*copies the host address into the sockaddr_in structure*/
	if(net_connect(socket, (struct sockaddr *)&server, sizeof(server))){
		printf("Darn, failed to connect\n");
		return -1;
	}
	else
		printf("Successfully connected!\n");

	char *getstring = (char *)malloc(strlen("GET  HTTP/1.0\r\n\r\n")+strlen(filepath));
	sprintf(getstring,"GET %s HTTP/1.0\r\n\r\n", filepath);/*the minimum request necessary to get back a page, the format is GET file HTTP/version\r\n\r\n basically*/
	int len = strlen(getstring);
	int sent = net_write(socket, getstring, len);/*writes the request to the socket*/
	printf("sent %d of %d bytes\n", sent, len);
	int bufferlen = 1025;/*creates a buffer for receiving*/
	char *buf = (char *)malloc(bufferlen);/*creates a buffer for receiving*/
	unsigned int received = 0;
	int read = 0;
	response.text = (char *)malloc(sizeof(char)*32);
	FILE *fp;
	fp = fopen("sd:/temp.txt", "w");
	char *line = (char *)malloc(bufferlen);
	char *linebegin, *lineend;
	bool dataStarted = false;
	int headerlength = 0;
	while((read = net_read(socket,buf, bufferlen-1))>0){/*while we have more data, read it into the buffer and print it out*/
		buf[read]='\0';/*null terminate the amount read, not sure if necessary but better safe than sorry*/
		linebegin = buf;
		while((lineend = strchr(linebegin, '\n'))!=NULL){
			memset(line, '\0', 1025);
			strncpy(line, linebegin, lineend-linebegin);
			if(!dataStarted){
				if(!strncmp(line, "HTTP/", 5)){
					sscanf(line, "HTTP/%f %d %s\n", &(response.version), &(response.response_code), response.text);
				}else if(!strncmp(line, "Content-Length", 14)){
					sscanf(line, "Content-Length: %d", &response.content_length);
				}else if(!strncmp(line, "Content-Type", 12)){
					response.content_type = (char *)malloc(strchr(line, ';')-strchr(line, ' ')+1);
					response.charset = (char *)malloc(lineend-strchr(line, '=')+1);
					sscanf(line, "Content-Type: %s; charset=%s", response.content_type, response.charset);
				}else if(!strncmp(line, "Last-Modified", 13)){
					response.modified = (char *)malloc(lineend - strchr(line, ' ')+1);
					sscanf(line, "Last-Modified: %s", response.modified);
				}else if(!strcmp(line, "\r")){
					printf("end of http header\n");
					dataStarted = true;
					headerlength = lineend - buf + 1;
				}
			}
			else{
				printf("%s\n", line);
				fprintf(fp, "%s\n", line);
			}
			linebegin = lineend + 1;
		}
		received+=read;
	}
	received-=headerlength;
	fclose(fp);
	printResponse(response);
	if(read==0)
		printf("Reached EOF\n");
	if(read==-1)
		printf("Read error\n");
	net_close(socket);
	if(received == response.content_length){
		printf("Received %d bytes, everything seems to be in order.\n\n", received);
	}else{
		printf("Received %d of %d bytes, something went wrong.\n\n", received, response.content_length);
		return -1;
	}
	return 0;
}
int main(int argc, char **argv) {
	init();
	printf("Wii HTTP Test\n\nAttempting to initialize network\n");
	/*tries to initialize network*/
	char *myip = (char *)malloc(16*sizeof(char));
	if(if_config(myip, NULL, NULL, true)){
		printf("Failed to initialize network. Goodbye.");
		exit(-1);
	}
	printf("Network initialized. Wii's IP is %s.\n", myip);

	displayInetFile(defaulturl);
	/*the rest is from the template.c in libogc, press home to quit at this point*/
	while(1) {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
	}

	return 0;
}

void init(){
	/*this stuff is all from the libogc wii template*/
	VIDEO_Init();
	WPAD_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	printf("\x1b[2;0H");
	SYS_SetResetCallback(reset);/*should set reset as the callback function on reset*/

	if (!fatInitDefault()) {
		printf("fatInitDefault failure: terminating\n");
	}
	syslog = Syslog_Start("wii");
	if( !syslog ) {
		printf("Syslog failed to initialize\n");
	/* Error */
	}

	if( !Syslog_SetDestination(syslog, "192.168.1.8", 514) ) {
	printf("Syslog error %d: %s\n", Syslog_GetError(syslog),
	             Syslog_GetErrorMessage(syslog));
	}
	if( !Syslog_Send(syslog, SYSLOG_PRI_INTERNAL, SYSLOG_SEV_DEBUG, "syslog: starting")) {
		printf("Syslog error %d: %s\n", Syslog_GetError(syslog),
	             Syslog_GetErrorMessage(syslog));
	}
}
/*
 * This is supposed to be a callback function for when reset is called, but either
 * reset doesn't mean pressing home button or I did something wrong.
 */
void reset(){
	printf("Hmm, someone wants to reset. Boo.\n");
	exit(0);
}

void printResponse(struct httpresponse response){
	printf("\tHTTP/%1.1f %d %s\n", response.version, response.response_code, response.text);
	printf("\t%d bytes long\tModified: %s\n", response.content_length, response.modified);
	printf("\tContent Type: %s; charset %s\n\n", response.content_type, response.charset);
}
