# TimescaleDB trends interface
## Summary
Запись значений трендов полученных из пайпа в БД.
iTSDB - перекладыватель из пайпа в коннектор БД
TSDBConnector - общение с БД

"Правильный" пример находится во второй части example/mainIface.cpp
## TODO
- [ ] Реализация первого чтения. Получение (или отдельная функция) из пайпа словаря трендов - тег, описание, ???
- [ ] Оптимизация по скорости
  - [ ] Отказ от создания новых элементов типа trendValue в пользу переназначения существующих элементов.
  - [x] Эффективная конвертация из trendValue в строку для записи в БД