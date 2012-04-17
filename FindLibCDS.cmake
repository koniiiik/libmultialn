# Locate the LibCDS compact data structure library.

FIND_PATH(LIBCDS_INCLUDE_DIR
    NAMES
        itSequenceDArray.h
        BitSequence.h
        BitSequenceRG.h
        BitSequenceRRR.h
        BitSequenceSDArray.h
        BitString.h
    HINTS
        $ENV{LIBCDS_ROOT}/includes
        ${LIBCDS_ROOT}/includes
)
MARK_AS_ADVANCED(LIBCDS_INCLUDE_DIR)

FIND_LIBRARY(LIBCDS_LIBRARY cds
    HINTS
        $ENV{LIBCDS_ROOT}/lib
        ${LIBCDS_ROOT}/lib
)
MARK_AS_ADVANCED(LIBCDS_LIBRARY)


# handle the QUIETLY and REQUIRED arguments and set LIBCDS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibCDS DEFAULT_MSG LIBCDS_LIBRARY LIBCDS_INCLUDE_DIR)

IF(LIBCDS_FOUND)
    SET(LIBCDS_LIBRARIES ${LIBCDS_LIBRARY})
    SET(LIBCDS_INCLUDE_DIRS ${LIBCDS_INCLUDE_DIR})
ENDIF(LIBCDS_FOUND)

MARK_AS_ADVANCED(LIBCDS_LIBRARIES LIBCDS_INCLUDE_DIRS)
