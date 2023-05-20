set -e

username="yudiazevedo"
remote_path="/home/${username}/02-modbus"
ipaddr="164.41.98.26"
        
rsync \
    -rqptz \
    --progress --delete --delete-excluded \
    --exclude=uart \
    -e 'ssh -p 13508' \
    * \
    "${username}@${ipaddr}:${remote_path}"
