#pragma once

#include "LinkedList.h"

/**
 * @brief Класс управления файлом с записями о звонках
 * Предоставляет высокоуровневый интерфейс для работы с данными
 */
class FileManager {
private:
    LinkedList* list;     // Указатель на список звонков

public:
    FileManager(const char* fname);
    ~FileManager();

    // Основные операции с записями
    bool addRecord(const Call& call);                  // Добавление записи
    bool deleteRecord(long offset);                    // Удаление записи
    bool updateRecord(long offset, const Call& call);  // Обновление записи
    void displayRecord(const Call* call);              // Вывод записи
    Call* getRecord(long offset);                      // Получение записи
    void insertRecord(long offset, const Call& call);  // Вставка записи
    void sortRecords();                                // Сортировка записей
    void showPage(int pageNum, int recordsPerPage);    // Постраничный вывод
    void compressFile();                               // Сжатие файла
    
    // Специальные операции
    void showUnpaidCalls();                            // Вывод неоплаченных звонков
    double calculateUnpaidAmount();                    // Подсчет суммы к оплате

    // Тестовые операции
    void runTests();  // Запуск всех тестов

private:
//    bool isValidOffset(long offset); // Проверка корректности смещения
    bool isDateEmpty(const char* date);                // Проверка пустой даты
};