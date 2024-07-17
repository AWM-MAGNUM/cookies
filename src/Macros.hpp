#ifndef MACROS_HPP
# define MACROS_HPP

// CPP Includes
# include <iostream>
# include <iomanip>
# include <sstream>
# include <fstream>
# include <string>
# include <limits>
# include <cstdio>

// CPP Containers
# include <map>
# include <vector>
# include <algorithm>
# include <iterator>
# include <list>
# include <utility>


// C Includes
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <time.h>
# include <limits.h>
# include <errno.h>
# include <dirent.h>

// C System
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/time.h>

// Default
# define DEFAULT_CONFIG "config/default.conf"

// Colors
# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define RESET "\033[0m"

extern std::map<std::string, std::string> mimeTypes;

template <typename T>
std::string toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

# endif
