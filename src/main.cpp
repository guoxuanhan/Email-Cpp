#include "smtp.h"

int main()
{
	/*
		�÷�����QQ����Ϊ���������ý����IMAP/SMTP����������Ȩ�룬����Ȩ����д�����봦���ɷ����ʼ�
	*/

	Smtp smtp(
		25,					// smtp�˿�
		"smtp.qq.com",		// smtp��������ַ
		"from@qq.com",		// �����������ַ
		"password",			// ���������������smtp���֤���к�
		"to@qq.com",		// �����������ַ
		"title",			// �ʼ�����
		"Ҫ���͵�����"		// �ʼ�����
	);

	int error = 0;
	if ((error = smtp.SendEmailEx()) != 0)
	{
		switch (error)
		{
		case 1:
			std::cout << "����1���������粻ͨ��������ʧ�ܣ�" << std::endl; break;
		case 2:
			std::cout << "����2���û���������˶ԣ�" << std::endl; break;
		case 3:
			std::cout << "����3���û����������˶ԣ�" << std::endl; break;
		case 4:
			std::cout << "����4�����鸽��Ŀ¼�Ƿ���ȷ���Լ��ļ��Ƿ���ڣ�" << std::endl; break;
		default:
			break;
		}
	}

	system("pause");
	return 0;
}