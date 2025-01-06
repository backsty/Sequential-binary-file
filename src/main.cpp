#include <iostream>
#include <cstring>
#include <limits>
#include <windows.h>

#include "filemanager.h"

void clearBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

Call inputCall() {
    Call call;
    
    std::cout << " Введите город:";
    std::cin.getline(call.city, Call::CITY_SIZE);
    
    std::cout << " Введите код города:";
    std::cin >> call.cityCode;
    clearBuffer();
    
    std::cout << " Введите тариф:";
    std::cin >> call.tariff;
    clearBuffer();
    
    std::cout << " Введите дату звонка (DD.MM.YYYY):";
    std::cin.getline(call.callDate, Call::DATE_SIZE);
    
    std::cout << " Введите время звонка (HH:MM):";
    std::cin.getline(call.callTime, Call::TIME_SIZE);
    
    std::cout << " Введите продолжительность (мин):";
    std::cin >> call.duration;
    clearBuffer();
    
    std::cout << " Введите телефон:";
    std::cin.getline(call.phoneNumber, Call::PHONE_SIZE);
    
    std::cout << " Введите дату оплаты (DD.MM.YYYY или пусто):";
    std::cin.getline(call.paymentDate, Call::DATE_SIZE);
    
    return call;
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    
    FileManager fm("calls.bin");
    int choice;
    
    while (true) {
        std::cout << "\nМеню:\n"
                  << "1. Добавить запись\n"
                  << "2. Показать запись по номеру\n"
                  << "3. Удалить запись\n"
                  << "4. Вставить запись после номера\n"
                  << "5. Редактировать запись\n"
                  << "6. Сортировать записи\n"
                  << "7. Постраничный просмотр\n"
                  << "8. Сжать файл\n"
                  << "9. Показать неоплаченные звонки\n"
                  << "10. Показать сумму к оплате\n"
                  << "11. Запустить тесты\n"
                  << "0. Выход\n"
                  << "Выберите действие: ";
        
        std::cin >> choice;
        clearBuffer();
        
        switch (choice) {
            case 1: {
                Call call = inputCall();
                if (fm.addRecord(call)) {
                    std::cout << " Запись добавлена успешно\n";
                }
                break;
            }
            case 2: {
                long number;
                std::cout << " Введите номер записи:";
                std::cin >> number;
                clearBuffer();
                
                Call* call = fm.getRecord(number);
                delete call;
                break;
            }
            case 3: {
                long number;
                std::cout << " Введите номер записи:";
                std::cin >> number;
                clearBuffer();
                
                if (fm.deleteRecord(number)) {
                    std::cout << " Запись удалена\n";
                }
                break;
            }
            case 4: {
                long number;
                std::cout << " Введите номер записи, после которой вставить:";
                std::cin >> number;
                clearBuffer();
                
                Call call = inputCall();
                fm.insertRecord(number, call);
                std::cout << " Запись вставлена\n";
                break;
            }
            case 5: {
                long number;
                std::cout << " Введите номер записи:";
                std::cin >> number;
                clearBuffer();
                
                Call call = inputCall();
                if (fm.updateRecord(number, call)) {
                    std::cout << " Запись обновлена\n";
                }
                break;
            }
            case 6: {
                fm.sortRecords();
                std::cout << " Записи отсортированы\n";
                break;
            }
            case 7: {
                int page, count;
                std::cout << " Введите номер страницы:";
                std::cin >> page;
                std::cout << " Введите количество записей на странице:";
                std::cin >> count;
                clearBuffer();
                
                fm.showPage(page, count);
                break;
            }
            case 8: {
                fm.compressFile();
                std::cout << " Файл сжат\n";
                break;
            }
            case 9: {
                fm.showUnpaidCalls();
                break;
            }
            case 10: {
                double amount = fm.calculateUnpaidAmount();
                std::cout << " Общая сумма к оплате: " << amount << " руб.\n";
                break;
            }
            case 11: {
                fm.runTests();
                break;
            }
            case 0:
                return 0;
            default:
                std::cout << " Неверный выбор\n";
        }
    }
    
    return 0;
}