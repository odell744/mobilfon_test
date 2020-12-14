# CMakeList.txt: проект CMake для mobilfon_test; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project ("mobilfon_test")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (mobilfon_test "mobilfon_test.cpp" "mobilfon_test.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
