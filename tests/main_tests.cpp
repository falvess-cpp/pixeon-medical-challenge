#include <iostream>
#include <string>
#include <QCoreApplication>
#include "core/AuthManager.hpp"
#include "core/GlobalState.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QResource>

using namespace std;

int tests_passed = 0;
int tests_total = 0;

void test_assert(bool condition, const string& name) {
    ++tests_total;
    if (condition) {
        cout << " [V] PASS: " << name << endl;
        ++tests_passed;
    } else {
        cout << " [X] FAIL: " << name << endl;
    }
}

void test_auth_logic() {
    test_assert(AuthManager::validateToken("123456") == true,  "Token_Sistema_Valido");
    test_assert(AuthManager::validateToken("000000") == false, "Token_Incorreto_Rejeitado");
    test_assert(AuthManager::validateToken("") == false,       "Token_Vazio_Rejeitado");
}

void test_path_portability() {
    QString path = GlobalState::getExamsPath("CM001");
    
    bool hasStructure = path.contains("data") && path.contains("exames") && path.contains("CM001");
    test_assert(hasStructure, "Path_Mantem_Estrutura_Logica");

    bool hasValidSeparators = path.contains("/") || path.contains("\\");
    test_assert(hasValidSeparators, "Path_Usa_Separadores_Validos");
}

void test_global_state() {
    QJsonObject testData;
    testData["version"] = "1.0";
    GlobalState::instance().setData(testData);
    test_assert(GlobalState::instance().getData()["version"].toString() == "1.0", "GlobalState_Integrity");
}

void test_resource_json_integration() {

    QString resourcePath = ":/pacientes.json"; 
    QFile file(resourcePath);
    
    bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
    test_assert(opened == true, "Resource_File_Exists_And_Opens");

    if (opened) {

        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        test_assert(!doc.isNull(), "Resource_JSON_Is_Valid");
        
        QJsonObject root = doc.object();
        QJsonArray pacientes = root["pacientes"].toArray();
        
        bool foundJoao = false;
        for (int i = 0; i < pacientes.size(); ++i) {
            QJsonObject p = pacientes.at(i).toObject();
            if (p["id"].toString() == "CM001") {
                foundJoao = true;
                break;
            }
        }
        test_assert(foundJoao == true, "Integration_Found_CM001_In_Resources");
        
        file.close();
    }
}

int main(int argc, char *argv[]) {

    QCoreApplication app(argc, argv);
	
	Q_INIT_RESOURCE(resources); 

    cout << "\n=========================================" << endl;
    cout << " PIXEON CHALLENGE - AUTOMATED TESTS      " << endl;
    cout << " Autor: Fabiano Souza                    " << endl;
    cout << "=========================================" << endl;

    test_auth_logic();
    test_path_portability();
    test_global_state();
	test_resource_json_integration();

    cout << "=========================================" << endl;
    cout << " RESULTADO FINAL: " << tests_passed << "/" << tests_total << endl;
    
    if (tests_passed == tests_total && tests_total > 0) {
        cout << " >>> STATUS: SUCESSO TOTAL [OK] <<< " << endl;
    } else {
        cout << " >>> STATUS: FALHA ENCONTRADA [X] <<< " << endl;
    }
    cout << "=========================================\n" << endl;

    return (tests_passed == tests_total) ? 0 : 1;
}
