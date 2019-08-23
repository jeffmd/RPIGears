VPATH = src:include:src/gpu:include/gpu:obj
OBJS = $(addprefix obj/, RPIGears.o matrix_math.o gear.o user_options.o window.o print_info.o \
	demo_state.o key_input.o tasks.o scene.o RPi_Logo256.o camera.o \
	xwindow.o xinput.o static_array.o gles3.o fp16.o shaders.o gldebug.o \
	gpu_texture.o gpu_framebuffer.o gpu_shader_unit.o gpu_shader.o \
	gpu_vertex_buffer.o gpu_index_buffer.o gpu_batch.o gpu_uniform_buffer.o \
	gpu_vertex_format.o font.o test_quad.o text.o exit.o window_manager.o)
	
BIN = RPIGears.bin
	
CFLAGS += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -DHAVE_LIBOPENMAX=2 -ftree-vectorize -ftree-vectorizer-verbose=6 -g -Og -pipe -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi -ffast-math -fsingle-precision-constant -mfloat-abi=hard
LDFLAGS += -L/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lrt -lm -lX11 -lXext -lfreetype

INCLUDES+=-Iinclude -Iinclude/gpu -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host -I/opt/vc/include/interface/vmcs_host/linux -I/usr/include/freetype2

CFLAGS +=$(INCLUDES) -MMD -MP

all: obj $(BIN)

obj:
	@mkdir obj

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

obj/%.o: %.c
obj/%.o: %.c %.d
	$(CC) -c $(CFLAGS) $< -o $@

DEPFILES = $(OBJS:.o=.d)	

$(DEPFILES):

list: $(BIN)
	objdump -d -S $(BIN) > list.txt
	
-include $(DEPFILES)

.PHONY: clean

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)
 
