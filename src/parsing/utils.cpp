
#include "ConfigServer.hpp"

bool isValidIPAddress(const std::string& ip) {
	int numSegments = 0;
	int numDigits = 0;
	int numDots = 0;
	int segmentValue = 0;

	for (size_t i = 0; i < ip.length(); ++i) {
		if (isdigit(ip[i])) {
			++numDigits;
			segmentValue = segmentValue * 10 + (ip[i] - '0');

			// Check if the segment value exceeds 255
			if (segmentValue > 255)
				return false;
			}
		else if (ip[i] == '.') {
			++numDots;
			if (numDigits == 0 || numDigits > 3 || numDots > 3)
				return false; // Each segment should contain at least 1 digit and at most 3 digits,
				// and there should be exactly 3 dots
			numSegments++;
			numDigits = 0;
			segmentValue = 0;
		} else
			return false; // Invalid character
	}
	// Ensure the last segment is valid
	if (numSegments != 3 || numDigits == 0 || numDigits > 3 || numDots != 3)
		return false;

	return true;
}

bool	isAllSpacesOrTabs(const std::string& str) {
    for (std::string::size_type i = 0; i < str.length(); ++i) {
        if (str[i] != ' ' && str[i] != '\t') {
            return false;
        }
    }
    return true;
}

std::vector<std::string>	splitVal(std::string& str) {
	std::vector<std::string>	vals;
	size_t						pos = 0;
	std::string	tmp;
	// Extract the word
	while (pos < str.size()) {
		while (pos < str.size() && str[pos] != ' ' && str[pos] != '\t') {
			tmp += str[pos];
			pos++;
		}
		// Push the word into the result vector
		if (!tmp.empty()) {
			vals.push_back(tmp);
			tmp.clear();
		}
		pos++;
	}
	return vals;
}

size_t isNum(std::string str) {
    size_t number = 0;
    for (size_t i = 0; i < str.length(); i++) {
        if (isdigit(str[i]))
            number = number * 10 + (str[i] - '0');
        else
            throw std::runtime_error("Error: Invalid code");
    }
    if (number > INT_MAX)
        throw std::runtime_error("Eroor: Invalid code (Big Number)!");
    return number;
}

bool	isUrl(const std::string& str) {
	return (str.substr(0, 7) == "http://") || (str.substr(0, 8) == "https://");
}

std::vector<std::string>	splitArgs(std::string val) {
	std::vector<std::string> result;
	std::string word;
	bool inQuotes = false;

    for (size_t i = 0; i < val.length(); ++i) {
        if (val[i] == '"') // Toggle the inQuotes flag
			inQuotes = !inQuotes;
        else if ((val[i] == ' ' || val[i] == '\t') && !inQuotes) {
            // If it's a space or tab and not within quotes, push the current word if not empty
            if (!word.empty()) {
                result.push_back(word);
                word.clear();
            }
        } else {
            // Otherwise, append the character to the current word
            word += val[i];
        }
    }
    if (!word.empty())
        result.push_back(word);

    return result;
}

size_t	parseMaxBodySize(char &unit, std::string& bodySize, size_t sizeEnd) {
	size_t size;
	if (std::isdigit(unit)) {
		size = std::strtol(bodySize.c_str(), NULL, 10);
		unit = ' ';
	} else
		size = std::strtol(bodySize.substr(0, sizeEnd).c_str(), NULL, 10);

	switch (unit) {
		case 'k':
			size *= 1024LL; // 1 kilobyte = 1024 bytes
			break;
		case 'm':
			size *= 1024LL * 1024LL; // 1 megabyte = 1024 kilobytes
			break;
		case 'g':
			size *= 1024LL * 1024LL * 1024LL; // 1 gigabyte = 1024 megabytes
			break;
		case 't':
			size *= 1024LL * 1024LL * 1024LL * 1024LL; // 1 terabyte = 1024 gigabytes
			break;
		default:
		// No unit letter, so the size is in bytes by default
			break;
	}
	return size;
}

ConfigLocation	ConfigServer::parseLocation(std::vector<t_tokens> &tok, std::vector<t_tokens>::iterator& it) {
	ConfigLocation	loc;
	int rt = 0, ind = 0, met = 0, bd = 0, aut = 0, up = 0;
	int	err = 0, red = 0;

	loc.setLocationName(it->_value);
	it++;
	while (it != tok.end() && it->_type != "}") {
		if (it->_type == "root") {
			loc.setRoot(it->_value);
			rt++;
		}
		else if (it->_type == "index") {
			loc.setIndex(it->_value);
			ind++;
		}
		else if (it->_type == "allowed_methods") {
			loc.setMethods(it->_value);
			met++;
		}
		else if (it->_type == "client_max_body_size") {
			loc.setBodySize(it->_value);
			bd++;
		}
		else if (it->_type == "autoindex") {
			loc.setAutoIndex(it->_value);
			aut++;
		}
		else if (it->_type == "upload") {
			loc.setUpload(it->_value);
			up++;
		}
		else if (it->_type == "error_page") {
			loc.setErrorPage(it->_value);
			err++;
		}
		else if (it->_type == "return") {
			loc.setRedirect(true);
			loc.setRedirection(it->_value);
			red++;
		}
		else if (it->_type == "interpreter") {
			loc.setInterpreter(it->_value);
		}
		else if (it->_type.empty()){
			it++;
			continue;
		}
		else {
			throw ConfigServerException("Error: Unexpected parametre in location block.");
			break; //error invalid type
		}
		it++;
    }
	if (rt != 1 || ind > 1)
		throw ConfigServerException("Error: Should have one root/index parametre.");
	else if (met != 1)
		throw ConfigServerException("Error: Should have one allowed_methods parametre.");
	else if (bd > 1)
		throw ConfigServerException("Error: Must have one body_size parametre.(Duplicate)");
	else if (aut != 1)
		throw ConfigServerException("Error: Should have one autoindex parametre.");
	else if (up > 1)
		throw ConfigServerException("Error: Must have one uplod parametre.(Duplicate)");
	else if (red > 1)
		throw ConfigServerException("Error: Must have one return parametre.(Duplicate)");

	if (it->_type != "}")
		throw ConfigServerException("Error: expected '}' in the end of location directive.");
	if (it->_value == "}")
		throw ConfigServerException("Error: Unexpected token");
	return (loc);
}
