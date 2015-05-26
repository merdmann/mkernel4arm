CPU=stm32f
toolchain=arm-none-eabi
board=disco
app=myapp


target=../trunk/cpus/$(CPU)
toolchain_def=../trunk/toolchains/$(toolchain).txt

all clean :: tools $(CPU)
	$(MAKE) -C./tools $@
	$(MAKE) -C./$(CPU) $@

tools :
	mkdir -p tools
	( cd tools && cmake -G "Unix Makefiles"  ../trunk/tools )

$(CPU) :
	mkdir -p $(CPU)
	( cd $(CPU) && cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$(toolchain_def) ../trunk/target )

clean ::
	rm -rf build-$(CPU) tools $(CPU) bin
