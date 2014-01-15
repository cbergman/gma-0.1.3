CC=	gcc
CFLAGS=	-g -Wall -DNERSC
CFLAGS=	-g -Wall 
LIBS=	-lz -lbam -lm 	
OBJS_MAP=	main_mapper.o genfa.o mapper.o genfq.o runall.o tech_map.o
OBJS_RED=	main_reducer.o reducer.o prob_cal.o tech_red.o
#OBJS_NULLRED=	main_reducer.o null_reducer.o 
LDFLAGS=	-L../lib

all: main-build post-build

main-build: mapper reducer 

post-build:	
	@echo "----------------------------------------------"
	mv mapper reducer ../bin
	@echo "New gma executable files are available in bin/"
	@echo "----------------------------------------------"

%.o:	%.c
	$(CC) -c $(CFLAGS) $< -o $@ 

mapper: $(OBJS_MAP)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS_MAP) -o $@ $(LIBS)

reducer: $(OBJS_RED)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS_RED) -o $@ $(LIBS)

#null_reducer: $(OBJS_NULLRED)
#	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS_NULLRED) -o $@ $(LIBS)
clean:
	rm -f *.o mapper reducer 

