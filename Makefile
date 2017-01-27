installlib:
	sudo mkdir -p /usr/local/include/autojson_src
	sudo cp ./src/lib/*.cpp /usr/local/include/autojson_src/
	sudo cp ./src/lib/*.hpp /usr/local/include/autojson_src/
	sudo cp ./src/lib/json /usr/local/include/json

install: installlib

cleanlib:
	sudo rm -rf /usr/local/include/autojson_src
	sudo rm /usr/local/include/json

clean: cleanlib
