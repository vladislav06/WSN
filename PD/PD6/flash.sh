if [ "$1" != 'sensor' ] && [ "$1" != 'relay' ] && [ "$1" != 'gateway' ]; then
    echo "Target must pe specified: sensor, relay or gateway"
    exit
fi
sudo docker run --device=/dev/ttyUSB0 -v ${PWD}:/srv/repo -e MOSROOT='/srv/mans_os' -e TARG=$1 mansos-telosb bash -c "cd /srv/repo && make telosb upload"

