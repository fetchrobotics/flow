load("@flow//:bazel/flow_rules.bzl", "flow_cc_gtest")


flow_cc_gtest(
    name="batch",
    srcs=["batch.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)


flow_cc_gtest(
    name="chunk",
    srcs=["chunk.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)


flow_cc_gtest(
    name="next",
    srcs=["next.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)


flow_cc_gtest(
    name="throttled",
    srcs=["throttled.cpp"],
    deps=["//flow:flow"],
    timeout="short",
)
