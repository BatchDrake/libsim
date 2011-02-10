CC=gcc
TARGET=libsim.a
DRAW_OBJ=draw/axis.o draw/cpi.o draw/draw.o draw/text.o
PSO_OBJ=pso/pso.o
COMMON_OBJ=common/hook.o common/list.o common/util.o common/vector.o
WBMP_OBJ=wbmp/draw.o wbmp/load.o wbmp/save.o
TESTS_PREFIX=test
TESTS=$(TESTS_PREFIX).cpi $(TESTS_PREFIX).pi $(TESTS_PREFIX).pokemon
TESTS_CFLAGS=-I. -O3 -g -finline-functions
TESTS_LDFLAGS=-lSDL -L. -lsim -lm
SYSTEM_INCLUDE_DIR=/usr/include
SIMINCLUDE=$(SYSTEM_INCLUDE_DIR)/libsim

$(TARGET) : $(DRAW_OBJ) $(PSO_OBJ) $(COMMON_OBJ) $(WBMP_OBJ)
	ar r $(TARGET) $(DRAW_OBJ) $(PSO_OBJ) $(COMMON_OBJ) $(WBMP_OBJ)

tests : $(TESTS)

$(TESTS_PREFIX).%.o : $(TESTS_PREFIX).%.c $(TARGET)
	$(CC) -c $(TESTS_CFLAGS) $<

$(TESTS_PREFIX).% : $(TESTS_PREFIX).%.o
	$(CC) $< -o $@ $(TESTS_LDFLAGS)

# Don't be lazy and include source files also here
$(DRAW_OBJ) :
	make -C draw

$(PSO_OBJ) :
	make -C pso

$(COMMON_OBJ) :
	make -C common

$(WBMP_OBJ) :
	make -C wbmp

clean :
	make -C draw clean
	make -C pso clean
	make -C common clean
	make -C wbmp clean
	rm -Rf $(TESTS) *.o

install : $(TARGET)
	mkdir -p $(SIMINCLUDE)/draw $(SIMINCLUDE)/wbmp $(SIMINCLUDE)/pso $(SIMINCLUDE)/common
	cp draw/*.h   $(SIMINCLUDE)/draw
	cp wbmp/*.h   $(SIMINCLUDE)/wbmp
	cp pso/*.h    $(SIMINCLUDE)/pso
	cp common/*.h $(SIMINCLUDE)/common
	cp libsim.h $(SIMINCLUDE)
	echo '#include <libsim/libsim.h>' > $(SYSTEM_INCLUDE_DIR)/libsim.h
	cp $(TARGET) /usr/lib

