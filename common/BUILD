cc_library(
    name = "common",
    srcs = glob(["*.cc"]),
    hdrs = glob([
        "*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//common/crypto:types",
        "//common/stats",
        "//proto:hub_grpc_cc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)
