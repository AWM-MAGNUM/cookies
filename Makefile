CPP = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3 

SRC = src/main.cpp\
	src/parsing/Config.cpp src/parsing/ParseFile.cpp\
	src/parsing/ConfigServer.cpp src/parsing/utils.cpp\
	src/parsing/ConfigLocation.cpp\
	src/networking/NetworkClient.cpp \
	src/networking/WebServer.cpp \
	src/networking/sendResponse.cpp\
	src/networking/HttpRequest.cpp \
	src/response/HttpResponse.cpp\
	src/response/mimeTypes.cpp\
	src/response/GET.cpp\
	src/response/POST.cpp\
	src/response/DELETE.cpp\
	src/CGI/CGI.cpp

OBJS = $(SRC:.cpp=.o)

NAME = webserv

$(NAME) : $(OBJS)
	@$(CPP) $(FLAGS) $(OBJS) -o $(NAME)
	@printf "\033[0;32m- Webserver is ready ^^ \033[0m\n"

%.o: %.cpp
	@printf "\033[0;93m- Waiting...\033[0m \n"
	@$(CPP) $(FLAGS) -c $< -o $@

all : $(NAME) $(SRC)
	./$(NAME)

clean :
	@rm -rf $(OBJS)
	@printf "\033[1;31m- Object files removed !\033[0m\n"

fclean : clean
	@rm -rf $(NAME)
	@printf "\033[1;31m- Binaries removed !\033[0m\n"

re : clean all

.SILENT : $(OBJS)

.PHONY: all clean fclean re
