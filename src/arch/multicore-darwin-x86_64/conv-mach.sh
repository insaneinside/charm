CMK_MACOSX=1
CMK_DEFS=' -D_REENTRANT '

CMK_CPP_CHARM="/usr/bin/cpp -P"
CMK_CPP_C="gcc -arch x86_64 -E"
CMK_CC="gcc -arch x86_64 -Wno-long-double -fPIC -dynamic -fno-common $CMK_DEFS "
CMK_CXX="g++ -arch x86_64 -Wno-long-double -fPIC -dynamic -fno-common $CMK_DEFS "
CMK_CXXPP="g++ -arch x86_64 -x g++ -E "
CMK_LDXX="$CMK_CXX -multiply_defined suppress $CMK_DEFS "
CMK_XIOPTS=""
CMK_QT="generic64"
CMK_LIBS="-lckqt"
CMK_RANLIB="ranlib"

# Assumes GNU fortran compiler:
CMK_CF77="g77"
CMK_CF90="g90"

# setting for shared lib
# need -lstdc++ for c++ reference, and it needs to be put at very last
# of command line.
# Mac environment varaible
test -z "$MACOSX_DEPLOYMENT_TARGET" && export MACOSX_DEPLOYMENT_TARGET=10.5
CMK_SHARED_SUF="dylib"
CMK_LD_SHARED=" -dynamic -dynamiclib -undefined dynamic_lookup "
CMK_LD_SHARED_LIBS="-lstdc++"
CMK_LD_SHARED_ABSOLUTE_PATH=true

