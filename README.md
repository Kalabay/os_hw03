# Отчёт, Калабай Михаил Иванович БПИ219
## Работа претендует на 5 баллов, Вариант 19

19. Задача о болтунах. N болтунов имеют телефоны. Они либо ждут
звонков, либо звонят друг другу, чтобы побеседовать. Выбор этого
состояния осуществляется случайно и равновероятно. Если телефон случайного абонента занят, болтун будет звонить другому абоненту, пока ему кто-нибудь не ответит. Побеседовав некоторое время, болтун равновероятно или ждет звонка, или звонит на другой
случайный номер. Создать приложение, моделирующее поведение болтунов. Каждый болтун — отдельный клиент. Сервер
получает число болтунов при запуске и является коммутатором.
Запуск клиентов можно осуществлять скриптом или вручную.

## Решение
Для проверок свободности болтунов я использую именованые семафоры (и процессы). Внутри кода есть параметр exec_time, 
который означает сколько секунд длится симуляция. По истечении времени сразу конец всего. 
Сервер-коммутатор главный, он решает, кто я является звонящим, а кто ждёт звонка и т.д.
А также сервер посылает информацию клиентам-болтунам, кому из свободных болтун должен позвонить.
Далее в зависимости от сценария клиент звонит или слушает звонящего (сделал через sleep(10))

## Про решение на 4-5 
[код-сервер](https://github.com/Kalabay/os_hw03/blob/main/mainserver.c)
[код-клиент](https://github.com/Kalabay/os_hw03/blob/main/callclient.c)
Передаю через ввод в терминале серверу аргументы = количество болтунов и порт.
Передаю через ввод в терминале клиенту аргументы = адресс и порт


### Пример
Вот разобранный пример, где двое сначала говорят, а потом оказывается, что оба звонят, поэтому продолжение нет.
![hw003](https://github.com/Kalabay/os_hw03/assets/90344366/e91b2402-743e-4d6c-8239-17fc44d26d70)
