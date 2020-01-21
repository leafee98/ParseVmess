.PHONY : help
help :
	@echo "    gitSubModule         update submodule of git"
	@echo "    dependency           copy dependency from submodule"
	@echo "    all                  build the project"
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

.PHONY : all
all : dependency
	[ -d ./out/ ] || mkdir ./out/
	g++ ./src/main.cpp -o ./out/main.out -L./src/lib -lmyBase64 -ljsoncpp
