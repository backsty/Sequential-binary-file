#pragma once

#include "Call.h"

#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>

class LinkedList {
private:
    std::fstream file;
    long headOffset;
    const char* filename;
    
public:
    class Iterator {
    private:
        std::fstream& file;        // Ссылка на файловый поток
        long currentOffset;        // Текущее смещение в файле
    
    public:
        Iterator(std::fstream& f, long offset);
        Call operator*();
        Iterator& operator++();
        bool operator!=(const Iterator& other);
    };

    LinkedList(const char* fname);
    ~LinkedList();

    Iterator begin();
    Iterator end();

    void addCall(const Call& call);                     // Добавление звонка
    bool removeCall(long offset);                       // Удаление звонка
    bool updateCall(long offset, const Call& call);     // Обновление звонка
    Call* findCall(long offset);                        // Поиск звонка
    void insertAfter(long offset, const Call& call);    // Вставка после указанной позиции
    void sort();                                        // Сортировка по дате
    std::streampos getFileSize();
    void compress();                                    // Сжатие файла
    void displayPage(int pageNum, int recordsPerPage);  // Постраничный вывод

    // Методы для тестирования разных типов данных
    void testWithInt();        // Тест с int
    void testWithDouble();     // Тест с double
    void testWithCall();       // Тест со структурой

    long findOffsetByNumber(long logicalNumber);

private:
    bool writeCallToFile(const Call& call, long offset);
    bool updateNextOffset(long currentOffset, long newNextOffset);
    long findLastOffset();
};