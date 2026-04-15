# You will need SDL3 (http://www.libsdl.org) which is still unreleased/unpackaged.

##---------------------------------------------------------------------
## Variables
##---------------------------------------------------------------------

CXX = g++
IMGUI_DIR = ./thirdparty/imgui_1.92.7
BUILD_DIR = ./build
BIN_DIR = ./build/bin
SRC_DIR = ./src

APP_NAME = main

SRCS = $(SRC_DIR)/main.cpp
SRCS += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SRCS += $(IMGUI_DIR)/backends/imgui_impl_sdl3.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SRCS)))))

LINUX_GL_LIBS = -lGL
CXXFLAGS = -std=c++23 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -ggdb -Wall -Wformat -Wextra
CXXFLAGS += `pkg-config sdl3 --cflags`
LIBS = $(LINUX_GL_LIBS) -ldl `pkg-config sdl3 --libs`

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(BIN_DIR)/$(APP_NAME)

$(BIN_DIR)/$(APP_NAME): $(OBJS) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(IMGUI_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(IMGUI_DIR)/backends/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN_DIR): $(BUILD_DIR)
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
