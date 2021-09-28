#include "smtp.h"

int main()
{
	/*
		用法：以QQ邮箱为例，在设置界面打开IMAP/SMTP服务并生成授权码，将授权码填写在密码处即可发送邮件
	*/

	Smtp smtp(
		25,					// smtp端口
		"smtp.qq.com",		// smtp服务器地址
		"from@qq.com",		// 发送者邮箱地址
		"password",			// 发送者邮箱密码或smtp许可证序列号
		"to@qq.com",		// 接收者邮箱地址
		"title",			// 邮件主题
		"要发送的内容"		// 邮件正文
	);

	int error = 0;
	if ((error = smtp.SendEmailEx()) != 0)
	{
		switch (error)
		{
		case 1:
			std::cout << "错误1：由于网络不通畅，发送失败！" << std::endl; break;
		case 2:
			std::cout << "错误2：用户名错误，请核对！" << std::endl; break;
		case 3:
			std::cout << "错误3：用户密码错误，请核对！" << std::endl; break;
		case 4:
			std::cout << "错误4：请检查附件目录是否正确，以及文件是否存在！" << std::endl; break;
		default:
			break;
		}
	}

	system("pause");
	return 0;
}