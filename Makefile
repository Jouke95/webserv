NAME    = webserv
CXX     = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS    = src/main.cpp \
		  src/Config.cpp \
		  src/Server.cpp \
		  src/Client.cpp \
		  src/HttpRequest.cpp \
		  src/HttpResponse.cpp

OBJS    = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I includes -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
