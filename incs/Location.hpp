#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <map>
#include <vector>


class Location {
	private:
		int m_max_body_size;
		bool m_auto_index;
		std::string m_upload_path;
		std::string m_uri;
		std::string m_auth_key;
		std::string m_root;
		int m_return_num;
		std::string m_return_url;
		std::vector<std::string> m_allow_methods;
		std::vector<std::string> m_indexs;
		std::map<int, std::string> m_error_pages;
		std::map<std::string, std::string> m_cgi;
	
	public:
		Location();
		virtual ~Location(){};
		Location(const Location &src);
		Location& operator=(const Location &src);
};

#endif