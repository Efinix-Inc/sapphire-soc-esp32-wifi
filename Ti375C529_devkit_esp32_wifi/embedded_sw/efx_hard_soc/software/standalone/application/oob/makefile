PROJ_NAME=oob

STANDALONE = ../..

CFLAGS+=-DSMP

SRCS = 	$(wildcard src/*.c) \
		$(wildcard src/*.cpp) \
		$(wildcard src/*.S) \
		${STANDALONE}/common/start.S \
		${STANDALONE}/common/trap.S \
		${STANDALONE}/common/smpInit.S

CFLAGS+=-Isrc
include ${STANDALONE}/common/bsp.mk
include ${STANDALONE}/common/riscv64-unknown-elf.mk
include ${STANDALONE}/common/standalone.mk

LDSCRIPT ?= ${BSP_PATH}/linker/default.ld
