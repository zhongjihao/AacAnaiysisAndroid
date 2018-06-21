LOCAL_PATH := $(call my-dir)
 
include $(CLEAR_VARS)
 
LOCAL_MODULE    	:= aacAnalyzer

LOCAL_SRC_FILES 	:= \
        ./jni_aacanaly_wrap.cpp \
        ./faad2-2.7/frontend/aac_decode.cpp \
        ./faad2-2.7/frontend/audio.c \
        ./faad2-2.7/libfaad/bits.c \
        ./faad2-2.7/libfaad/cfft.c \
        ./faad2-2.7/libfaad/common.c \
        ./faad2-2.7/libfaad/decoder.c \
        ./faad2-2.7/libfaad/drc.c \
        ./faad2-2.7/libfaad/drm_dec.c \
        ./faad2-2.7/libfaad/error.c \
        ./faad2-2.7/libfaad/filtbank.c \
        ./faad2-2.7/libfaad/hcr.c \
        ./faad2-2.7/libfaad/huffman.c \
        ./faad2-2.7/libfaad/ic_predict.c \
        ./faad2-2.7/libfaad/is.c \
        ./faad2-2.7/libfaad/lt_predict.c \
        ./faad2-2.7/libfaad/mdct.c \
        ./faad2-2.7/libfaad/mp4.c \
        ./faad2-2.7/libfaad/ms.c \
        ./faad2-2.7/libfaad/output.c \
        ./faad2-2.7/libfaad/pns.c \
        ./faad2-2.7/libfaad/ps_dec.c \
        ./faad2-2.7/libfaad/ps_syntax.c \
        ./faad2-2.7/libfaad/pulse.c \
        ./faad2-2.7/libfaad/rvlc.c \
        ./faad2-2.7/libfaad/sbr_dct.c \
        ./faad2-2.7/libfaad/sbr_dec.c \
        ./faad2-2.7/libfaad/sbr_e_nf.c \
        ./faad2-2.7/libfaad/sbr_fbt.c \
        ./faad2-2.7/libfaad/sbr_hfadj.c \
        ./faad2-2.7/libfaad/sbr_hfgen.c \
        ./faad2-2.7/libfaad/sbr_huff.c \
        ./faad2-2.7/libfaad/sbr_qmf.c \
        ./faad2-2.7/libfaad/sbr_syntax.c \
        ./faad2-2.7/libfaad/sbr_tf_grid.c \
        ./faad2-2.7/libfaad/specrec.c \
        ./faad2-2.7/libfaad/ssr.c \
        ./faad2-2.7/libfaad/ssr_fb.c \
        ./faad2-2.7/libfaad/ssr_ipqf.c \
        ./faad2-2.7/libfaad/syntax.c \
        ./faad2-2.7/libfaad/tns.c \
        ./faad2-2.7/common/faad/getopt.c \
        ./faad2-2.7/common/mp4ff/mp4atom.c \
        ./faad2-2.7/common/mp4ff/mp4ff.c \
        ./faad2-2.7/common/mp4ff/mp4meta.c \
        ./faad2-2.7/common/mp4ff/mp4sample.c \
        ./faad2-2.7/common/mp4ff/mp4tagupdate.c \
        ./faad2-2.7/common/mp4ff/mp4util.c


LOCAL_C_INCLUDES := \
    ./faad2-2.7/libfaad/analysis.h \
    ./faad2-2.7/libfaad/bits.h \
    ./faad2-2.7/libfaad/cfft.h \
    ./faad2-2.7/libfaad/cfft_tab.h \
    ./faad2-2.7/libfaad/common.h \
    ./faad2-2.7/libfaad/drc.h \
    ./faad2-2.7/libfaad/drm_dec.h \
    ./faad2-2.7/libfaad/error.h \
    ./faad2-2.7/libfaad/filtbank.h \
    ./faad2-2.7/libfaad/fixed.h \
    ./faad2-2.7/libfaad/huffman.h \
    ./faad2-2.7/libfaad/ic_predict.h \
    ./faad2-2.7/libfaad/iq_table.h \
    ./faad2-2.7/libfaad/is.h \
    ./faad2-2.7/libfaad/kbd_win.h \
    ./faad2-2.7/libfaad/lt_predict.h \
    ./faad2-2.7/libfaad/mdct.h \
    ./faad2-2.7/libfaad/mdct_tab.h \
    ./faad2-2.7/libfaad/mp4.h \
    ./faad2-2.7/libfaad/ms.h \
    ./faad2-2.7/libfaad/output.h \
    ./faad2-2.7/libfaad/pns.h \
    ./faad2-2.7/libfaad/ps_dec.h \
    ./faad2-2.7/libfaad/ps_tables.h \
    ./faad2-2.7/libfaad/pulse.h \
    ./faad2-2.7/libfaad/rvlc.h \
    ./faad2-2.7/libfaad/sbr_dct.h \
    ./faad2-2.7/libfaad/sbr_dec.h \
    ./faad2-2.7/libfaad/sbr_e_nf.h \
    ./faad2-2.7/libfaad/sbr_fbt.h \
    ./faad2-2.7/libfaad/sbr_hfadj.h \
    ./faad2-2.7/libfaad/sbr_hfgen.h \
    ./faad2-2.7/libfaad/sbr_huff.h \
    ./faad2-2.7/libfaad/sbr_noise.h \
    ./faad2-2.7/libfaad/sbr_qmf.h \
    ./faad2-2.7/libfaad/sbr_qmf_c.h \
    ./faad2-2.7/libfaad/sbr_syntax.h \
    ./faad2-2.7/libfaad/sbr_tf_grid.h \
    ./faad2-2.7/libfaad/sine_win.h \
    ./faad2-2.7/libfaad/specrec.h \
    ./faad2-2.7/libfaad/ssr.h \
    ./faad2-2.7/libfaad/ssr_fb.h \
    ./faad2-2.7/libfaad/ssr_ipqf.h \
    ./faad2-2.7/libfaad/ssr_win.h \
    ./faad2-2.7/libfaad/structs.h \
    ./faad2-2.7/libfaad/syntax.h \
    ./faad2-2.7/libfaad/tns.h \
    ./faad2-2.7/libfaad/codebook/hcb_10.h \
    ./faad2-2.7/libfaad/codebook/hcb_11.h \
    ./faad2-2.7/libfaad/codebook/hcb_1.h \
    ./faad2-2.7/libfaad/codebook/hcb_2.h \
    ./faad2-2.7/libfaad/codebook/hcb_3.h \
    ./faad2-2.7/libfaad/codebook/hcb_4.h \
    ./faad2-2.7/libfaad/codebook/hcb_5.h \
    ./faad2-2.7/libfaad/codebook/hcb_6.h \
    ./faad2-2.7/libfaad/codebook/hcb_7.h \
    ./faad2-2.7/libfaad/codebook/hcb_8.h \
    ./faad2-2.7/libfaad/codebook/hcb_9.h \
    ./faad2-2.7/libfaad/codebook/hcb.h \
    ./faad2-2.7/libfaad/codebook/hcb_sf.h \
    ./faad2-2.7/frontend/audio.h \
    ./faad2-2.7/include/faad.h \
    ./faad2-2.7/include/neaacdec.h \
    ./faad2-2.7/common/faad/aacinfo.h \
    ./faad2-2.7/common/faad/filestream.h \
    ./faad2-2.7/common/faad/getopt.h \
    ./faad2-2.7/common/faad/id3v2tag.h \
    ./faad2-2.7/common/mp4ff/mp4ff.h \
    ./faad2-2.7/common/mp4ff/mp4ffint.h \
    ./faad2-2.7/common/mp4ff/mp4ff_int_types.h

LOCAL_LDLIBS := -llog

LOCAL_CFLAGS +=  -DHAVE_CONFIG_H -DUSE_TAGGING

include $(BUILD_SHARED_LIBRARY)
