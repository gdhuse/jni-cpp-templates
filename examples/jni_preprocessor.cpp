/*---------------------------------------------------------------------------
 * The expanded macro block for specialization of the JNITypeDeclarations
 * template for 'jint' and 'jintArray' (formatted preprocessor output)
 *-------------------------------------------------------------------------*/
template<> struct JNITypeDeclarations<IntDeclarations::NativeType>
{
   typedef IntDeclarations Declarations;
   typedef IntDeclarations::NativeType NativeType;
   typedef IntDeclarations::ArrayType  ArrayType;
   static const char *signature() {
	  return IntDeclarations::signature();
   }
}; 	 

template<> struct JNITypeDeclarations<IntDeclarations::ArrayType>
{
   typedef IntDeclarations Declarations;
   typedef IntDeclarations::NativeType NativeType;
   typedef IntDeclarations::ArrayType  ArrayType;
   static const char *signature() {
	  return IntDeclarations::array_signature();
   }
}; 	 

/*---------------------------------------------------------------------------
 * The expanded macro block for JNIFieldId<jint> 
 * (formatted preprocessor output)
 *-------------------------------------------------------------------------*/

template<> class JNIFieldId<IntDeclarations::NativeType> :
   public JNIGenericFieldId {
public:
   template<class T>
   JNIFieldId(JNIEnv *env, T protoClass, const char *name) :
	  JNIGenericFieldId(env,
						env->GetFieldID(JNIClass(env, protoClass),
										name, IntDeclarations::signature()))
   {}

   IntDeclarations::NativeType Get(jobject obj) const {
	  return _env->GetIntField(obj, _id);
   }

   void Set(jobject obj, IntDeclarations::NativeType val) {
	  _env->SetIntField(obj, _id, val);
   }
};

/*---------------------------------------------------------------------------
 * The expanded macro block for JNIStaticFieldId<jint> 
 * (formatted preprocessor output)
 *-------------------------------------------------------------------------*/
template<> class JNIStaticFieldId<IntDeclarations::NativeType> :
   public JNIGenericFieldId	{
public:
   template<class T>
   JNIStaticFieldId(JNIEnv *env, T protoClass, const char *name) :
	  JNIGenericFieldId(env,
						env->GetStaticFieldID(JNIClass(env, protoClass),
											  name,
											  IntDeclarations::signature()))
   {}

   IntDeclarations::NativeType Get(jclass clazz) const {
	  return _env->GetStaticIntField(clazz, _id);
   }
   void Set(jclass clazz, IntDeclarations::NativeType val) {
	  _env->SetStaticIntField(clazz, _id, val);
   }
};

/*---------------------------------------------------------------------------
 * The expanded macro block for JNIArraySettings<jint>
 * (formatted preprocessor output)
 *-------------------------------------------------------------------------*/
inline IntDeclarations::NativeType  *
JNIArraySettings<IntDeclarations::NativeType>::GetF::operator()
   (JNIEnv *env, IntDeclarations::ArrayType array) const
{
   return env->GetIntArrayElements(array, _isCopy);
}

inline void	JNIArraySettings<IntDeclarations::NativeType>::ReleaseF::operator()
   (JNIEnv *env, IntDeclarations::ArrayType array,
	IntDeclarations::NativeType *nativeArray) const
{
   env->ReleaseIntArrayElements(array, nativeArray, _mode);
}
