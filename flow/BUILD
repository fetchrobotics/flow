load("@flow//:bazel/flow_rules.bzl", "flow_cc_library")

flow_cc_library(
    name="utility",
    hdrs=glob(["include/utility/*.hpp",]),
    include_prefix="flow",
    strip_include_prefix="//flow/include",
    visibility=["//visibility:public"],
    deps=[],
)

flow_cc_library(
    name="flow_impl",
    hdrs=glob(["src/**/*.hpp",]),
    visibility=["//visibility:private"],
    deps=[
        ":utility"
    ],
)

flow_cc_library(
    name="flow",
    hdrs=glob([
        "include/*.hpp",
        "include/captor/*.hpp",
        "include/dispatch/*.hpp",
        "include/driver/*.hpp",
        "include/follower/*.hpp",
    ]),
    include_prefix="flow",
    includes=["include"],
    strip_include_prefix="//flow/include",
    visibility=["//visibility:public"],
    deps=[
        ":flow_impl",
        ":utility"
    ],
)
