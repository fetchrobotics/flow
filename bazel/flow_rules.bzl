def __flow_copts(name, sanitize=False, debug_build=False, asserts_as_exceptions=False):
    '''
    Common copts used in custom macros
    '''
    copts = ["-std=c++1y", "-Werror", "-Wall", "-Wextra", "-Wno-deprecated-declarations", "-Wno-unused-parameter", "-g"]

    if debug_build or sanitize:
        # No optimizations
        copts += ["-O0"]

        # Log about this
        print("[" + name + "] being built in DEBUG mode")
    else:
        # Full optimizations; no debug asserts
        copts += ["-O3", "-DNDEBUG"]

    # AddressSanitizer options
    if sanitize:
        print("[" + name + "] being built with 'AddressSanitizer' copts")
        copts += ["-fsanitize=address"]

    return copts


def __flow_linkopts(name, sanitize=False):
    '''
    Common linkopts used in custom macros
    '''

    # AddressSanitizer options
    if sanitize:
        print("[" + name + "] being built with 'AddressSanitizer' linkopts")
        return ["-lasan"]

    return []


def flow_cc_library(name, copts=[], linkopts=[], sanitize=False, debug_build=False, **kwargs):
    '''
    A wrapper around cc_library.
    Adds options to the compilation command.
    '''
    default_copts = __flow_copts(name=name,
                                 sanitize=sanitize,
                                 debug_build=debug_build,
                                 asserts_as_exceptions=debug_build)

    default_linkopts = __flow_linkopts(name=name,
                                       sanitize=sanitize)

    native.cc_library(
        name=name,
        copts=copts + default_copts,
        linkopts=linkopts + default_linkopts,
        **kwargs
    )


def flow_cc_binary(name, copts=[], linkopts=[], sanitize=False, debug_build=False, **kwargs):
    '''
    A wrapper around cc_binary.
    Adds options to the compilation command.
    '''

    default_copts = __flow_copts(name=name,
                                 sanitize=sanitize,
                                 debug_build=debug_build,
                                 asserts_as_exceptions=debug_build)

    default_linkopts = __flow_linkopts(name=name,
                                       sanitize=sanitize)

    native.cc_binary(
        name=name,
        copts=copts + default_copts,
        linkopts=linkopts + default_linkopts,
        **kwargs
    )


def flow_cc_test(name, copts=[], linkopts=[], sanitize=False, debug_build=False, **kwargs):
    '''
    A wrapper around cc_test.
    Adds options to the compilation command.
    '''

    default_copts = __flow_copts(name=name,
                                 sanitize=sanitize,
                                 debug_build=debug_build,
                                 asserts_as_exceptions=True)

    default_linkopts = __flow_linkopts(name=name,
                                       sanitize=sanitize)
    native.cc_test(
        name=name,
        copts=copts + default_copts,
        linkopts=linkopts + default_linkopts,
        **kwargs
    )


def flow_cc_gtest(name, copts=[], linkopts=[], deps=[], sanitize=False, debug_build=False, **kwargs):
    '''
    A wrapper around cc_test for gtests
    Adds options to the compilation command.
    '''
    _GTEST_COPTS = [
        "-Iexternal/googletest/googletest/include",
        "-Iexternal/googletest/googlemock/include"
    ]


    _GTEST_DEPS = [
        "@googletest//:gtest",
        "@googletest//:gmock",
    ]

    flow_cc_test(name=name,
                 copts=_GTEST_COPTS + copts,
                 deps=_GTEST_DEPS + deps,
                 linkopts=linkopts,
                 sanitize=sanitize,
                 debug_build=debug_build,
                 **kwargs)


def create_all_flow_cc_gtests(main, testcase_file_patterns):
    """
    Automatically creates gtests from a lists of files, located with glob
    """
    unit_test_files = native.glob(testcase_file_patterns)
    for file in unit_test_files:
        no_ext = file.split('.')[0]
        testcase = no_ext.split('/')[-1]
        flow_cc_gtest(
            name=testcase,
            srcs=[file, main],
            deps=["//:flow"],
            timeout="short",
        )
