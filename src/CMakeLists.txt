add_library(meta_utils)
target_sources(meta_utils
    PUBLIC
        FILE_SET CXX_MODULES FILES
            meta_utils/meta_utils.cppm
            meta_utils/const_math.cppm
            meta_utils/fixed_string.cppm
            meta_utils/sequence.cppm
            meta_utils/types.cppm
)
