SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
BUILD_DIR = Build

OBJS=$(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRCS)))
DEPS=$(patsubst %.o,%.d, $(OBJS))

CC = gcc
TARGET = ftp_client.exe
CFLAGS = $(addprefix -I,$(SRC_DIR)/)
CFLAGS += -o2 -mtune=native -march=native

LDFLAGS = -pthread
#ヘッダディレクトリ
INC_DIR = -I/usr/local/include
INC_DIR += 
#リンクライブラリー
#LIB_DIR = -L/usr/local/lib
#LIB_DIR += -L/usr/lib
#LINK_LIB = -larchive
LINK_LIB = -lrt

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
 
$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LINK_LIB)

$(BUILD_DIR)/%.o : %.c
	mkdir -p $(dir $@); \
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.d : %.c
	mkdir -p $(dir $@); \
	$(CC) -MM $(CFLAGS) $< \
	| sed 's/$(notdir $*).o/$(subst /,\/,$(patsubst %.d,%.o,$@) $@)/' > $@ ; \
	[ -s $@ ] || rm -f $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
-include $(DEPS)