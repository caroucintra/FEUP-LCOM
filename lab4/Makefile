# name of the program (Minix service)
PROG=lab4

# source code files to be compiled
SRCS = mouse.c timer.c utils.c lab4.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf
CPPFLAGS += -D LAB4

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
