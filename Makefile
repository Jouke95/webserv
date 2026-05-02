NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -Iincludes -Iincludes/gzip -std=c++17

SRCS = src/CGI.cpp \
		src/Client.cpp \
		src/Config.cpp \
		src/ConfigValidator.cpp \
		src/HttpRequest.cpp \
		src/HttpResponse.cpp \
		src/RequestHandler.cpp \
		src/RequestParser.cpp \
		src/RequestValidator.cpp \
		src/ResponseBuilder.cpp \
		src/Server.cpp \
		src/main.cpp \
		src/utils.cpp \
		src/gzip/DeflateCompress.cpp \
		src/gzip/DeflateDecompress.cpp \
		src/gzip/Gzip.cpp \
		src/gzip/Huffman.cpp \
		src/gzip/LZ77.cpp \

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
