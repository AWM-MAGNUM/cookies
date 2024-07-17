#ifndef CONFIGLOCATION_HPP
# define CONFIGLOCATION_HPP

#include "../Macros.hpp"
#include <stdexcept>
#include <cstdlib>

#include "Config.hpp"
#include "ConfigServer.hpp"

class Config;
class ConfigServer;
class ConfigLocation {
	public:
		ConfigLocation();
		ConfigLocation(const ConfigLocation &src);
		ConfigLocation& operator=(const ConfigLocation &src);
		~ConfigLocation();

		// Setters
		void	setLocationName(std::string& locationName);
		void	setRoot(std::string& root);
		void	setIndex(std::string& index);
		void	setMethods(std::string& methods);
		void	setBodySize(std::string& bodySize);
		void	setAutoIndex(std::string& autoindex);
		void	setUpload(std::string& upload);
		void	setErrorPage(std::string& errorPage);
		void	setRedirect(bool redirect);
		void	setRedirection(std::string& redirection);
		void	setInterpreter(std::string& interpreter);

		// Getters
		std::string& 							getLocationName();
		std::string& 							getRoot();
		std::vector<std::string>&				getIndex();
		std::vector<std::string>&				getMethods();
		size_t&									getMaxBodySize();
		bool&									getAutoIndex();
		std::string&							getUpload();
		std::map<int, std::string>&				getErrorPage();
		bool&									getRedirect();
		int&									getRedirectCode();
		std::string&							getRedirection();
		std::map<std::string, std::string>&		getInterpreter();
		bool&									getSuppCgi();

		class	ConfigLocationException: public std::exception{
			std::string message;
			public:
				ConfigLocationException(std::string msg) : message(msg){};
			const char *what() const throw()
			{
				return (this->message.c_str());
			}
			~ConfigLocationException() throw(){};
		};

	private:
		std::string							_locationName;
		std::string							_root;
		std::vector<std::string>			_index;
		std::vector<std::string>			_methods;
		size_t								_maxBodySize;
		bool								_autoindex;
		std::string							_upload;
		std::map<int, std::string>			_errorPage;
		bool								_redirect;
		std::string							_redirection;
		int									_redirectCode;
		std::map<std::string, std::string>	_interpreter;
		bool								_supportCgi;
};

#endif
