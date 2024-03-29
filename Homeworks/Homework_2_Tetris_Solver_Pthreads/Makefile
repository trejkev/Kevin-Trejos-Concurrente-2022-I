APPNAME=tetris_solver_pthreads
DIRS=src
HEADERS=$(wildcard $(DIRS:%=%/*.h))
SOURCES=$(wildcard $(DIRS:%=%/*.c))
OBJECTS=$(SOURCES:%.c=build/%.o)
EXEFILE=bin/$(APPNAME)

CC=cc -std=gnu11
CFLAGS=-Wall -Wextra
LIBS= -pthread 

LINTFILTERS=$\
	-build/c++11,$\
	-build/header_guard,$\
	-build/include_subdir,$\
	-readability/casting,$\
	-runtime/references

debug: CFLAGS += -g
release: CFLAGS += -O3 -DNDEBUG

asan: CFLAGS += -fsanitize=address -fno-omit-frame-pointer
msan: CFLAGS += -fsanitize=memory
msan: CC = clang
tsan: CFLAGS += -fsanitize=thread
ubsan: CFLAGS += -fsanitize=undefined

.SECONDEXPANSION:

default: $(EXEFILE)
debug: $(EXEFILE)
release: $(EXEFILE)
asan: $(EXEFILE)
msan: $(EXEFILE)
tsan: $(EXEFILE)
ubsan: $(EXEFILE)

$(EXEFILE): $(OBJECTS) | $$(@D)/.
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(LIBS2)

build/%.o: %.c | $$(@D)/.
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS) $(LIBS2)

.PRECIOUS: %/.
%/.:
	mkdir -p $(dir $@)

.PHONY: memcheck
memcheck:
	valgrind $(EXEFILE)

.PHONY: helgrind
helgrind:
	valgrind --tool=helgrind --quiet $(EXEFILE)

.PHONY: rebuild
rebuild: clean $(EXEFILE)

.PHONY: lint
lint:
	cpplint --filter=$(LINTFILTERS) $(HEADERS) $(SOURCES)

.PHONY: gitignore
gitignore:
	echo "bin/ build/" | tr " " "\n" > .gitignore

.PHONY: clean
clean:
	rm -rf bin/ build/