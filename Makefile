CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Werror -Iinclude

TARGET_API := build/test_api_compile
TARGET_INPUT := build/test_input_validation

.PHONY: test clean

test: $(TARGET_API) $(TARGET_INPUT)
	./$(TARGET_API)
	./$(TARGET_INPUT)

$(TARGET_API): tests/test_api_compile.c src/ppg_ibi.c include/ppg_ibi.h include/ppg_ibi_config.h src/ppg_ibi_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_api_compile.c src/ppg_ibi.c -o $(TARGET_API)

$(TARGET_INPUT): tests/test_input_validation.c src/ppg_ibi.c include/ppg_ibi.h include/ppg_ibi_config.h src/ppg_ibi_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_input_validation.c src/ppg_ibi.c -o $(TARGET_INPUT)

clean:
	rm -rf build
