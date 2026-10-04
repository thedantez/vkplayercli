#variables
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -O2
#libs
LDFLAGS = -lcurl -lmpv
#names
TARGET = vk_player
#original files
SRCS = main.cpp network.cpp parser.cpp audio_engine.cpp ui.cpp
#auto .cpp to .o
OBJS = $(SRCS:.cpp=.o)
#target
all: $(TARGET)
#how to build final file
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
#	rm -f *.o
#rename everyone .cpp to .o (obj file)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
