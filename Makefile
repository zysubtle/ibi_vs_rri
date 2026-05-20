CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Werror -Iinclude
TARGET := build/test_api_compile

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

$(TARGET): tests/test_api_compile.c src/ppg_ibi.c include/ppg_ibi.h include/ppg_ibi_config.h src/ppg_ibi_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_api_compile.c src/ppg_ibi.c -o $(TARGET)

clean:
	rm -rf build
