.PYONY: build all install clean

TARGET_NAME = world4utau
TARGET_TYPE = bin
EXE = resampler.exe # 临时用来兼容OpenUtau

CUR_DIR  = $(shell pwd)

OUT_DIR = $(CUR_DIR)/bin
SRC_DIR = $(CUR_DIR)/src
OBJ_DIR = $(CUR_DIR)/obj

SRCS = $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.c))
OBJS = $(notdir $(patsubst %.c, %.o, $(SRCS)))
OBJS_WITH_DIR = $(addprefix $(OBJ_DIR)/,$(OBJS))

TARGET = $(OUT_DIR)/$(TARGET_NAME)
INCLUDES = -I$(SRC_DIR)
CFLAGS =  -D_DEBBUG -O3 #-g -Wall
LIBS = -lfftw3 -lm

CC = gcc

build: setup ${TARGET}
	cp ${TARGET} ${EXE}

setup:
	mkdir -p $(OBJ_DIR)
	mkdir -p ${OUT_DIR}

$(TARGET):$(OBJS)
	$(CC) $(OBJS_WITH_DIR) -o $(TARGET) $(CFLAGS) $(LIBS)

$(OBJS): %.o : $(SRC_DIR)/%.c
	$(info $@ $<)
	$(CC) -o $(OBJ_DIR)/$@ -c $< $(CFLAGS)

clean:
	@rm -rf obj/*.o
	@rm ${TARGET}
	@rm $(EXE)