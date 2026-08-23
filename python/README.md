PyROOT integration - каркас

Кратко:
- Предположения: ROOT установлен и доступен (например, выполнён `source /path/to/ROOT/bin/thisroot.sh`).
- Этот каталог содержит примеры, скрипт для создания виртуального окружения и список Python-зависимостей.

Быстрый старт:
1. Открой терминал и убедись, что ROOT корректно настроен:
   source /path/to/ROOT/bin/thisroot.sh
2. Создай виртуальное окружение и установи зависимости:
   cd python
   ./setup_pyroot.sh
3. Запусти пример чтения ROOT-файла:
   python/examples/01_read_root_file.py /path/to/your.root

Файлы в этой папке:
- `setup_pyroot.sh` — создаёт `.venv` и ставит Python-зависимости.
- `requirements.txt` — пакеты pip (numpy, matplotlib, uproot, scikit-learn).
- `examples/01_read_root_file.py` — демонстрация открытия ROOT-файла и перечисления деревьев/ветвей.
- `examples/02_analyze_events.py` — пример построения гистограммы через PyROOT.
- `examples/03_ml_example.py` — пример извлечения массивов (через `uproot`) и простого ML-примера.

Советы:
- PyROOT обычно идёт в составе ROOT — не пытайтесь `pip install pyroot` без явной необходимости.
- Если хотите использовать только Python (без PyROOT), рассмотрите `uproot` + `awkward`.
