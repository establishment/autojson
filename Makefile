installlib:
	sudo mkdir -p /usr/local/include/autojson_src
	sudo cp ./src/lib/*.cpp /usr/local/include/autojson_src/
	sudo cp ./src/lib/*.hpp /usr/local/include/autojson_src/
	sudo cp ./src/lib/json /usr/local/include/json


installbin:
	sudo g++ -std=c++14 -O2 ./src/bin/bin.cpp -o /usr/local/bin/json-maker

install: installlib installbin

cleanlib:
	sudo rm -rf /usr/local/include/autojson_src
	sudo rm /usr/local/include/json

cleanbin:
	sudo rm -rf /usr/local/bin/json-maker

clean: cleanlib cleanbin
