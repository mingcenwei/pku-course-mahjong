CXX              := c++

CXXFLAGS         := \
	-std=c++17 \
	-Wall \
	-Wextra \
	-Wconversion \
	-Wsign-conversion \
	-pedantic-errors \
	\
	-Weverything \
	-Wno-c++98-compat \
	-Wno-c++98-compat-pedantic \
	-Wno-covered-switch-default \
	-Wno-unreachable-code-break \
	-Wno-weak-vtables \
	-Wno-global-constructors \
	-Wno-exit-time-destructors \

LDFLAGS          := \
	# -lm \
	# -pthread \

APP_SRC          := \
	$(wildcard src/*.cpp) \
	# $(wildcard src/classes/*.cpp) \
	# $(wildcard src/**/*.cpp) \

TEST_SRC         := \
	$(wildcard test/*.cpp) \
	# $(wildcard src/**/*.cpp) \

APP_TARGET       := mahjong-ai
TEST_TARGET      := test
INCLUDE          := -Iinclude/
INSTALL_DIR      := ./bin


BUILD_DIR        := ./build
APP_BUILD_DIR    := $(BUILD_DIR)/app
APP_OBJ_DIR      := $(APP_BUILD_DIR)/objects
APP_BIN_DIR      := $(APP_BUILD_DIR)/bin
APP_OBJECTS      := $(APP_SRC:%.cpp=$(APP_OBJ_DIR)/%.o)
TEST_BUILD_DIR   := $(BUILD_DIR)/test
TEST_OBJ_DIR     := $(TEST_BUILD_DIR)/objects
TEST_BIN_DIR     := $(TEST_BUILD_DIR)/bin
TEST_OBJECTS     := $(TEST_SRC:%.cpp=$(TEST_OBJ_DIR)/%.o)


$(APP_OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_BIN_DIR)/$(APP_TARGET): $(APP_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(APP_BIN_DIR)/$(APP_TARGET) $(APP_OBJECTS) $(LDFLAGS)

$(TEST_OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(TEST_BIN_DIR)/$(TEST_TARGET): $(TEST_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(TEST_BIN_DIR)/$(TEST_TARGET) $(TEST_OBJECTS) $(LDFLAGS)


.PHONY: all
all: release


.PHONY: build
build: $(APP_BIN_DIR)/$(APP_TARGET)
	@mkdir -p $(APP_BIN_DIR)
	@mkdir -p $(APP_OBJ_DIR)


.PHONY: clean
clean:
	-@rm -rv $(BUILD_DIR)


.PHONY: clean_app
clean_app:
	-@rm -rv $(APP_BUILD_DIR)


.PHONY: clean_test
clean_test:
	-@rm -rv $(TEST_BUILD_DIR)


.PHONY: debug
debug: CXXFLAGS += \
	-DDEBUG_ \
	-g \
	-fPIE \
	-pie \
	\
	-fsanitize=address \
	-fsanitize=thread \
	\
	-fsanitize=memory \
	-fsanitize-memory-track-origins \
	\
	-fsanitize=undefined \
	-fsanitize=float-divide-by-zero \
	-fsanitize=unsigned-integer-overflow \
	-fsanitize=implicit-conversion \
	-fsanitize=nullability \
	\
	-fno-omit-frame-pointer \
	-fno-optimize-sibling-calls \

debug: build


.PHONY: install
install:
	@mkdir -p $(INSTALL_DIR)
	@cp -v $(APP_BIN_DIR)/$(APP_TARGET) $(INSTALL_DIR)


.PHONY: release
release: CXXFLAGS += -O3 -DNDEBUG -g -Werror
release: build


.PHONY: release_without_debug_info
release_without_debug_info: CXXFLAGS += -O3 -DNDEBUG -Werror
release_without_debug_info: build


.PHONY: test
test: CXXFLAGS += \
	-DDEBUG_ \
	-g \
	-fPIE \
	-pie \
	\
	-fsanitize=address \
	-fsanitize=thread \
	\
	-fsanitize=memory \
	-fsanitize-memory-track-origins \
	\
	-fsanitize=undefined \
	-fsanitize=float-divide-by-zero \
	-fsanitize=unsigned-integer-overflow \
	-fsanitize=implicit-conversion \
	-fsanitize=nullability \
	\
	-fno-omit-frame-pointer \
	-fno-optimize-sibling-calls \

test: $(TEST_BIN_DIR)/$(TEST_TARGET)
	@mkdir -p $(TEST_BIN_DIR)
	@mkdir -p $(TEST_OBJ_DIR)


# Help Target
.PHONY: help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... clean_app"
	@echo "... clean_test"
	@echo "... debug"
	@echo "... help"
	@echo "... install"
	@echo "... release"
	@echo "... release_without_debug_info"
	@echo "... test"
