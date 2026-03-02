add_library(meta_utils)
target_sources(meta_utils
    PUBLIC
        FILE_SET CXX_MODULES FILES
            meta_utils/concepts.cppm
            meta_utils/math.cppm
            meta_utils/fixed_string.cppm
            meta_utils/for_constexpr.cppm
            meta_utils/meta_utils.cppm
            meta_utils/overload.cppm
            meta_utils/sequence.cppm
            meta_utils/trait.cppm
            meta_utils/tuple.cppm
            # meta_utils/type_math.cppm
            meta_utils/variant.cppm
)
