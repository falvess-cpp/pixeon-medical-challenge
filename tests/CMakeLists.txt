# 1. Definir os recursos (DEVE vir antes do add_executable)
set(TEST_RESOURCES "../resources/resources.qrc")

# 2. Fontes e Headers com Q_OBJECT
set(TEST_SOURCES
    main_tests.cpp
    ../src/core/AuthManager.cpp
    ../src/core/GlobalState.cpp
    ../src/core/PerfilModel.cpp
)

set(TEST_HEADERS
    ../include/core/GlobalState.hpp
)

# 3. Criar o executável ÚNICO com tudo incluído (Fontes + Headers + Recursos)
add_executable(PixeonTests 
    ${TEST_SOURCES} 
    ${TEST_HEADERS} 
    ${TEST_RESOURCES}
)

# 4. Configurações de Automação do Qt
set_target_properties(PixeonTests PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# 5. Includes e Links
target_include_directories(PixeonTests PRIVATE 
    ../include
    ../include/core
    ../include/ui
    ../external/logger/include
)

target_link_libraries(PixeonTests PRIVATE 
    AppLogger 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
)

# 6. Lógica de Deploy (DLLs)
get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET PixeonTests POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-compiler-runtime --verbose 0 "$<TARGET_FILE:PixeonTests>"
        COMMENT ">>> [DEPLOY] Copiando DLLs para a pasta de testes..."
    )
endif()

# 7. Execução Automática (O teste vai rodar aqui)
add_custom_command(TARGET PixeonTests POST_BUILD
    COMMAND "$<TARGET_FILE:PixeonTests>"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT ">>> [UNIT TESTS] Executando validacao automatica..."
)
