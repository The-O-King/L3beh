CC := g++ 
CFLAGS := -std=c++17 -Wall -g 
INCLUDE := -I. -I./third_party
LINK := -lglew32 -lopengl32 -lglfw3dll
TARGET := build/bin/output

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard quickstart/*.cpp core/systems/*.cpp core/components/*.cpp core/*.cpp third_party/mikktspace/*.cpp)
# location of intermediate build obj files
OBJDIR := build/intermediate/obj
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.cpp, $(OBJDIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(LINK)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE)

clean:
	rm -r build/bin build/intermediate
	
.PHONY: all clean