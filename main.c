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


	parseconf_load_file(TINYFTPD_CONF);
	if(tunable_listen_address == NULL)
		printf("tunable_listen_address=NULL\n");
	printf("%s\n",tunable_listen_address);

	int listenfd = tcp_server(tunable_listen_address,tunable_listen_port);

	session_t sess = 
	{
		/* 控制连接 */
		0, -1, "", "", "",
		/* 数据连接 */
		NULL, -1, -1, 0,
		/* 限速 */
		0, 0, 0, 0,
		/* 父子进程通道 */
		-1, -1,
		/* FTP协议状态 */
		0, 0, NULL, 0,
		/* 连接数限制 */
		0, 0
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