# KillSwitch
Эльф для аварийного выключения телефона, если он завис в бесконечном цикле. 
Бывает полезно, если невозможно передёрнуть батарею, а телефон наглухо завис.

Эльф по таймеру опрашивает матрицу клавиатуры и при нажатии определённой комбинации кнопок выключает телефон.

**Комбинация по-умолчанию:** Volume+ + #

Комбинация подходит как минимум для E71/C81. Для остальных моделей может потребоваться настройка.

# Сборка
```
git clone https://github.com/siemens-mobile-hacks/sdk
git clone https://github.com/siemens-mobile-hacks/elfloader3-elfs-archive
cd elfloader3-elfs-archive/KillSwitch
make
```

# Настройка
1. Закинуть kill-switch.elf в 0:\zbin\daemons\ и запустить его.
2. Посмотреть значения PORT0-PORT2 для желаемой комбинации клавиш с помощью эльфа [KeypadTester](https://github.com/siemens-mobile-hacks/elfloader3-elfs-archive/tree/main/KeypadTester).
3. Прописать их в конфиге KillSwitch.bcfg в соответствующих полях.
