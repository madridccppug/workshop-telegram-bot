import os

from conans import ConanFile, CMake, tools


class TelegramWordnet(ConanFile):
    settings = "os", "compiler", "build_type", "arch", "cppstd"
    generators = "cmake"

    def requirements(self):
        self.requires("tgbot_cpp/1.1@jgsogo/stable")
        self.requires("fmt/5.2.1@bincrafters/stable")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

