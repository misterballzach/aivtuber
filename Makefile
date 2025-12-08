CXX = g++
CXXFLAGS = -std=c++17 -Wall -Ibattle_engine/include

SRC = battle_engine/main.cpp battle_engine/src/Implementation.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = battle_engine/run_battle

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
