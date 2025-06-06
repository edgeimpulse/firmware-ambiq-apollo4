# Put your local overrides in here

# PLATFORM := apollo3_evb
# AS_VERSION := R3.1.1
#
PLATFORM := apollo4p_blue_kxr_evb
EXAMPLE := mpu_data_collection

#TARGET := mpu_data_collection

# TF_VERSION := 0264234_Nov_15_2023
# TF_VERSION := bb4fc83d_Mar_28_2024
# TF_VERSION := ce72f7b8_Feb_17_2024

# TOOLCHAIN := arm
# GCC13_EXPERIMENTAL := 1
# DEFINES+= NS_AUDIO_DMA_BUFFER_SIZE=16000
# DEFINES+= NS_MALLOC_HEAP_SIZE_IN_K=32
# LEGACY_MALLOC := 1
STACK_SIZE_IN_32B_WORDS := 1024
NS_MALLOC_HEAP_SIZE_IN_K := 256
# DEFINES += DEF_ACC32BIT_OPT
# DEFINES+= WSF_TRACE_ENABLED
# DEFINES+= HCI_TRACE_ENABLED
# DEFINES+= CFG_TUSB_DEBUG=2
