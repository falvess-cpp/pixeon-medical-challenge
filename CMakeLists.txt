cmake_minimum_required(VERSION 3.14)

# Dica: Força o CMake a procurar o compilador se o gerador automático falhar
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER cl.exe)
endif()

project(PixeonChallenge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# Habilita o MOC, RCC e UIC do Qt automaticamente
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 1. Encontrar o Qt (Necessário para a UI e Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

# 2. Configurar a Biblioteca de Log Externa
# Criamos uma biblioteca estática com os seus arquivos existentes
set(LOGGER_SOURCES
    external/logger/src/fsdate.cpp
    external/logger/src/fserror.cpp
    external/logger/src/fsexc.cpp
    external/logger/src/fslogger.cpp
)

add_library(AppLogger STATIC ${LOGGER_SOURCES})
target_include_directories(AppLogger PUBLIC external/logger/include)

# 3. Listar os fontes do projeto principal
# Certifique-se de que esses arquivos existem em src/core e src/ui
set(PROJECT_SOURCES
    src/main.cpp
    src/core/GlobalState.cpp
	src/core/AuthManager.cpp
	src/core/PerfilModel.cpp
	src/core/LoginController.cpp
	src/core/MainController.cpp
    src/core/Worker.cpp
    src/ui/MainWindow.cpp
    src/ui/LoginView.cpp
	src/ui/PatientSelectionView.cpp
	src/ui/MedicalRecordView.cpp
    src/ui/DoctorView.cpp
    src/ui/ReceptionView.cpp
    include/ui/MainWindow.hpp
    include/ui/LoginView.hpp
    include/ui/DoctorView.hpp
    include/ui/ReceptionView.hpp
    include/core/GlobalState.hpp
	include/core/AuthManager.hpp
	include/core/PerfilModel.hpp
	include/core/LoginController.hpp
	include/ui/PatientSelectionView.hpp
	include/ui/MedicalRecordView.hpp
	include/core/MainController.hpp
)

qt_add_resources(RESOURCES_RCC "resources/resources.qrc")

# 2. No add_executable, mantenha como está
add_executable(${PROJECT_NAME} 
    ${PROJECT_SOURCES} 
    ${RESOURCES_RCC}
)


# 5. Linkar as dependências
# Linkamos o seu Logger e as bibliotecas do Qt
target_link_libraries(${PROJECT_NAME} PRIVATE 
    AppLogger 
    Qt6::Widgets
	Qt6::Gui
	Qt6::Core
)

# 6. Incluir pastas de cabeçalho do projeto
# Isso permite que você faça #include "MainWindow.hpp" sem o caminho completo
target_include_directories(${PROJECT_NAME} PRIVATE 
    include
	include/core 
    include/ui
    external/logger/include
)

enable_testing()
add_subdirectory(tests)

# 7. Gestão do Arquivo de Configuração (.ini)
# Copia o settings.ini para a pasta de build para facilitar o teste
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config/settings.ini")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config/settings.ini"
        "${CMAKE_CURRENT_BINARY_DIR}/settings.ini"
        COPYONLY
    )
endif()

# 8. Setup para Windows (Garantir que as DLLs do Qt sejam encontradas se necessário)
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()


