all: ifr

ifr:
	mkdir -p ./build
	g++ main.cpp UEFI.cpp EFI.cpp -o ./build/ifrextractor

clean:
	rm -rf ./build
