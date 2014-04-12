set(BIOSXX_LIB_NAME "biosxx")
set(BIOSXX_STATIC_LIB_NAME "biosxx_static")

list(APPEND BIOSXX_SOURCES
  bed.cc
  bedgraph.cc
  bitfield.cc
  blast.cc
  blat.cc
  bowtie.cc
  conf.cc
  eland.cc
  elandmulti.cc
  exportpe.cc
  fasta.cc
  fastq.cc
  geneontology.cc
  interval.cc
  linestream.cc
  misc.cc
  number.cc
  seq.cc
  string.cc
  worditer.cc)

add_library(biosxx_core OBJECT ${BIOSXX_SOURCES})
add_library(${BIOSXX_LIB_NAME} SHARED $<TARGET_OBJECTS:biosxx_core>)
add_library(${BIOSXX_STATIC_LIB_NAME} STATIC $<TARGET_OBJECTS:biosxx_core>)
set_target_properties(biosxx_static PROPERTIES OUTPUT_NAME biosxx)
target_link_libraries(${BIOSXX_LIB_NAME} ${LIBS})
