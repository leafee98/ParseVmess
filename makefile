.PHONY : help
help :
	@echo "    gitSubModule         update submodule of git"
	@echo "    dependency           copy dependency from submodule"
	@echo "    build                build the project"
	@echo "    install              install the binary file and template file"
	@echo "    all                  build and install the project"
	@echo "    clean                clean files created by build"

.PHONY : gitSubModule
gitSubModule :
	git submodule init
	git submodule update

.PHONY : dependency
dependency : gitSubModule
	make --directory=./3rdparty/myBase64/ all
	[ -d ./src/lib/ ] || mkdir -p ./src/lib
	cp -f ./3rdparty/myBase64/out/libmyBase64.a ./src/lib/libmyBase64.a
	[ -d ./src/include/ ] || mkdir -p ./src/include/
	cp -f ./3rdparty/myBase64/src/myBase64.h ./src/include/myBase64/myBase64.h

.PHONY : clean
clean :
	make --directory=./3rdparty/myBase64/ clean
	rm -rf ./out/

.PHONY : build
build : dependency
	[ -d ./out/ ] || mkdir ./out/
	g++ --std=c++17 ./src/main.cpp -o ./out/parseVmess -L./src/lib -lmyBase64 -ljsoncpp

.PHONY : install
install : build
	install ./out/parseVmess /usr/bin/
	[ -f /etc/v2ray/config_template.json ] || install ./config_template.json /etc/v2ray/

.PHONY : all
all : build install

.PHONY : uninstall
uninstall :
	rm /usr/bin/parseVmess
	rm /etc/v2ray/config_template.json
