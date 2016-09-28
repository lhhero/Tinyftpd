#include "common.h"
#include "sysutil.h"
#include "session.h"


int main()
{
	if(getuid() != 0)
	{
		fprintf(stderr, "Tinyftpd must be started by root!\n");
		exit(EXIT_FAILURE);
	}

	int listenfd = tcp_server(NULL,5188);
	session_t sess = {
		-1,"","","",
		-1,-1
	};
	while(1)
	{
		int conn;
		pid_t pid;
		conn = accept_timeout(listenfd,NULL,0);
		if(conn == -1)
			ERR_EXIT("accept_timeout");
		pid = fork();
		if(pid < 0)
			ERR_EXIT("fork");
		if(pid == 0)
		{
			close(listenfd);
			sess.ctrl_fd = conn;
			begin_session(&sess);
		}
		else
			close(conn);

	}	
	return 0;
}