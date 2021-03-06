CMK_DEFS="$CMK_DEFS -D_REENTRANT -m32"
CMK_LIBS="-lpthread $CMK_LIBS "

CMK_CPP_CHARM="/lib/cpp -P"
CMK_CPP_C="gcc -E $CMK_DEFS "
CMK_CC="gcc -fPIC $CMK_DEFS "
CMK_CXX="g++ -fPIC $CMK_DEFS "
CMK_CXXPP="$CMK_CXX -x c++ -E $CMK_DEFS "
CMK_XIOPTS=""
CMK_LD="$CMK_CC $CMK_DEFS "
CMK_LDXX="$CMK_CXX $CMK_DEFS "
CMK_LD_SHARED="-shared"
CMK_LD_LIBRARY_PATH="-Wl,-rpath,$CHARMLIBSO/"
CMK_LIBS="$CMK_LIBS -lckqt"
CMK_RANLIB="ranlib"

# native compiler for compiling charmxi, etc
CMK_NATIVE_CC="$CMK_CC $CMK_DEFS -fPIC "
CMK_NATIVE_CXX="$CMK_CXX $CMK_DEFS -fPIC "
CMK_NATIVE_LD="$CMK_CC $CMK_DEFS -fPIC "
CMK_NATIVE_LDXX="$CMK_CXX $CMK_DEFS -fPIC "

# native compiler for compiling charmxi, etc
CMK_SEQ_CC="$CMK_NATIVE_CC"
CMK_SEQ_CXX="$CMK_NATIVE_CXX"
CMK_SEQ_LD="$CMK_NATIVE_LD"
CMK_SEQ_LDXX="$CMK_NATIVE_LDXX"

CMK_CF77="g77 "
CMK_CF90="f90 "
CMK_CF90_FIXED="$CMK_CF90 -W132 "
CMK_F90LIBS="-L/usr/absoft/lib -L/opt/absoft/lib -lf90math -lfio -lU77 -lf77math "
CMK_F77LIBS="-lg2c "
CMK_F90_USE_MODDIR=1
CMK_F90_MODINC="-p"

CMK_QT='generic64'

