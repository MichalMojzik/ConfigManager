#
# ConfigManager is library for program interaction with
# configuration files ".ini".
# There are two build options: the library itself
#                              library with tests
# (Unit testing is done in cppTest framework 
#  http://cpptest.sourceforge.net/index.html  )
#

SOURCEDIR=./src/ConfigManager/
INCDIR=./src
CPPTESTDIR=./lib/cpptest-1.1.2/
TESTDIR=./tests/

CC=gcc
STD=-std=c++14
MAXERROR=-fmax-errors=1
LSTD=-lstdc++
OPTIONS=${STD} -I ${INCDIR} ${MAXERROR} ${LSTD}

all: ConfigManager test

typespecifiers.obj:
	${CC} ${SOURCEDIR}typespecifiers.cpp -o typespecifiers.obj ${OPTIONS} -c

option.obj:
	${CC}	${SOURCEDIR}option.cpp -o option.obj ${OPTIONS} -c
	
optionnode.obj:
	${CC} ${SOURCEDIR}optionnode.cpp -o optionnode.obj ${OPTIONS} -c

sectionnode.obj:
	${CC} ${SOURCEDIR}sectionnode.cpp -o sectionnode.obj ${OPTIONS} -c
	
section.obj:
	${CC} ${SOURCEDIR}section.cpp -o section.obj ${OPTIONS} -c

configuration.obj:
	${CC} ${SOURCEDIR}configuration.cpp -o configuration.obj ${OPTIONS} -c

utilities.obj:
	${CC} ${SOURCEDIR}utilities.cpp -o utilities.obj ${OPTIONS} -c



ConfigManager: configuration.obj option.obj section.obj typespecifiers.obj sectionnode.obj optionnode.obj utilities.obj
	ar -rcs ./lib/libconfigmanager.a *.obj
	
cppTest: cppTestConfigure
	cd ${CPPTESTDIR}; make ; make install
	
cppTestConfigure:
	cd ${CPPTESTDIR}; chmod +x configure; ./configure --prefix $(CURDIR)
	
Testing: ConfigManager cppTest 
	${CC} ${TESTDIR}tests.cpp -L ./lib -lconfigmanager -lcpptest -o configmanagerTest.exe ${OPTIONS} -Wl,-R./lib
	
test: Testing
	echo "--------- TESTING ----------"; ./configmanagerTest.exe