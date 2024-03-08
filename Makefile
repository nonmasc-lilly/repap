_build: src/repap.c src/inc/glad.c
	gcc src/repap.c src/inc/glad.c -o build/repap.so -shared -fPIC -lglfw -funsigned-char
test: src/test/test.c
	gcc src/test/test.c\
	    src/repap.c\
		src/inc/glad.c\
	    -o src/test/test -lglfw -funsigned-char
	src/test/test
stest: _build src/test/test.c
	gcc src/test/test.c -lrepap -funsigned-char -o build/test
	build/test

install: _build
	cp build/repap.so /usr/lib/librepap.so
	cp build/repap.h /usr/include/repap.h
	cp build/repap_glad.h /usr/include/repap_glad.h
	cp build/repap_khrplatform.h /usr/include/repap_glad.h
