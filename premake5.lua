require 'submodules/premake-qt/qt'

newoption {
  trigger = "qt-root",
  value = "path",
  description = "Path of qt root (contains lib/libQt5Core.a include/Qt5Core bin)",
  category = "script"
}

newoption {
  trigger = "qt-version",
  value = "version",
  description = "Version of Qt",
  default = "5.15.0",
  category = "script"
}

local qt = premake.extensions.qt

if _OPTIONS["qt-root"] ~= nil then
  QtRoot = _OPTIONS["qt-root"]
end

print("Environment variable Qt5_Dir:", os.getenv("Qt5_Dir"))
print("Environment variable Qt6_Dir:", os.getenv("Qt6_Dir"))
print("Environment variable QT_ROOT_DIR:", os.getenv("QT_ROOT_DIR"))
print("Environment variable QTDIR:", os.getenv("QTDIR"))
print("Environment variable QT_DIR:", os.getenv("QT_DIR"))
print("QtRoot: ", QtRoot)

workspace "qt-json-editor"
  location "solution/%{_ACTION}"
  configurations { "Debug", "Release" }
  platforms { "x32", "x64" }

  cppdialect "C++17"
  warnings "Extra"
  externalanglebrackets "On"
  externalwarnings "Off"

  objdir "obj/%{_ACTION}" -- premake adds $(platformName)/$(configName)/$(AppName)
  targetdir "bin/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}"

  qt.enable(string.sub(_OPTIONS["qt-version"], 1, 1))
  qtuseexternalinclude ( true )

  if (QtRoot ~= nil and QtRoot ~= "") then
    qtpath(QtRoot)
  end
  qtversion ( _OPTIONS["qt-version"] )

if premake.checkVersion(_OPTIONS["qt-version"], '>=6.0 <6.1') and os.target() ~= premake.WINDOWS then
  forceincludes "src/Qt6_0_workaround.h"
end

  filter "platforms:x32"
    architecture "x32"
  filter "platforms:x64"
    architecture "x64"
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
    kind "WindowedApp"
    files { "src/QtJsonEditor/**.*" }

    includedirs "src"

    qtmodules { "core", "gui", "widgets" }
    qtmain(true)

    links { "qt-json-editor-dialog", "qt-json-widget" }

    filter { "system:windows" }
      postbuildcommands { '"%{cfg.qtpath}/bin/windeployqt.exe" "%{cfg.buildtarget.abspath}"' }
    filter {}

  project "qt-json-editor-dialog"
    kind "StaticLib"
    files { "src/QtJsonEditorDialog/**.*" }
    files { "ts/*.ts" }

    qtlreleaseargs { "-nounfinished" }
    qtqmgenerateddir "%{cfg.targetdir}"
    qtmodules { "core", "gui", "widgets" }

  if _ACTION == "gmake" or _ACTION == "codeblocks" then
    -- qrelease doesn't create intermediate directory
    -- and those actions don't create sub-directories neither for custombuild
    -- So do it as pre-build step
    prebuildcommands { "{MKDIR} %{cfg.qtqmgenerateddir}" }
  end

    includedirs "src"
    includedirs "submodules/valijson/include"

    filter {"files:ts/*_en.ts"}
      qtlreleaseargs { "-removeidentical" }
    filter {}

  project "qt-json-widget"
    kind "StaticLib"
    files { "src/QtJsonWidget/**.*" }

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
