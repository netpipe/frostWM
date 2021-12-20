noinst_LIBRARIES=libDCLib.a
libDCLib_a_SOURCES=	DCL_InitFileParser.cpp DCL_InitFileParser.h \
					DCL_MemMap.cpp DCL_MemMap.h \
				   	DCL_DualLnkList.cpp DCL_DualLnkList.h \
					DCL_Error.cpp DCL_Error.h \
					DCLib.h DCL_Types.h

bin_PROGRAMS=DCL_Test
DCL_Test_SOURCES=DCL_TestMain.cpp
DCL_Test_LDADD=libDCLib.a
