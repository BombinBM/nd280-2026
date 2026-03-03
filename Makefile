# Компилятор и флаги

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -g
ROOTFLAGS := $(shell root-config --cflags)
ROOTLIBS := $(shell root-config --libs)

# Директории

SRCDIR := src
INCDIR := inc
BUILDDIR := build
TARGET := bin/project

# Исходные файлы

SOURCES := $(wildcard $(SRCDIR)/*.cpp) main.cpp
OBJECTS := $(patsubst %.cpp, $(BUILDDIR)/%.o, $(notdir $(SOURCES)))
DEPENDS := $(OBJECTS:.o=.d)

# Включаемые директории с библиотеками

INCLUDES := -I$(INCDIR)

# Объединение всех флагов
CXXFLAGS += $(ROOTFLAGS) $(INCLUDES)
LDFLAGS := $(ROOTLIBS)

# Цели по умолчанию

.PHONY: all clean run info dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(dir $(TARGET))

# Линковка исполняемого файла
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "Done!"

# Компиляция с автоматическим отслеживанием зависимостей
$(BUILDDIR)/%.o: %.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Подключение зависимостей
-include $(DEPENDS)

# Очистка
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILDDIR) $(TARGET)
	@echo "Done!"

# Запуск программы
run: all
	@echo "Running $(TARGET)..."
	@$(TARGET)

# Информация о конфигурации
info:
	@echo "Project Configuration:"
	@echo "  CXX: $(CXX)"
	@echo "  CXXFLAGS: $(CXXFLAGS)"
	@echo "  ROOT version: $(shell root-config --version)"
	@echo "  ROOT flags: $(ROOTFLAGS)"
	@echo "  ROOT libs: $(ROOTLIBS)"
	@echo "  Source files: $(SOURCES)"
	@echo "  Target: $(TARGET)"

#  Помощь
help:
	@echo "Available targets:"
	@echo "  all    - Build the program (default)"
	@echo "  run    - Build and run the program"
	@echo "  clean  - Remove build files"
	@echo "  info   - Show configuration"
	@echo "  help   - Show this help"

# Файлы, не являющиеся целями
.PRECIOUS: $(BUILDDIR)/%.o