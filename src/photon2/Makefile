all: ship monitor

ship: build flash

build:
	particle compile p2

flash:
	particle flash --local
	sleep 5

monitor:
	particle serial monitor --follow