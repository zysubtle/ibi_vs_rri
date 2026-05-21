CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Werror -Iinclude

TARGET_API := build/test_api_compile
TARGET_INPUT := build/test_input_validation
TARGET_SIGNAL := build/test_signal_quality
TARGET_PULSE := build/test_pulse_detector
TARGET_STATE := build/test_state_machine

.PHONY: test clean

test: $(TARGET_API) $(TARGET_INPUT) $(TARGET_SIGNAL) $(TARGET_PULSE) $(TARGET_STATE)
	./$(TARGET_API)
	./$(TARGET_INPUT)
	./$(TARGET_SIGNAL)
	./$(TARGET_PULSE)
	./$(TARGET_STATE)

$(TARGET_API): tests/test_api_compile.c src/ppg_ibi.c include/ppg_ibi.h include/ppg_ibi_config.h src/ppg_ibi_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_api_compile.c src/ppg_ibi.c -o $(TARGET_API)

$(TARGET_INPUT): tests/test_input_validation.c src/ppg_ibi.c include/ppg_ibi.h include/ppg_ibi_config.h src/ppg_ibi_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_input_validation.c src/ppg_ibi.c -o $(TARGET_INPUT)

$(TARGET_SIGNAL): tests/test_signal_quality.c src/ppg_ibi.c include/ppg_ibi.h include/ppg_ibi_config.h src/ppg_ibi_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_signal_quality.c src/ppg_ibi.c -o $(TARGET_SIGNAL)

clean:
	rm -rf build

$(TARGET_PULSE): tests/test_pulse_detector.c src/ppg_ibi.c include/ppg_ibi.h include/ppg_ibi_config.h src/ppg_ibi_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_pulse_detector.c src/ppg_ibi.c -o $(TARGET_PULSE)

$(TARGET_STATE): tests/test_state_machine.c src/ppg_ibi.c include/ppg_ibi.h include/ppg_ibi_config.h src/ppg_ibi_internal.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_state_machine.c src/ppg_ibi.c -o $(TARGET_STATE)
