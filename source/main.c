#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <network.h>
#include <string.h>
#include <fat.h>
#include <ogc/lwp.h>

/*Uncomment this part if you want to use debugging code
#define DEBUG 1*/
#ifdef DEBUG
#include <debug.h>
#endif

#define NUMTHREADS 10
/*
Author: Ibrahim Awwal
*/
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
void init();
void *helloGoodbyeThread();

lwp_t threads[NUMTHREADS];

int main(int argc, char **argv) {
	init();
	int i;
	for(i = 0; i<NUMTHREADS; i++){
		int *arg = (int *)malloc(sizeof(int));
		*arg = i;
		LWP_CreateThread(threads + i, &helloGoodbyeThread, arg, NULL,0, 1);
	}
	/*the rest is from the template.c in libogc, press home to quit at this point*/
	while(1) {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
	}

	return 0;
}

void *helloGoodbyeThread(int *threadnum){
	printf("Hello! I am thread %d\n", *threadnum);
	LWP_YieldThread();
	printf("This is thread %d saying goodbye!\n", *threadnum);
	return NULL;
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

	if (!fatInitDefault()) {
		printf("fatInitDefault failure: terminating\n");
	}

	#ifdef DEBUG
	DEBUG_Init(GDBSTUB_DEVICE_WIFI, 8001); // Port 8001 (use whatever you want)
	_break();
	#endif
}
