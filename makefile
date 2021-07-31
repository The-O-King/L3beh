CC := g++ 
CFLAGS := -std=c++17 -Wall -g 
INCLUDE := -I. 
LINK := -lglew32 -lopengl32 -lglfw3  -lglfw3dll
TARGET := output

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard quickstart/*.cpp core/systems/*.cpp core/*.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LINK)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE)

clean:
	rm $(OBJS) $(TARGET)
	
.PHONY: all clean