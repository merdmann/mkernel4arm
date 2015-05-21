/c/arm/openocd-0.8.0/bin-x64/openocd-x64-0.8.0.exe --file  interface/stlink-v2.cfg  -c "gdb_memory_map enable"  -c "gdb_flash_program enable"  --file target/stm32f4x_stlink.cfg &
adg bin/myapp.elf

pid=`ps | grep ocd | awk '{ print $1 }'`
if [ "x${pid}" != "x" ] ; then
   echo "Killing ${pid}"
   kill -9 ${pid}
fi 