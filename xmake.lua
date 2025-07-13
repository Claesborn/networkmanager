add_rules("mode.debug", "mode.release")

set_languages("c++20")
add_cxxflags("-Wall")

-- Add dependencies
-- add_requires("imgui-sfml")

target("networkmanager")
    set_kind("binary")
    add_files("src/*.cpp")
    set_targetdir("build")

    -- Link X11
--    add_syslinks("X11")