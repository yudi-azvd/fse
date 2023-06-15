final_dir="/encoder/"

files="*"
scp -r ${files} pi@rpi3-24.local:~${final_dir}
# scp -r ${files} yudi@ratlas.local:~${final_dir}
