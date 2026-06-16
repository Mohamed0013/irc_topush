CC = c++
CFLAGS = -Wall -Wextra -std=c++98 
INCLUDE = -I./headers
NAME = ircserv

SRCS = srcs/main.cpp \
       srcs/server.cpp \
       srcs/client.cpp \
       srcs/channel.cpp \
       srcs/bot.cpp \
       srcs/execute.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
.SECONDARY : ${OBJ_FILE}