# JNI C++ Template Library

This library provides a C++ interface to the [Java Native Interface](http://en.wikipedia.org/wiki/Java_Native_Interface "JNI") with the goal of simplifying usage, enabling more concise code, and avoiding resource management pitfalls.

For a good illustration of the benefits of using this approach, compare [examples/jni\_example.cpp](examples/jni_example.cpp) to the original written in standard JNI [example/jni\_example\_org.cpp](examples/jni_example_org.cpp).

The meat of this library is modified from code written by Evgeniy Gabrilovich and Lev Finkelstein for their article ["JNI - C++ Integration Made Easy", published in the C/C++ Users Journal, 19(1):10-21, January 2001](http://www.cs.technion.ac.il/~gabr/papers/cuj_jni.pdf "article"), and distributed under their original [License](LICENSE.txt).  The unmodified source code can be found [here](https://github.com/gdhuse/jni-cpp-templates/tree/original).


# Usage

The article referenced above includes numerous usage examples.  For code samples, look in the [examples](examples) directory.


# Supported Platforms

### Tested Compilers:
 - clang (5.0)
 - gcc (4.6, 4.8)
 - Should build with any modern C++ compiler

### Tested Platforms:
  - OSX ([example](examples/macos))
  - Android NDK ([example](examples/android))
  - Should build with minor modifications on any platform supporting JDK 1.2+
