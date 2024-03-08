PROJECT_PATH = ..

OUT := cli

# target source
AR =
CC = gcc
CXX = g++
#TAG := cpptest

# 头文件路径

INC_DIR += -I$(PROJECT_PATH)/
INC_DIR += -I$(PROJECT_PATH)/inc/
# 库路径
LIBS_DIR += -L./

# 依赖的库文件
LIBS += -lpthread

# 编译器参数
CFLAGS += $(INC_DIR)
CFLAGS += $(LIBS_DIR)

#CFLAGS += -Wall
CFLAGS += -w

SRC_FILES := \
		$(PROJECT_PATH)/public/QUtils.cpp	\
		$(PROJECT_PATH)/client/FileInfoManager.cpp \
		$(PROJECT_PATH)/public/ConnectManage.cpp  \
		$(PROJECT_PATH)/public/FileIO.cpp
OBJ_FILE := \
		client.cpp

OBJ := $(SRC_FILES) $(OBJ_FILE)


run1:
	$(CXX) -g $(OBJ) $(CFLAGS) -o $(OUT)
	
