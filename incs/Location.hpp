#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <map>
#include <vector>
#include <limits>

class Location {
	private:
		int		m_max_body_size;
		int		m_return_num;
		bool	m_auto_index;
		std::string m_root;
		std::string m_upload_path; // where are you from 
		std::string m_uri;
		std::string m_auth_key;
		std::string m_return_url;
		std::vector<std::string> m_allow_methods;
		std::vector<std::string> m_indexs;
		std::map<int, std::string> m_error_pages;
		std::map<std::string, std::string> m_cgi;
	
	public:
		Location();
		virtual ~Location();
		Location(const Location &src);
		Location& operator=(const Location &src);

		//get
		int &getMaxBodySize();
		bool &getAutoIndex();
		std::string &getUploadPath();
		std::string &getUri();
		std::string &getAuthKey();
		std::string &getRoot();
		int &getReturnNum();
		std::string &getReturnUrl();
		std::vector<std::string> &getAllowMethods();
		std::vector<std::string> &getIndexs();
		std::map<int, std::string> &getErrorPages();
		std::map<std::string, std::string> &getCgi();

		//set
		void setMaxBodySize(int size);
		void setReturnNum(int return_num);
		void setAutoIndex(bool auto_index);
		void setRoot(std::string root);
		void setUploadPath(std::string path);
		void setUri(std::string uri);
		void setAuthKey(std::string auth_key);
		void setReturnUrl(std::string return_url);
};

#endif