# Project WakeUpStrip v2.0
## Краткое описание
WakeUpStrip - это  бионический будильник, при помощи которого просыпаться станет легче.
### Почему это важно?
Когда наступает зима, рассвет начинается очень поздно. Из-за этого нам сложно вставать, и мы приходим на работу или в школу сонными и уставшими. Организм просто отказывается пробуждаться, если за окном темно. Дело в том, что рецепторы глаз реагируют на недостаточность освещения и подают мозгу соответствующий сигнал. В результате продолжает работать гормон сна мелатонин, а выработка кортизола затормаживается. Только при достаточном количестве солнечного света мы наконец-то получаем кортизол и легко просыпаемся.
## Информация по девайсу
### Протокол настроек
* $0; - Синхронизация(вывод всех параметров девайса)
* $1 0 <hour> <min> <sec>; - установка времени
* $1 1 <day> <month> <year>; - установка даты
* $1 2 <i>; - смена состояния i-го будильника (НУМЕРАЦИЯ НАЧИНАЕТСЯ С 0)
* $2 0; - выключение света ленты
* $2 1 <red> <green> <blue>; - включение света ленты в формате RGB
