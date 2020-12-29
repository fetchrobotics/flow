load("@flow//:bazel/flow_rules.bzl", "flow_cc_gtest")


flow_cc_gtest(
    name="captor",
    srcs=["captor.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)


flow_cc_gtest(
    name="dispatch",
    srcs=["dispatch.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)


flow_cc_gtest(
    name="dispatch_queue",
    srcs=["dispatch_queue.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)


flow_cc_gtest(
    name="synchronizer",
    srcs=["synchronizer.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)


flow_cc_gtest(
    name="synchronizer_mt_example",
    srcs=["synchronizer_mt_example.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)


flow_cc_gtest(
    name="synchronizer_st_example",
    srcs=["synchronizer_st_example.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)
