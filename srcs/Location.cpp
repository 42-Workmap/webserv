#include "../incs/Location.hpp"

Location::Location()
{
	//
}

Location::~Location()
{
}

Location::Location(const Location &src)
{
	*this = src;
}

Location&	Location::operator=(const Location &src)
{
	this->m_max_body_size = src.m_max_body_size;
	this->m_return_num = src.m_return_num;
	this->m_auto_index = src.m_auto_index;
	this-> m_root = src.m_root;
	this->m_upload_path = src.m_upload_path;
	this->m_uri = src.m_uri;
	this->m_auth_key = src.m_auth_key;
	this->m_return_url = src.m_return_url;
	this->m_allow_methods = src.m_allow_methods;
	this->m_error_pages = src.m_error_pages;
	this->m_cgi = src.m_cgi;
	return *this;
}

int		Location::getMaxBodySize()
{
	return m_max_body_size;
}

bool	Location::getAutoIndex()
{
	return m_auto_index;
}

std::string		Location::getUploadPath()
{
	return m_upload_path;
}

std::string		Location::getUri()
{
	return m_uri;
}

std::string		Location::getAuthKey()
{
	return m_auth_key;
}

std::string		Location::getRoot()
{
	return m_root;
}

int		Location::getReturnNum()
{
	return m_return_num;
}

std::string		Location::getReturnUrl()
{
	return m_return_url;
}

std::vector<std::string>	Location::getAllowMethods()
{
	return m_allow_methods;
}

std::vector<std::string>	Location::getIndexs()
{
	return m_indexs;
}

std::map<int, std::string>	Location::getErrorPages()
{
	return m_error_pages;
}

std::map<std::string, std::string>	Location::getCgi()
{
	return m_cgi;
}

void	Location::setMaxBodySize(int size)
{
	m_max_body_size = size;
}

void	Location::setReturnNum(int return_num)
{
	m_return_num = return_num;
}

void	Location::setAutoIndex(bool auto_index)
{
	m_auto_index = auto_index;
}

void	Location::setRoot(std::string root)
{
	m_root = root;
}

void	Location::setUploadPath(std::string path)
{
	m_upload_path = path;
}

void	Location::setUri(std::string uri)
{
	m_uri = uri;
}

void	Location::setAuthKey(std::string auth_key)
{
	m_auth_key = auth_key;
}

void	Location::setReturnUrl(std::string return_url)
{
	m_return_url = return_url;
}










