# TestProject

Консольное приложение для расчета медианы цен из CSV-файлов с потоковыми данными биржевых торгов.

## Описание
Проект считывает CSV-файлы формата:  receive_ts;exchange_ts;price;quantity;side;[rebuild - опционально]
Данные сортируются по временной метке `receive_ts`, вычисляется медиана цены, проверяется изменение медианы, и в выходной файл (по умолчанию `output.csv`) записываются временные метки изменения медианы и сама медиана в формате:  receive_ts;price
Настройка входного и выходного каталога, а также масок файлов, производится через конфигурационный файл `config.toml`. При отсутствии маски читаются все файлы.  

Проект использует CMake для сборки, а зависимости подключаются через FetchContent:  
- `tomlplusplus 3.4.0`  
- `spdlog 1.13.0`  
- `Boost 1.84.0` (`accumulators`, `program_options`, `filesystem`, `system`)  
- Для тестирования используется `Catch2 3.5.0`.  

Для ускорения обработки CSV применяется двойная буферизация с асинхронной загрузкой файлов.

## Сборка
1. Клонировать проект:  
  ```bash
  git clone https://github.com/username/TestProject.git
2.Перейти в корневую папку проекта:
  cd TestProject
  Создать папку сборки и перейти в неё:
  mkdir build
  cd build
3.Запустить CMake:
  cmake ..
  cmake --build .
Важно: необходим интернет для загрузки зависимостей.

### Использование

Запуск с указанием конфигурационного файла:
csv_median_calculator -config config.toml
Если файл конфигурации не указан, config.toml ищется в директории исполняемого файла: csv_median_calculator

Формат config.toml
[main]
input = './data'           # обязательный параметр
output = './results'       # опционально
filename_mask = ['trade']  # опционально

####Тесты
Запуск тестовой версии:
csv_median_calculator
Тестовые файлы расположены в папке test_files/ и необходимы для корректного прохождения тестов.

#####Структура проекта
CMakeLists.txt
src/                # исходники
  asyncbuffer.cpp
  asyncbuffer.h
  buffer.cpp        # не используется
  buffer.h          # не используется
  filescompare.h    # необходим для тестов
  logger.h
  main.cpp
  main_test.cpp     # необходим для тестов
  median_test.cpp
  mediantracker.h
  processor.cpp
  processor.h
  utility.cpp
  utility.h
test_files/         # файлы для тестов

