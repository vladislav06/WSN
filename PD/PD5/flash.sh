if [ "$1" != 'transmitter' ] && [ "$1" != 'receiver' ]; then
    echo "Target must pe specified: transmitter or receiver"
    exit
fi
sudo docker run --device=/dev/ttyUSB0 -v ${PWD}:/srv/repo -e MOSROOT='/srv/mans_os' -e TARG=$1 mansos-telosb bash -c "cd /srv/repo && make telosb upload"

