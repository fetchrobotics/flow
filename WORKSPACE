workspace(name="flow")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# GTest and GMock
http_archive(
    name="googletest",
    url="https://github.com/google/googletest/archive/c9ccac7cb7345901884aabf5d1a786cfa6e2f397.zip",
    sha256="8bb781f17c7a463bba95f95b29a4df4645b053e3a8d95ebcc017800664834cbb",
    build_file="@flow//:bazel/third_party/googletest.BUILD",
    strip_prefix="googletest-c9ccac7cb7345901884aabf5d1a786cfa6e2f397",
)
