import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class batch_file_renameRecipe(ConanFile):
    name = "batch-file-rename"
    version = "0.1"
    package_type = "application"

    license = "BSD"
    author = "gaojunxin gjx.xin@qq.com"
    url = "https://git.gaojunxin.cn/gjx-projects/my-ssh.git"
    description = "一个轻巧快速的批量文件重命名工具，支持多种重命名规则和批量操作。"
    topics = ("c++", "wxwidgets", "gjx", "batch-file-rename")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def source(self):
        work_path = "wxWidgets"
        if not os.path.exists("wxWidgets"):
            self.run("git clone https://github.com/wxWidgets/wxWidgets.git --depth=1")
            self.run("git submodule update --init --recursive --depth=1", cwd=work_path)
        if self.settings.os == "Windows":
            self.run("cmake . -G \"MinGW Makefiles\" -DCMAKE_INSTALL_PREFIX=../libs/wxWidgets", cwd=work_path)
        else:
            self.run("cmake . -DCMAKE_INSTALL_PREFIX=../libs/wxWidgets", cwd=work_path)
        self.run("cmake --build . --target install --parallel 10", cwd=work_path)




