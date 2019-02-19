make -f scripts/Makefile.stat clean
make -f scripts/Makefile.stat
strip -S librose.a
ranlib librose.a

mkdir -p validate/kernel/include
cp kernel/include/* validate/kernel/include

mkdir -p validate/port/x86
cp port/x86/*.h validate/port/x86

mkdir -p validate/lib/include
cp lib/include/* validate/lib/include

mkdir -p validate/apps
cp apps/defines.h validate/apps
cp apps/main.c validate/apps

cp librose.a validate/
cp scripts/Makefile.test validate/Makefile
cp -R testapps validate/
cp testapps/system_clk_10ms validate
cp testapps/system_clk_100ms validate

tar -czvf validate.tar.gz validate
