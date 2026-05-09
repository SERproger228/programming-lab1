CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -pedantic -Iinclude

APP := lr1
TEST_APP := tests_runner

SRC := src/main.c src/type_info.c src/dynamic_array.c src/polynomial.c
TEST_SRC := tests/test_main.c src/type_info.c src/dynamic_array.c src/polynomial.c

.PHONY: all run test clean

all: $(APP)

$(APP): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(APP)

$(TEST_APP): $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_APP)

run: $(APP)
	./$(APP)

test: $(TEST_APP)
	./$(TEST_APP)

clean:
	rm -f $(APP) $(TEST_APP)
