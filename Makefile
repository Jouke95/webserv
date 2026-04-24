NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -Iincludes -Iincludes/gzip -std=c++17

SRCS = src/main.cpp src/RequestParser.cpp src/Server.cpp src/HttpRequest.cpp src/HttpResponse.cpp \
	   src/Client.cpp src/Config.cpp src/ResponseBuilder.cpp src/RequestHandler.cpp \
	   src/gzip/DeflateCompress.cpp src/gzip/DeflateDecompress.cpp src/gzip/Gzip.cpp src/gzip/Huffman.cpp src/gzip/LZ77.cpp \
	   src/utils.cpp src/RequestValidator.cpp src/ConfigValidator.cpp \

OBJ_DIR = obj

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
