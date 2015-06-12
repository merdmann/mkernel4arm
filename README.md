This is a port of mkernel to the ARM architecture.


Starting
========

Simply clone this repository into some suitable place on your system. Before 
you can start you should have installed or know where the following tools
are installed on your system.

1. libopencm3
2. openocd
3. GNU ARM compiler
4. CMake

In order to start pls do the following

$ cd trunk
$ cp site.config.template site.config
.. udpate the file site.config ...

The file site.config is expected to be ignored by git.

The ARM compiler needs to be in your PATH since cmake will try to detect the 
compiler.

In order to compile the package change into the root directory of the package,
this is the place where you find this file as well, and do the following:

$ make 

This will create the build files and compile all.

TODO
====
Currently a default application is compiled and flashed immediately. Since you
might want to do this automatically this has to be changed.

