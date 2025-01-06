#include "filemanager.h"

#include <iostream>
#include <cstring>

FileManager::FileManager(const char* fname) : filename(fname) {
    list = new LinkedList(fname);
}

FileManager::~FileManager() {
    if (list) {
        delete list;
    }
}

bool FileManager::addRecord(const Call& call) {
    try {
        list->addCall(call);
        return true;
    } catch (const std::exception& e) {
        std::cout << "Ошибка при добавлении записи: " << e.what() << std::endl;
        return false;
    }
}

bool FileManager::deleteRecord(long logicalNumber) {
    if (logicalNumber < 1) {
        std::cout << "Некорректный номер записи\n";
        return false;
    }
    
    long offset = list->findOffsetByNumber(logicalNumber - 1);
    if (offset == -1) {
        std::cout << "Запись не найдена\n";
        return false;
    }
    return list->removeCall(offset);
}

bool FileManager::updateRecord(long logicalNumber, const Call& call) {
    if (logicalNumber < 1) {
        std::cout << "Некорректный номер записи\n";
        return false;
    }
    
    long offset = list->findOffsetByNumber(logicalNumber - 1); // -1 т.к. нумерация с 1
    if (offset == -1) {
        std::cout << "Запись не найдена\n";
        return false;
    }
    
    return list->updateCall(offset, call);
}

void FileManager::displayRecord(const Call* call) {
    if (!call) {
        std::cout << "Запись не найдена\n";
        return;
    }
    
    std::cout << " Город: " << call->city << "\n"
              << " Код города: " << call->cityCode << "\n"
              << " Тариф: " << call->tariff << "\n"
              << " Дата: " << call->callDate << "\n"
              << " Время: " << call->callTime << "\n"
              << " Длительность: " << call->duration << " мин\n"
              << " Телефон: " << call->phoneNumber << "\n"
              << " Дата оплаты: " << (strlen(call->paymentDate) > 0 ? 
                                    call->paymentDate : "Не оплачено") << "\n"
              << "------------------------\n";
}

Call* FileManager::getRecord(long logicalNumber) {
    if (logicalNumber < 1) {
        std::cout << "Некорректный номер записи\n";
        return nullptr;
    }
    
    Call* call = list->findCall(logicalNumber - 1);
    if (call) {
        displayRecord(call);
    }
    return call;
}

void FileManager::insertRecord(long logicalNumber, const Call& call) {
    if (logicalNumber < 1) {
        std::cout << "Некорректный номер записи\n";
        return;
    }
    
    long offset = list->findOffsetByNumber(logicalNumber - 1);
    if (offset == -1) {
        std::cout << "Запись не найдена\n";
        return;
    }
    list->insertAfter(offset, call);
}

void FileManager::sortRecords() {
    list->sort();
}

void FileManager::showPage(int pageNum, int recordsPerPage) {
    if (pageNum < 0 || recordsPerPage <= 0) {
        std::cout << "Некорректные параметры страницы\n";
        return;
    }
    list->displayPage(pageNum, recordsPerPage);
}

// void FileManager::compressFile() {
//     list->compress();
// }

void FileManager::compressFile() {
    if (!list) {
        std::cout << "Ошибка: список не инициализирован\n";
        return;
    }
    
    // Получаем размер до сжатия
    std::streampos sizeBefore = list->getFileSize();
    if (sizeBefore <= sizeof(long)) {
        std::cout << "Файл пуст\n";
        return;
    }
    
    // Выполняем сжатие
    list->compress();
    
    // Получаем размер после сжатия
    std::streampos sizeAfter = list->getFileSize();
    if (sizeAfter < sizeof(long)) {
        std::cout << "Ошибка при сжатии файла\n";
        return;
    }
    
    std::cout << "Размер файла до сжатия: " << sizeBefore << " байт\n"
              << "Размер файла после сжатия: " << sizeAfter << " байт\n"
              << "Уменьшение размера на: " << (sizeBefore - sizeAfter) << " байт\n";
}

void FileManager::showUnpaidCalls() {
    for (auto it = list->begin(); it != list->end(); ++it) {
        Call call = *it;
        if (isDateEmpty(call.paymentDate)) {
            std::cout << "\nГород: " << call.city
                     << "\nДата: " << call.callDate
                     << "\nВремя: " << call.callTime
                     << "\nДлительность: " << call.duration << " мин"
                     << "\nСумма: " << call.tariff * call.duration << " руб."
                     << "\n------------------------\n";
        }
    }
}

double FileManager::calculateUnpaidAmount() {
    double totalAmount = 0.0;
    for (auto it = list->begin(); it != list->end(); ++it) {
        Call call = *it;
        if (isDateEmpty(call.paymentDate)) {
            totalAmount += call.tariff * call.duration;
        }
    }
    return totalAmount;
}

void FileManager::runTests() {
    std::cout << "\n=== Тестирование операций ===\n";
    
    // Тест с int
    list->testWithInt();
    
    // Тест с double
    list->testWithDouble();
    
    // Тест со структурой Call
    list->testWithCall();
}

bool FileManager::isValidOffset(long offset) {
    return offset >= sizeof(long);
}

bool FileManager::isDateEmpty(const char* date) {
    return !date || strlen(date) == 0;
}
