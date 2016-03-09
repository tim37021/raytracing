EXEC = raytracing
LIB_OBJ_PARSER = obj_parser
.PHONY: all
all: $(EXEC)

CC ?= gcc
AR ?= ar
CFLAGS = \
	-std=c99 -Wall -O0 -g
LDFLAGS = \
	-L. -lm -l$(LIB_OBJ_PARSER)
	
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

OBJS_OBJPARSER := \
	obj_parser.o \
	list.o \
	string_extra.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXEC): $(OBJS) lib$(LIB_OBJ_PARSER).a
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 

lib$(LIB_OBJ_PARSER).a: $(OBJS_OBJPARSER)
	$(AR) rcs $@ $^

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
	$(RM) $(EXEC) lib$(LIB_OBJ_PARSER).a $(OBJS) $(OBJS_OBJPARSER) use-models.h \
		out.ppm gmon.out
