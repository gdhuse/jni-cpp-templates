/*-----------------------------------------------------------------------------
 * Utilities for accessing Java object variables (regular member variables
 * in C++ terminology) and class variables (static member variables in C++
 * terminology).
 * In contrast to the original JNI design, we treat field ids as active
 * objects. A field id can be constructed from the class directly, given an
 * object of this class or a class name. After the field id is constructed,
 * there is no need to use the explicit forms Get<PrimitiveType>Field and 
 * Set<PrimitiveType>Field, since all the type information was already
 * deduced at construction time.
 * JNIField and JNIStaticField classes are implemented using
 * JNIFieldId and JNIStaticFieldId (respectively), and they constitute
 * a proxy for the corresponding Java object. This allows to treat them 
 * as a data field of the appropriate type (integer, float etc.).
 *---------------------------------------------------------------------------*/
 
#ifndef _JNI_FIELD_H_INCLUDED_
#define _JNI_FIELD_H_INCLUDED_

#include "jni_declarations.h"
#include "jni_class.h"

/*-----------------------------------------------------------------------------
 * JNIGenericFieldId represents a structure common for both regular and static
 * field ids, and contains an environment handle and a 'jfieldID' object.
 *---------------------------------------------------------------------------*/
class JNIGenericFieldId {
protected:
   JNIEnv *_env;  // Environment handle for subsequent field manipulation
   jfieldID _id;  // Field ID

protected:
   JNIGenericFieldId(JNIEnv *env, jfieldID id) : _env(env), _id(id) {
	  if (_id == 0)
		 throw JNIException("Field not found");
   }
};

/*-----------------------------------------------------------------------------
 * JNIFieldId class implements four ways to construct a field
 * using a class, an object of the class, a class name, or a JNIClass object.
 * The template is suitable for any JNI type, and is subsequently specialized
 * (preinstantiated) for the primitive types.
 *---------------------------------------------------------------------------*/
template<class JavaType>
class JNIFieldId : public JNIGenericFieldId {
public:
   // JNIFieldId constructor: given a 'protoClass' (i.e., 'jclass', 'jobject'
   // or 'const char *'), obtain the corresponding field id by calling 
   // GetFieldID, then pass it to the base class constructor along with 
   // the environment handle. If the signature declaration for the requested
   // type does not exist, the construction process fails.
   template<class T>
   JNIFieldId(JNIEnv *env, T protoClass, const char *name,
			  const char *sig = SIGNATURE_OF(JavaType)) :
	  JNIGenericFieldId(env,
						env->GetFieldID(JNIClass(env, protoClass), name, sig))
   {}

   // Get and Set utilities
   JavaType Get(jobject obj) const {
	  return static_cast<JavaType>(_env->GetObjectField(obj, _id));
   }
   void Set(jobject obj, JavaType val) {
	  _env->SetObjectField(obj, _id, val);
   }
};

/*-----------------------------------------------------------------------------
 * The specisalization of JNIFieldId template is defined as a macro block,
 * to facilitate its explicit instantiation for the primititve types.
 * The 'Type' parameter of the template serves an index into the lookup table
 * to retrieve the type-specific declarations.
 *
 * Preprocessor output for the integer primitive type (corresponding
 * to the 'jint' native type) is given in file 'jni_preprocessor.cpp'.
 *---------------------------------------------------------------------------*/

#define JNI_FIELD_ID_METHODS(Type)											\
template<> class JNIFieldId<NATIVE_TYPE(Type)> : public JNIGenericFieldId 	\
{																			\
public:																		\
   template<class T>														\
   JNIFieldId(JNIEnv *env, T protoClass, const char *name) :				\
	  JNIGenericFieldId(env, env->GetFieldID(JNIClass(env, protoClass),		\
											 name, SIGNATURE(Type))) {}		\
																			\
   NATIVE_TYPE(Type) Get(jobject obj) const {								\
      return _env->Get##Type##Field(obj, _id);								\
   }																		\
   void Set(jobject obj, NATIVE_TYPE(Type) val) {							\
      _env->Set##Type##Field(obj, _id, val);								\
   }																		\
};

/*-----------------------------------------------------------------------------
 * Combo instantiation of JNIFieldId specialization for all primitive types
 *---------------------------------------------------------------------------*/

INSTANTIATE_FOR_PRIMITIVE_TYPES(JNI_FIELD_ID_METHODS)

/*-----------------------------------------------------------------------------
 * JNIStaticFieldId template is identical to the JNIFieldId template,
 * except for the call to GetStaticFieldID instead of GetFieldId.
 *---------------------------------------------------------------------------*/
template<class JavaType>
class JNIStaticFieldId : public JNIGenericFieldId {
public:
   // JNIStaticFieldId constructor: given a 'protoClass' (i.e., 'jclass', 
   // 'jobject' or 'const char *'), obtain the corresponding static field id 
   // by calling GetStaticFieldID, then pass it to the base class constructor 
   // along with the environment handle. If the signature declaration for the 
   // requested type does not exist, the construction process fails.
   template<class T>
   JNIStaticFieldId(JNIEnv *env, T protoClass, const char *name,
					const char *sig = SIGNATURE_OF(JavaType)) :
	  JNIGenericFieldId(env, env->GetStaticFieldID(JNIClass(env, protoClass),
												   name, sig)) {}

   // Get and Set utilities
   JavaType Get(jclass clazz) const {
	  return static_cast<JavaType>(_env->GetStaticObjectField(clazz, _id));
   }
   void Set(jclass clazz, JavaType val) {
	  _env->SetStaticObjectField(clazz, _id, val);
   }
};

/*-----------------------------------------------------------------------------
 * Similarly to JNIFieldId, the specialization of JNIStaticFieldId template
 * is defined as a macro block. The Type parameter serves an index into
 * the lookup table to retrieve the type-specific declarations.
 *
 * Preprocessor output for the integer primitive type (corresponding to
 * the 'jint' native type) is given in file 'jni_preprocessor.cpp'.
 *---------------------------------------------------------------------------*/

#define JNI_STATIC_FIELD_ID_METHODS(Type)									\
template<>																	\
class JNIStaticFieldId<NATIVE_TYPE(Type)> : public JNIGenericFieldId		\
{																			\
public:																		\
   template<class T>														\
   JNIStaticFieldId(JNIEnv *env, T protoClass, const char *name) :			\
      JNIGenericFieldId(env,												\
						env->GetStaticFieldID(JNIClass(env, protoClass),	\
											  name, SIGNATURE(Type))) {}	\
																			\
   NATIVE_TYPE(Type) Get(jclass clazz) const {								\
      return _env->GetStatic##Type##Field(clazz, _id);						\
   }																		\
   void Set(jclass clazz, NATIVE_TYPE(Type) val) {							\
      _env->SetStatic##Type##Field(clazz, _id, val);						\
   }																		\
};

/*-----------------------------------------------------------------------------
 * Combo instantiation of JNIStaticFieldId specialization for all primitive
 * types
 *---------------------------------------------------------------------------*/

INSTANTIATE_FOR_PRIMITIVE_TYPES(JNI_STATIC_FIELD_ID_METHODS)

/*-----------------------------------------------------------------------------
 * JNIField is a template parameterized with a native type ('jint',
 * 'jchar' etc.) It has two members: a JNIFieldId and an object itself.
 * After a JNIField object is constructed, it behaves as a proxy to the
 * corresponding actual Java field.
 *---------------------------------------------------------------------------*/
template<class NativeType>
class JNIField {
   typedef JNIField<NativeType> _self;
   
private:
   jobject _obj;				// The Java object that hosts the field
   JNIFieldId<NativeType> _id;	// field id

public:
   // Construct a field given a field id and an object
   JNIField(JNIFieldId<NativeType> id, jobject obj) : _obj(obj), _id(id) {}
	  
   // Construct a field given an object, field name and signature
   JNIField(JNIEnv *env, jobject obj, const char *name, const char *sig) :
	  _obj(obj), _id(env, obj, name, sig) {}

   // Construct a field given an object and field name (this constructor
   // is appropriate for classes that have a JNITypeDeclarations structure).
   JNIField(JNIEnv *env, jobject obj, const char *name) :
	  _obj(obj), _id(env, obj, name) {}

   // Assignment operator
   _self &operator= (const _self &rhs) {
	  if (this != &rhs)
		 _id.Set(_obj, rhs.Get(rhs._obj));
	  return *this;
   }

   // Assignment operator that receives a native type object
   // (note that the operator returns a JNIField<NativeType> object).
   // This operator is necessary, because NativeType cannot be simply
   // converted to JNIField.
   _self &operator= (const NativeType &rhs) {
	  _id.Set(_obj, rhs);
	  return *this;
   }

   // Casting to NativeType
   operator NativeType() const {
	  return _id.Get(_obj);
   }
};

/*-----------------------------------------------------------------------------
 * JNIStaticField is similar to JNIField.
 * It has two members: JNIFieldId and (unlike JNIField) a Java class object.
 * After a JNIStaticField object is constructed, it behaves as a proxy to the
 * corresponding actual Java field.
 *---------------------------------------------------------------------------*/
template<class NativeType>
class JNIStaticField {
   typedef JNIStaticField<NativeType> _self;
   
private:
   jclass _clazz;					   	// The Java class that hosts the field
   JNIStaticFieldId<NativeType> _id;	// field id

public:
   // Construct a field given a field id and a class
   JNIStaticField(JNIStaticField<NativeType> &id, jclass clazz) :
	  _clazz(clazz), _id(id) {}
	  
   // Construct a field given some object from which a class can be 
   // constructed, a field name and signature.
   template<class T>
   JNIStaticField(JNIEnv *env, T protoClass, const char *name,
				  const char *sig) :
	  _clazz(JNIClass(env, protoClass)),
	  _id(env, JNIClass(env, protoClass), name, sig) {}

   // Construct a field given some object from which a class can be 
   // constructed, a field name and signature (this constructor is
   // appropriate for classes that have a JNITypeDeclarations structure).
   template<class T>
   JNIStaticField(JNIEnv *env, T protoClass, const char *name) :
	  _clazz(JNIClass(env, protoClass)),
	  _id(env, JNIClass(env, protoClass), name) {}

   // Assignment operator
   _self &operator= (const _self &rhs) {
	  if (this != &rhs)
		 _id.Set(_clazz, rhs.Get(rhs._clazz));
	  return *this;
   }

   // Assignment operator that receives a native type object
   // (note that the operator returns a JNIField<NativeType> object).
   _self &operator= (const NativeType &rhs) {
	  _id.Set(_clazz, rhs);
	  return *this;
   }

   // Casting to NativeType
   operator NativeType() const {
	  return _id.Get(_clazz);
   }
};

#endif /* _JNI_FIELD_H_INCLUDED_ */
