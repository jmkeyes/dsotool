
SRCDIR  := .
OBJECTS := $(patsubst %.c,%.o,$(wildcard $(SRCDIR)/*.c))
OUTPUT  := dsotool.so

CFLAGS  += -fPIC
LDFLAGS += -shared

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) -f $(OBJECTS) $(OUTPUT)

all: $(OUTPUT)
