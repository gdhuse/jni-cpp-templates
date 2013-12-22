/*-----------------------------------------------------------------------------
 * This file provides encapsulation for 'jclass'
 *---------------------------------------------------------------------------*/
 
#ifndef _JNI_CLASS_H_INCLUDED_
#define _JNI_CLASS_H_INCLUDED_

#include "jni_declarations.h"

/*-----------------------------------------------------------------------------
 * JNIClass encapsulates a 'jclass' object.
 * Its only purpose is to provide a variety of constructors of the form
 * JNIClass(JNIEnv *, ProtoClass), where ProtoClass can be 'jclass', 
 * 'jobject', or 'const char *' (anything from which a 'jclass' can
 * be constructed.
 * JNIClass also has a casting operator to 'jclass' type, so that a call
 * JNIClass(env, arg) can be used in any place where 'jclass' is required.
 *---------------------------------------------------------------------------*/
class JNIClass {
   jclass _clazz;	// class handle
   
public:
   // Constructors of the form JNIClass(JNIEnv *env, T cls),
   // where T can be 'jclass', 'jobject' or 'const char *'.
   JNIClass(JNIEnv *env, jclass clazz) : _clazz(clazz) {}
   JNIClass(JNIEnv *env, jobject obj) : _clazz(env->GetObjectClass(obj)) {
	  if (_clazz == 0)
		 throw JNIException("Failed to get a class");
   }
   JNIClass(JNIEnv *env, const char *name) : _clazz(env->FindClass(name)) {
	  if (_clazz == 0)
		 throw JNIException("Failed to get a class");
   }

   // construct JNIClass from a class
   JNIClass(jclass clazz) : _clazz(clazz) {}

   // Casting operators
   operator jclass() { return _clazz; }
   operator const jclass() const { return _clazz; }
};
   
#endif /* _JNI_CLASS_H_INCLUDED_ */
