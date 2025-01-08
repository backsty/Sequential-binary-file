#include "linkedlist.h"

#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <io.h>


// Конструктор - открывает или создает файл
LinkedList::LinkedList(const char* fname) : filename(fname), headOffset(-1) {
    if (!fname) {
        throw std::invalid_argument("Имя файла не может быть nullptr");
    }

    file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        file.open(filename, std::ios::binary | std::ios::out);
        file.write(reinterpret_cast<char*>(&headOffset), sizeof(long));
        file.close();
        file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    } else {
        file.seekg(0);
        file.read(reinterpret_cast<char*>(&headOffset), sizeof(long));
    }
}

// Деструктор - закрывает файл
LinkedList::~LinkedList() {
    if (file.is_open()) {
        file.close();
    }
}

// Реализация методов Iterator
LinkedList::Iterator::Iterator(std::fstream& f, long offset) 
    : file(f), currentOffset(offset) {}

Call LinkedList::Iterator::operator*() {
    Call call;
    if (currentOffset >= 0) {
        file.seekg(currentOffset);
        file.read(reinterpret_cast<char*>(&call), sizeof(Call));
    }
    return call;
}

LinkedList::Iterator& LinkedList::Iterator::operator++() {
    if (currentOffset >= 0) {
        Call call;
        file.seekg(currentOffset);
        file.read(reinterpret_cast<char*>(&call), sizeof(Call));
        currentOffset = call.nextOffset;
    }
    return *this;
}

bool LinkedList::Iterator::operator!=(const Iterator& other) {
    return currentOffset != other.currentOffset;
}

// Методы для работы с итератором
LinkedList::Iterator LinkedList::begin() {
    return Iterator(file, headOffset);
}

LinkedList::Iterator LinkedList::end() {
    return Iterator(file, -1);
}

long LinkedList::findLastOffset() {
    if (headOffset == -1) return -1;
    
    long currentOffset = headOffset;
    Call call;
    
    while (true) {
        file.seekg(currentOffset);
        file.read(reinterpret_cast<char*>(&call), sizeof(Call));
        if (call.nextOffset == -1) break;
        currentOffset = call.nextOffset;
    }
    
    return currentOffset;
}

bool LinkedList::writeCallToFile(const Call& call, long offset) {
    file.clear();
    file.seekp(offset);
    if (!file.good()) return false;
    
    file.write(reinterpret_cast<const char*>(&call), sizeof(Call));
    return file.good();
}

bool LinkedList::updateNextOffset(long currentOffset, long newNextOffset) {
    Call call;
    file.clear();
    file.seekg(currentOffset);
    if (!file.good()) return false;
    
    if (!file.read(reinterpret_cast<char*>(&call), sizeof(Call))) {
        return false;
    }
    
    call.nextOffset = newNextOffset;
    return writeCallToFile(call, currentOffset);
}

long LinkedList::findOffsetByNumber(long logicalNumber) {
    if (logicalNumber < 0 || headOffset == -1) return -1;
    
    long currentOffset = headOffset;
    long currentNumber = 0;
    
    while (currentOffset != -1 && currentNumber < logicalNumber) {
        Call current;
        file.seekg(currentOffset);
        if (!file.read(reinterpret_cast<char*>(&current), sizeof(Call))) {
            return -1;
        }
        currentOffset = current.nextOffset;
        currentNumber++;
    }
    
    return currentOffset;
}



void LinkedList::addCall(const Call& call) {
    file.clear();
    file.seekp(0, std::ios::end);
    if (!file.good()) {
        std::cout << "Ошибка позиционирования в файле\n";
        return;
    }
    
    long currentPos = file.tellp();
    if (currentPos == -1) {
        std::cout << "Ошибка определения позиции в файле\n";
        return;
    }
    
    // Если файл новый или пустой
    if (currentPos <= sizeof(long)) {
        headOffset = sizeof(long);
        file.seekp(0);
        if (!file.write(reinterpret_cast<char*>(&headOffset), sizeof(long))) {
            std::cout << "Ошибка записи заголовка\n";
            return;
        }
        currentPos = headOffset;
    }
    
    Call newCall = call;
    newCall.nextOffset = -1;
    
    if (headOffset == sizeof(long) && currentPos == sizeof(long)) {
        // Первая запись
        if (!writeCallToFile(newCall, headOffset)) {
            std::cout << "Ошибка записи первого элемента\n";
            return;
        }
    } else {
        // Добавление в конец списка
        long lastOffset = findLastOffset();
        if (lastOffset != -1) {
            if (!updateNextOffset(lastOffset, currentPos)) {
                std::cout << "Ошибка обновления связи\n";
                return;
            }
        }
        if (!writeCallToFile(newCall, currentPos)) {
            std::cout << "Ошибка записи элемента\n";
            return;
        }
    }
    file.flush();
}

bool LinkedList::removeCall(long offset) {
    file.clear();
    if (offset < 0) return false;

    // Если удаляем первый элемент
    if (offset == headOffset) {
        Call current;
        file.seekg(offset);
        if (!file.read(reinterpret_cast<char*>(&current), sizeof(Call))) {
            return false;
        }

        headOffset = current.nextOffset;
        file.seekp(0);
        file.write(reinterpret_cast<char*>(&headOffset), sizeof(long));
        file.flush();
        return true;
    }

    // Поиск предыдущей записи
    long prevOffset = headOffset;
    while (prevOffset != -1) {
        Call prev;
        file.seekg(prevOffset);
        if (!file.read(reinterpret_cast<char*>(&prev), sizeof(Call))) {
            return false;
        }

        if (prev.nextOffset == offset) {
            Call current;
            file.seekg(offset);
            if (!file.read(reinterpret_cast<char*>(&current), sizeof(Call))) {
                return false;
            }

            prev.nextOffset = current.nextOffset;
            writeCallToFile(prev, prevOffset);
            file.flush();
            return true;
        }
        prevOffset = prev.nextOffset;
    }
    return false;
}

bool LinkedList::updateCall(long offset, const Call& call) {
    if (offset < 0) return false;

    Call current;
    file.seekg(offset);
    if (!file.read(reinterpret_cast<char*>(&current), sizeof(Call))) {
        return false;
    }
    
    // Сохраняем nextOffset
    Call newCall = call;
    newCall.nextOffset = current.nextOffset;
    
    return writeCallToFile(newCall, offset);
}

Call* LinkedList::findCall(long logicalNumber) {
    long offset = findOffsetByNumber(logicalNumber);
    if (offset < 0) return nullptr;
    
    Call* call = new Call();
    file.clear();
    file.seekg(offset);
    
    if (!file.read(reinterpret_cast<char*>(call), sizeof(Call))) {
        delete call;
        return nullptr;
    }
    return call;
}

void LinkedList::insertAfter(long offset, const Call& call) {
    file.clear();
    if (offset < 0) {
        addCall(call);
        return;
    }

    Call current;
    file.seekg(offset);
    if (!file.read(reinterpret_cast<char*>(&current), sizeof(Call))) {
        return;
    }

    file.seekp(0, std::ios::end);
    long newOffset = file.tellp();

    Call newCall = call;
    newCall.nextOffset = current.nextOffset;
    
    if (!writeCallToFile(newCall, newOffset)) {
        return;
    }

    current.nextOffset = newOffset;
    writeCallToFile(current, offset);
    file.flush();
}

void LinkedList::sort() {
    if (headOffset == -1) return;

    auto compareDates = [](const char* date1, const char* date2) -> bool {
        int day1, month1, year1;
        int day2, month2, year2;
        
        sscanf(date1, "%d.%d.%d", &day1, &month1, &year1);
        sscanf(date2, "%d.%d.%d", &day2, &month2, &year2);
        
        if (year1 != year2) return year1 > year2;
        if (month1 != month2) return month1 > month2;
        return day1 > day2;
    };

    bool swapped;
    do {
        swapped = false;
        long currentOffset = headOffset;

        while (currentOffset != -1) {
            Call current, next;
            file.seekg(currentOffset);
            if (!file.read(reinterpret_cast<char*>(&current), sizeof(Call))) {
                std::cout << "Ошибка чтения текущей записи\n";
                return;
            }

            if (current.nextOffset != -1) {
                file.seekg(current.nextOffset);
                if (!file.read(reinterpret_cast<char*>(&next), sizeof(Call))) {
                    std::cout << "Ошибка чтения следующей записи\n";
                    return;
                }

                // Изменено условие сравнения дат
                if (compareDates(current.callDate, next.callDate)) {
                    long currentNext = current.nextOffset;
                    long nextNext = next.nextOffset;

                    std::swap(current.city, next.city);
                    std::swap(current.cityCode, next.cityCode);
                    std::swap(current.tariff, next.tariff);
                    std::swap(current.callDate, next.callDate);
                    std::swap(current.callTime, next.callTime);
                    std::swap(current.duration, next.duration);
                    std::swap(current.phoneNumber, next.phoneNumber);
                    std::swap(current.paymentDate, next.paymentDate);

                    current.nextOffset = currentNext;
                    next.nextOffset = nextNext;

                    if (!writeCallToFile(current, currentOffset) || 
                        !writeCallToFile(next, current.nextOffset)) {
                        std::cout << "Ошибка записи при сортировке\n";
                        return;
                    }
                    swapped = true;
                }
            }
            currentOffset = current.nextOffset;
        }
    } while (swapped);

    file.flush();
}

std::streampos LinkedList::getFileSize() {
    file.seekg(0, std::ios::end);
    std::streampos size = file.tellg();
    file.seekg(0, std::ios::beg);
    return size;
}

void LinkedList::compress() {
    if (headOffset == -1) return;

    // Создаем временный файл точного размера
    std::fstream tempFile("temp.bin", std::ios::binary | std::ios::out | std::ios::trunc);
    if (!tempFile) return;

    // Записываем начальное смещение
    long newOffset = sizeof(long);
    tempFile.write(reinterpret_cast<char*>(&newOffset), sizeof(long));

    // Копируем только активные записи подряд
    long readPos = headOffset;
    long writePos = sizeof(long);
    
    while (readPos != -1) {
        Call record;
        file.clear();
        file.seekg(readPos);
        
        if (!file.read(reinterpret_cast<char*>(&record), sizeof(Call))) {
            break;
        }

        // Сохраняем текущее смещение следующей записи
        long nextReadPos = record.nextOffset;

        // Обновляем смещение для новой последовательной структуры
        record.nextOffset = (nextReadPos != -1) ? writePos + sizeof(Call) : -1;

        // Записываем запись в новую позицию
        tempFile.clear();
        tempFile.seekp(writePos);
        tempFile.write(reinterpret_cast<char*>(&record), sizeof(Call));

        // Переходим к следующей записи
        readPos = nextReadPos;
        writePos += sizeof(Call);
    }

    // Закрываем файлы для замены
    tempFile.close();
    file.close();

    // Заменяем файлы
    remove(filename);
    rename("temp.bin", filename);

    // Открываем сжатый файл
    file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    headOffset = sizeof(long);
}

void LinkedList::displayPage(int pageNum, int recordsPerPage) {
    if (headOffset == -1) {
        std::cout << "Файл пуст\n";
        return;
    }

    long currentOffset = headOffset;
    int currentRecord = 0;
    int startRecord = pageNum * recordsPerPage;
    bool recordsFound = false;

    // Пропускаем записи до нужной страницы
    while (currentOffset != -1 && currentRecord < startRecord) {
        Call current;
        file.seekg(currentOffset);
        if (!file.read(reinterpret_cast<char*>(&current), sizeof(Call))) {
            return;
        }
        currentOffset = current.nextOffset;
        currentRecord++;
    }

    // Выводим записи текущей страницы
    int recordsOnPage = 0;
    while (currentOffset != -1 && recordsOnPage < recordsPerPage) {
        Call current;
        file.seekg(currentOffset);
        if (!file.read(reinterpret_cast<char*>(&current), sizeof(Call))) {
            return;
        }

        std::cout << "\nЗапись #" << (currentRecord + 1) << ":\n"
                  << "Город: " << current.city << "\n"
                  << "Код города: " << current.cityCode << "\n"
                  << "Тариф: " << current.tariff << "\n"
                  << "Дата: " << current.callDate << "\n"
                  << "Время: " << current.callTime << "\n"
                  << "Длительность: " << current.duration << " мин\n"
                  << "Телефон: " << current.phoneNumber << "\n"
                  << "Дата оплаты: " << (strlen(current.paymentDate) > 0 ? 
                                       current.paymentDate : "Не оплачено") << "\n"
                  << "------------------------\n";

        currentOffset = current.nextOffset;
        currentRecord++;
        recordsOnPage++;
        recordsFound = true;
    }

    if (!recordsFound) {
        std::cout << "Нет записей на данной странице\n";
    }
}

// Тестирование с типом int
void LinkedList::testWithInt() {
    std::cout << "\nТестирование с типом int:\n";
    
    // Тест добавления
    int testValues[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) {
        Call testCall;
        testCall.cityCode = testValues[i];
        addCall(testCall);
        std::cout << "Добавлено значение: " << testValues[i] << "\n";
    }
    
    // Тест чтения
    displayPage(0, 5);
}

// Тестирование с типом double
void LinkedList::testWithDouble() {
    std::cout << "\nТестирование с типом double:\n";
    
    // Тест добавления
    double testValues[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    for (int i = 0; i < 5; i++) {
        Call testCall;
        testCall.tariff = testValues[i];
        addCall(testCall);
        std::cout << "Добавлено значение: " << testValues[i] << "\n";
    }
    
    // Тест чтения
    displayPage(0, 5);
}

// Тестирование со структурой Call
void LinkedList::testWithCall() {
    std::cout << "\nТестирование со структурой Call:\n";
    
    // Создаем тестовую запись
    Call testCall;
    strcpy(testCall.city, "Тестовый город");
    testCall.cityCode = 123;
    testCall.tariff = 1.5;
    strcpy(testCall.callDate, "01.01.2024");
    strcpy(testCall.callTime, "12:00");
    testCall.duration = 10;
    strcpy(testCall.phoneNumber, "9991234567");
    strcpy(testCall.paymentDate, "02.01.2024");
    
    // Тест добавления
    addCall(testCall);
    std::cout << "Добавлена тестовая запись\n";
    
    // Тест чтения
    displayPage(0, 1);
}
