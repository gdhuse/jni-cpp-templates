/*-----------------------------------------------------------------------------
 * This file provides android-specific resource encapsulation
 *---------------------------------------------------------------------------*/

#ifndef _JNI_ANDROID_H_INCLUDED_
#define _JNI_ANDROID_H_INCLUDED_

#include "jni_resource.h"

#include <android/bitmap.h>


/*-----------------------------------------------------------------------------
 * Accessing Bitmap information and pixel data from C++
 *---------------------------------------------------------------------------*/

struct JNIAndroidBitmapSettings {
	typedef jobject JResource;
   typedef void* Resource;

   // Lock the Bitmap pixel data
   struct GetF {
      Resource operator() (JNIEnv *env, JResource jbitmap) const {
       void *pixels;
       if(AndroidBitmap_lockPixels(env, jbitmap, &pixels) == ANDROID_BITMAP_RESULT_SUCCESS) {
         return pixels;
       }
       else {
         return NULL;
       }
     }
   };

   // Unlock the Bitmap pixel data
   struct ReleaseF {
      void operator() (JNIEnv *env, JResource jbitmap, Resource pixels) const {
       if(pixels) {
         AndroidBitmap_unlockPixels(env, jbitmap);
       }
      }
   };
};

class JNIAndroidBitmap : public JNIResource<JNIAndroidBitmapSettings> {   
private:
   AndroidBitmapInfo _bitmapInfo;

public:
   JNIAndroidBitmap() {}
   JNIAndroidBitmap(JNIEnv *env, jobject jbitmap) :
      JNIResource<JNIAndroidBitmapSettings>(env, jbitmap) {
      AndroidBitmap_getInfo(env, jbitmap, &_bitmapInfo);
   }

   void* const data() const {
      return _resource;
   }

   const uint32_t width() const {
      return _bitmapInfo.width;
   }   

   const uint32_t height() const {
      return _bitmapInfo.height;
   }

   const uint32_t stride() const {
      return _bitmapInfo.stride;
   }

   const int32_t format() const {
      return _bitmapInfo.format;
   }
};

#endif /* _JNI_ANDROID_H_INCLUDED_ */
