CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: clean s21_grep

s21_cat:
	$(CC) $(CFLAGS) s21_grep.c -o s21_grep

test: s21_grep
	sh test_grep.sh

check:
	leaks -atExit -- ./s21_grep -e if s21_grep.c
	clang-format -n s21_grep.c

clean: 
	rm -rf s21_grep
	rm -rf s21_grep_test
	rm -rf grep