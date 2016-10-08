GCC =

SOURCE = source/
BUILD  = build/

TARGET = $(BUILD)db
OBJECTS += $(patsubst $(SOURCE)%.c, $(BUILD)%.o, $(wildcard $(SOURCE)*.c))

all: $(TARGET)

clean:
	rm -r $(BUILD)

$(TARGET): $(OBJECTS)
	$(CC) -Wall -std=c11 -g -o $@ $(OBJECTS)

$(BUILD)%.o: $(SOURCE)%.c $(BUILD)
	$(CC) -Wall -std=c11 -g -c -o $@ $<

$(BUILD):
	mkdir $(BUILD)
