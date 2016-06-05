NAME = miro

all: $(NAME)


SOURCES=
OBJS=

include Makedefs
-include .deps/*.d

CFLAGS += -std=c++14

$(NAME): $(OBJS)
	$(ECHO) "Linking $@..."
	$(CC) -o $@ *.o $(LIBS) $(LIBDIRS) #$(OBJS) 
	$(ECHO) "Built $@!"

