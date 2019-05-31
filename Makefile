OBJS = RPIGears.o matrix_math.o gear.o user_options.o window.o print_info.o \
	demo_state.o key_input.o tasks.o scene.o RPi_Logo256.o camera.o \
	xwindow.o xinput.o static_array.o gles3.o fp16.o shaders.o gldebug.o \
	gpu_texture.o gpu_framebuffer.o gpu_shader_unit.o gpu_shader.o \
	gpu_vertex_buffer.o gpu_index_buffer.o gpu_batch.o gpu_uniform_buffer.o \
	gpu_vertex_format.o font.o test_quad.o
	
BIN = RPIGears.bin
	
CFLAGS += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -DHAVE_LIBOPENMAX=2 -ftree-vectorize -ftree-vectorizer-verbose=6 -g -Og -pipe -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi -ffast-math -fsingle-precision-constant -mfloat-abi=hard
LDFLAGS += -L$(SDKSTAGE)/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lrt -lm -lX11 -lXext -lfreetype

INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I$(SDKSTAGE)/usr/include/freetype2

CFLAGS+=$(INCLUDES)

all: $(BIN)

RPIGears.o: user_options.h demo_state.h window.h  gear.h tasks.h image.h \
	camera.h xwindow.h gles3.h print_info.h scene.h gpu_texture.h font.h \
	test_quad.h
RPi_Logo256.o: image.h
gear.o: gles3.h gpu_vertex_buffer.h gpu_index_buffer.h gpu_uniform_buffer.h gpu_batch.h
demo_state.o: demo_state.h gear.h matrix_math.h
key_input.o: gles3.h window.h gear.h gpu_texture.h demo_state.h print_info.h \
  user_options.h camera.h shaders.h test_quad.h
matrix_math.o: matrix_math.c
tasks.o: demo_state.h key_input.h
window.o: gles3.h gldebug.h print_info.h gpu_texture.h gpu_framebuffer.h
xwindow.o: xinput.h tasks.h window.h
xinput.o: tasks.h key_input.h
shaders.o: gles3.h gpu_shader.h
gpu_shader_unit.o: gles3.h static_array.h 
gpu_shader.o: gles3.h static_array.h gpu_shader_unit.h
gpu_batch.o: gles3.h static_array.h gpu_vertex_buffer.h gpu_index_buffer.h
gpu_vertex_buffer.o: gles3.h static_array.h gpu_vertex_format.h
gpu_vertex_format.o: gles3.h fp16.h static_array.h gpu_shader.h
gpu_uniform_buffer.o: gles3.h static_array.h gpu_shader.h
gpu_texture.o: gles3.h static_array.h 
scene.o: gles3.h gear.h matrix_math.h camera.h gpu_texture.h demo_state.h user_options.h window.h shaders.h gpu_shader.h gpu_uniform_buffer.h
font.o: gles3.h gpu_texture.h static_array.h
test_quad.o: gles3.h gpu_vertex_buffer.h gpu_index_buffer.h gpu_uniform_buffer.h gpu_batch.h shaders.h


$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 


list: $(BIN)
	objdump -d -S $(BIN) > list.txt
	
.PHONY: clean

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)
 
