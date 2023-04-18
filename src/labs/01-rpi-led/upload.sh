final_dir="/fse/01-rpi"

files="*.py *.c"
scp -r ${files}  pi@rpi3-06.local:~${final_dir}
