EXE = MatrixMultiplication

CC = mpicc

SRC_DIR = src

OBJ_DIR = obj

INC_DIR = include

DATA_DIR=data

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS += -I$(INC_DIR)

CFLAGS += -Wall -std=c99

LDFLAGS += -Llib

LDLIBS += -lm

# Default behavior of make
all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

.PHONY: all clean