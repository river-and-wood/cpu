CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic -O2
CPPFLAGS ?= -Isrc/common -Isrc/memory -Isrc/cpu/in_order -Isrc/cpu/ooo
DEPFLAGS := -MMD -MP

SRC_DIR := src
BUILD_DIR := build
SCRIPT_DIR := scripts

TARGET := riscv_sim
OOO_TARGET := ooo_sim

CPU_SRCS := \
	apps/main.cpp \
	common/common.cpp \
	memory/memory.cpp \
	cpu/in_order/cpu.cpp

OOO_SRCS := \
	apps/ooo_main.cpp \
	common/common.cpp \
	memory/memory.cpp \
	cpu/ooo/icache.cpp \
	cpu/ooo/core.cpp \
	cpu/ooo/frontend.cpp \
	cpu/ooo/scheduler.cpp \
	cpu/ooo/execute.cpp \
	cpu/ooo/rob.cpp \
	cpu/ooo/state.cpp \
	cpu/ooo/trace.cpp

CPU_OBJS := $(CPU_SRCS:%.cpp=$(BUILD_DIR)/%.o)
OOO_OBJS := $(OOO_SRCS:%.cpp=$(BUILD_DIR)/%.ooo.o)
DEPS := $(CPU_OBJS:.o=.d) $(OOO_OBJS:.o=.d)

.PHONY: all clean run test benchmark

all: $(TARGET) $(OOO_TARGET)

$(TARGET): $(CPU_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OOO_TARGET): $(OOO_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.ooo.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: all
	./$(SCRIPT_DIR)/run_tests.sh

benchmark: all
	./$(SCRIPT_DIR)/benchmark_report.sh

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(OOO_TARGET)
	rm -f riscv_full hci_smoke *.o *.ooo.o *.full.o hci_smoke.o functional_hci.o

-include $(DEPS)
