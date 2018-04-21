OBJS=RPIGears.o matrix_math.o gear.o user_options.o window.o demo_state.o key_input.o tasks.o RPi_Logo256.o camera.o xwindow.o xinput.o
BIN=RPIGears.bin
SRC= shaders.c gles1.c gles2.c scene.c
HDR=user_options.h demo_state.h window.h matrix_math.h gear.h key_input.h tasks.h image.h camera.h xwindow.h
CFLAGS+=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -mfpu=neon -ftree-vectorize -ftree-vectorizer-verbose=6 -g -Og -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi 

LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lrt -lm -lX11

INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux 

CFLAGS+=$(INCLUDES)

all: $(BIN)

RPIGears.o: $(SRC) $(HDR)
RPi_Logo256.o: image.h
camera.o:
gear.o:
demo_state.o: demo_state.h gear.h matrix_math.h
key_input.o: key_input.c window.h demo_state.h print_info.h camera.h
matrix_math.o: matrix_math.c
tasks.o: demo_state.h key_input.h
user_options.o:
window.o:
xwindow.o: xinput.h tasks.h window.h
xinput.o: tasks.h


$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 

list: $(BIN)
	objdump -d -S $(BIN) > list.txt
	
.PHONY: clean

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)
 
