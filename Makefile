CXX      := g++
CXXSTD   := -std=c++17

BIN_DIR  := bin
OBJ_DIR  := build
INC_DIR  := include
SRC_DIR  := src
TARGET   := $(BIN_DIR)/panorama
TEST_TARGET := $(BIN_DIR)/run_tests
TEST_DIR := ./src/tests

OPENCV_PKG := $(shell pkg-config --exists opencv4 && echo opencv4 || echo opencv)
PKG_CONFIG_LIBS := $(OPENCV_PKG)

CXXFLAGS := $(CXXSTD) -Wall -Wextra -O2 -MMD -MP \
            -I$(INC_DIR) \
            $(shell pkg-config --cflags $(PKG_CONFIG_LIBS))

LDFLAGS  :=
LDLIBS   := $(shell pkg-config --libs $(PKG_CONFIG_LIBS))

SRCS      := $(wildcard $(SRC_DIR)/*.cpp)
MAIN_SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(SRCS))
OBJS      := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
LIB_OBJS  := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRCS))

TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/test_%.o,$(TEST_SRCS))
DEPS      := $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(TEST_DIR) -c $< -o $@

$(TEST_TARGET): $(LIB_OBJS) $(TEST_OBJS) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean all

run: all
	./$(TARGET) $(ARGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)

.PHONY: all debug run test clean