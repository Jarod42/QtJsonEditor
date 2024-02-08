newoption {
  trigger = "qt-root",
  value = "path",
  description = "path of qt root (contains lib/libQt5Core.a include/Qt5Core bin)"
}

if (_ACTION == nil) then
  return
end

local qt = premake.extensions.qt

if _OPTIONS["qt-root"] ~= nil then
  QtRoot = _OPTIONS["qt-root"]
end

print("QtRoot: ", QtRoot)

workspace "qt-json-editor"
  location ( path.join("solution", _ACTION) )
  configurations { "Debug", "Release" }

  --architecture "x64"
  cppdialect "C++17"
  warnings "Extra"
  externalanglebrackets "On"
  externalwarnings "Off"

  objdir(path.join("obj", _ACTION)) -- premake add $(configName)/$(AppName)
  targetdir(path.join("bin", _ACTION, "%{cfg.buildcfg}"))

  qt.enable()
  qtuseexternalinclude ( true )

  if (QtRoot ~= nil and QtRoot ~= "") then
    qtpath(QtRoot)
  end
  qtprefix "Qt5"

  filter "configurations:Debug"
    optimize "Off"
    symbols "On"
    defines "DEBUG"
    qtsuffix "d"
  filter "configurations:Release"
    optimize "On"
    symbols "Off"
    defines { "NDEBUG", "QT_NO_DEBUG" }

  filter "toolset:gcc"
    buildoptions {"-Wno-unused-parameter"}
  filter "toolset:clang"
    buildoptions {"-Wno-unused-parameter"}
    defines {"__STDC_LIMIT_MACROS", "__STDC_CONSTANT_MACROS"}

  filter "system:windows"
    defines "WIN32"

  filter "toolset:not msc*"
    pic "On" -- position independent code required when Qt was built with -reduce-relocations

  filter {}

  startproject "qt-json-editor"
  project "qt-json-editor"
    kind "ConsoleApp"
    files { "src/main.cpp", "src/ui/**.*" }
    files { "ts/*.ts" }

    qtlreleaseargs { "-nounfinished" }
    qtqmgenerateddir "%{cfg.targetdir}"

    includedirs "src"
    includedirs "submodules/valijson/include"

    qtmodules { "core", "gui", "widgets" }

    links "qt-json-widget"

    filter {"files:ts/*_en.ts"}
      qtlreleaseargs { "-removeidentical" }
    filter {}

  project "qt-json-widget"
    kind "StaticLib"
    files { "src/widgets/**.*" }

    includedirs "src"
    includedirs "submodules/valijson/include"

    qtmodules { "core", "gui", "widgets" }

  project "qt-json-designer-plugin"
    kind "SharedLib"
    files { "src/QtDesignerPlugin/**.*" }

    includedirs "src"
    qtmodules { "core", "gui", "widgets" }

    links "qt-json-widget"

    qtmocargs { "-I%{cfg.qtincludepath}" }

    filter { "configurations:release" }
        targetdir "%{cfg.qtpath}/plugins/designer"
    filter {}
