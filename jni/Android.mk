MY_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)

### lapack

include $(CLEAR_VARS)

LOCAL_PATH := $(NVXS_ROOT)/CLAPACK

LOCAL_MODULE := lapack
LOCAL_SRC_FILES := \
	$(patsubst $(LOCAL_PATH)/%, %, \
		$(wildcard $(LOCAL_PATH)/SRC/*.c) \
		$(wildcard $(LOCAL_PATH)/INSTALL/*.c) \
	)

include $(BUILD_STATIC_LIBRARY)

### blas

include $(CLEAR_VARS)

LOCAL_PATH := $(NVXS_ROOT)/CLAPACK/BLAS

LOCAL_MODULE := blas
LOCAL_SRC_FILES := \
	$(patsubst $(LOCAL_PATH)/%, %, \
		$(wildcard $(LOCAL_PATH)/SRC/*.c) \
	)

include $(BUILD_STATIC_LIBRARY)

### tmglib

include $(CLEAR_VARS)

LOCAL_PATH := $(NVXS_ROOT)/CLAPACK/TESTING/MATGEN

LOCAL_MODULE := tmglib
LOCAL_SRC_FILES := \
	$(patsubst $(LOCAL_PATH)/%, %, \
		$(wildcard $(LOCAL_PATH)/*.c) \
	)

include $(BUILD_STATIC_LIBRARY)

### f2c

include $(CLEAR_VARS)

LOCAL_PATH := $(NVXS_ROOT)/CLAPACK/F2CLIBS

LOCAL_MODULE := f2c
LOCAL_SRC_FILES := \
	$(patsubst $(LOCAL_PATH)/%, %, \
		$(wildcard $(LOCAL_PATH)/libf2c/*.c) \
	)

LOCAL_CFLAGS := -DINTEGER_STAR_8 -U__linux__

include $(BUILD_STATIC_LIBRARY)

### nvxs

include $(CLEAR_VARS)

LOCAL_PATH := $(NVXS_ROOT)

LOCAL_MODULE := nvxs
LOCAL_SRC_FILES := \
	$(patsubst $(LOCAL_PATH)/%, %, \
		$(wildcard $(LOCAL_PATH)/nv_core/*.c) \
		$(wildcard $(LOCAL_PATH)/nv_face/*.c) \
		$(wildcard $(LOCAL_PATH)/nv_ip/*.c) \
		$(wildcard $(LOCAL_PATH)/nv_ml/*.c) \
		$(wildcard $(LOCAL_PATH)/nv_num/*.c) \
	)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/nv_core/ \
	$(LOCAL_PATH)/nv_face/ \
	$(LOCAL_PATH)/nv_ip/ \
	$(LOCAL_PATH)/nv_ml/ \
	$(LOCAL_PATH)/nv_num/ \
	$(LOCAL_PATH)/CLAPACK/INCLUDE/ \
	$(LOCAL_PATH)/CLAPACK/SRC/

include $(BUILD_STATIC_LIBRARY)

LOCAL_PATH := $(MY_PATH)

include $(CLEAR_VARS)

LOCAL_LDLIBS := \
	-L$(LOCAL_PATH)/../obj/local/armeabi/ \
	-lnvxs \
	-llapack \
	-lblas \
	-ltmglib \
	-lf2c

LOCAL_MODULE 	:= nvxs-bridge
LOCAL_SRC_FILES := nvxs-bridge.c

LOCAL_STATIC_LIBRARIES := nvxs lapack blas tmglib f2c

LOCAL_C_INCLUDES := \
	$(NVXS_ROOT)/nv_core/ \
	$(NVXS_ROOT)/nv_face/ \
	$(NVXS_ROOT)/nv_ip/ \
	$(NVXS_ROOT)/nv_ml/ \
	$(NVXS_ROOT)/nv_num/

include $(BUILD_SHARED_LIBRARY)
