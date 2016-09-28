#include "ftpproto.h"
#include "common.h"
#include "sysutil.h"
#include "str.h"
#include "ftpcodes.h"


void ftp_reply(session_t *sess, int status, const char *text);
void ftp_lreply(session_t *sess, int status, const char *text);

static void do_user(session_t* sess);
static void do_pass(session_t* sess);

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

		if(strcmp(sess->cmd,"USER") == 0)
			do_user(sess);
		else if(strcmp(sess->cmd,"PASS") == 0)
			do_pass(sess);
		
	}
}

void ftp_reply(session_t *sess, int status, const char *text)
{
	char buf[1024] = {0};
	sprintf(buf, "%d %s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
}

void ftp_lreply(session_t *sess, int status, const char *text)
{
	char buf[1024] = {0};
	sprintf(buf, "%d-%s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
}


static void do_user(session_t *sess)
{
	//USER HiiL
	struct passwd *pw = getpwnam(sess->arg);
	if (pw == NULL)
	{
		// 用户不存在
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	sess->uid = pw->pw_uid;
	ftp_reply(sess, FTP_GIVEPWORD, "Please specify the password.");
	
}

static void do_pass(session_t *sess)
{
	// PASS 071215
	struct passwd *pw = getpwuid(sess->uid);
	if (pw == NULL)
	{
		// 用户不存在
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	printf("name=[%s]\n", pw->pw_name);
	struct spwd *sp = getspnam(pw->pw_name);
	if (sp == NULL)
	{
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}


	// 将明文进行加密
	char *encrypted_pass = crypt(sess->arg, sp->sp_pwdp);
	// 验证密码
	if (strcmp(encrypted_pass, sp->sp_pwdp) != 0)
	{
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	setegid(pw->pw_gid);
	seteuid(pw->pw_uid);
	chdir(pw->pw_dir);
	ftp_reply(sess, FTP_LOGINOK, "Login successful.");
}