CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic -O2
CPPFLAGS ?= -Isrc
DEPFLAGS := -MMD -MP

SRC_DIR := src
BUILD_DIR := build

TARGET := riscv_sim
OOO_TARGET := ooo_sim

CPU_SRCS := main.cpp common.cpp memory.cpp cpu.cpp
OOO_SRCS := ooo_main.cpp common.cpp memory.cpp ooo_cpu.cpp

CPU_OBJS := $(CPU_SRCS:%.cpp=$(BUILD_DIR)/%.o)
OOO_OBJS := $(OOO_SRCS:%.cpp=$(BUILD_DIR)/%.ooo.o)
DEPS := $(CPU_OBJS:.o=.d) $(OOO_OBJS:.o=.d)

.PHONY: all clean run test

all: $(TARGET) $(OOO_TARGET)

$(TARGET): $(CPU_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OOO_TARGET): $(OOO_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.ooo.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: all
	./run_tests.sh

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(OOO_TARGET)
	rm -f riscv_full hci_smoke *.o *.ooo.o *.full.o hci_smoke.o functional_hci.o

-include $(DEPS)
