/*-----------------------------------------------------------------------------
 * This file provides encapsulation of basic JNI facilities.
 *---------------------------------------------------------------------------*/

#ifndef _JNI_RESOURCE_H_INCLUDED_
#define _JNI_RESOURCE_H_INCLUDED_

#include <string>
using std::string;

#include "jni_declarations.h"
#include "jni_resource_base.h"

/*-----------------------------------------------------------------------------
 * Auxiliary utilities for resource construction
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * ObjectTag<T> helps to determine whether T is a 'jobject' or not.
 * ObjectTag<T>::tag() returns object_tag() if T is a 'jobject',
 * and non_object_tag() for native types ('jint', etc.).
 *---------------------------------------------------------------------------*/
struct object_tag {};
struct non_object_tag {};

template<class T>
struct ObjectTag {
   typedef non_object_tag tag;
};

template<> struct ObjectTag<jobject> {
   typedef object_tag tag;
};

/*-----------------------------------------------------------------------------
 * GetJResource class provides an interface for easy resource construction. 
 * 
 * The class has two template memeber functions, which can be invoked
 * the following three ways (the optional boolean parameter indicates
 * whether the requested resource is a static field of the Java class):
 *    GetJResource()(JNIEnv *, jobject, const char * [, bool])
 *    GetJResource()(JNIEnv *, jclass, const char * [, bool])
 *    GetJResource()(JNIEnv *, const char *, const char * [, bool])
 *
 * Calls to
 *    GetJResource()(JNIEnv *, jobject, const char *, false)
 *    GetJResource()(JNIEnv *, jobject, const char *)
 *  attempt to access a non-static field.
 * Calls to
 *    GetJResource()(JNIEnv *, jobject, const char *, true)
 *    GetJResource()(JNIEnv *, const char *, const char *)
 *    GetJResource()(JNIEnv *, const char *, const char *, true)
 *  attempt to access a static field.
 * Finally, a call to
 *    GetJResource()(JNIEnv *, const char *, const char *, false)
 * generates an exception.
 * 
 * Note, that since 'jclass' is a subclass of 'jobject',
 *    GetJResource()(JNIEnv *, jclass, const char *)
 * attempts to access the NON-STATIC field of 'jclass' 
 * as if it were a simple object.
 *---------------------------------------------------------------------------*/
template<class JavaType>
class GetJResource {
   JavaType operator() (JNIEnv *env, jobject obj, const char *name,
						const object_tag &) const {
	  return JNIField<JavaType>(env, obj, name);
   }
   
   JavaType operator() (JNIEnv *env, jobject obj, const char *name,
						const object_tag &, bool isStatic) const {
	  if (isStatic)
		 return JNIStaticField<JavaType>(env, obj, name);
	  else
		 return JNIField<JavaType>(env, obj, name);
   }

   template<class T> 
   JavaType operator() (JNIEnv *env, T protoClass, const char *name,
						const non_object_tag &) const {
	  return JNIStaticField<JavaType>(env, protoClass, name);
   }

   template<class T> 
   JavaType operator() (JNIEnv *env, T protoClass, const char *name,
						const non_object_tag &, bool isStatic) const {
	  if (!isStatic)
		 throw JNIException("Ambiguos attempt to construct a field");
	  else
		 return JNIStaticField<JavaType>(env, protoClass, name);
   }

public:
   // The following functions first detect the type of the second parameter
   // through the call to ObjectTag<T>::tag(), and then invoke
   // the corresponding resource allocation function.
   template<class T>
   JavaType operator() (JNIEnv *env, T arg, const char *name) const {
     typename ObjectTag<T>::tag tag;
	  return (*this)(env, arg, name, tag);
   }

   template<class T>
   JavaType operator() (JNIEnv *env, T arg, const char *name,
						bool isStatic) const {
     typename ObjectTag<T>::tag tag;
	  return (*this)(env, arg, name, tag, isStatic);
   }
};

/*-----------------------------------------------------------------------------
 * Case 1: Accessing String characters (Unicode)
 *
 * JResource type corresponds to 'jstring',
 * and Resource type corresponds to 'const jchar *'.
 * JNIStringCharsSettings implements JNIResourceSettings, which can
 * serve a parameter to JNIResource template above.
 * Applications should use JNIStringChars, which inherits from 
 * JNIResource<JNIStringCharsSettings>.
 *---------------------------------------------------------------------------*/

struct JNIStringCharsSettings {
   typedef jstring JResource;
   typedef const jchar *Resource;

   // Obtaining string characters: GetF uses GetStringChars().
   // To use the 'isCopy' parameter in GetStringChars(), pass it 
   // explicitly to GetF constructor.
   struct GetF {
	  jboolean *_isCopy;
	  GetF(jboolean *isCopy = 0) : _isCopy(isCopy) {}
      Resource operator() (JNIEnv *env, JResource jstr) const {
         return (jstr == 0) ? 0 : env->GetStringChars(jstr, _isCopy);
      }
   };

   // Releasing string characters: ReleaseF uses ReleaseStringChars()
   struct ReleaseF {
      void operator() (JNIEnv *env, JResource jstr, Resource str) const {
         if (jstr != 0)
		    env->ReleaseStringChars(jstr, str);
      }
   };
};

class JNIStringChars : public JNIResource<JNIStringCharsSettings> {
   typedef JNIStringCharsSettings _settings;
   typedef JNIResource<_settings> _super;

public:
   JNIStringChars() {}
   JNIStringChars(JNIEnv *env, jstring jstr) : _super(env, jstr) {}
   JNIStringChars(JNIEnv *env, jstring jstr, jboolean *isCopy) :
      _super(env, jstr, _settings::GetF(isCopy)) {}

   // The following two constructors access the required Java
   // resource field by calling GetJResource<jstring>(...),
   // and call the regular resource constructor to build a handle
   // to a C++ object (const jchar *).
   template<class T>
   JNIStringChars(JNIEnv *env, T arg, const char *name) :
	  _super(env, GetJResource<jstring>()(env, arg, name)) {}

   template<class T>
   JNIStringChars(JNIEnv *env, T arg, const char *name, bool isStatic) :
	  _super(env, GetJResource<jstring>()(env, arg, name, isStatic)) {}

   const jchar &operator[] (int i) const { return _resource[i]; }
   const int length() const { return _env->GetStringLength(_jresource); }
};

/*-----------------------------------------------------------------------------
 * Case 2: Accessing String characters (UTF-8)
 *
 * JResource type corresponds to 'jstring', 
 * and Resource type corresponds to 'const char *'.
 * JNIStringUTFCharsSettings implements JNIResourceSettings, which can
 * serve a parameter to JNIResource template above.
 * Applications should use JNIStringUTFChars, which inherits from 
 * JNIResource<JNIStringUTFCharsSettings> and provides asString() conversion
 * function (for convenient usage of C++ std::string instead of raw 'char *';
 * note that such conversion physically copies the characters).
 *---------------------------------------------------------------------------*/

struct JNIStringUTFCharsSettings {
   typedef jstring JResource;
   typedef const char *Resource;

   // Obtaining string characters: GetF uses GetStringUTFChars()
   // To use the 'isCopy' parameter in GetStringUTFChars(), pass it 
   // explicitly to GetF constructor.
   struct GetF {
	  jboolean *_isCopy;
	  GetF(jboolean *isCopy = 0) : _isCopy(isCopy) {}
      Resource operator() (JNIEnv *env, JResource jstr) const {
		 return (jstr == 0) ? 0 : env->GetStringUTFChars(jstr, _isCopy);
      }
   };

   // Releasing string characters: ReleaseF uses ReleaseStringUTFChars()
   struct ReleaseF {
      void operator() (JNIEnv *env, JResource jstr, Resource str) const {
         if (jstr != 0)
		    env->ReleaseStringUTFChars(jstr, str);
      }
   };
};

class JNIStringUTFChars : public JNIResource<JNIStringUTFCharsSettings> {
   typedef JNIStringUTFCharsSettings _settings;
   typedef JNIResource<_settings> _super;

public:
   JNIStringUTFChars() {}
   JNIStringUTFChars(JNIEnv *env, jstring jstr) : _super(env, jstr) {}
   JNIStringUTFChars(JNIEnv *env, jstring jstr, jboolean *isCopy) :
      _super(env, jstr, _settings::GetF(isCopy)) {}
   
   // The following two constructors access the required Java
   // resource field by calling GetJResource<jstring>(...),
   // and call the regular resource constructor to build a handle
   // to a C++ object (const jchar *).

   template<class T>
   JNIStringUTFChars(JNIEnv *env, T arg, const char *name) :
	  _super(env, GetJResource<jstring>()(env, arg, name)) {}

   template<class T>
   JNIStringUTFChars(JNIEnv *env, T arg, const char *name, bool isStatic) :
	  _super(env, GetJResource<jstring>()(env, arg, name, isStatic)) {}

   const char &operator[] (int i) const { return _resource[i]; }
   const int length() const { return _env->GetStringUTFLength(_jresource); }

   string asString() const { return string(_resource); }
};

/*-----------------------------------------------------------------------------
 * Case 3: Native arrays are exported from Java arrays, so that they 
 *         can be used in C++.
 *
 * This code combines resource management with template and macro definitions,
 * to provide array definitions for all primitive types.
 *
 * The template requires two parameters: NativeType (array type) and
 * ArrayType (Java array type).
 * JResource type corresponds to ArrayType, and Resource type corresponds 
 * to a pointer to NativeType.
 * JNIArraySettings implements JNIResourceSettings,  which can serve 
 * a parameter to JNIResource template above.
 * Applications should use JNIArray, which inherits from 
 * JNIResource<JNIArraySettings> and provides additional access functions.
 *---------------------------------------------------------------------------*/

template<class NativeType>
struct JNIArraySettings {
   typedef typename ARRAY_TYPE_OF(NativeType) JResource;
   typedef NativeType *Resource;

   // Attach to the array: 
   // GetF uses Get<PrimitiveType>ArrayElements (see below).
   // To use the 'isCopy' parameter in Get<PrimitiveType>ArrayElements, 
   // pass it to GetF constructor.
   struct GetF {
	  jboolean *_isCopy;
	  GetF(jboolean *isCopy = 0) : _isCopy(isCopy) {}
      inline Resource operator() (JNIEnv *env, JResource array) const;
   };

   // Detach from the array: 
   // ReleaseF uses Release<PrimitiveType>ArrayElements (see below).
   // To use the 'mode' parameter in Release<PrimitiveType>ArrayElements,
   // pass it to ReleaseF constructor.
   struct ReleaseF {
	  jint _mode;
	  ReleaseF(jint mode = 0) : _mode(mode) {}
      inline void operator() (JNIEnv *env, JResource array,
							  Resource nativeArray) const;
   };
};

template<class NativeType>
class JNIArray : public JNIResource<JNIArraySettings<NativeType> >
{
   typedef JNIArraySettings<NativeType> _settings;
   typedef JNIResource<_settings> _super;
   typedef typename ARRAY_TYPE_OF(NativeType) ArrayType;

public:
   JNIArray() {}
   JNIArray(JNIEnv *env, ArrayType array) : _super(env, array) {}
   JNIArray(JNIEnv *env, ArrayType array, jboolean *isCopy) :
      _super(env, array, _settings::GetF(isCopy)) {}
   
   // The following two constructors access the requested Java
   // resource field by calling GetJResource<jstring>(...), and
   // call the regular resource constructor to build a handle to
   // a C++ object (NativeType *).
   template<class T>
   JNIArray(JNIEnv *env, T arg, const char *name) :
	  _super(env, GetJResource<ArrayType>()(env, arg, name)) {}

   template<class T>
   JNIArray(JNIEnv *env, T arg, const char *name, bool isStatic) :
	  _super(env, GetJResource<ArrayType>()(env, arg, name, isStatic)) {}

   void CustomRelease(int mode = 0) {
	  ReleaseResource(_settings::ReleaseF(mode));
   }

   NativeType &operator[] (int i) { return this->_resource[i]; }
   const NativeType &operator[] (int i) const { return this->_resource[i]; }

   const int size() const { return this->_env->GetArrayLength(this->_jresource); }
};

/*-----------------------------------------------------------------------------
 * The member functions of JNIArraySettings are defined as a macro block,
 * to facilitate their explicit instantiation for all primititve types.
 * The Type parameter of the templates serves as an index into
 * the lookup table to retrieve the type-specific definitions (NativeType
 * and ArrayType) for each instantiation.
 *
 * Example: the preprocessor output for the integer primitive type 
 * (corresponding to the 'jint' native type) is given in file 
 * 'jni_preprocessor.cpp'.
 *---------------------------------------------------------------------------*/

#define JNI_ARRAY_SETTINGS(Type)                                              \
template<>                                                                    \
inline NATIVE_TYPE(Type) *                                                    \
JNIArraySettings<NATIVE_TYPE(Type)>::GetF::operator()                         \
   (JNIEnv *env, ARRAY_TYPE(Type) array) const {                              \
   return (array == 0) ? 0 : env->Get##Type##ArrayElements(array, _isCopy);   \
}                                                                             \
                                                                              \
template<>                                                                    \
inline void                                                                   \
JNIArraySettings<NATIVE_TYPE(Type)>::ReleaseF::operator()                     \
(JNIEnv *env, ARRAY_TYPE(Type) array, NATIVE_TYPE(Type) *nativeArray) const { \
   if (array != 0)                                                            \
      env->Release##Type##ArrayElements(array, nativeArray, _mode);           \
}
   
/*-----------------------------------------------------------------------------
 * Combo instantiation of JNIArraySettings member functions for all primitive
 * types.
 *---------------------------------------------------------------------------*/

INSTANTIATE_FOR_PRIMITIVE_TYPES(JNI_ARRAY_SETTINGS)
					
/*-----------------------------------------------------------------------------
 * Case 4: Monitors
 *
 * JResource type corresponds to jobject.
 * Resource type is redundant, but for completeness is associated 'jobject'
 * too.
 * JNIMonitorSettings implements JNIResourceSettings, which can
 * serve a parameter to JNIResource template above.
 * Applications should use JNIMonitor, which inherits from 
 * JNIResource<JNIMonitorSettings>.
 *---------------------------------------------------------------------------*/

struct JNIMonitorSettings {
   typedef jobject JResource;
   typedef jobject Resource;

   // entering the monitor: GetF uses MonitorEnter()
   struct GetF {
      Resource operator() (JNIEnv *env, JResource obj) const {
		 env->MonitorEnter(obj);
		 return obj;
      }
   };

   // leaving the monitor: ReleaseF uses MonitorExit()
   struct ReleaseF {
      void operator() (JNIEnv *env, JResource obj, Resource dummy) const {
		 env->MonitorExit(obj);
      }
   };
};

class JNIMonitor : public JNIResource<JNIMonitorSettings> {
public:
   JNIMonitor() {}
   JNIMonitor(JNIEnv *env, jobject obj) :
      JNIResource<JNIMonitorSettings>(env, obj) {}
};

/*----------------------------------------------------------------------------
 * Case 5: Reservation of global references
 *
 * Both Resource and JResource types correspond to 'jobject'.
 * JNIGlobalRefSettings implements JNIResourceSettings, which can
 * serve a parameter to JNIResource template above.
 * Applications should use JNIGlobalRef, which inherits from 
 * JNIResource<JNIGlobalRefSettings> and provides equality comparison operator 
 * (for comparing whether two proxies actually refer to the same object).
 *---------------------------------------------------------------------------*/

template <class T>
struct JNIGlobalRefSettings {
   typedef T JResource;
   typedef T Resource;

   // global reference acquisition: DefaultGetF uses NewGlobalRef
   struct GetF {
      Resource operator() (JNIEnv *env, JResource obj) const {
		 return static_cast<JResource>(env->NewGlobalRef(obj));
      }
   };

   // releasing a global reference: DefaultReleaseF uses DeleteGlobalRef
   struct ReleaseF {
      void operator() (JNIEnv *env, JResource obj, Resource ref) const {
       //std::cout << "Deleting ref: " << ref << std::endl;
		 env->DeleteGlobalRef(ref);
      }
   };
};

template<class T>
class JNIGlobalRef : public JNIResource<JNIGlobalRefSettings<T> > {
public:
   JNIGlobalRef() {}
   JNIGlobalRef(JNIEnv *env, T obj) :
      JNIResource<JNIGlobalRefSettings<T> >(env, obj) {}

   // Due to problems with Microsoft Visual C++ 6.0 compiler, the equality
   // comparison operator is implemented as a member function.
   // The version using
   // friend bool operator== (const JNIGlobalRef &x, const JNIGlobalRef &y)
   // passes compilation with g++, but fails for VC++.
   bool operator== (const JNIGlobalRef<T> &x) {
	  if (this->_env != x._env)
		 return false;
	  return (this->_env->IsSameObject(this->_resource, x._resource) != JNI_FALSE);
   }
};

#endif /* _JNI_RESOURCE_H_INCLUDED_ */
