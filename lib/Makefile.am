lib_LTLIBRARIES = libofx.la
libofx_la_SOURCES =  messages.cpp \
		ofx_utilities.cpp \
		file_preproc.cpp \
		ofx_preproc.cpp \
		ofx_container_generic.cpp \
		ofx_container_main.cpp \
		ofx_container_security.cpp \
		ofx_container_statement.cpp \
		ofx_container_account.cpp \
		ofx_container_transaction.cpp \
		ofx_containers_misc.cpp \
		ofx_sgml.cpp \
		ofc_sgml.cpp

noinst_HEADERS = ${top_srcdir}/inc/libofx.h \
		messages.hh \
		ofx_preproc.hh \
		file_preproc.hh \
		ofx_sgml.hh \
		ofc_sgml.hh \
		ofx_error_msg.hh \
		ofx_containers.hh \
		ofx_utilities.hh \
		tree.hh

AM_CPPFLAGS = \
	-I. \
	-I${top_srcdir}/inc \
	-I${OPENSPINCLUDES} \
	-DMAKEFILE_DTD_PATH=\"${LIBOFX_DTD_DIR}/\"

libofx_la_LIBADD = ${OPENSPLIBS} -lstdc++
libofx_la_LDFLAGS = -version-info ${LIBOFX_MAJOR_VERSION}:${LIBOFX_MINOR_VERSION}:0



