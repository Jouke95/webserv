NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -Iincludes

SRCS = src/main.cpp src/Parser.cpp src/Server.cpp src/HttpRequest.cpp src/HttpResponse.cpp \
src/endpoints/CommonGatewayInterface.cpp src/endpoints/AEndpoint.cpp src/endpoints/TextEndpoint.cpp src/Client.cpp

OBJ_DIR = obj

OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re