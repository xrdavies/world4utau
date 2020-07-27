.PYONY: build all install clean

BIN=world4utau
EXE=resampler.exe # 临时用来兼容OpenUtau

build:
	gcc -c ./src/*.c -D_DEBBUG
	gcc -o ${BIN} *.o -lfftw3
	cp ${BIN} ${EXE}

clean:
	@rm *.o
	@rm ${BIN}
	@rm -rf ./build