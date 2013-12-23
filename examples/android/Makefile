all: JniExample.h JniComplexExample.h
	ndk-build

%.h: %.class
	javah $(patsubst %.class, %, $<)

%.class: ../%.java
	javac -d . $<

clean:
	rm -f *.class *.h *.o
	rm -rf obj libs
