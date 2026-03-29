def ctre_tests(name, srcs, deps = []):
    for src in srcs:
        test_name = src.replace("/", "_").replace(".cpp", "")
        native.cc_library(
            name = "ctre_test_" + test_name,
            srcs = [src],
            deps = deps,
        )
