#include "show_thread.h"

void show(char *friend_name, char *message, int direction)
{

	time_t time_now;
	time(&time_now);
	struct tm tmn;
	localtime_r(&time_now, &tmn);
	int show_string_len = sizeof(struct tm) + sizeof(LIGHT_RED) + strlen(friend_name) * sizeof(char) + sizeof(COLOR_NONE) + strlen(message) * sizeof(char) + 13 * sizeof(char);//here 13 is the length of ':' '@' ' ' \n and \t counts sum
	char *show_string = (char *)malloc_string_safe(show_string, show_string_len);
        if (direction == SHOW_DIRECTION_IN) {
        	sprintf(show_string, "%d-%d-%d %d:%d:%d %s%s%s:\n\t%s\n", (&tmn)->tm_year+1900, (&tmn)->tm_mon+1, (&tmn)->tm_mday, (&tmn)->tm_hour, (&tmn)->tm_min, (&tmn)->tm_sec, LIGHT_RED, friend_name,COLOR_NONE, message);
        }else if (direction == SHOW_DIRECTION_OUT) {
		sprintf(show_string, "%d-%d-%d %d:%d:%d @%s%s%s:\n\t%s\n", (&tmn)->tm_year+1900, (&tmn)->tm_mon+1, (&tmn)->tm_mday, (&tmn)->tm_hour, (&tmn)->tm_min, (&tmn)->tm_sec, LIGHT_BLUE, friend_name,COLOR_NONE, message);
        }else if (direction == SHOW_DIRECTION_SYSTEM_INFO) {
        	sprintf(show_string, "[info] %s %s\n", friend_name, message);
        }
        
         
        int command_length = show_string_len + (strlen("echo \" \">>") + strlen(show_tty_running->show_tty_name)) * sizeof(char);
        char *command = (char *)malloc_string_safe(command, command_length);
        sprintf(command, "%s%s%s%s", "echo \"", show_string, "\">>", show_tty_running->show_tty_name);
	system(command);
	free_safe(show_string);	
	free_safe(command);	
}


void init_show()
{	
	show_tty_running = (struct show_tty*)malloc_safe(show_tty_running, sizeof(struct show_tty));
	show_tty_running->show_tty_name = (char *)malloc_string_safe(show_tty_running->show_tty_name, SHOW_TTY_NAME_BUFSIZE);
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
	
	int ttyname_length;
	fseek(file, 0L, SEEK_END);   
    	ttyname_length = ftell(file); 
    	char *show_tty_name = (char *)malloc_string_safe(show_tty_name, ttyname_length * sizeof(char));
    	char *show_tty_pidbuf = (char *)malloc_string_safe(show_tty_pidbuf, ttyname_length * sizeof(char));
    	
    	fseek(file, 0L, SEEK_SET);  
    	fscanf(file, "%s %s", show_tty_pidbuf, show_tty_name);
    	
    	
    	show_tty_running->show_tty_pid = atoi(show_tty_pidbuf);
    	memset(show_tty_running->show_tty_name, 0, SHOW_TTY_NAME_BUFSIZE);
	memcpy(show_tty_running->show_tty_name, show_tty_name,strlen(show_tty_name) * sizeof(char));
    	free_safe(show_tty_pidbuf);
    	free_safe(show_tty_name);
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
	
 	
/*	pthread_mutex_destroy(&lock);*/
	
	pthread_exit((void *)NULL);
}

void destroy_show_tty()
{
	free_safe(show_tty_running->show_tty_name);
	free_safe(show_tty_running);
	system("bash show_tty_daemon.sh killsame");
}
