/*-----------------------------------------------------------------------------
 * Example: given a Java object of type 'JniExample',
 *          first print its fields, then modify them.
 *---------------------------------------------------------------------------*/
 
#include <iostream>
#include <exception>

#include <jni.h>
#include <stdlib.h>
#include <stdlib.h>
#include <windows.h>      // for DLL construction

#include "JniExample.h"
#include "jni_declarations.h"  // for JNIException class

using namespace std;

// utility for fieldId validation
inline void validateFieldID(const jfieldID &id) {
   if (id == 0)
	  throw JNIException("Field not found");
}
	  
JNIEXPORT void JNICALL Java_JniExample_org_1native_1call
  (JNIEnv *env, jclass clazz, jobject obj)
{
   try {
	  // Lookup the integer field ('intField') in 'obj' and get its value
	  jfieldID intFieldId = env->GetFieldID(clazz, "intField", "I");
	  validateFieldID(intFieldId);
	  int intFieldVal = env->GetIntField(obj, intFieldId);

 	  // Lookup the static String field ('stringField') in 'obj',
	  // then convert the Java string representation to
	  // C++ type 'const char *'.
	  jfieldID strFieldId = env->GetStaticFieldID(clazz, "stringField",
												  "Ljava/lang/String;");
	  validateFieldID(strFieldId);
	  jstring jstr = (jstring) env->GetStaticObjectField(clazz, strFieldId);
	  // Warning: the following line makes the code not exception-safe,
      // since if an exception is thrown later in the code, the memory
      // pointed at by 'str' will not be released properly.
	  const char *str = (jstr == 0) ? 0 : env->GetStringUTFChars(jstr, 0);

	  // Lookup the integer array field ('intArray') in 'obj',
	  // then convert it to C++ type 'jint *'.
	  jfieldID arrFieldId = env->GetFieldID(clazz, "intArray", "[I");
	  validateFieldID(arrFieldId);
	  jintArray jarr = (jintArray) env->GetObjectField(obj, arrFieldId);
	  // Warning: the following line makes the code not exception-safe,
      // since if an exception is thrown later in the code, the memory
      // pointed at by 'arr' will not be released properly.
	  jint *arr = env->GetIntArrayElements(jarr, 0);

	  // Print old values
	  cout << "In native call:" << endl
		   << "  intField = " << intFieldVal
		   << ", stringField = " << str << endl
		   << "  intArray[0] = " << arr[0]
		   << ", intArray[1] = " << arr[1] << endl;

	  // Set new values
	  env->SetIntField(obj, intFieldId, 0);
	  arr[0] = 0;
	  arr[1] = 0;

	  env->SetStaticObjectField(clazz, strFieldId,
								env->NewStringUTF("Good-bye, world!"));

	  // Explicitely release resources
	  env->ReleaseIntArrayElements(jarr, arr, 0);
	  env->ReleaseStringUTFChars(jstr, str);
   }
   catch (std::exception &e) {
	  cerr << "Exception: " << e.what() << endl;
   }
   catch (...) {
	  cerr << "Unknown exception" << endl;
   }
}
