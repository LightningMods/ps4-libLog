PROJECTNAME 	:= libLog
TOOLCHAIN	:= $(OO_PS4_TOOLCHAIN)

# Libraries linked into the ELF.
LIBS 					:= -lSceLibcInternal -lkernel

# Compiler options. You likely won't need to touch these.
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
		CC  			:= clang++
		LD 			:= ld.lld
		CDIR			:= linux
                ASMLR                   := as
endif
ifeq ($(UNAME_S),Darwin)
		CC 				:= /usr/local/opt/llvm/bin/clang++
		LD 				:= /usr/local/opt/llvm/bin/ld.lld
		CDIR			:= macos
endif
ODIR	 				:= build
SDIR 					:= src
IDIRS 				:= -Iinclude -I$(TOOLCHAIN)/include -I$(TOOLCHAIN)/include/c++/v1
LDIRS 				:= -L$(TOOLCHAIN)/lib
COMPILE_OPTS	:= -D__ORBIS__
CFLAGS 				:= -cc1 -triple x86_64-pc-freebsd-elf -munwind-tables $(IDIRS) -fuse-init-array -debug-info-kind=limited -debugger-tuning=gdb -emit-obj -Wall
LFLAGS 				:= -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr $(LDIRS) $(LIBS) $(TOOLCHAIN)/lib/crtlib.o

CFILES 		:= $(wildcard $(SDIR)/*.c)
CPPFILES 	:= $(wildcard $(SDIR)/*.cpp)
ASMFILES 	:= $(wildcard $(SDIR)/*.s)
OBJS 		:= $(patsubst $(SDIR)/%.s, $(ODIR)/%.o, $(ASMFILES)) $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(CFILES))  $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(CPPFILES))
STUBOBJS 	:= $(patsubst $(SDIR)/%.c, $(ODIR)/%.o.stub, $(CFILES)) $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o.stub, $(CPPFILES))

TARGET 				= $(PROJECTNAME).prx
TARGETSTUB 		= $(PROJECTNAME)_stub.so

# Create the intermediate directory incase it doesn't already exist.
_unused 			:= $(shell mkdir -p $(ODIR))

# Make rules
$(TARGET): $(ODIR) $(OBJS)
	$(LD) $(ODIR)/*.o -o $(ODIR)/$(PROJECTNAME).elf $(LFLAGS)
	$(TOOLCHAIN)/bin/$(CDIR)/create-lib -in=$(ODIR)/$(PROJECTNAME).elf -out=$(ODIR)/$(PROJECTNAME).oelf --paid 0x3800000000000011
	mv $(ODIR)/$(PROJECTNAME).prx $(TARGET)

$(TARGETSTUB): $(ODIR) $(STUBOBJS)
	$(CC) $(ODIR)/*.o.stub -o $(TARGETSTUB) -target x86_64-pc-linux-gnu -shared -fuse-ld=lld -ffreestanding -nostdlib -fno-builtin $(LDIRS) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) $(COMPILE_OPTS) -o $@ $<


$(ODIR)/%.o: $(SDIR)/%.s
	$(ASMLR) -o $@ $<

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) $(COMPILE_OPTS) -o $@ $<

$(ODIR)/%.o.stub: $(SDIR)/%.c
	$(CC) -target x86_64-pc-linux-gnu -ffreestanding -nostdlib -fno-builtin -fPIC -c $(IDIRS) $(COMPILE_OPTS) -o $@ $<

$(ODIR)/%.o.stub: $(SDIR)/%.cpp
	$(CC) -target x86_64-pc-linux-gnu -ffreestanding -nostdlib -fno-builtin -fPIC -c $(IDIRS) $(COMPILE_OPTS) -o $@ $<

$(ODIR):
	@mkdir $@

.PHONY: clean
.DEFAULT_GOAL	:= all

all: $(TARGET) $(TARGETSTUB)

clean:
	rm -rf $(TARGET) $(TARGETSTUB) $(ODIR)

install:
	yes | cp -f $(TARGETSTUB) $(OO_PS4_TOOLCHAIN)/lib/$(TARGETSTUB)