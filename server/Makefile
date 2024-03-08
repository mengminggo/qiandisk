PROJECT_PATH = ..
LDFLAGS = -lpthread 
INCLUDE_DIR := -I$(PROJECT_PATH)/
INCLUDE_DIR += -I$(PROJECT_PATH)/inc/

CFLAGS := -g


SRC_FILES := \
		$(PROJECT_PATH)/public/QUtils.cpp	\
		$(PROJECT_PATH)/public/ConnectManage.cpp \
		$(PROJECT_PATH)/public/FileIO.cpp
#		$(PROJECT_PATH)/public/FileManager.cpp 


OBJ_FILE := \
		server_select.cpp


OBJ3_FILE := \
		server_3.cpp

run:
	g++ -g $(OBJ3_FILE) $(SRC_FILES) $(LDFLAGS) $(INCLUDE_DIR) -o svr

#run_select:
#	g++ $(OBJ_FILE) $(SRC_FILES) $(LDFLAGS) $(INCLUDE_DIR) -o svr
