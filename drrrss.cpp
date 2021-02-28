
#include<fstream>
#include<io.h>
#include<stdio.h>
#include<Windows.h>
#include<iostream>
#include<winsock.h>
#include<sstream>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
void initialization();
bool fileExists(char *filename) {
	return (_access(filename, 0) == 0);
}
char *i2c(int n) {
	stringstream ss;
	ss << n;
	char ans[20480];
	ss >> ans;
	ss.clear();
	return ans;
}
char *getRequest(const char *url,const char *cookie) {
	char buf[2048] = "";
	memset(buf, 0, sizeof(buf));
	const char s1[] = "curl -H \"";
	const char s2[] = "\" -o out.txt  ";
	strcat_s(buf, s1);
	strcat_s(buf, cookie);
	strcat_s(buf, s2);
	strcat_s(buf, url);
	cout << buf << endl;
	system(buf);
	while (!fileExists) {
		Sleep(100);
		cout << "Looking for response file..." << endl;
	}
	fstream f;
	f.open("out.txt", ios::in);
	char res[20480];
	memset(res, 0, 20480);
	f.getline(res, 20480);
	f.close();
	return res;
}
int main() {


	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	//定义发送缓冲区和接受缓冲区
	char send_buf[20480];
	char recv_buf[20480];
	memset(send_buf, 0, sizeof(send_buf));
	memset(recv_buf, 0, sizeof(recv_buf));
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;
	SOCKET s_accept;
	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	initialization();
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5010);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "套接字绑定失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字绑定成功！" << endl;
	}
	//设置套接字为监听状态
	if (listen(s_server, SOMAXCONN) < 0) {
		cout << "设置监听状态失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "设置监听状态成功！" << endl;
	}
	cout << "服务端正在监听连接，请稍候...." << endl;
	//接受连接请求
	len = sizeof(SOCKADDR);
	s_accept = accept(s_server, (SOCKADDR *)&accept_addr, &len);
	if (s_accept == SOCKET_ERROR) {
		cout << "连接失败！" << endl;
		WSACleanup();
		return 0;
	}
	cout << "连接建立，准备接受数据" << endl;
	//接收数据
	char response[] =
		"HTTP/1.1 200 OK\r\n"
		"Access-Control-Allow-Origin:*\r\n"
		"Content-Length:";

	char responsep2[]="\r\nContent-Type:text/html\r\n\r\n";
	while (1) {
		cout << "DONE" << endl;
		memset(send_buf, 0, 20480);
		recv_len = recv(s_accept, recv_buf, 20480, 0);
		if (recv_len < 0) {
			cout << "接受失败！" << endl;
			MessageBox(NULL, TEXT("ERROR"), TEXT("ERROR"), 16);
			break;
		}
		else {
			cout << "客户端信息:" << recv_buf << endl;
		}
		if (recv_buf[5] == '1') {
			char tmp[20480];
			memset(tmp, 0, 20480);
			strcat_s(tmp, getRequest("https://drrr.com/json.php?fast=1", "Cookie: __cfduid=dc1cc7f1fd5e08249e32c12275e3590be1614510829; drrr-session-1=ug46u99198276toha3k69c4e72; _ga=GA1.2.489149162.1614510835; _gid=GA1.2.773823393.1614510835"));
			strcat_s(send_buf, response);
			int sum = 0;
			for (int i = 20480 - 1; i >= 0; i--) {
				if (int(tmp[i]) <= -2) {
					tmp[i] = '\0';
					sum++;
				}
				//cout << int(tmp[i]) << "";
			}
			//cout << "DEBUG:" << sum;
			//system("pause");
			strcat_s(send_buf, i2c(20479 - sum));
			strcat_s(send_buf, responsep2);
			strcat_s(send_buf, tmp);

			send_len = send(s_accept, send_buf, 20479, 0);
			if (send_len < 0) {
				cout << "发送失败！" << endl;
				MessageBox(NULL, TEXT("ERROR"), TEXT("ERROR"), 16);
				break;
			}
		}
	}
	//关闭套接字
	closesocket(s_server);
	closesocket(s_accept);
	//释放DLL资源
	WSACleanup();
	return 0;
}
void initialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}
	//填充服务端地址信息

}
