TARGET_LIST  := queue_test.exe
CC := gcc
INC := -I.
LIST_SRC := queue.c queue_test.c 

SRC_PATH := $(dir $(SRC))

vpath %.c $(dir $(SRC))

C_FLAG = -g -O0

$(TARGET_LIST): $(LIST_SRC)
	$(CC) $(INC) $(C_FLAG) $^ -o $@

print-%:
	@echo $(*) = $($(*))


clean:
	rm $(TARGET_LIST)