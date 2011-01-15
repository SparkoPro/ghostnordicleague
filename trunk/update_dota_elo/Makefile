SHELL = /bin/sh
SYSTEM = $(shell uname)
C++ = g++
DFLAGS =
OFLAGS = -O3
LFLAGS = -lmysqlclient
CFLAGS =

ifeq ($(SYSTEM),Darwin)
DFLAGS += -D__APPLE__
OFLAGS += -flat_namespace
endif

ifeq ($(SYSTEM),FreeBSD)
DFLAGS += -D__FREEBSD__
endif

ifeq ($(SYSTEM),SunOS)
DFLAGS += -D__SOLARIS__
LFLAGS += -lresolv -lsocket -lnsl
endif

CFLAGS += $(OFLAGS) $(DFLAGS) -I. -I../ghost/

GHOSTOBJS = config.o
OBJS = elo.o update_dota_elo.o
DOBJS = update_dota_decay.o
PROGS = ./update_dota_elo
DPROGS = ./update_dota_decay

all: $(GHOSTOBJS) $(OBJS) $(DOBJS) $(PROGS) $(DPROGS)

./update_dota_elo: $(GHOSTOBJS) $(OBJS) $(COBJS)
	$(C++) -o ./update_dota_elo $(GHOSTOBJS) $(OBJS) $(LFLAGS)

all: $(GHOSTOBJS) $(DOBJS) $(DPROGS)

./update_dota_decay: $(GHOSTOBJS) $(DOBJS) $(COBJS)
	$(C++) -o ./update_dota_decay $(GHOSTOBJS) $(DOBJS) $(LFLAGS)

clean:
	rm -f $(GHOSTOBJS) $(OBJS) $(PROGS)

$(GHOSTOBJS): %.o: ../ghost/%.cpp
	$(C++) -o $@ $(CFLAGS) -c $<

$(OBJS): %.o: %.cpp
	$(C++) -o $@ $(CFLAGS) -c $<

$(DOBJS): %.o: %.cpp
	$(C++) -o $@ $(CFLAGS) -c $<

./update_dota_elo: $(GHOSTOBJS) $(OBJS)

./update_dota_decay: $(GHOSTOBJS) $(DOBJS)

all: $(PROGS) $(DPROGS)
config.o: ../ghost/ghost.h ../ghost/config.h
elo.o: elo.h
update_dota_elo.o: ../ghost/config.h elo.h
config.o: ../ghost/ghost.h ../ghost/config.h
update_dota_decay.o: ../ghost/config.h
