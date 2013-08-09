EXTRA_DIST = m4
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
AM_MAKEFLAGS = --no-print-directory

AM_CPPFLAGS = \
	-I${top_srcdir}/src

AM_CXXFLAGS = ${my_CXXFLAGS}

PC_SED = \
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(SED) \
	-e 's,@VERSION\@,$(VERSION),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@exec_prefix\@,$(exec_prefix),g' \
	-e 's,@libdir\@,$(libdir),g' \
	-e 's,@includedir\@,$(includedir),g' \
	< $< > $@ || rm $@

%.pc: %.pc.in Makefile
	$(PC_SED)

lib_LTLIBRARIES = libbios++.la
libbios___la_SOURCES = \
    src/bios/bed.cc \
    src/bios/bedgraph.cc \
    src/bios/bitfield.cc \
    src/bios/blast.cc \
    src/bios/blat.cc \
    src/bios/bowtie.cc \
	src/bios/conf.cc \
    src/bios/eland.cc \
    src/bios/elandmulti.cc \
    src/bios/exportpe.cc \
    src/bios/fasta.cc \
    src/bios/fastq.cc \
    src/bios/geneontology.cc \
    src/bios/interval.cc \
    src/bios/linestream.cc \
	src/bios/misc.cc \
    src/bios/number.cc \
    src/bios/seq.cc \
    src/bios/string.cc \
	src/bios/worditer.cc

libbios___la_LIBADD = -lm -lgsl

nobase_dist_include_HEADERS = \
	src/bios/bed.hh \
	src/bios/bedgraph.hh \
	src/bios/bitfield.hh \
	src/bios/blast.hh \
	src/bios/blat.hh \
	src/bios/bowtie.hh \
	src/bios/conf.hh \
	src/bios/eland.hh \
	src/bios/elandmulti.hh \
	src/bios/exportpe.hh \
	src/bios/fasta.hh \
	src/bios/fastq.hh \
	src/bios/geneontology.hh \
	src/bios/interval.hh \
	src/bios/linestream.hh \
	src/bios/misc.hh \
	src/bios/number.hh \
	src/bios/seq.hh \
	src/bios/string.hh \
	src/bios/worditer.hh

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = biosxx.pc
EXTRA_DIST += biosxx.pc

check_LTLIBRARIES = \
	libgtest.la \
	libgtest_main.la

libgtest_la_SOURCES = \
	ext/gtest/include/gtest/gtest.h \
	ext/gtest/src/gtest-all.cc

libgtest_la_CPPFLAGS = $(GTEST_CPPFLAGS)
libgtest_la_CXXFLAGS = $(GTEST_CXXFLAGS)
libgtest_la_LDFLAGS = $(GTEST_LDFLAGS)
libgtest_la_LIBADD = $(GTEST_LIBS)

libgtest_main_la_SOURCES = \
	ext/gtest/include/gtest/gtest.h \
	ext/gtest/src/gtest_main.cc

libgtest_main_la_CPPFLAGS = $(GTEST_CPPFLAGS)
libgtest_main_la_CXXFLAGS = $(GTEST_CXXFLAGS)
libgtest_main_la_LDFLAGS = $(GTEST_LDFLAGS)
libgtest_main_la_LIBADD = $(GTEST_LIBS) libgtest.la

TEST_ENVIRONMENT =
TESTS =
UNIT_TESTS =
check_PROGRAMS = \
	test/worditer_test \
	test/bed_test

test_worditer_test_SOURCES = test/worditer_test.cc
test_worditer_test_CPPFLAGS = -I$(top_srcdir)/src $(GTEST_CPPFLAGS)
test_worditer_test_CXXFLAGS = $(GTEST_CXXFLAGS)
test_worditer_test_LDFLAGS = $(GTEST_LDFLAGS)
test_worditer_test_LDADD = $(top_srcdir)/libbios++.la \
			  $(GTEST_LIBS) \
			  $(top_srcdir)/libgtest_main.la

test_bed_test_SOURCES = test/bed_test.cc
test_bed_test_CPPFLAGS = -I$(top_srcdir)/src $(GTEST_CPPFLAGS)
test_bed_test_CXXFLAGS = $(GTEST_CXXFLAGS)
test_bed_test_LDFLAGS = $(GTEST_LDFLAGS)
test_bed_test_LDADD = $(top_srcdir)/libbios++.la \
			  $(GTEST_LIBS) \
			  $(top_srcdir)/libgtest_main.la

UNIT_TESTS += $(check_PROGRAMS)
TESTS += $(UNIT_TESTS)

TESTS_ENVIRONMENT_WITH_VALGRIND = \
	"libtool --mode=execute valgrind --leak-check=full"

.PHONY: check-with-valgrind
check-with-valgrind:
	$(MAKE) $(AM_MAKEFLAGS) check \
		TESTS_ENVIRONMENT=$(TESTS_ENVIRONMENT_WITH_VALGRIND)

.PHONY: check-unit-tests
check-unit-tests:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS=$(UNIT_TESTS)

.PHONY: check-unit-tests-with-valgrind
check-unit-tests-with-valgrind:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS=$(UNIT_TESTS) \
		TESTS_ENVIRONMENT=$(TESTS_ENVIRONMENT_WITH_VALGRIND)

# Doxygen
if USE_DOXYGEN

doxygen:
	cd doc && doxygen Doxyfile

clean-local:
	rm -rf $(top_srcdir)/doc/html

endif
