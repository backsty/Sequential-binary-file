#pragma once

/**
 * @brief Структура для хранения информации о междугородном звонке
 * Хранит данные о звонке и смещение для реализации односвязного списка
 */
struct Call {
     // Добавляем размер строковых полей
    static const int CITY_SIZE = 50;
    static const int DATE_SIZE = 11;
    static const int TIME_SIZE = 6;
    static const int PHONE_SIZE = 15;

    char city[CITY_SIZE];        // Название города
    int cityCode;         // Телефонный код города
    double tariff;        // Тариф за минуту разговора
    char callDate[DATE_SIZE];    // Дата звонка в формате DD.MM.YYYY
    char callTime[TIME_SIZE];     // Время звонка в формате HH:MM
    int duration;         // Продолжительность разговора в минутах
    char phoneNumber[PHONE_SIZE]; // Номер телефона абонента
    char paymentDate[DATE_SIZE]; // Дата оплаты (пустая строка если не оплачено)
    long nextOffset;      // Смещение следующей записи в файле (-1 если последняя)

    Call();
    ~Call() = default;
};