#pragma once
#ifndef __SMTP_H__
#define __SMTP_H__

#ifdef _MSC_VER
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <unistd.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/ioctl.h>
#endif // _MSC_VER

#include <iostream>
#include <list>

const int MAX_EMAIL_MESSAGE_LEN = 1024;
static const char base64Char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

class Smtp
{
public:
	Smtp();
	Smtp(
		int port,					// smtp端口（默认25）
		std::string strDomain,		// smtp服务器域名（默认smtp.qq.com）
		std::string userName,		// 用户名
		std::string userPwd,		// 密码
		std::string targetEmail,	// 目的邮件地址
		std::string title,			// 主题
		std::string content			// 内容
	);
	~Smtp();
public:
	bool CreateConnect();
	int  Login();
	bool Recv();
	void FormatEmailHead(std::string& strEmail);
public:
	bool Send(std::string& strMessage);
	bool SendEmailHead();
	bool SendTextBody();
	bool SendEnd();
public:
	int  SendEmailEx();
	char* base64Encode(char const* pOrigSigned, unsigned origLength);
public:
	void SetDomain(std::string& strDomain) { this->_domain = strDomain; }
	void SetUser(std::string& strUser) { this->_user = strUser; }
	void SetPwd(std::string& strPwd) { this->_pwd = strPwd; }
	void SetTargetEmailAddr(std::string& strTargetAddr) { this->_targetAddr = strTargetAddr; }
	void SetTitle(std::string& strTitle) { this->_title = strTitle; }
	void SetContent(std::string& strContent) { this->_content = strContent; }
	void SetPort(int iPort) { this->_port = iPort; }
private:
	int _port;
	std::string _domain;
	std::string _user;
	std::string _pwd;
	std::string _targetAddr;
	std::string _title;
	std::string _content;
private:
	char _buff[MAX_EMAIL_MESSAGE_LEN + 1];
	int _buffLen;
	int _sockClient;
};

#endif