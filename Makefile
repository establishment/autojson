installlib:
	mkdir -p /usr/local/include/autojson_src
	cp ./src/lib/*.cpp /usr/local/include/autojson_src/
	cp ./src/lib/*.hpp /usr/local/include/autojson_src/
	cp ./src/lib/json /usr/local/include/json

install: installlib

cleanlib:
	rm -rf /usr/local/include/autojson_src
	rm /usr/local/include/autojson

clean: cleanlib
