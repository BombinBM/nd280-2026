#!/usr/bin/env bash
set -euo pipefail

# Создаёт виртуальное окружение и устанавливает зависимости из requirements.txt
if [ -d ".venv" ]; then
  echo ".venv already exists. Activate it with: source .venv/bin/activate"
  exit 0
fi

python3 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
pip install -r requirements.txt

echo
echo "Virtualenv создан в .venv и зависимости установлены."
echo "Не забудьте перед запуском примеров настроить окружение ROOT, например:"
echo "  source /path/to/ROOT/bin/thisroot.sh"
