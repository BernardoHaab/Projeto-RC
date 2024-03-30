MAKEFLAGS += --jobs=4 --output-sync=target

ARCHIVE          = serviçonoticias-pl8-BernardoHaab-LuísGóis.zip
INCLUDE_DIR      = $(PWD)/include
OBJ_DIR          = obj
SRC_DIR          = src
TARGETS          = class_server class_client
GNS3_DIR         = project-files
GNS3_DOCKER_DIR := $(GNS3_DIR)/docker

HEADERS  = $(shell find $(INCLUDE_DIR) \
	   -name "*.h" -o \
	   -name "*.hpp" \
	   | tr '\n' ' ')

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror
CFLAGS += -Wno-error=unused-parameter
CFLAGS += -Wno-error=unused-variable
CFLAGS += -Wno-error=unused-but-set-variable
CFLAGS += -I$(INCLUDE_DIR)
LINKS   =

all: $(TARGETS)

debug: CFLAGS += -g -DDEBUG=1
debug: clean all

$(OBJ_DIR)/%.c.o: %.c $(HEADERS)
	@printf "\n\033[31m"
	@printf "########################\n"
	@printf "Building %s\n" $@
	@printf "########################\n"
	@printf "\033[0m\n"
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -c -o $@ $<

SOURCES = server tcp/socket tcp/server udp/socket udp/server command admin class debug utils
class_server: $(SOURCES:%=$(OBJ_DIR)/$(SRC_DIR)/%.c.o)
	$(CC) $(CFLAGS) -o $@ $^

SOURCES = client tcp/socket tcp/client debug utils
class_client: $(SOURCES:%=$(OBJ_DIR)/$(SRC_DIR)/%.c.o)
	$(CC) $(CFLAGS) -o $@ $^

install-files: assets/users.txt
	for dir in $(shell find $(GNS3_DOCKER_DIR) -maxdepth 1 -type d) ; do \
		sudo install --owner=root --group=root --mode=0644 --verbose -D $< "$$dir"/home/$< ; \
		done

install-exe: $(TARGETS)
	for dir in $(shell find $(GNS3_DOCKER_DIR) -maxdepth 1 -type d) ; do \
		sudo install --owner=root --group=root --mode=0755 --verbose $< "$$dir"/home/ ; \
		done

install: install-exe install-files

$(OBJ_DIR)/%.md: %.md
	mkdir --parents `dirname $@`
	cp $< $@
	sed -i 's|date: date|date: '$(shell date +'%d/%m/%Y')'|' $@

relatorio.pdf: $(OBJ_DIR)/docs/relatorio.md
	pandoc --standalone --resource-path=assets --output $@ $<

archive: $(ARCHIVE)

.PHONY: $(ARCHIVE)
$(ARCHIVE): relatorio.pdf
	git archive --output=$@ --add-file=$< HEAD

setup: .clangd

.PHONY: .clangd
.clangd:
	rm --force $@

	@echo Diagnostics: | tee --append $@
	@echo '  UnusedIncludes: Strict' | tee --append $@
	@echo '  MissingIncludes: Strict' | tee --append $@
	@echo CompileFlags: | tee --append $@
	@echo '  Add:' | tee --append $@

	@for flag in $(CFLAGS) ; do echo "    - $$flag" | tee --append $@ ; done

.PHONY: clean
clean:
	@printf "Cleaning\n"
	rm -f $(TARGETS)
	rm -rf $(OBJ_DIR)

.PHONY: watch
watch:
	@while true ;\
	do \
		clear ; \
		$(MAKE) --no-print-directory; \
		inotifywait --quiet --event modify --recursive $(INCLUDE_DIR) --recursive $(SRC_DIR); \
	done

.PHONY: PRINT-MACROS
PRINT-MACROS:
	@make -p MAKE-MACROS| grep -A1 "^# makefile" | grep -v "^#\|^--" | sort
