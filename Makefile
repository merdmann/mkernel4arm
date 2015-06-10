CPU=stm32f
toolchain=arm-none-eabi
board=disco
app=myapp

root=$(shell pwd)


target=../trunk/cpus/$(CPU)
toolchain_def=$(root)/trunk/toolchains/$(toolchain).txt

all clean :: build/tools build/myapp
	$(MAKE) -C./build/tools $@
	$(MAKE) -C./build/myapp $@

build/tools :
	mkdir -p build/tools
	( cd build/tools && cmake -G "Unix Makefiles"  $(root)/trunk/tools )

build/myapp :
	mkdir -p build/myapp
	( cd  build/myapp && cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$(toolchain_def) $(root)/trunk/target )

clean ::
	rm -rf build
