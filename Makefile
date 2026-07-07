CXX      := g++
CXXSTD   := -std=c++17

BIN_DIR  := bin
OBJ_DIR  := build
INC_DIR  := include
SRC_DIR  := src
TARGET   := $(BIN_DIR)/panorama

OPENCV_PKG := $(shell pkg-config --exists opencv4 && echo opencv4 || echo opencv)

PKG_CONFIG_LIBS := $(OPENCV_PKG)

CXXFLAGS := $(CXXSTD) -Wall -Wextra -O2 -MMD -MP \
            -I$(INC_DIR) \
            $(shell pkg-config --cflags $(PKG_CONFIG_LIBS))

LDFLAGS  :=
LDLIBS   := $(shell pkg-config --libs $(PKG_CONFIG_LIBS))
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean all

run: all
	./$(TARGET) $(ARGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)
