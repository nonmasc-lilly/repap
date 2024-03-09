_build: src/repap.c src/inc/glad.c
	gcc src/repap.c src/inc/glad.c -o build/repap.so -shared -fPIC -lglfw -funsigned-char -g
test: src/test/test.c
	gcc src/test/test.c\
	    src/repap.c\
		src/inc/glad.c\
	    -o src/test/test -lglfw -funsigned-char -g
stest: _build src/test/test.c
	gcc src/test/test.c build/repap.so -funsigned-char -o build/test -g
	build/test

install: _build
	cp build/repap.so /usr/lib/librepap.so
	cp build/repap.h /usr/include/repap.h
	cp build/repap_glad.h /usr/include/repap_glad.h
	cp build/repap_khrplatform.h /usr/include/repap_glad.h
