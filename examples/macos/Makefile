CC = clang++
CFLAGS = -I. \
		 -I../../include \
	     -I/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers

all: JniExample JniComplexExample
	java -Xcheck:jni JniExample
	java -Xcheck:jni JniComplexExample

JniExample: JniExample.class libjni_example.jnilib

JniComplexExample: JniComplexExample.class libjni_complex_example.jnilib	

libjni_example.jnilib: jni_example.o
	$(CC) -dynamiclib -o $@ $<

libjni_complex_example.jnilib: jni_complex_example.o
	$(CC) -dynamiclib -o $@ $<

jni_example.o: ../jni_example.cpp JniExample.h
	$(CC) -c -o $@ $< $(CFLAGS)

jni_complex_example.o: ../jni_complex_example.cpp JniComplexExample.h
	$(CC) -c -o $@ $< $(CFLAGS)

%.h: %.class
	javah $(patsubst %.class, %, $<)

%.class: ../%.java
	javac -d . $<

clean:
	rm -f *.class *.h *.o *.jnilib
