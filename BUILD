load("@flow//:bazel/flow_rules.bzl", "flow_cc_library")

flow_cc_library(
    name="utility",
    hdrs=glob(["include/flow/utility/*.hpp",]),
    strip_include_prefix="//include",
    visibility=["//visibility:public"],
    deps=[],
)

flow_cc_library(
    name="flow_impl",
    hdrs=glob(["include/flow/impl/**/*.hpp",]),
    visibility=["//visibility:private"],
    strip_include_prefix="//include",
    deps=[
        ":utility"
    ],
)

flow_cc_library(
    name="flow",
    hdrs=glob([
        "include/flow/*.hpp",
        "include/flow/captor/*.hpp",
        "include/flow/dispatch/*.hpp",
        "include/flow/driver/*.hpp",
        "include/flow/follower/*.hpp",
    ]),
    includes=["include"],
    strip_include_prefix="//include",
    visibility=["//visibility:public"],
    deps=[
        ":flow_impl",
        ":utility"
    ],
)
