# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/19 12:54:54 by nesdebie          #+#    #+#              #
#    Updated: 2025/04/19 13:02:06 by nesdebie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Compiler settings
CXX            := g++
CXXFLAGS       := -std=c++17 -O2 -fPIC
LDFLAGS +=	-Wl,-rpath,@executable_path/third_party/Vulkan-Loader/build/install/lib \
			-Wl,-rpath,@executable_path/third_party/GLFW/build/install/lib


# Project structure
SRC_DIR        := src
BUILD_DIR      := build
BIN            := scop

SRC            := $(wildcard $(SRC_DIR)/*.cpp)
OBJ            := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

# Vendor paths
VULKAN_HEADERS_DIR        := third_party/Vulkan-Headers
VULKAN_HEADERS_INSTALL    := $(VULKAN_HEADERS_DIR)/build/install

VULKAN_LOADER_DIR         := third_party/Vulkan-Loader
VULKAN_LOADER_INSTALL     := $(VULKAN_LOADER_DIR)/build/install

GLFW_DIR                  := third_party/GLFW
GLFW_INSTALL              := $(GLFW_DIR)/build/install

# Include & lib flags
INCLUDES := \
	-Iinclude \
	-I$(VULKAN_HEADERS_INSTALL)/include \
	-I$(VULKAN_LOADER_INSTALL)/include \
	-I$(GLFW_INSTALL)/include

LIBS := \
	-L$(VULKAN_LOADER_INSTALL)/lib -lvulkan \
	-L$(GLFW_INSTALL)/lib -lglfw \
	-framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation

.PHONY: all vendor clean

all: vendor $(BIN)

# Vendor step: clone & install headers, loader, and shared GLFW
vendor: \
	$(VULKAN_HEADERS_INSTALL)/share/cmake/VulkanHeaders/VulkanHeadersConfig.cmake \
	$(VULKAN_LOADER_INSTALL)/lib/libvulkan.so \
	$(GLFW_INSTALL)/lib/libglfw.dylib

$(VULKAN_HEADERS_INSTALL)/share/cmake/VulkanHeaders/VulkanHeadersConfig.cmake:
	git clone --depth 1 https://github.com/KhronosGroup/Vulkan-Headers.git $(VULKAN_HEADERS_DIR)
	mkdir -p $(VULKAN_HEADERS_DIR)/build
	cd $(VULKAN_HEADERS_DIR) && \
	  cmake -B build -DCMAKE_INSTALL_PREFIX=build/install && \
	  cmake --build build --target install

$(VULKAN_LOADER_INSTALL)/lib/libvulkan.so:
	git clone --depth 1 https://github.com/KhronosGroup/Vulkan-Loader.git $(VULKAN_LOADER_DIR)
	mkdir -p $(VULKAN_LOADER_DIR)/build
	cd $(VULKAN_LOADER_DIR) && \
	  cmake -B build \
	    -DCMAKE_INSTALL_PREFIX=build/install \
	    -DCMAKE_PREFIX_PATH=$(abspath $(VULKAN_HEADERS_INSTALL)) && \
	  cmake --build build --target install

$(GLFW_INSTALL)/lib/libglfw.dylib:
	git clone --depth 1 https://github.com/glfw/glfw.git $(GLFW_DIR)
	mkdir -p $(GLFW_DIR)/build
	cd $(GLFW_DIR) && \
	  cmake -B build \
	    -DBUILD_SHARED_LIBS=ON \
	    -DCMAKE_INSTALL_PREFIX=build/install && \
	  cmake --build build --target install

# Build rules
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(BUILD_DIR) $(BIN) third_party
