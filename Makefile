.PYONY: build all install clean

BIN=world4utau

build: 
	gcc -c ./src/*.c
	gcc -o ${BIN} *.o -lfftw3

clean:
	@rm *.o
	@rm ${BIN}
	@rm -rf ./build