BAUD=115200

debug_build:
	cd src; scons baud=${BAUD} sdf;	cd ../


sim: debug_build
	echo "hi"