/*-----------------------------------------------------------------------------
 * This file provides encapsulation for 'JNIEnv'
 *---------------------------------------------------------------------------*/
 
#ifndef _JNI_ENV_H_INCLUDED_
#define _JNI_ENV_H_INCLUDED_

#include "jni_declarations.h"

#define JNI_VERSION JNI_VERSION_1_2

/*-----------------------------------------------------------------------------
 * JNIEnvironment encapsulates a JNIEnv
 * JNIEnv* are only allowed to be accessed by their owning thread and should
 * not be saved in member variables.  Instead, a JavaVM* can be saved,
 * and a new loca JNIEnv* can be associated with the VM when needed
 *---------------------------------------------------------------------------*/
class JNIEnvironment {
   JavaVM *_vm;    // Java virtual machine
   JNIEnv *_env;   // Encapsulated JNI environment
   bool _attached; // Attached to the current thread
   
public:
   JNIEnvironment(JavaVM *vm) : _vm(vm), _attached(false) {
      int state = vm->GetEnv((void **)&_env, JNI_VERSION);
      if(state == JNI_EDETACHED) {
         if(vm->AttachCurrentThread((void **)&_env, NULL) != 0) {
            throw JNIException("Failed to attach JNIEnv to Java VM");
         }
         else {
            _attached = true;
         }
      }
      else if(state == JNI_EVERSION) {
         throw JNIException("JNI version not supported");
      }
   }

   ~JNIEnvironment() {
      if(_attached) {
         _vm->DetachCurrentThread();
      }
   }

   JNIEnv* Get() { return _env; }

   // Casting operators
   operator JNIEnv*() { return _env; }
   operator const JNIEnv*() const { return _env; }
};
   
#endif /* _JNI_ENV_H_INCLUDED_ */
