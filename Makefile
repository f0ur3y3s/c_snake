MAIN_NAME = main

INCLUDES = include
LINKS = -lpthread
CFLAGS = -Wall -I$(INCLUDES)

CC = gcc
BIN = bin
SRC = src
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SRCS))
DEPS = $(wildcard $(INCLUDES)/*.h)

all: $(OBJS) $(BIN)/$(MAIN_NAME)

test:
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(DEPS)

debug: CFLAGS += -g3 -DDEBUG
debug: all

$(BIN)/$(MAIN_NAME): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS) $(LINKS)

$(BIN)/%.o: $(SRC)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS) $(LINKS)

clean:
	@rm -rf $(BIN)
	@find . -name "*.o" -exec rm -rf {} \;

clean-obj:
	@find . -name "*.o" -exec rm -rf {} \;