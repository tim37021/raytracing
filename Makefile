EXEC = raytracing
.PHONY: all
all: $(EXEC)

CC ?= gcc
CFLAGS = \
	-std=c99 -Wall -O0 -g
LDFLAGS = \
	-lm
	
ifeq ($(PROFILE),1)
PROF_FLAGS = -pg

CFLAGS += $(PROF_FLAGS)
LDFLAGS += $(PROF_FLAGS) 
endif

OBJS := \
	objects.o \
	raytracing.o \
	primitives.o \
	main.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

main.c: use-models.h
use-models.h: models.inc Makefile
	@echo '#include "models.inc"' > use-models.h
	@egrep "^(sphere|rectangle|triangle) " models.inc | \
		sed -e 's/^sphere //g' \
			-e 's/sphere[0-9]/&.vt=\&vt_sphere;/g' \
			-e 's/^rectangle //g'\
			-e 's/rectangle[0-9]/&.vt=\&vt_rectangle;/g' \
			-e 's/^triangle //g'\
			-e 's/triangle[0-9]/&.vt=\&vt_triangle;/g' \
			-e 's/ = {//g' >> use-models.h
	@egrep "^(light|sphere|rectangle|triangle) " models.inc | \
	    sed -e 's/^light /append_light/g' \
	        -e 's/light[0-9]/(\&&, \&lights);/g' \
	        -e 's/^sphere /append_object/g' \
	        -e 's/sphere[0-9]/((const object *)\&&, \&objects);/g' \
	        -e 's/^rectangle /append_object/g' \
	        -e 's/rectangle[0-9]/((const object *)\&&, \&objects);/g' \
	        -e 's/^triangle /append_object/g' \
	        -e 's/triangle[0-9]/((const object *)\&&, \&objects);/g' \
	        -e 's/ = {//g' >> use-models.h

clean:
	$(RM) $(EXEC) $(OBJS) use-models.h \
		out.ppm gmon.out
