.PHONY : dependency
dependency :
	make --directory=./3rdparty/myBase64/ all
	cp -f ./3rdparty/myBase64/out/libmyBase64.a ./src/lib/libmyBase64.a
	cp -f ./3rdparty/myBase64/src/myBase64.h ./src/include/myBase64/myBase64.h

.PHONY : clean
clean :
	make --directory=./3rdparty/myBase64/ clean

.PHONY : all
all : dependency
	[ -d ./out/ ] || mkdir ./out/
	g++ ./src/main.cpp -o ./out/main.out -L./src/lib -lmyBase64 -ljsoncpp
