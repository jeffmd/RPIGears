OBJS=RPIGears.o matrix_math.o gear.o user_options.o window.o demo_state.o key_input.o tasks.o RPi_Logo256.o camera.o
BIN=RPIGears.bin
SRC= shaders.c gles1.c gles2.c scene.c
HDR=user_options.h demo_state.h window.h matrix_math.h gear.h key_input.h tasks.h image.h camera.h
CFLAGS+=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lbcm_host -lrt -lm 

INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux 

all: $(BIN)

RPIGears.o: RPIGears.c $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations

RPi_Logo256.o: RPi_Logo256.c image.h
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations

camera.o: camera.c
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations

gear.o: gear.c
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations
	
demo_state.o: demo_state.c demo_state.h gear.h matrix_math.h
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations
	
key_input.o: key_input.c window.h demo_state.h print_info.h camera.h
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations
	
matrix_math.o: matrix_math.c
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations

tasks.o: tasks.c demo_state.h key_input.h
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations

user_options.o: user_options.c 
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations

window.o: window.c 
	$(CC) $(CFLAGS) $(INCLUDES) -g -c -o $@ $< -Wno-deprecated-declarations

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 

.PHONY: clean

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)
 
