#include "show_thread.h"
static pthread_mutex_t lock;
static pthread_t *show_thread_id;

void show(char *friend_name, char *message)
{
	pthread_mutex_lock(&lock);
	FILE *show_tty_file = fopen(show_tty_running->show_tty_name,"r");
	pthread_mutex_unlock(&lock);
	time_t now;  
	char date[11] = {0};  
	char time[9] = {0};  
	strftime(date, sizeof(date), "%Y-%m-%d", localtime(&now));  
        strftime(time, sizeof(time), "%H:%M:%S", localtime(&now));  
	fprintf(show_tty_file, "%s %s %s:\n\t%s\n\n", date, time, friend_name, message);
	fclose(show_tty_file);
}

void init_show()
{	
	show_tty_running = (struct show_tty*)malloc(sizeof(struct show_tty));
	show_tty_running->show_tty_name = (char *)malloc(SHOW_TTY_NAME_BUFSIZE);
	pthread_mutex_init(&lock,NULL);
	system("gnome-terminal -t \"Chatting\" -x bash -c \"bash show_tty_daemon.sh show\"");
	refresh_show_tty();
	show_thread_id = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(show_thread_id, NULL, show_thread, 0);
	//pthread_detach(*show_thread_id);
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
    	pthread_mutex_lock(&lock);
    	show_tty_running->show_tty_pid = atoi(show_tty_pidbuf);
/*    	if (show_tty_running->show_tty_name == NULL) {*/
/*    		show_tty_running->show_tty_name = (char *)malloc(strlen(show_tty_name) * sizeof(char));*/
/*    	}*/
    	memset(show_tty_running->show_tty_name, 0, SHOW_TTY_NAME_BUFSIZE);
/*	printf("%d %s",strlen(show_tty_name),show_tty_name);*/
    	memcpy(show_tty_running->show_tty_name, show_tty_name,(strlen(show_tty_name) + 1) * sizeof(char));
/*    	printf("%d %s %s",strlen(show_tty_name) * sizeof(char),show_tty_pidbuf,show_tty_running->show_tty_name);*/
/*    	int a;*/
/*    	scanf("%d",&a);*/
    	pthread_mutex_unlock(&lock);
    	free(show_tty_pidbuf);
    	free(show_tty_name);
    	
	fclose(file);
}

void *show_thread(void *arg)
{
	pthread_detach(pthread_self());
	while(!client_shutdown){
		int isalive;
		isalive = system("gnome-terminal -t \"Chatting\" -x bash -c \"bash show_tty_daemon.sh isalive\"");
		if (!isalive) {
			refresh_show_tty();
		}
		sleep(1);
	}
	free(show_tty_running->show_tty_name);
	free(show_tty_running);
	free(show_thread_id);
	pthread_mutex_destroy(&lock);
	//pthread_detach(pthread_self());
	pthread_exit((void *)NULL);
	//return (void *)NULL;
}
