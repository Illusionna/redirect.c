.PHONY: clean

CC = gcc
TARGET = main

SRC = $(call rwildcard, ./, %.c)
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

ifeq ($(OS), Windows_NT)
	REMOVE = del
	TARGET := redirector.exe
	SRC := $(subst /,\,$(SRC))
	LIBRARY = -lws2_32 -lm
else
	REMOVE = rm
	TARGET := redirector
	SRC := $(subst \,/,$(SRC))
	LIBRARY = -pthread -lm
endif

preprocessing = $(SRC:.c=.i)
compilation = $(preprocessing:.i=.s)
assembly = $(compilation:.s=.o)

$(TARGET): $(assembly)
	$(CC) $(assembly) -o $(TARGET) -s -flto $(LIBRARY)

$(assembly): %.o: %.s
	$(CC) -c $< -o $@

$(compilation): %.s: %.i
	$(CC) -S $< -o $@ -O2 -flto

$(preprocessing): %.i: %.c
	$(CC) -E $< -o $@

clean:
	$(REMOVE) $(preprocessing)
	$(REMOVE) $(compilation)
	$(REMOVE) $(assembly)
	$(REMOVE) $(TARGET)