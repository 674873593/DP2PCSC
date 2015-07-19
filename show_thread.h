#ifndef __show_thread_H__
	#define __show_thread_H__
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <time.h> 
	//#include <pthread.h>
	#include <string.h>
	
	#include "client.h"
	
	#define COLOR_NONE "\033[m"
	#define LIGHT_RED "\033[1;31m"
	#define LIGHT_BLUE "\033[1;34m" 
	
	#define SHOW_TTY_FILE "show_tty_name.txt"
	#define SHOW_TTY_NAME_BUFSIZE 50
	#define SHOW_DIRECTION_IN 0
	#define SHOW_DIRECTION_OUT 1
	#define SHOW_DIRECTION_SYSTEM_INFO 3
	extern int client_shutdown;
	
	struct show_tty{
		int show_tty_pid;
		char *show_tty_name;
	};
	struct show_tty *show_tty_running;
	
	void show(char *friend_name, char *message, int dirction);
	void init_show();
	void *show_thread(void *arg);
	void refresh_show_tty();
	void destroy_show_tty();
#endif /* __show_thread_H__ */ 
