## Homework_5

Напишите программу, выполняющую сжатие двухпроходным [алгоритмом Хаффмана](https://en.wikipedia.org/wiki/Huffman_coding). Покройте ваш код юнит-тестами.

Программа должна принимать в качестве аргументов ключ `-c` или `-d` для сжатия или распаковки соответственно, входной файл и выходной файл.

**Пример запуска в терминале Unix/Linux:**

```shell
$ huffman [-v] (-c|-d) input_file output_file
```

Программа должна выводить на экран статистику сжатия/распаковки: размер исходных данных, размер полученных данных и размер, который был использован для хранения вспомогательных данных (например, таблицы).

Помимо самого алгоритма вам нужно будет покрыть ваш код юнит-тестами. Протестируйте решение на типичных граничных случаях, удостоверьтесь, что после разжатия сжатого файла получается исходный файл:
* для пустого файла;
* для файла, содержащего ровно один байт;
* для файла, содержащего байт с кодом ноль;
* для файла, содержащего байт с кодом 255;
* для файла, содержащего ровно 256 различных байт;
* для файла, содержащего только два одинаковых байта;
* для больших текстовых файлов;
* для больших бинарных файлов;
* для больших файлов, содержащих случайные байты и, соответственно, плохо сжимающиеся.

#### Описание файлов:
+ src - папка решением;
+ test - папка с тестами.
