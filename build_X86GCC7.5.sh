#！ /bin/bash

#Usage:
#1: ./build_X86GCC7.5			编译Demo，依次检查并编译lib库
#2：./build_X86GCC7.5 clean		清除Demo目录的产物

cross=X86GCC7.5
lib_build=./Build

#clean
if [ $# -eq 1 ] && [ $1 = "clean" ];then
	make clean
	exit 0
fi

#判断lib库编译目录是否存在
if [ ! -d $lib_build ];then
	echo "$lib_build not exist"
	exit 1
fi

#进入lib库编译目录并依次检查并执行cmake && make && make install
cd $lib_build
if [ ! -d CMakeFiles ];then
	rm -rf *
	cmake -D CROSS_PLATFORM=$cross ..
	if [ $? != 0 ];then
		exit $?
	fi
fi

#if [ ! -d Bin -o ! -f Bin/lib* ];then
#	make clean && make -j9 && make install
#	if [ $? != 0 ];then
#		exit $?
#	fi
#fi
#if [ ! -d include ];then
#	make install
#	if [ $? != 0 ];then
#		exit $?
#	fi
#fi

if [ ! -f libGB28181.a ];then
	make clean && make
fi

#回到当前目录编译Demo
cd -
make CROSS_PLATFORM=$cross
