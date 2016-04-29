NAME = miro

all: $(NAME)


SOURCES=
OBJS=

include Makedefs
-include .deps/*.d

CFLAGS += -std=c++14 -DNO_OPENGL=1

$(NAME): $(OBJS)
	$(ECHO) "Linking $@..."
	$(CC) -o $@ *.o $(LIBS) $(LIBDIRS) #$(OBJS) 
	$(ECHO) "Built $@!"

