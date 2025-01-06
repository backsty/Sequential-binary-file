#include "call.h"
#include <cstring>

Call::Call() {
    // Проверка корректности констант
    static_assert(CITY_SIZE > 0, "CITY_SIZE должен быть положительным");
    static_assert(DATE_SIZE == 11, "DATE_SIZE должен быть равен 11 для формата DD.MM.YYYY");
    static_assert(TIME_SIZE == 6, "TIME_SIZE должен быть равен 6 для формата HH:MM");
    static_assert(PHONE_SIZE > 0, "PHONE_SIZE должен быть положительным");

    // Инициализация строковых полей пустыми строками
    memset(city, 0, CITY_SIZE);
    memset(callDate, 0, DATE_SIZE);
    memset(callTime, 0, TIME_SIZE);
    memset(phoneNumber, 0, PHONE_SIZE);
    memset(paymentDate, 0, DATE_SIZE);
    
    // Инициализация числовых полей
    cityCode = 0;
    tariff = 0.0;
    duration = 0;
    
    // Установка признака конца списка
    nextOffset = -1;
}