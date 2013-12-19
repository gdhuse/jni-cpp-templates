JNI encapsulation files
-----------------------

- C++ code

  * jni_declarations.h       - common declarations
  * jni_class.h              - encapsulation for jclass
  * jni_field.h              - utilities for accessing Java object variables 
                               (C++ regular member variables) and class variables 
                               (C++ static member variables)
  * jni_utils.h              - templates for accessing array regions
  * jni_resource_base.h      - generic resource management for the JNI
  * jni_resource.h           - encapsulation of basic JNI facilities
  * jni_master.h             - master header file, which includes all the files above

  * jni_preprocessor.cpp     - sample preprocessor output for some of the definitions

  * JniExample.h             - automatically generated header file
  * jni_example.cpp          - example native code (serves as the running example
                               in the article; yields 'jni_example.dll')
  * jni_example_org.cpp      - example native code without the proposed template framework

  * jni_complex_example.cpp  - native code for a more elaborate example 
                               (multithreaded generation of random strings, 
                               with subsequent sorting)

  * jni_example.dsp          - example project file for MS VC++ 6.0
  * jni.dsw                  - workspace file for MS VC++ 6.0

- Java code

  * JniExample.java          - the Java part of the running example
                               (contains function 'main' that can invoke both
                               the running example and the more elaborate example)

  * JniComplexExample.java   - the Java part of the more elaborate example

  * JniExample.vjp           - example project file for MS VJ++ 6.0
  * JniExample.sln           - solution file for MS VJ++ 6.0

- readme.txt                 - this file

- LICENSE.txt                - licensing terms
