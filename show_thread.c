#include "show_thread.h"
//static pthread_mutex_t lock;
//static pthread_t *show_thread_id;

void show(char *friend_name, char *message, int dirction)
{
/*	pthread_mutex_lock(&lock);*/
/*	pthread_mutex_unlock(&lock);*/

	time_t time_now;
	time(&time_now);
	struct tm tmn;
	localtime_r(&time_now, &tmn);
	int show_string_len = sizeof(struct tm) + sizeof(LIGHT_RED) + strlen(friend_name) * sizeof(char) + sizeof(COLOR_NONE) + strlen(message) * sizeof(char) + 13 * sizeof(char);//here 13 is the length of ':' '@' ' ' \n and \t counts sum
	char *show_string = (char *)malloc(show_string_len);
	memset(show_string, 0, show_string_len);
        if (dirction == SHOW_DIRECTION_IN) {
        	sprintf(show_string, "%d-%d-%d %d:%d:%d %s%s%s:\n\t%s\n", (&tmn)->tm_year+1900, (&tmn)->tm_mon+1, (&tmn)->tm_mday, (&tmn)->tm_hour, (&tmn)->tm_min, (&tmn)->tm_sec, LIGHT_RED, friend_name,COLOR_NONE, message);
        }else{
		sprintf(show_string, "%d-%d-%d %d:%d:%d @%s%s%s:\n\t%s\n", (&tmn)->tm_year+1900, (&tmn)->tm_mon+1, (&tmn)->tm_mday, (&tmn)->tm_hour, (&tmn)->tm_min, (&tmn)->tm_sec, LIGHT_BLUE, friend_name,COLOR_NONE, message);
        }
        
         
        int command_length = show_string_len + (strlen("echo \" \">") + 1 + strlen(show_tty_running->show_tty_name)) * sizeof(char);
        char *command = (char *)malloc(command_length);
        memset(command, 0, command_length);
        sprintf(command, "%s%s%s%s", "echo \"", show_string, "\">", show_tty_running->show_tty_name);
	system(command);
	free(show_string);	
	free(command);	
}

void init_show()
{	
	show_tty_running = (struct show_tty*)malloc(sizeof(struct show_tty));
	show_tty_running->show_tty_name = (char *)malloc(SHOW_TTY_NAME_BUFSIZE);
/*	pthread_mutex_init(&lock,NULL);*/
	system("bash show_tty_daemon.sh show");
	refresh_show_tty();
	pthread_t show_thread_id;
	pthread_create(&show_thread_id, NULL, show_thread, 0);
}

void refresh_show_tty()
{
	FILE *file = fopen(SHOW_TTY_FILE,"r");
	if (file == NULL) {
		return;
	}
	char *show_tty_name;
	char *show_tty_pidbuf;
	int ttyname_length;
	fseek(file, 0L, SEEK_END);   
    	ttyname_length = ftell(file); 
    	show_tty_name = (char *)malloc(ttyname_length * sizeof(char));
    	show_tty_pidbuf = (char *)malloc(ttyname_length * sizeof(char));
    	
    	fseek(file, 0L, SEEK_SET);  
    	fscanf(file, "%s %s", show_tty_pidbuf, show_tty_name);
    	
/*    	pthread_mutex_lock(&lock);*/
    	
    	show_tty_running->show_tty_pid = atoi(show_tty_pidbuf);
    	memset(show_tty_running->show_tty_name, 0, SHOW_TTY_NAME_BUFSIZE);
    	memcpy(show_tty_running->show_tty_name, show_tty_name,(strlen(show_tty_name) + 1) * sizeof(char));

/*    	pthread_mutex_unlock(&lock);*/
    	free(show_tty_pidbuf);
    	free(show_tty_name);
	fclose(file);
}

void *show_thread(void *arg)
{
	pthread_detach(pthread_self());
	sleep(3);//wait for ensure no conflict with create tty in init method
	while(!client_shutdown){
		int is_tty_reboot = 0;
		is_tty_reboot = system("bash show_tty_daemon.sh isalive");
		if (is_tty_reboot) {//if a new tty created and olds are killed
			refresh_show_tty();
		}
		sleep(1);
	}
	system("bash show_tty_daemon.sh killsame");
 	free(show_tty_running->show_tty_name);
	free(show_tty_running);
/*	pthread_mutex_destroy(&lock);*/
	
	pthread_exit((void *)NULL);
}

