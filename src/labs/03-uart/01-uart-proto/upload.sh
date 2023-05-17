set -e

files="*"
final_dir="~/fse/03-uart/01-uart-proto/"
scp -q -r ${files} yudi@ratlas.local:${final_dir}
