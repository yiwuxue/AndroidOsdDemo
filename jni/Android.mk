LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/sdl2
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/sdl2_ttf
LOCAL_MODULE := osd-test
LOCAL_LDLIBS :=  -L$(call host-path, $(LOCAL_PATH)) -lm -llog -lc -L. -lSDL2_ttf -lSDL2 -lhidapi
LOCAL_SRC_FILES :=  main.cpp \
		    OsdCtl.cpp \
		    OsdObj.cpp \
		    ttf2bmp.c \
			bmp2yuv.cpp \
			yuvosd.c

include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)

