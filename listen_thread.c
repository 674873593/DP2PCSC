#include "listen_thread.h"

void *listen_thread(void *arg)
{
	pthread_detach(pthread_self());
	while (!client_shutdown) {
		socket_fd talk_socket_fd;
		struct sockaddr client_addr_in;
		socklen_t client_addr_in_len = sizeof(struct sockaddr);
		talk_socket_fd = accept(listen_socket_fd,
						(struct sockaddr *)&client_addr_in,
						&client_addr_in_len);
		if (talk_socket_fd > 2) {//0 stdin; 1 stdout; 2 stderr
			pthread_t talk_thread_id;
			struct talk_thread_arg *tt_arg = malloc_safe(tt_arg, sizeof(struct talk_thread_arg));
			tt_arg->connect_socket_fd = talk_socket_fd;
			tt_arg->connect_launcher = FALSE;
			tt_arg->file_trans_fd = ERROR;
			pthread_create(&talk_thread_id, NULL, talk_thread, (void *)tt_arg);
		}else{
			usleep(500);
		}
	}
	pthread_exit((void *)NULL);
}



