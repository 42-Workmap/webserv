#include "../incs/Client.hpp"
#include "../incs/Response.hpp"
#include "../incs/Config.hpp"

bool	Response::isDirectory(std::string path)
{
	struct stat info;

	if (stat(path.c_str(), &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;
	else
		return false;
}

bool Response::isExist(std::string path)
{
	struct stat info;
	return (stat(path.c_str(), &info) == 0);
}


void Response::addStatusLine(int status)
{
	std::string statuscode;
	std::string codemsg;

	statuscode = std::to_string(status);
	codemsg = Config::getConfig()->getStatusCodeMap()[statuscode];
	m_message += "HTTP/1.1 "+statuscode + " " + codemsg + "\r\n";
	return ;
}

void Response::addDate(void)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buff[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buff, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	m_message += "Date: " + std::string(buff) + "\r\n";

	return ;
}

void Response::addContentLanguage()
{
	m_message += "Content-Language: ko-kr\r\n";
	return ;
}

void Response::addContentType(std::string type)
{
	std::string contenttype = "";
	if (Config::getConfig()->getMimeTypeMap().count(type) == 0)
		contenttype = "application/octet-stream";
	else
		contenttype = Config::getConfig()->getMimeTypeMap()[type];
	m_message += "Content-Type: " + contenttype + "\r\n";
}

void Response::addContentLength(int size)
{
	m_message += "Content-Length: " + std::to_string(size) + "\r\n";
}

void Response::addEmptyLine(void)
{
	m_message += "\r\n";
}

void Response::addServer(void)
{
	m_message += "Server: ft_nginx\r\n";
}

void Response::addLocation(std::string &url)
{
	m_message += "Location: " + url + "\r\n";
}