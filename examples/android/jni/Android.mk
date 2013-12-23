LOCAL_PATH      := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES	:= ../../jni_example.cpp ../../jni_complex_example.cpp
LOCAL_C_INCLUDES := ../../include .
LOCAL_EXPORT_C_INCLUDES := 

LOCAL_MODULE := examples_static
LOCAL_MODULE_FILENAME := libexamples
include $(BUILD_STATIC_LIBRARY)

LOCAL_MODULE := examples
LOCAL_MODULE_FILENAME := libexamples
include $(BUILD_SHARED_LIBRARY)
