#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "tunable.h"
#include "parseconf.h"
int main()
{
	if(getuid() != 0)
	{
		fprintf(stderr, "Tinyftpd must be started by root!\n");
		exit(EXIT_FAILURE);
	}

	parseconf_load_file("Tinyftpd.conf");
	int listenfd = tcp_server(tunable_listen_address,tunable_listen_port);
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