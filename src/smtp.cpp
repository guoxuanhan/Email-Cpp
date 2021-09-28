#include <fstream>
#include "smtp.h"

Smtp::Smtp()
{
	_port = 25;
	_domain = "";
	_user = "";
	_pwd = "";
	_targetAddr = "";
	_title = "";
	_content = "";
	_sockClient = 0;
#ifdef _MSC_VER
	WORD wVersionRequeseted;
	WSADATA wsaData;
	int error;
	wVersionRequeseted = MAKEWORD(2, 1);
	error = WSAStartup(wVersionRequeseted, &wsaData);
	if (error != 0)
		throw new std::runtime_error("WSAStartup error!");
#endif // _MSC_VER
}

Smtp::Smtp(
	int port,					// smtp端口（默认25）
	std::string strDomain,		// smtp服务器域名（默认smtp.qq.com）
	std::string userName,		// 用户名
	std::string userPwd,		// 密码
	std::string targetEmail,	// 目的邮件地址
	std::string title,			// 主题
	std::string content			// 内容
)
{
	_port = port;
	_domain = strDomain;
	_user = userName;
	_pwd = userPwd;
	_targetAddr = targetEmail;
	_title = title;
	_content = content;
	_sockClient = 0;
#ifdef _MSC_VER
	WORD wVersionRequeseted;
	WSADATA wsaData;
	int error;
	wVersionRequeseted = MAKEWORD(2, 1);
	error = WSAStartup(wVersionRequeseted, &wsaData);
	if (error != 0)
		throw new std::runtime_error("WSAStartup error!");
#endif // _MSC_VER
}

Smtp::~Smtp()
{
#ifdef _MSC_VER
	closesocket(_sockClient);
	WSACleanup();
#else
	close(_sockClient);
#endif // _MSC_VER
}

bool Smtp::CreateConnect()
{
	int skClientTemp = socket(AF_INET, SOCK_STREAM, 0);
	hostent* pHostent = gethostbyname(_domain.c_str());

	sockaddr_in saddr;
	saddr.sin_addr.s_addr = *((unsigned long*)pHostent->h_addr_list[0]);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(_port);
	int error = connect(skClientTemp, (sockaddr*)&saddr, sizeof(saddr));
	if (error != 0)
		return false;
	_sockClient = (int)skClientTemp;
	if (false == Recv())
		return false;
	return true;
}

int  Smtp::Login()
{
	std::string sendBuff = "EHLO ";
	sendBuff += _user;
	sendBuff += "\r\n";

	if (false == Send(sendBuff) || false == Recv())
		return 1;

	sendBuff.clear();
	sendBuff = "AUTH LOGIN\r\n";
	if (false == Send(sendBuff) || false == Recv())
		return 1;

	sendBuff.clear();
	int pos = _user.find('@', 0);
	sendBuff = _user.substr(0, pos);

	char* ecode = base64Encode(sendBuff.c_str(), (unsigned int)strlen(sendBuff.c_str()));
	sendBuff.clear();
	sendBuff = ecode;
	sendBuff += "\r\n";
	delete[] ecode;

	if (false == Send(sendBuff) || false == Recv())
		return 1;

	sendBuff.clear();
	ecode = base64Encode(_pwd.c_str(), (unsigned int)strlen(_pwd.c_str()));
	sendBuff = ecode;
	sendBuff += "\r\n";
	delete[] ecode;

	if (false == Send(sendBuff) || false == Recv())
		return 1;
	if (NULL != strstr(_buff, "550"))
		return 2;
	if (NULL != strstr(_buff, "535"))
		return 3;
	
	return 0;
}

bool Smtp::Recv()
{
	memset(_buff, 0, sizeof(char) * (MAX_EMAIL_MESSAGE_LEN + 1));
	int error = recv(_sockClient, _buff, MAX_EMAIL_MESSAGE_LEN, 0);
	if (error < 0)
		return false;
	_buff[error] = '\0';

	std::cout << "Recv: " << _buff << std::endl;
	return true;
}

void Smtp::FormatEmailHead(std::string& strEmail)
{
	strEmail = "From: ";
	strEmail += _user;
	strEmail += "\r\n";

	strEmail += "To: ";
	strEmail += _targetAddr;
	strEmail += "\r\n";

	strEmail += "Subject: ";
	strEmail += _title;
	strEmail += "\r\n";

	strEmail += "MIME-Version: 1.0";
	strEmail += "\r\n";

	strEmail += "Content-Type: multipart/mixed;boundary=qwertyuiop";
	strEmail += "\r\n";
	strEmail += "\r\n";
}

bool Smtp::Send(std::string& strMessage)
{
	int error = send(_sockClient, strMessage.c_str(), strMessage.length(), 0);
	if (error < 0)
		return false;

	std::cout << "Send: " << strMessage.c_str() << std::endl;
	return true;
}

bool Smtp::SendEmailHead()
{
	std::string sendBuff;
	sendBuff = "MAIL FROM: <" + _user + ">\r\n";
	if (false == Send(sendBuff) || false == Recv())
		return false;

	sendBuff.clear();
	sendBuff = "RCPT TO: <" + _targetAddr + ">\r\n";
	if (false == Send(sendBuff) || false == Recv())
		return false;

	sendBuff.clear();
	sendBuff = "DATA\r\n";
	if (false == Send(sendBuff) || false == Recv())
		return false;

	sendBuff.clear();
	FormatEmailHead(sendBuff);
	if (false == Send(sendBuff))
		return false;

	return true;
}

bool Smtp::SendTextBody()
{
	std::string sendBuff;
	sendBuff = "--qwertyuiop\r\n";
	sendBuff += "Content-Type: text/plain;";
	sendBuff += "charset=\"gb2312\"\r\n\r\n";
	sendBuff += _content;
	sendBuff += "\r\n\r\n";
	return Send(sendBuff);
}

bool Smtp::SendEnd()
{
	std::string sendBuff;
	sendBuff = "--qwertyuiop--";
	sendBuff += "\r\n.\r\n";
	if (false == Send(sendBuff) || false == Recv())
		return false;

	std::cout << "SendEnd: " << _buff << std::endl;
	
	sendBuff.clear();
	sendBuff = "QUIT\r\n";
	return (Send(sendBuff) && Recv());
}

int  Smtp::SendEmailEx()
{
	if (false == CreateConnect())
		return 1;
	int error = Login();
	if (error != 0)
		return error;
	if (false == SendEmailHead())
		return 1;
	if (false == SendTextBody())
		return 1;
	if (false == SendEnd())
		return 1;
	return 0;
}

char* Smtp::base64Encode(char const* pOrigSigned, unsigned origLength)
{
	unsigned char const* orig = (unsigned char const*)pOrigSigned; // in case any input bytes have the MSB set
	if (orig == NULL) return NULL;

	unsigned const numOrig24BitValues = origLength / 3;
	bool havePadding = origLength > numOrig24BitValues * 3;
	bool havePadding2 = origLength == numOrig24BitValues * 3 + 2;
	unsigned const numResultBytes = 4 * (numOrig24BitValues + havePadding);
	char* result = new char[numResultBytes + 3]; // allow for trailing '/0'

	unsigned i;
	for (i = 0; i < numOrig24BitValues; ++i)
	{
		result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
		result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
		result[4 * i + 2] = base64Char[((orig[3 * i + 1] << 2) | (orig[3 * i + 2] >> 6)) & 0x3F];
		result[4 * i + 3] = base64Char[orig[3 * i + 2] & 0x3F];
	}

	if (havePadding)
	{
		result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
		if (havePadding2)
		{
			result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
			result[4 * i + 2] = base64Char[(orig[3 * i + 1] << 2) & 0x3F];
		}
		else
		{
			result[4 * i + 1] = base64Char[((orig[3 * i] & 0x3) << 4) & 0x3F];
			result[4 * i + 2] = '=';
		}
		result[4 * i + 3] = '=';
	}

	result[numResultBytes] = '\0';
	return result;
}