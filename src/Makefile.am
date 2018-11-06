if ADALM_SUPPORT
adalm_src = adalm.c
endif

bin_PROGRAMS=beacon
beacon_SOURCES=iq.c cw.c main.c $(adalm_src)
beacon_LDADD = $(LIBOBJS)