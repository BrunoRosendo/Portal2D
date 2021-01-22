# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c loop.c game.c graphics.c kbc.c menu.c mouse.c timer.c utils.c sprites.c asprite.c rtc.c font.c uart.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -D __LCOM_OPTIMIZED__

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
