#-------------------------------------------------------------------------------
# Author      : Ethan Rietz
# Date        : 2022-01-05
# Description : Makefile to compile and submit code for CS344 assignment 3
#-------------------------------------------------------------------------------

CC := gcc
CFLAGS := --std=gnu99

# This clever trick was stolen from the makefile here
# https://web.engr.oregonstate.edu/~goinsj/resources/general/C/Makefile
DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS += -g -Wall
else
	CFLAGS += -DNDEBUG -O3
endif

BIN := smallsh
SRC := src
OBJ := obj
TEST := test
TEST_OBJ := $(TEST)/obj
TEST_BIN := $(TEST)/run_tests

SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
INCLUDES := $(wildcard $(SRC)/*.h)
TESTS := $(wildcard $(TEST)/*.c)
TEST_OBJS := $(patsubst $(TEST)/%.c, $(TEST_OBJ)/%.o, $(TESTS))
# cannot include two main functions
TEST_DEPS := $(filter-out $(OBJ)/main.o, $(OBJS)) $(INCLUDES)

SUBMIT := rietze_program3.zip

.PHONY: all
all: $(OBJ) $(BIN)

$(OBJ):
	mkdir $@

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS)

$(TEST_OBJ):
	mkdir $@

$(TEST_OBJ)/%.o: $(TEST)/%.c $(TEST_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) $(TEST_DEPS)
	$(CC) -o $@ $(TEST_OBJS) $(TEST_DEPS)

.PHONY: test
test: $(OBJ) $(TEST_OBJ) $(TEST_BIN)
	./$(TEST_BIN)

.PHONY: leaks
leaks: $(BIN)
	valgrind --leak-check=yes --show-reachable=yes ./$(BIN)

.PHONY: clean
clean:
	rm $(OBJS)
	rmdir $(OBJ)
	rm $(BIN)
	rm $(TEST_OBJS)
	rmdir $(TEST_OBJ)
	rm $(TEST_BIN)

	rm -rf ./test/scratch/*

.PHONY: run
run: $(OBJ) $(BIN)
	@./smallsh

.PHONY: submit
submit:
	zip -r $(SUBMIT) ./ -x "$(BIN)" -x "$(OBJ)*" -x "$(TEST_OBJ)*"

