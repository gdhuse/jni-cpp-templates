/* This file contains common declarations */

#ifndef _JNI_DECLARATIONS_H_INCLUDED_
#define _JNI_DECLARATIONS_H_INCLUDED_

#include <string>
#include <exception>
#include <jni.h>

using std::string;
using std::exception;

/*-----------------------------------------------------------------------------
 * JNIException class defines exceptions for use in the JNI routines
 *---------------------------------------------------------------------------*/

class JNIException : public exception {
   string _msg;		// Exception message
public:
   JNIException(const string &msg = "Unknown exception") : _msg(msg) {}
   JNIException(const char *msg) : _msg(msg) {}

   virtual ~JNIException() throw() {}

   virtual const char *what() const throw() { return _msg.c_str(); }
};

/*---------------------------------------------------------------------------
 * Lookup table for primitive types.
 * Each primitive type (PrimitiveType) has a block of declarations 
 * entitled <PrimitiveType>Declarations, which has four constituents:
 * 	- NativeType:       Java type corresponding to PrimitiveType
 *  - ArrayType:        Java array type corresponding to PrimitiveType
 *	- signature:        type signature (a string constant corresponding
 *                      to PrimitiveType)
 *	- array_signature:  array type signature (a string constant
 *				        corresponding to ArrayType)
 *-------------------------------------------------------------------------*/

struct BooleanDeclarations {
   typedef jboolean NativeType;
   typedef jbooleanArray ArrayType;
   static const char *signature() { return "Z"; }
   static const char *array_signature() { return "[Z"; }
};

struct ByteDeclarations {
   typedef jbyte NativeType;
   typedef jbyteArray ArrayType;
   static const char *signature() { return "B"; }
   static const char *array_signature() { return "[B"; }
};

struct CharDeclarations {
   typedef jchar NativeType;
   typedef jcharArray ArrayType;
   static const char *signature() { return "C"; }
   static const char *array_signature() { return "[C"; }
};

struct ShortDeclarations {
   typedef jshort NativeType;
   typedef jshortArray ArrayType;
   static const char *signature() { return "S"; }
   static const char *array_signature() { return "[S"; }
};

struct IntDeclarations {
   typedef jint NativeType;
   typedef jintArray ArrayType;
   static const char *signature() { return "I"; }
   static const char *array_signature() { return "[I"; }
};

struct LongDeclarations {
   typedef jlong NativeType;
   typedef jlongArray ArrayType;
   static const char *signature() { return "L"; }
   static const char *array_signature() { return "[L"; }
};

struct FloatDeclarations {
   typedef jfloat NativeType;
   typedef jfloatArray ArrayType;
   static const char *signature() { return "F"; }
   static const char *array_signature() { return "[F"; }
};

struct DoubleDeclarations {
   typedef jdouble NativeType;
   typedef jdoubleArray ArrayType;
   static const char *signature() { return "D"; }
   static const char *array_signature() { return "[D"; }
};

/*-----------------------------------------------------------------------------
 * Macros for mapping a given PrimitiveType into
 * the corresponding NativeType, ArrayType, and signatures.
 * For example,
 *   NATIVE_TYPE(Boolean) = BooleanDeclarations::NativeType (= jboolean)
 *---------------------------------------------------------------------------*/

#define NATIVE_TYPE(Type) 		Type##Declarations::NativeType
#define ARRAY_TYPE(Type)  		Type##Declarations::ArrayType
#define SIGNATURE(Type)   		Type##Declarations::signature()
#define ARRAY_SIGNATURE(Type)   Type##Declarations::array_signature()

/*-----------------------------------------------------------------------------
 * Macros for explicit instantiation of templates (realized as macros)
 * for all primitive types
 *---------------------------------------------------------------------------*/

#define INSTANTIATE_FOR_PRIMITIVE_TYPES(BLOCK_MACRO)	\
BLOCK_MACRO(Boolean)									\
BLOCK_MACRO(Byte)										\
BLOCK_MACRO(Char)										\
BLOCK_MACRO(Short)										\
BLOCK_MACRO(Int)										\
BLOCK_MACRO(Long)										\
BLOCK_MACRO(Float)										\
BLOCK_MACRO(Double)

/*---------------------------------------------------------------------------
 * Lookup table for JNI native types and native array types.
 * For example, given the 'jint' type, it should be possible to determine
 * that its signature is "I", and the corresponding array type is 'jintArray'.
 * Similarly, for 'jintArray' it should be possible to determine that its 
 * signature is "[I", and the corresponding native type is 'jint'.
 *
 * This feature is implemented using the template specialization technique.
 * The default implementation of 'JNITypeDeclarations' structure is empty.
 * Specialization for native types is performed using the preprocessor.
 *
 * Here is a sample preprocessor output for the 'jint' type:
 *
 * template<> struct JNITypeDeclarations<IntDeclarations::NativeType>
 * {
 *    typedef IntDeclarations Declarations;
 *    typedef IntDeclarations::NativeType NativeType;
 *    typedef IntDeclarations::ArrayType  ArrayType;
 *    static const char *signature() { return IntDeclarations::signature(); }
 * }; 	 
 *
 * The corresponding structure for 'jintArray' only differs in the signature 
 * function, which yields IntDeclarations::array_signature().
 *
 * A bug in Microsoft Visual C++ 6.0 prohibits nested declarations of 
 * the form JNITypeDeclarations<NativeType>::Declarations::ArrayType,
 * otherwise NativeType and ArrayType fields could be omitted.
 *
 *-------------------------------------------------------------------------*/

template<class JavaType>
struct JNITypeDeclarations {};

#define JNI_TYPE_DECLARATIONS(Type)											\
template<> struct JNITypeDeclarations<NATIVE_TYPE(Type)> {					\
   typedef Type##Declarations Declarations;									\
   typedef NATIVE_TYPE(Type) NativeType;									\
   typedef ARRAY_TYPE(Type) ArrayType;										\
   static const char *signature() { return SIGNATURE(Type); }				\
};

#define JNI_ARRAY_DECLARATIONS(Type)										\
template<> struct JNITypeDeclarations<ARRAY_TYPE(Type)> {					\
   typedef Type##Declarations Declarations;									\
   typedef NATIVE_TYPE(Type) NativeType;									\
   typedef ARRAY_TYPE(Type) ArrayType;										\
   static const char *signature() { return ARRAY_SIGNATURE(Type); }			\
};

/*-----------------------------------------------------------------------------
 * Combo instantiation of JNITypeDeclarations specializations for all
 * primitive types
 *---------------------------------------------------------------------------*/

INSTANTIATE_FOR_PRIMITIVE_TYPES(JNI_TYPE_DECLARATIONS)
INSTANTIATE_FOR_PRIMITIVE_TYPES(JNI_ARRAY_DECLARATIONS)

/*-----------------------------------------------------------------------------
 * In order to be able to provide default parameters like
 * SIGNATURE_OF(JavaType) = JNITypeDeclarations<JavaType>::signature()
 * (for example, see the constructor of JNIGenericField), 
 * we need to declare the corresponding structures for 'jobject' type. 
 * These declarations will cause a run-time error if 'signature()' or 
 * 'array_signature()' of 'jobject' type are actually invoked.
 *---------------------------------------------------------------------------*/

struct ObjectDeclarations {
   typedef jobject NativeType;
   typedef jobjectArray ArrayType;
   static const char *signature() {
	  throw JNIException("No signature available for jobject");
   }
   static const char *array_signature() {
	  throw JNIException("No signature available for jobjectArray");
   }
};

JNI_TYPE_DECLARATIONS(Object)

/*-----------------------------------------------------------------------------
 * Macros for mapping any JNI type (jint, jintArray, jobject, etc.)
 * to the corresponding Declarations structure, 
 * NativeType, ArrayType, and signatures.
 * For example,
 *   SIGNATURE_OF(jintArray) = JNITypeDeclarations<jintArray>::signature() =
 *                             IntDeclarations::array_signature() = "[I"
 * If a type JNITypeDeclarations<jintArray> is not declared, a compilation
 * error will occur.
 *---------------------------------------------------------------------------*/

#define DECLARATIONS_OF(JavaType)								\
JNITypeDeclarations<JavaType>::Declarations

#define NATIVE_TYPE_OF(JavaType)								\
JNITypeDeclarations<JavaType>::NativeType

#define ARRAY_TYPE_OF(JavaType)									\
JNITypeDeclarations<JavaType>::ArrayType

#define SIGNATURE_OF(JavaType)									\
JNITypeDeclarations<JavaType>::signature()

/*-----------------------------------------------------------------------------
 * Lookup tables for other JNI types may also be declared, 
 * here is an example for 'jstring'.
 *---------------------------------------------------------------------------*/

struct StringDeclarations {
   typedef jstring NativeType;
   typedef jobject ArrayType;
   static const char *signature() { return "Ljava/lang/String;"; }
   static const char *array_signature() { return "[Ljava/lang/String;"; }
};

JNI_TYPE_DECLARATIONS(String)

#endif /* _JNI_DECLARATIONS_H_INCLUDED_ */
