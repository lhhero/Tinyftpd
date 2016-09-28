#include "ftpproto.h"
#include "common.h"
#include "sysutil.h"
#include "str.h"

void handler_child(session_t* sess)
{
	int ret;
	writen(sess->ctrl_fd,"220 (Tinyftpd 0.1v)\r\n",strlen("220 (Tinyftpd 0.1v)\r\n"));
	while(1)
	{
		memset(sess->cmdline,0,MAX_COMMAND_LINE);
		memset(sess->cmd,0,MAX_COMMAND);
		memset(sess->arg,0,MAX_ARG);

		ret = readline(sess->ctrl_fd,sess->cmdline,sizeof(sess->cmdline));
		if(ret == -1)
			ERR_EXIT("readline");
		if(ret == 0)
			exit(EXIT_SUCCESS);
		//解析ftp命令和参数
		str_trim_crlf(sess->cmdline);
		str_split(sess->cmdline,sess->cmd,sess->arg,' ');
		printf("[cmd]=%s,[arg]=%s\n",sess->cmd,sess->arg);
		str_upper(sess->cmd);
		
	}
}