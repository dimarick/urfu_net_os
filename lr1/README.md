Лабораторная работа 1
=====================

Постановка задачи
-----------------

Разработать программу, анализирующую содержание каталога. 
Для всех объектов определяется их размер. 
Содержимое подкаталогов распечатывается, для файлов определяется их тип.

Решение
-------

Программа рекурсивного отображения содержимого директории
`lsr [OPTIONS] [DIRECTORY]`
`lsr --help`
`lsr --max-depth <n>, default n = 3`

Примеры вызова:

`./lsr`
`./lsr /etc`
`./lsr --max-depth 2 /etc`
`./lsr --max-depth 3 /etc`
`./lsr --help`