#include "ftpproto.h"
#include "common.h"
#include "sysutil.h"


void handler_child(session_t* sess)
{
	writen(sess->ctrl_fd,"220 (Tinyftpd 0.1v)\r\n",strlen("220 (Tinyftpd 0.1v)\r\n"));
	while(1)
	{
		readline(sess->ctrl_fd,sess->cmdline,sizeof(sess->cmdline));
		//解析ftp命令和参数
		//
	}
}