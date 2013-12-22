/*-----------------------------------------------------------------------------
 * Resource management for JNI is provided using the C++ "construction as
 * resource acquisition" idiom: resources are allocated in the constructor, 
 * and are released in the destructor of dedicated auxiliary objects.
 * This idiom is implemented using the Proxy pattern, with functionality 
 * similar to that of 'auto_ptr'.
 *---------------------------------------------------------------------------*/

#ifndef _JNI_RESOURCE_BASE_H_INCLUDED_
#define _JNI_RESOURCE_BASE_H_INCLUDED_

#include "jni_declarations.h"
#include "jni_env.h"

/*-----------------------------------------------------------------------------
 * JNIResource template: general resource management
 *
 * The template parameter must provide the following four definitions:
 * - JResource type: the original Java resource type (e.g., 'jintArray')
 * - Resource type: the corresponding exported resource (e.g., 'jint *')
 * - GetF:           a functional object, which provides resource allocation 
 *                   of the form
 *                     Resource GetF::operator()(JNIEnv *, JResource)
 * - ReleaseF:       a functional object, which provides resource deallocation 
 *                   of the form
 *                     void ReleaseF::operator()(JNIEnv *, JResource, Resource)
 *
 * The 'GetF' and 'ReleaseF' functional objects supply default allocation and 
 * deallocation facilities. In general, the JNI allows customized resource 
 * management (i.e., the 'isCopy' parameter for allocation and the 'mode' 
 * parameter for deallocation). This behavios could be achieved by supplying 
 * a user-defined functional object as an additional parameter to the
 * constructor (in case of allocation), or to function 'ReleaseResource'
 * (to be used prior to destruction for explicit customized deallocation).
 *
 * JNIResource functionality:
 * - Constructors, which use functional objects of the form
 *     Resource GetF::operator()(JNIEnv *, JResource)
 * - Destructor/ReleaseResource, which use functional objects of the form
 *     void ReleaseF::operator()(JNIEnv *, JResource, Resource)
 * - smart pointer functionality: op=, casting operator, get()/release()
 *-------------------------------------------------------------------------*/
 
template<class JNIResourceSettings>
class JNIResource {
   typedef JNIResource<JNIResourceSettings> _self;
   typedef typename JNIResourceSettings::JResource JResource;
   typedef typename JNIResourceSettings::Resource Resource;
   typedef typename JNIResourceSettings::GetF DefaultGetF;
   typedef typename JNIResourceSettings::ReleaseF DefaultReleaseF;

private:
   bool _owns;		        // true if the current object owns the resource
   
protected:
   JavaVM *_vm;	   		// Java environment handle
   JResource _jresource;	// Java resource handle
   Resource _resource;		// exported resource handle

public:

   // Default constructor (to allow arrays of resources)
   JNIResource() : _vm(0), _jresource(0), _resource(0), _owns(0) {}

   // Constructors use a functional object of the form
   //   Resource GetF::operator()(JNIEnv *, JResource)
   // for resource allocation (by default, DefaultGetF())

   JNIResource(JNIEnv *env, JResource jresource) :
      _owns(true), _jresource(jresource) {
      env->GetJavaVM(&_vm);
      _resource = DefaultGetF()(env, _jresource);
   }
   
   template<class GetF>
   JNIResource(JNIEnv *env, JResource jresource, GetF &getF) :
      _owns(true), _jresource(jresource) {
      env->GetJavaVM(&_vm);
      _resource = getF(env, _jresource);
   }
   
   template<class GetF>
   JNIResource(JNIEnv *env, JResource jresource, const GetF &getF) :
      _owns(true), _jresource(jresource) {
      env->GetJavaVM(&_vm);
      _resource = getF(env, _jresource);
   }

   // Copy constructor
   JNIResource(const _self &x) :
	  _vm(x._vm), _owns(x._owns),
	  _jresource(x._jresource), _resource(x.release())
   {}
   
   // Assignment operator
   _self &operator= (const _self &x) {
	  if (&x != this) {
		 if (_owns)
			ReleaseResource();
      
		 _vm = x._vm;
		 _owns = x._owns;
		 _jresource = x._jresource;
		 _resource = x.release();
      }
	  
      return *this;
   }

   // Destructor: calls the default ReleaseResource()
   ~JNIResource() {
     try {
        JNIEnvironment env(_vm);
        ReleaseResource(env); 
     }
     catch(JNIException &e) {
         // Unable to release resources, most likely because the JVM has exited
     }     
   }

   // ReleaseResource methods use a functional object of the form
   //   void ReleaseF::operator()(JNIEnv *, JResource, Resource)
   // for resource deallocation (by default, DefaultReleaseF())
   
   void ReleaseResource(JNIEnv *env) {
      if (_owns) {
		 DefaultReleaseF()(env, _jresource, release());
	  }
   }
   
   template<class ReleaseF>
   void ReleaseResource(JNIEnv *env, ReleaseF &releaseF) {
      if (_owns) {
		 releaseF(env, _jresource, release());
	  }
   }
   
   template<class ReleaseF>
   void ReleaseResource(JNIEnv *env, const ReleaseF &releaseF) {
      if (_owns) {
		 releaseF(env, _jresource, release());
	  }
   }
   
   // casting operators
   operator Resource() { return get(); }
   operator const Resource() const { return get(); }
   
   // function get() returns the resource
   Resource &get() { return _resource; }
   const Resource &get() const { return _resource; }

   // function release() clears the resource handle,
   // and returns the resource itselfs
   Resource release() {
	  _owns = false;
	  Resource tmp = _resource;
	  _resource = 0;
	  return tmp;
   }
};

#endif /* _JNI_RESOURCE_BASE_H_INCLUDED_ */
