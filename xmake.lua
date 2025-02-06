add_rules("mode.debug", "mode.release")

set_languages("c++23")

target("pngstrip")
    add_files("src/*.cpp")
