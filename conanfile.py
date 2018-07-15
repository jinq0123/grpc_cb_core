from conans import ConanFile, CMake

class GrpccbConan(ConanFile):
    name = "grpc_cb_core"
    version = "0.2"
    license = "Apache-2.0"
    url = "https://github.com/jinq0123/grpc_cb_core"
    description = "C++ gRPC core library with callback interface."
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    
    requires = "grpc/1.13.0@inexorgame/stable ",

    generators = "cmake", "Premake"  # A custom generator: PremakeGen/0.1@memsharded/testing
    build_requires = "PremakeGen/0.1@memsharded/testing"
    exports_sources = "src*", "include*", "CMakeLists.txt"

    def build(self):
        cmake = CMake(self)
        self.run('cmake %s %s' % (self.source_folder, cmake.command_line))
        self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.h")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["grpc_cb_core"]
